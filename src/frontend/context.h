#ifndef SYSY_COMPILER_FRONTEND_CONTEXT_H
#define SYSY_COMPILER_FRONTEND_CONTEXT_H

#include <stack>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include "symbol_table.h"
#include "loop_info.h"

// 用于IR生成的context
struct Context {
    // 注意：顺序不可调整，否则初始化顺序可能存在问题
    llvm::LLVMContext llvmCtx;
    llvm::Module module;
    llvm::IRBuilder<> builder;
    SymbolTable<llvm::Value *> symbolTable;

    // 区分全局/局部变量，仅在进入、退出函数时发生改变
    llvm::Function *function;

    // 循环信息栈，记录嵌套循环，用于continue/break
    std::stack<LoopInfo> loops;

    Context() : llvmCtx(),
                module("SysY_src", llvmCtx),
                builder(llvmCtx),
                symbolTable(),
                function(nullptr) {}
};

#endif //SYSY_COMPILER_FRONTEND_CONTEXT_H
