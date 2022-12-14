#include <llvm/IR/LegacyPassManager.h>
#include <llvm/Passes/PassBuilder.h>
#include <llvm/MC/TargetRegistry.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Target/TargetMachine.h>
#include "IR.h"
#include "hello_world_pass.h"
#include "mem2reg_pass.h"
#include "pass_manager.h"
#include <llvm/CodeGen/RegAllocRegistry.h>

// 使用llvm的新pass manager
// https://llvm.org/docs/NewPassManager.html
void PassManager::run(int optLevel, const std::string &filename) {

    llvm::InitializeAllTargetInfos();
    llvm::InitializeAllTargets();
    llvm::InitializeAllTargetMCs();
    llvm::InitializeAllAsmParsers();
    llvm::InitializeAllAsmPrinters();

    std::string err;
    auto triple = "arm-unknown-linux-gnu";
    auto target = llvm::TargetRegistry::lookupTarget(triple, err);
    if (!target) {
        throw std::logic_error(err);
    }

    auto CPU = "generic";
    auto features = "";
    llvm::TargetOptions opt;
#ifdef CONF_HARD_FLOAT
    opt.FloatABIType = llvm::FloatABI::Hard;
#endif
    auto targetMachine =
            target->createTargetMachine(triple, CPU, features, opt, {});

    IR::ctx.module.setDataLayout(targetMachine->createDataLayout());
    IR::ctx.module.setTargetTriple(triple);

    if (optLevel != 0) {
        llvm::LoopAnalysisManager LAM;
        llvm::FunctionAnalysisManager FAM;
        llvm::CGSCCAnalysisManager CGAM;
        llvm::ModuleAnalysisManager MAM;

        llvm::PassBuilder PB(targetMachine);

        PB.registerModuleAnalyses(MAM);
        PB.registerCGSCCAnalyses(CGAM);
        PB.registerFunctionAnalyses(FAM);
        PB.registerLoopAnalyses(LAM);
        PB.crossRegisterProxies(LAM, FAM, CGAM, MAM);

        // 在优化管道前端加入自己的pass
        PB.registerPipelineStartEPCallback(
                [&](llvm::ModulePassManager &MPM, llvm::OptimizationLevel level) {
                    MPM.addPass(llvm::createModuleToFunctionPassAdaptor(HelloWorldPass()));
                    MPM.addPass(llvm::createModuleToFunctionPassAdaptor(llvm::PromotePass()));
                }
        );

//        llvm::ModulePassManager MPM = PB.buildPerModuleDefaultPipeline(
//                llvm::OptimizationLevel::O3
//        );

        log("PM") << "optimizing module" << std::endl;
        //MPM.run(IR::ctx.module, MAM);

        // 展示优化后的IR
        IR::show();
    }

    // 生成汇编文件
    std::error_code EC;
    llvm::raw_fd_ostream file(filename, EC, llvm::sys::fs::OF_None);
    if (EC) {
        throw std::runtime_error("Could not open file: " + EC.message());
    }

    log("PM") << "generate assembly" << std::endl;
    llvm::RegisterRegAlloc::setDefault(llvm::createBasicRegisterAllocator);
    llvm::legacy::PassManager codeGenPass;
    auto fileType = llvm::CGFT_AssemblyFile;
    if (targetMachine->addPassesToEmitFile(codeGenPass, file, nullptr, fileType)) {
        throw std::logic_error("TargetMachine can't emit a file of this type");
    }

    codeGenPass.run(IR::ctx.module);
}
