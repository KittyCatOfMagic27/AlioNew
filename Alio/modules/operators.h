#ifndef OPERATORS_H
#define OPERATORS_H

#include <string>
#include <iostream> 
#include <list>
#include <cstring>
#include <vector>

namespace ops{

/*
* THIS IS A PROOF OF CONCEPT TO ADD TO THE ALIO COMPILER 
* This allows operator overloading, better type checking, more concise code, ect.
* This also gives each operator an 'expression_priority' to be used in expression trees
* It is also very much big pretty :3
* The only thing (partially) implemented here is matching the ops but it also outlines how these structs can be used to compile these ops
*      and provides information within the structs to parse them as well
*/

using namespace std;

vector<string> opSymbolsList;

enum SYNTAX_TYPE {
    SYN_NONE = 0,
    SYN_OP1 = 0x01,
    SYN_OP2 = 0x02,
    SYN_ANY_TYPES = 0x04,
    SYN_MATCHING_TYPES = 0x08,
    SYN_MISMATCHING_TYPES = 0x10,
    SYN_SPECIFIED_TYPES = 0x20, // in additionalInfo write "spectypessyn: <type1> <type2>"
    SYN_BRACKET = 0x40, // things like arr[i]
    SYN_DEREF = 0x80  // @ptr
};

enum TYPE_SPECIFIER {
    TS_NONE = 0,
    TS_I8 = 0x0001, TS_I16 = 0x0002, TS_I32 = 0x0004, TS_I64 = 0x0008, // signed ints 
    TS_U8 = 0x0010, TS_U16 = 0x0020, TS_U32 = 0x0040, TS_U64 = 0x0080, // unsigned ints
    TS_PTR = 0x0100, TS_SPECIFIED_STRUCT = 0x0200, // in additionalInfo write "specstruct<arg number>: <struct name>"
    TS_STRUCT = 0x0400, TS_STRING = 0x0800, TS_IRREGULAR_SIZE = 0x1000, // irregular as in not 8 16 32 or 64
    TS_SPECIFIED_IRREGULAR_SIZE = 0x2000, // in additionalInfo write "specirrsize<arg number>: i<bits>"
    TS_ANY = 0x4000, TS_ASSIGNMENT = 0x8000
};

enum OUT_TYPE_SPECIFIER{
    OT_NONE = 0,
    OT_BIGGER_TYPE = 0x01, OT_SMALLER_TYPE = 0x02, OT_TYPE_OF_PTR_TYPE = 0x04,
    OT_SPECIFIED_TYPE = 0x08, // in additionalInfo write "outType: <type1>"
    OT_FAVOR_U = 0x10, OT_FAVOR_I = 0x20,
    OT_REQUESTED = 0x40, OT_FIRST_ARG = 0x80, OT_SECOND_ARG = 0x0100,
};

enum ASM_TYPE{
    ASMT_OP1,
    ASMT_SIMPLE_OP2,
    ASMT_MAD_OP2, // Mult And Div (MAD) syntax in additionalCompInfo write "<reg1sig> <reg2sig>"   ex: "a d" for rax and rdx
    ASMT_PROCEDURE_CALL, // in additionalCompInfo write "<procedure name> <arg1> <arg2> <arg3>..."
    ASMT_INLINE_ASM, // in additionalCompInfo write the asm with <arg1> and <arg2> replacing said values
                        // Example:
                        /*
                        *   lea rax, <arg1>\n
                        *   mov <arg2>, rax\n
                        */
    ASMT_ASSIGNMENT, // instruction becomes the place written to ([rax] for example)
                        // and additionalCompInfo is any asm writen before assignment
};

struct OpSpec{
    unsigned char expression_priority = 0; // currently unused, will be used when building expression trees
    unsigned char specificity = 1; // if multiple operators match the expression, the one with the high specificity will win 
                                   // all operators must have a specificity greater than 0 
    // lexer and inter
    string symbol; // "+", "-", ect 
    SYNTAX_TYPE synType; // bit flags (can be |'d together)
    TYPE_SPECIFIER argType1; // bit flags
    TYPE_SPECIFIER argType2;
    OUT_TYPE_SPECIFIER outType;
    string additionalInfo;
    // compiler
    ASM_TYPE asmType;
    string instruction; // ex: "add", "div", "idiv"
    string additionalCompInfo;

    void print(){
        cout << "{\n"
        "unsigned char expression_priority = " << expression_priority << "\n"
        "unsigned char specificity = " << specificity << "\n"
        "\n"
        "string symbol = " << symbol << "\n"
        "SYNTAX_TYPE synType = " << synType << "\n"
        "TYPE_SPECIFIER argType1 = " << argType1 << "\n"
        "TYPE_SPECIFIER argType2 = " << argType2 << "\n"
        "OUT_TYPE_SPECIFIER outType = " << outType << "\n"
        "string additionalInfo = " << additionalInfo << "\n"
        "\n"
        "ASM_TYPE asmType = " << asmType << "\n"
        "string instruction = " << instruction << "\n"
        "string additionalCompInfo = " << additionalCompInfo << "\n"
        "}\n";
    }
};

void AddOperatorSymbolsToList(list<OpSpec> &oplist){
    for(OpSpec &op : oplist){
        if(find(opSymbolsList.begin(), opSymbolsList.end(), op.symbol) == opSymbolsList.end()) opSymbolsList.push_back(op.symbol);
    }
}

void InitOperators(list<OpSpec> &oplist){
    /* pausing dev on this bc it is annoying and very difficult
    OpSpec currentOp = (OpSpec){ //Assign
        0, 50,
        "=assign", SYN_ANY_TYPES | SYN_OP2, 
        TS_ANY,
        TS_ANY,
        OT_NONE, "",
        ASMT_SIMPLE_OP2,
        "mov", ""
    };
    oplist.push_back(currentOp);
    currentOp = (OpSpec){ //Int Add
        0, 2,
        "+", SYN_ANY_TYPES | SYN_OP2, 
        TS_I8 | TS_I16 | TS_I32 | TS_I64,
        TS_I8 | TS_I16 | TS_I32 | TS_I64,
        OT_BIGGER_TYPE | OT_FAVOR_I, "",
        ASMT_SIMPLE_OP2,
        "iadd", "" // i dont know if iadd is real, but it doesn't matter rn for prototyping
    };
    oplist.push_back(currentOp);
    currentOp = (OpSpec){ //Uint Add
        0, 1,
        "+", SYN_ANY_TYPES | SYN_OP2, 
        TS_U8 | TS_U16 | TS_U32 | TS_U64,
        TS_U8 | TS_U16 | TS_U32 | TS_U64,
        OT_BIGGER_TYPE | OT_FAVOR_U, "",
        ASMT_SIMPLE_OP2,
        "add", ""
    };
    oplist.push_back(currentOp);
    currentOp = (OpSpec){ //Int Sub
        0, 2,
        "-", SYN_ANY_TYPES | SYN_OP2, 
        TS_I8 | TS_I16 | TS_I32 | TS_I64,
        TS_I8 | TS_I16 | TS_I32 | TS_I64,
        OT_BIGGER_TYPE | OT_FAVOR_I, "",
        ASMT_SIMPLE_OP2,
        "isub", "" // i dont know if isub is real, but it doesn't matter rn for prototyping
    };
    oplist.push_back(currentOp);
    currentOp = (OpSpec){ //Uint Sub
        0, 1,
        "-", SYN_ANY_TYPES | SYN_OP2, 
        TS_U8 | TS_U16 | TS_U32 | TS_U64,
        TS_U8 | TS_U16 | TS_U32 | TS_U64,
        OT_BIGGER_TYPE | OT_FAVOR_U, "",
        ASMT_SIMPLE_OP2,
        "sub", ""
    };
    oplist.push_back(currentOp);


    currentOp = (OpSpec){ //Deref Assign
        0, 1,
        "@", SYN_DEREF | SYN_OP2, 
        TS_PTR,
        TS_ASSIGNMENT,
        OT_TYPE_OF_PTR_TYPE | OT_SECOND_ARG, "",
        ASMT_ASSIGNMENT,
        "[<tempreg3>]", "   mov <tempreg3>, <arg1>\n"
    };
    oplist.push_back(currentOp);

    currentOp = (OpSpec){ //Deref Get Value
        0, 1,
        "@", SYN_DEREF | SYN_OP1, 
        TS_PTR,
        TS_NONE,
        OT_TYPE_OF_PTR_TYPE | OT_REQUESTED , "",
        ASMT_INLINE_ASM,
        "", "   mov <tempreg1>, <arg1>\n    mov <tempreg2>, [<tempreg1>]"
    };
    oplist.push_back(currentOp);
    */


    AddOperatorSymbolsToList(oplist);
}

enum VALUE_TYPE{
    ERROR = 0, 
    UINT, INT,
    LONG, SHORT,
    PTR, BOOL,
    CHAR, STRUCT,
    STRING, PTR_OF_TYPE,
    VOID
};

struct VALUE{
    string value;
    VALUE_TYPE type;
};

int size_of(VALUE_TYPE t){
    switch(t){
        case UINT: case INT: return 4; break;
        case LONG: case PTR: return 8; break;
        case BOOL: case CHAR: return 1; break;
        case SHORT: return 2; break;
        default: return -1; break;
    }
}

bool isUnsigned(VALUE_TYPE t){
    switch(t){
        case INT: return false; break;
        default: return true; break;
    }
}

bool doesValueTypeQualify(OpSpec* op, VALUE_TYPE argType, unsigned char argNumber){
    if(argNumber == 1 && op->argType1 == TS_ANY) return true;
    else if(argNumber == 2 && op->argType2 == TS_ANY) return true;
    TYPE_SPECIFIER opArgType = argNumber == 1 ? op->argType1 : op->argType2;
    bool match = false;
    unsigned int mask = 0;
    switch(argType){
        case UINT: mask = TS_U32 & opArgType; break;
        case INT: mask = (TS_U32 | TS_I32) & opArgType; break;
        case LONG: mask = (TS_U64 | TS_I64) & opArgType; break;
        case SHORT: mask = (TS_U16 | TS_I16) & opArgType; break;
        case PTR: mask = (TS_U16 | TS_I16 | TS_PTR) & opArgType; break;
        case BOOL: case CHAR: mask = (TS_U8 | TS_I8) & opArgType; break;
        case STRING: mask = TS_STRING & opArgType; break;
        case STRUCT: mask = TS_STRUCT & opArgType; break;
        default:
            cerr << "ERROR: TYPE "<< argType <<" NOT IN FUNCTION 'doesValueTypeQualify' TO MATCH OPS TO IT.\n";
        break;
    }
    match = mask != 0;
    return match;
}

/**

OpSpec* opMatcher(string sym, VALUE arg1, VALUE arg2, list<OpSpec> &oplist){
    vector<OpSpec*> matchingOps;
    for(OpSpec &op : oplist){
        if(op.symbol == sym){
            if(doesValueTypeQualify(&op, arg1.type, 1) && doesValueTypeQualify(&op, arg2.type, 2)){
                bool isMatching = true;
                if(op.synType & SYN_MATCHING_TYPES) isMatching = arg1.type == arg2.type;
                else if(op.synType & SYN_MISMATCHING_TYPES) isMatching = arg1.type != arg2.type;

                matchingOps.push_back(&op);
            }
        }
    }

    unsigned char topSpecificity = 0;
    OpSpec* chosenOp = nullptr;
    for(OpSpec* op : matchingOps){
        if(op->specificity > topSpecificity){
            chosenOp = op;
            topSpecificity = op->specificity;
        }
    }

    return chosenOp;
}

*/

VALUE_TYPE outTypeOfExpression(OpSpec* op, VALUE_TYPE x, VALUE_TYPE y){ 
    #define outTypeOfExpressionFAVOR \
    if(size_of(x) == size_of(y)){ if(op->outType & OT_FAVOR_U) {returnType = isUnsigned(x) ? x : y;} else if(op->outType & OT_FAVOR_I)  {returnType = isUnsigned(x) ? y : x;}}

    // note the current impl is not great and that it does not account for different types of the same size
    // this could allow a iadd to return a uint for example
    // they could be made bit flags and allow for things like "OT_FAVOR_U", "OT_FAVOR_I", ect.

    int favor_mask = (OT_FAVOR_U | OT_FAVOR_I) ^ 0xFF;
    VALUE_TYPE returnType = ERROR;
    switch(op->outType & favor_mask){
        case OT_BIGGER_TYPE: returnType = size_of(x) >= size_of(y) ? x : y; outTypeOfExpressionFAVOR break;
        case OT_SMALLER_TYPE: returnType = size_of(x) <= size_of(y) ? x : y; outTypeOfExpressionFAVOR break;
        case 0: returnType = VOID; break;
        //case OT_TYPE_OF_PTR_TYPE:
        //case OT_SPECIFIED_TYPE:
        default:
        cerr << "ERROR: Using unimplemented outType protocol for an op that has the symbol " << op->symbol << "\n";
        exit(-1);
        break;
    }
    return returnType;
}

}

#endif