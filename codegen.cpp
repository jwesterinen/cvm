#include <stdio.h>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <list>
#include <array>
#include "codegen.h"


CVMCode::CVMCode(std::ofstream& _outputFile) : 
    outputFile(_outputFile), labelId(0), returnId(0)
{
}

CVMCode::~CVMCode()
{
}

void CVMCode::SetFileName(const char* _fileName)
{
	size_t pos = 0;

	curFileName = _fileName;
	if ((pos = curFileName.find_last_of("/")) != std::string::npos)
	{
		curFileName = curFileName.substr(pos+1);
	}
}

void CVMCode::WriteInit()
{
	// set the initial stack, frame, and expression base pointers
	outputFile << "    " << "@" << STACK_BASE << std::endl;
	outputFile << "    " << "D=A" << std::endl;
	outputFile << "    " << "@SP" << std::endl;
	outputFile << "    " << "M=D" << std::endl;
	outputFile << "    " << "@" << FRAME_BASE << std::endl;
	outputFile << "    " << "D=A" << std::endl;
	outputFile << "    " << "@BP" << std::endl;
	outputFile << "    " << "M=D" << std::endl;
	outputFile << "    " << "@" << EXPR_BASE << std::endl;
	outputFile << "    " << "D=A" << std::endl;
	outputFile << "    " << "@EP" << std::endl;
	outputFile << "    " << "M=D" << std::endl;
	
	// jump to main()
	outputFile << "    " << "@main" << std::endl;
	outputFile << "    " << "0;JMP" << std::endl;
}

void CVMCode::WritePop()
{
    outputFile << "    " << "\n// pop" << std::endl;
    
	outputFile << "    " << "@SP" << std::endl;
	outputFile << "    " << "M=M-1" << std::endl;
}

void CVMCode::WriteCall(const std::string& fctname)
{
    outputFile << "    " << "\n// call" << std::endl;
    
    std::string label;
    outputFile << "    " << "@L" << ++labelId << std::endl;     // push return address
    outputFile << "    " << "D=A" << std::endl;
    outputFile << "    " << "@SP" << std::endl;
    outputFile << "    " << "M=M+1" << std::endl;               
    outputFile << "    " << "A=M" << std::endl;
    outputFile << "    " << "M=D" << std::endl;
    outputFile << "    " << "@" << fctname << std::endl;        // jump to function
    outputFile << "    " << "0;JMP" << std::endl;
    outputFile << "(L" << labelId << ")" << std::endl;          // return address
}

void CVMCode::WriteEntry(const std::string& fctname, const std::string& localVarQty)
{
    outputFile << "    " << "\n// entry" << std::endl;

    curFctName = fctname;    
    
    outputFile << "(" << fctname << ")" << std::endl;
    outputFile << "    " << "@BP" << std::endl;                 // push current BP
    outputFile << "    " << "D=M" << std::endl;
    outputFile << "    " << "@SP" << std::endl;
    outputFile << "    " << "M=M+1" << std::endl;
    outputFile << "    " << "A=M" << std::endl;
    outputFile << "    " << "M=D" << std::endl;    
    outputFile << "    " << "@SP" << std::endl;                 // move SP to BP
    outputFile << "    " << "D=M" << std::endl;
    outputFile << "    " << "@BP" << std::endl;
    outputFile << "    " << "M=D" << std::endl;
	outputFile << "    " << "@" << localVarQty << std::endl;    // adjust SP past local variables
	outputFile << "    " << "D=A" << std::endl;
    outputFile << "    " << "@SP" << std::endl;
    outputFile << "    " << "M=D+M" << std::endl;
}

void CVMCode::WriteReturn()
{
    // optimization: no need to return from main - WriteEnd() takes care of that
    
    outputFile << "    " << "\n// return" << std::endl;
    
    outputFile << "    " << "@BP" << std::endl;                 // move BP to SP to remove local vars
    outputFile << "    " << "D=M-1" << std::endl;
    outputFile << "    " << "@SP" << std::endl;
    outputFile << "    " << "M=D" << std::endl;    
    outputFile << "    " << "@SP" << std::endl;                 // restore old BP
    outputFile << "    " << "A=M+1" << std::endl;
    outputFile << "    " << "D=M" << std::endl;
    outputFile << "    " << "@BP" << std::endl;
    outputFile << "    " << "M=D" << std::endl;            
    if (curFctName == "main")
    {
        outputFile << "    " << "@__Exit" << std::endl;         // for main() jump to the end of the program -- essentially "exit"
        outputFile << "    " << "0;JMP" << std::endl;
    }
    else
    {
        outputFile << "    " << "@SP" << std::endl;             // pop old BP, now return is TOS
        outputFile << "    " << "M=M-1" << std::endl;
        outputFile << "    " << "A=M+1" << std::endl;           // restore the return address and return
        outputFile << "    " << "A=M" << std::endl;
        outputFile << "    " << "0;JMP" << std::endl;
    }
}

void CVMCode::WriteEnd()
{
    outputFile << "    " << "\n// end" << std::endl;
    
    outputFile << "(__Exit)" << std::endl;
    outputFile << "    " << "@__Exit" << std::endl;
    outputFile << "    " << "0;JMP\n" << std::endl;
}

void CVMCode::WriteLabel(const std::string& label)
{
    outputFile << "    " << "\n// label" << std::endl;
    
    outputFile << "(" << label << ")" << std::endl;
}

void CVMCode::WriteAlu(const std::string& mod)
{
    outputFile << "    " << "\n// alu" << std::endl;
    
	if (mod == "+" || mod == "-" || mod == "&" || mod == "|")
	{
		outputFile << "    " << "@SP" << std::endl;             // pop y
		outputFile << "    " << "M=M-1" << std::endl;
		outputFile << "    " << "A=M+1" << std::endl;
		outputFile << "    " << "D=M" << std::endl;
		outputFile << "    " << "A=A-1" << std::endl;
		if (mod == "+")
		{
			outputFile << "    " << "M=D+M" << std::endl;       // TOS = x + y
		}
		else if (mod == "-")
		{
			outputFile << "    " << "M=M-D" << std::endl;	    // TOS = x - y
		}
		else if (mod == "&")
		{
			outputFile << "    " << "M=D&M" << std::endl;	    // TOS = x & y
		}
		else if (mod == "|")
		{
			outputFile << "    " << "M=D|M" << std::endl;	    // TOS = x | y
		}
	}
	else if (mod == "*" || mod == "/")
	{
		if (mod == "*")
		{
			WriteCall("Multiply");			                    // TOS = x * y
		}
		else if (mod == "/")
		{
			WriteCall("Divide");			                    // TOS = x / y
		}
	    outputFile << "    " << "@retval" << std::endl;
	    outputFile << "    " << "D=M" << std::endl;
	    outputFile << "    " << "@SP" << std::endl;
	    outputFile << "    " << "M=M-1" << std::endl;
	    outputFile << "    " << "A=M" << std::endl;
	    outputFile << "    " << "M=D" << std::endl;
    }
#ifdef NEG_AND_NOT_IMPLMENTED_IN_THE_PARSER	
	else if (mod == "neg" || mod == "not" << std::endl;
	{
		outputFile << "    " << "@SP" << std::endl;			    // pop y
		outputFile << "    " << "A=M" << std::endl;
		if (mod == "neg")
		{
			outputFile << "    " << "M=-M" << std::endl;	    // [sp-1] = -y
		}
		else if (mod == "not")
		{
			outputFile << "    " << "M=!M" << std::endl;	    // [sp-1] = !y
		}
	}
#endif
	else if (mod == "==" || mod == "!=" || mod == ">" || mod == ">=" || mod == "<" || mod == "<=")
	{
	    
		outputFile << "    " << "@SP" << std::endl;			    // pop y
		outputFile << "    " << "M=M-1" << std::endl;
		outputFile << "    " << "A=M+1" << std::endl;
		outputFile << "    " << "D=M" << std::endl;
		outputFile << "    " << "A=A-1" << std::endl;
		outputFile << "    " << "D=M-D" << std::endl;           // make the logical comparison (x-y)
	    outputFile << "    " << "@SP" << std::endl;             // replace TOS with True (assume True)
	    outputFile << "    " << "A=M" << std::endl;
	    outputFile << "    " << "M=-1" << std::endl;
		outputFile << "    " << "@LT" << ++labelId << std::endl;    // jump if the following logical expressions are True
		if (mod == "==")
		{
			outputFile << "    " << "D;JEQ" << std::endl;       // x == y
		}
		else if (mod == "!=")
		{
			outputFile << "    " << "D;JNE" << std::endl;       // x != y           
		}
		else if (mod == ">")
		{
			outputFile << "    " << "D;JGT" << std::endl;       // x > y
		}
		else if (mod == ">=")   
		{
			outputFile << "    " << "D;JGE" << std::endl;       // x >= y
		}
		else if (mod == "<")
		{
			outputFile << "    " << "D;JLT" << std::endl;       // x < y
		}
		else if (mod == "<=")
		{
			outputFile << "    " << "D;JLE" << std::endl;       // x <= y
		}
		outputFile << "    " << "@SP" << std::endl;			    // otherwise replace TOS with False
		outputFile << "    " << "A=M" << std::endl;
		outputFile << "    " << "M=0" << std::endl;
		outputFile << "(LT" << labelId << ")" << std::endl;
	}
	else
	{
		outputFile << "illegal mod" << std::endl;
	}
}

void CVMCode::WriteJumpz(const std::string& dest)
{
    outputFile << "    " << "\n// jumpz" << std::endl;
    
    outputFile << "    " << "@SP" << std::endl;
    outputFile << "    " << "A=M" << std::endl;
    outputFile << "    " << "D=M" << std::endl;
    outputFile << "    " << "@" << dest << std::endl;
    outputFile << "    " << "D;JEQ" << std::endl;
}

void CVMCode::WriteJump(const std::string& dest)
{
    outputFile << "    " << "\n// jump" << std::endl;
    
    outputFile << "    " << "@" << dest << std::endl;
    outputFile << "    " << "0;JMP" << std::endl;
}

// set the effective address (M) based on the variable type
void CVMCode::WriteAccessVar(const std::string& vartype, const std::string& offsetStr, const std::string& globalName)
{
    int offset = atoi(offsetStr.c_str());
    
    if (vartype == "gbl")
    {
        if (offset == 0)
        {
            // global 0 signifies the global address of the return value of the function, "retval"
	        outputFile << "    " << "@retval" << "\t// fct return value" << std::endl;
	    }
	    else
	    {
            // use the global name directly
            outputFile << "    " << "@" << globalName << "\t// global" << std::endl;
        }
    }
    else if (vartype == "con")
    {
        // use the offset as the constant value
	    outputFile << "    " << "@" << offset << "\t// constant" << std::endl;
    }
    else if (vartype == "par")
    {
        // parameter n is at BP-(n+2)
        offset += 2;
        outputFile << "    " << "@" << offset << "\t// " << globalName << std::endl; // adjust the var offset from the BP
        outputFile << "    " << "D=A" << std::endl;
        outputFile << "    " << "@BP" << std::endl;     // point to the var
        outputFile << "    " << "A=M-D" << std::endl;
    }
    else if (vartype == "lcl")
    {
        // local variable n is at BP+n+1
        offset += 1;
        outputFile << "    " << "@" << offset << "\t// " << globalName << std::endl; // adjust the var offset from the BP
        outputFile << "    " << "D=A" << std::endl;
        outputFile << "    " << "@BP" << std::endl;     // point to the var
        outputFile << "    " << "A=D+M" << std::endl;
    }
    else
    {
        fprintf(stderr, "Error: illegal variable type - exiting...\n");
        exit(1);
    }
}

void CVMCode::WriteDec(const std::string& vartype, const std::string& offset, const std::string& globalName)
{
    outputFile << "    " << "\n// dec" << std::endl;
   
    WriteAccessVar(vartype, offset, globalName);
    outputFile << "    " << "M=M-1" << std::endl;       // decrement the var
    outputFile << "    " << "@SP" << std::endl;         // dummy push to accomodate the parser's insisting on a pop after inc
    outputFile << "    " << "M=M+1" << std::endl;
}

void CVMCode::WriteInc(const std::string& vartype, const std::string& offset, const std::string& globalName)
{
    outputFile << "    " << "\n// inc" << std::endl;
    
    WriteAccessVar(vartype, offset, globalName);
    outputFile << "    " << "M=M+1" << std::endl;       // increment the var
    outputFile << "    " << "@SP" << std::endl;         // dummy push to accomodate the parser's insisting on a pop after inc
    outputFile << "    " << "M=M+1" << std::endl;
}

// push the variable
void CVMCode::WriteLoad(const std::string& vartype, const std::string& offsetStr, const std::string& globalName)
{
    outputFile << "    " << "\n// load" << std::endl;
    
    WriteAccessVar(vartype, offsetStr, globalName);     // point to the effective address of the var
    if (vartype == "con")
    {
        // use the offset as the constant value
	    outputFile << "    " << "D=A" << std::endl;
    }
    else
    {
        outputFile << "    " << "D=M" << std::endl;
    }
    outputFile << "    " << "@SP" << std::endl;         // push the var
    outputFile << "    " << "M=M+1" << std::endl;               
    outputFile << "    " << "A=M" << std::endl;
    outputFile << "    " << "M=D" << std::endl;
}

// write TOS to the variable
void CVMCode::WriteStore(const std::string& vartype, const std::string& offsetStr, const std::string& globalName)
{
    outputFile << "    " << "\n// store" << std::endl;
    
    if (vartype == "gbl")
    {
        // use the global name directly
        outputFile << "    " << "@SP" << std::endl;         // copy TOS to D
        outputFile << "    " << "A=M" << std::endl;
        outputFile << "    " << "D=M" << std::endl;
        WriteAccessVar(vartype, offsetStr, globalName);     // write D to global address
	    outputFile << "    " << "M=D" << std::endl;	    
    }
    else
    {
        WriteAccessVar(vartype, offsetStr, globalName);     // get the var _address_
        outputFile << "    " << "D=A" << std::endl;        
        outputFile << "    " << "@SP" << std::endl;         // push the var address (<segment addr>+<index>)
        outputFile << "    " << "M=M+1" << std::endl;
        outputFile << "    " << "A=M" << std::endl;
        outputFile << "    " << "M=D" << std::endl;
        
        outputFile << "    " << "@SP" << std::endl;         // load D with the TOS-1 value
        outputFile << "    " << "A=M" << std::endl;
        outputFile << "    " << "A=A-1" << std::endl;
        outputFile << "    " << "D=M" << std::endl;
        outputFile << "    " << "@SP" << std::endl;         // pop the ea
        outputFile << "    " << "M=M-1" << std::endl;       //   [SP] = <segment addr>+<index>
        outputFile << "    " << "A=M+1" << std::endl;       //   ea = [<segment addr>+<index>]
        outputFile << "    " << "A=M" << std::endl;         // write the value to the ea
        outputFile << "    " << "M=D" << std::endl;
    }
}

// pass the equate line directly to the output file, the assembler will handle it
void CVMCode::WriteEqu(const std::string& equ)
{
    outputFile << equ << std::endl;
}


// end of codegen.cpp

