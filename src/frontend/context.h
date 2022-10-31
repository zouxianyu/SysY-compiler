#ifndef SYSY_COMPILER_FRONTEND_CONTEXT_H
#define SYSY_COMPILER_FRONTEND_CONTEXT_H

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include "symbol_table.h"

// 用于IR生成的context
struct Context {
    // 注意：顺序不可调整，否则初始化顺序可能存在问题
    llvm::LLVMContext llvmContext;
    llvm::Module module;
    llvm::IRBuilder<> builder;
    SymbolTable symbolTable;

    Context() : llvmContext(),
                module("master", llvmContext),
                builder(llvmContext) {}
};

#endif //SYSY_COMPILER_FRONTEND_CONTEXT_H
