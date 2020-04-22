#pragma once

#include "cvm.h"

#define STACK_BASE 256
#define FRAME_BASE 512
#define EXPR_BASE 768

// write instruction macro
#define WRITE_INSTRUCTION(s) outputFile << "    " << s << std::endl;
#define WRITE_INSTRUC_INT(s,i) outputFile << "    " << s << i << std::endl;

// write reference macro
#define WRITE_REFERENCE(s) outputFile << "    " << "@" << s << std::endl;

// write label macro
#define WRITE_LABEL(s) outputFile << "    " << "(" << s << ")" << std::endl;

// pop D macro
#define POPD	\
WRITE_INSTRUCTION("@SP")	\
WRITE_INSTRUCTION("M=M-1")	\
WRITE_INSTRUCTION("A=M+1")	\
WRITE_INSTRUCTION("D=M")

// push D macro
#define PUSHD	\
WRITE_INSTRUCTION("@SP")	\
WRITE_INSTRUCTION("M=M+1")	\
WRITE_INSTRUCTION("A=M")	\
WRITE_INSTRUCTION("M=D")	\

class CVMCode
{
public:
	CVMCode(std::ofstream& _outputFile, EQU_MAP& _equateMap);
	virtual ~CVMCode();

	void SetFileName(const char* _fileName);
    void WriteInit();
	void WritePop();
	void WriteReturn();
	void WriteEnd();
	void WriteLabel(const std::string& label);
	void WriteAlu(const std::string& mod);
	void WriteJumpz(const std::string& dest);
	void WriteJump(const std::string& dest);
	void WriteCall(const std::string& fctname);
	void WriteEntry(const std::string& fctname, const std::string& localVarQty);
    void WriteAccessVar(const std::string& vartype, const std::string& offset, const std::string& globalName);
	void WriteDec(const std::string& vartype, const std::string& offset, const std::string& globalName);
	void WriteInc(const std::string& vartype, const std::string& offset, const std::string& globalName);
	void WriteLoad(const std::string& vartype, const std::string& offset, const std::string& globalName);
	void WriteStore(const std::string& vartype, const std::string& offset, const std::string& globalName);

private:
	std::ofstream& outputFile;
    EQU_MAP& equateMap;
	std::string curFileName;
	std::string curFctName;
	int labelId;
	int returnId;
    unsigned curLocalVarQty;
};

