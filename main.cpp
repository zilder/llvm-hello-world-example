#include <cstdio>
#include <fstream>
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Function.h"
#include "llvm/ExecutionEngine/GenericValue.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/Support/raw_os_ostream.h"

using namespace llvm;

static IRBuilder<> Builder(getGlobalContext());

std::unique_ptr<Module> buildModule()
{
	std::unique_ptr<Module> module = llvm::make_unique<Module>("top", getGlobalContext());

	/* Create main function */
	FunctionType *funcType = FunctionType::get(Builder.getInt32Ty(), false);	
	Function *mainFunc = Function::Create(funcType, Function::ExternalLinkage, "main", module.get());
	BasicBlock *entry = BasicBlock::Create(getGlobalContext(), "entrypoint", mainFunc);
	Builder.SetInsertPoint(entry);

	/* String constant */
	Value *helloWorldStr = Builder.CreateGlobalStringPtr("hello world!\n");

	/* Create "puts" function */
	std::vector<Type *> putsArgs;
	putsArgs.push_back(Builder.getInt8Ty()->getPointerTo());
	ArrayRef<Type*> argsRef(putsArgs);
	FunctionType *putsType = FunctionType::get(Builder.getInt32Ty(), argsRef, false);
	Constant *putsFunc = module->getOrInsertFunction("puts", putsType);

	/* Invoke it */
	Builder.CreateCall(putsFunc, helloWorldStr);

	/* Return zero */
	Builder.CreateRet(ConstantInt::get(getGlobalContext(), APInt(32, 0)));

	return module;
}

void writeModuleToFile(Module *module)
{
	std::ofstream std_file_stream("program.ll");
	raw_os_ostream file_stream(std_file_stream);
	module->print(file_stream, nullptr);
}

int main()
{
	std::unique_ptr<Module> module = buildModule();
	writeModuleToFile(module.get());
	return 0;
}
