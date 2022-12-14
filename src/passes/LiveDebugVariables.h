#ifndef LLVM_LIB_CODEGEN_LIVEDEBUGVARIABLES_H
#define LLVM_LIB_CODEGEN_LIVEDEBUGVARIABLES_H

#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/Support/Compiler.h"

namespace llvm {

    template <typename T> class ArrayRef;
    class LiveIntervals;
    class VirtRegMap;

    class LLVM_LIBRARY_VISIBILITY LiveDebugVariables : public MachineFunctionPass {
        void *pImpl = nullptr;

    public:
        static char ID; // Pass identification, replacement for typeid

        LiveDebugVariables();
        ~LiveDebugVariables() override;

        /// splitRegister - Move any user variables in OldReg to the live ranges in
        /// NewRegs where they are live. Mark the values as unavailable where no new
        /// register is live.
        void splitRegister(Register OldReg, ArrayRef<Register> NewRegs,
                           LiveIntervals &LIS);

        /// emitDebugValues - Emit new DBG_VALUE instructions reflecting the changes
        /// that happened during register allocation.
        /// @param VRM Rename virtual registers according to map.
        void emitDebugValues(VirtRegMap *VRM);

        /// dump - Print data structures to dbgs().
        void dump() const;

    private:
        bool runOnMachineFunction(MachineFunction &) override;
        void releaseMemory() override;
        void getAnalysisUsage(AnalysisUsage &) const override;

        MachineFunctionProperties getSetProperties() const override {
            return MachineFunctionProperties().set(
                    MachineFunctionProperties::Property::TracksDebugUserValues);
        }
    };

} // end namespace llvm

#endif // LLVM_LIB_CODEGEN_LIVEDEBUGVARIABLES_H