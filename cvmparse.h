#pragma once

#include "cvm.h"

class CVMParser
{
	typedef std::array<std::string, 10> TOKENS;

public:
	CVMParser(std::ifstream& _inputFile);
	virtual ~CVMParser();

	bool HasMoreCommands();
	void Advance();
	void AddEquate();
	CVMCommandType CommandType();
	const std::string& Arg1() const
	{
		return arg1;
	}
	const std::string& Arg2() const
	{
		return arg2;
	}
	const std::string& Arg3() const
	{
		return arg3;
	}

private:
	std::ifstream& inputFile;
	std::string curInstr;
	char cmdbuf[80], argbuf1[80], argbuf2[80], argbuf3[80];
	std::string cmd, arg1, arg2, arg3;
};


// end of cvmparse.h

