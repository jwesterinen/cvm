#include <stdio.h>
#include <iostream>
#include <string>
#include <fstream>
#include <list>
#include <array>
#include "cvmparse.h"

struct CommandSpec
{
	std::string name;
	CVMCommandType command;
};

// 0 arg command list
const CommandSpec zeroArgCmdList[]
{
	{"pop",     C_POP       },
    {"return",  C_RETURN    },
    {"end",     C_END       }
};
int zeroArgCmdListSize = sizeof zeroArgCmdList / sizeof (CommandSpec);

// 1 arg command list
const CommandSpec oneArgCmdList[]
{
    {"label",   C_LABEL     },
    {"alu",     C_ALU       },
    {"jumpz",   C_JUMPZ     },
    {"jump",    C_JUMP      },
    {"call",    C_CALL      }
};
int oneArgCmdListSize = sizeof oneArgCmdList / sizeof (CommandSpec);

// 2 arg command list
const CommandSpec twoArgCmdList[]
{
    {"entry",   C_ENTRY     }
};
int twoArgCmdListSize = sizeof twoArgCmdList / sizeof(CommandSpec);

// 3 arg command list
const CommandSpec threeArgCmdList[]
{
    {"dec",     C_DEC       },
    {"inc",     C_INC       },
    {"load",    C_LOAD      },
    {"store",   C_STORE     }
};
int threeArgCmdListSize = sizeof threeArgCmdList / sizeof(CommandSpec);

// global arg list (string, string)
const CommandSpec globalArgCmdList[]
{
};
int globalArgCmdListSize = sizeof globalArgCmdList / sizeof(CommandSpec);

CVMParser::CVMParser(std::ifstream& _inputFile) : inputFile(_inputFile)
{
}


CVMParser::~CVMParser()
{
}

bool CVMParser::HasMoreCommands()
{
	std::getline(inputFile, curInstr);
	return !inputFile.eof();
}

void CVMParser::Advance()
{
	// screen out comments and blank lines
	while (curInstr == "" || curInstr[0] == '/')
		std::getline(inputFile, curInstr);
}

CVMCommandType CVMParser::CommandType()
{
	size_t pos;

	// CVM Language:
	
	// commands with 0 args
	// pop                  ; clear stack       // OP_POP:      pop TOS
	// return               ; end of function   // OP_RETURN:   function epilog
	// end                  ; end of program    // C_END:       emit program epilog
	// $$3      equ     2   ; main              // equate a symbol to a value - build string table on pass 1, ignore on pass 2
	
	// commands with 1 args
	// label    $$4                             // label:       convert to '('<new label>')'
	// alu      <=		    ; <=                // OP_ALU:      type
	// jump     $$7		    ; BREAK             // OP_JUMP:     dest
	// jumpz    $$4		    ; IF                // OP_JUMPZ:    dest
	// call     bar                             // OP_CALL:     fctname
	
	// commands with 2 args
	// entry	main    $$3                     // OP_ENTRY:    function prolog (name, local var count)
	
	// commands with 3 args
	// dec      lcl     0   ; x                 // OP_DEC:      vartype offset
	// inc      lcl     0	; x                 // OP_INC:      vartype offset
	// store    gbl     0   ; save result       // OP_STORE:    store return value to gbl,0 by def
	// load     gbl     1   ; g                 // OP_GLOBAL:   global var is in the comments
	

	// parse the op from the instruction string
	sscanf(curInstr.c_str(), "%s", cmdbuf);
	cmd = cmdbuf;
	//std::cout << "parsing command: " << cmd << std::endl;
	
	// return equates back without parsing
    if ((pos = curInstr.find("equ")) != std::string::npos)
    {
        // the single argument _is_ the current instruction, i.e. the full equate line
        arg1 = curInstr;
        return C_EQU;
    }
    
	// determine the command type and parse the opnds
	for (int i = 0; i < zeroArgCmdListSize; i++)
	{
		if ((pos = cmd.find(zeroArgCmdList[i].name)) != std::string::npos)
		{
			return zeroArgCmdList[i].command;
		}
	}
	for (int i = 0; i < oneArgCmdListSize; i++)
	{
		if ((pos = cmd.find(oneArgCmdList[i].name)) != std::string::npos)
		{
	        sscanf(curInstr.c_str(), "%s\t%s", cmdbuf, argbuf1);
	        arg1 = argbuf1;
			return oneArgCmdList[i].command;
		}
	}
	for (int i = 0; i < twoArgCmdListSize; i++)
	{
		if ((pos = cmd.find(twoArgCmdList[i].name)) != std::string::npos)
		{
	        sscanf(curInstr.c_str(), "%s\t%s\t%s", cmdbuf, argbuf1, argbuf2);
	        arg1 = argbuf1;
	        arg2 = argbuf2;
			return twoArgCmdList[i].command;
		}
	}
		
	for (int i = 0; i < threeArgCmdListSize; i++)
	{
		if ((pos = cmd.find(threeArgCmdList[i].name)) != std::string::npos)
		{
	        sscanf(curInstr.c_str(), "%s\t%s\t%s\t;\t%s", cmdbuf, argbuf1, argbuf2, argbuf3);
	        arg1 = argbuf1;
	        arg2 = argbuf2;
	        arg3 = argbuf3;
			return threeArgCmdList[i].command;
		}
	}
		
	return C_ILLEGAL;
}


// end of cvmparse.cpp

