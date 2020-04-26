// Main control module for the cvm to hack assembler converter

#include <stdio.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <fstream>
#include <list>
#include <array>
#include <vector>
#include "cvmparse.h"
#include "codegen.h"

// Returns a list of files in a directory (except the ones that begin with a dot)
void GetFilesInDirectory(std::vector<std::string> &out, const std::string &directory)
{
	DIR *dir;
	class dirent *ent;
	class stat st;

	dir = opendir(directory.c_str());
	while ((ent = readdir(dir)) != NULL) 
	{
		const std::string file_name = ent->d_name;
		const std::string full_file_name = directory + "/" + file_name;

		if (file_name[0] == '.')
			continue;

		if (stat(full_file_name.c_str(), &st) == -1)
			continue;

		const bool is_directory = (st.st_mode & S_IFDIR) != 0;

		if (is_directory)
			continue;

		out.push_back(full_file_name);
	}
	closedir(dir);
} 

int main(int argc, char** argv)
{
	if (argc < 2)
	{
		fprintf(stderr, "no input file specified\n");
		return 1;
	}
	
	// get a list of all of the source files
	std::string source(argv[1]);
	std::vector<std::string> inputFileNames;
	if ((source.find(".cvm")) != std::string::npos)
	{
		// argv[1] is a single source file
		inputFileNames.push_back(source);
	}
	else
	{
		// source is a directory so get all of the files
		GetFilesInDirectory(inputFileNames, source);
	}

	// create the output file and init the code generator
	size_t pos = 0;
	std::string outfileName(argv[1]);
	if ((pos = outfileName.find(".cvm")) != std::string::npos)
	{
		outfileName = outfileName.substr(0, pos) + ".asm";
	}
	else
	{
		std::string outfilePath(outfileName);
		if ((pos = outfileName.find_last_of("/")) != std::string::npos)
		{
			outfileName = outfilePath + outfileName.substr(pos) + ".asm";
		}
		else
		{
			outfileName = outfilePath + "/" + outfileName + ".asm";
		}
	}
	std::ofstream outputFile(outfileName);
	CVMCode code(outputFile);

	// generate the bootstrap code
	code.WriteInit();

	// parse all input files to the one and only output file
	for (std::vector<std::string>::iterator i = inputFileNames.begin(); i != inputFileNames.end(); i++)
	{
		// open the next .vm file and init its parser
		if (i->find(".cvm") == std::string::npos)
		{
			continue;
		}
		std::ifstream inputFile(*i);
		if (!inputFile.is_open())
		{
			std::cout << "cvm: cannot open file " << argv[1] << std::endl;
		}
		CVMParser parser(inputFile);

		// let the code generator know what input file name it's working on for correct labels
		std::string inputFileBaseName(*i);
		if ((pos = i->find_last_of("\\")) != std::string::npos)
		{
			inputFileBaseName = inputFileBaseName.substr(pos+1);
		}
		if ((pos = inputFileBaseName.find(".")) != std::string::npos)
		{
			inputFileBaseName = inputFileBaseName.substr(0, pos);
		}
		code.SetFileName(inputFileBaseName.c_str());

        // emit code based on the VM command
        CVMCommandType cmdType;
		while (parser.HasMoreCommands())
		{
			parser.Advance();
			switch ((cmdType = parser.CommandType()))
			{
				case C_POP:
					code.WritePop();
					break;

				case C_RETURN:
					code.WriteReturn();
					break;

				case C_END:
					code.WriteEnd();
					break;

				case C_LABEL:
					code.WriteLabel(parser.Arg1());
					break;

				case C_ALU:
					code.WriteAlu(parser.Arg1());
					break;

				case C_JUMPZ:
					code.WriteJumpz(parser.Arg1());
					break;

				case C_JUMP:
					code.WriteJump(parser.Arg1());
					break;

				case C_CALL:
					code.WriteCall(parser.Arg1());
					break;

				case C_ENTRY:
					code.WriteEntry(parser.Arg1(), parser.Arg2());
					break;

				case C_DEC:
					code.WriteDec(parser.Arg1(), parser.Arg2(), parser.Arg3());
					break;

				case C_INC:
					code.WriteInc(parser.Arg1(), parser.Arg2(), parser.Arg3());
					break;

				case C_LOAD:
					code.WriteLoad(parser.Arg1(), parser.Arg2(), parser.Arg3());
					break;

				case C_STORE:
					code.WriteStore(parser.Arg1(), parser.Arg2(), parser.Arg3());
					break;

				case C_EQU:
				    code.WriteEqu(parser.Arg1());
					break;

				default:
					std::cout << "Illegal command: " << cmdType << std::endl;
					break;
			}
		}
	}

	return 0;
}


// end of cvm.cpp

