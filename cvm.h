#pragma once

#include <unordered_map>

enum CVMCommandType
{
    C_ALU,   
    C_DEC,
    C_INC,
    C_LOAD,
    C_STORE,
    C_POP,
    C_JUMPZ,
    C_JUMP,
    C_CALL,
    C_ENTRY,
    C_RETURN,
    C_END,
    C_LABEL,
    C_EQU,
    C_ILLEGAL
};

// pseudo op codes
#define OP_ALU      "alu"       // arithmetic/logic op
#define OP_DEC      "dec"       // region, offset
#define OP_INC      "inc"       // region, offset
#define OP_LOAD     "load"      // region, offset
#define OP_STORE    "store"     // region, offset
#define OP_POP      "pop"       // 
#define OP_JUMPZ    "jumpz"     // label
#define OP_JUMP     "jump"      // label
#define OP_CALL     "call"      // parm-count, address
#define OP_ENTRY    "entry"     // local-frame-size
#define OP_RETURN   "return"    // 

// region modifiers
#define MOD_GLOBAL  "gbl"       // global region
#define MOD_PARAM   "par"       // parameter region
#define MOD_LOCAL   "lcl"       // local region
#define MOD_IMMED   "con"       // constant (load only)

// OP_ALU modifiers
#define ALU_ADD     "+"         // addition
#define ALU_SUB     "-"         // subtract
//#define ALU_NEG     "-"         // negate
#define ALU_LT      "<"         // compare as: <
#define ALU_GT      ">"         //             >
#define ALU_LE      "<="        //             <=
#define ALU_GE      ">="        //             >=
#define ALU_EQ      "=="        //             ==
#define ALU_NE      "!="        //             !=
#define ALU_AND     "&"         // bitwise and
#define ALU_OR      "|"         // bitwise or
//#define ALU_NOT     "!"         // bitwise inverse

// map for equates -- used to fixup the quantity of local variables while generating a function prolog
typedef std::unordered_map<std::string, unsigned> EQU_MAP;

