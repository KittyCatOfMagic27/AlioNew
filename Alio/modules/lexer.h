#ifndef LEXER_H
#define LEXER_H

#include <vector>
#include <fstream>
#include <string>
#include <cstdio>
#include <cassert>
#include <iostream>
#include <stdbool.h>
#include <algorithm>
#include <unordered_map>
#include <unistd.h>
#include <limits.h>
#include <utility>
#include <random>

#include "operators.h"

using namespace std;

namespace lex{
  //ENUMS AND STUCTS
  enum ENUM_TYPE{
    //Error
    ERROR = 0,
    VALUE,
    //Types
    UINT,
    INT,
    LONG,
    SHORT,
    PTR,
    BOOL,
    CHAR,
    STRUCT,
    STRING,
    PTR_OF_TYPE,
    VOID,
    //Ops
    ADD,
    SUB,
    MULT,
    DIV,
    MOD,
    INC,
    DEC,
    GCMP,
    LCMP,
    LECMP,
    GECMP,
    ECMP,
    NECMP,
    AND,
    BIT_AND,
    BIT_OR,
    NOT,
    XOR,
    SHL,
    SHR,
    DEREF,
    CALL,
    OFNAME, //::
    AMPERSAND,
    ARGLEA,
    SUBSET,
    //Keywords
    PROC,
    END,
    BEGIN,
    IN,
    OUT,
    MSTRUCT,
    PUSH,
    POP,
    SYSCALL,
    STATIC,
    PERSIST,
    EXTERN,
    GLOBAL,
    WHILE,
    IF,
    ELSE,
    //Symbols
    OPARA,
    CPARA,
    OSQRB,
    CSQRB,
    SEMICOLON,
    DOT,
    DEREF_FIELD,
    //Define
    DEFU, // UINT
    DEFI, // INT
    DEFL, // LONG
    DEFP, // PTR
    DEFB, // BOOL
    DEFC, // CHAR
    DEFS, // STRING
    DEFSHORT, //SHORT
    DEFSTRUCT, //STRUCT
    DEFP_OF_TYPE, // PTR (TYPE)
    //Temp
    TEMP,
    OPERATOR,
    SPECIAL_ASSIGN_FOR_OPS_PROCESSING
  };
  struct Token{
    ENUM_TYPE type;
    string value;
    unsigned int line;
    void* ptr = nullptr;
    ops::VALUE_TYPE getValueType(){
      return ((ops::VALUE_TYPE)type) - (ops::VALUE_TYPE)1;
    }
    Token(){}
    Token(ops::OpSpec* op){
      ptr = (void*) op;
      type = ENUM_TYPE::OPERATOR;
      value = op->symbol;
    }
  };
  struct Structure{
    string label;
    int size=0;
    vector<Token> fields;
  };
  
  //CONSTS
  string FILENAME;
  static const string WHITESPACE = " \n\r\t\f\v";
  static vector<string> TYPE_NAMES({
    //Error
    "ERROR",
    "VALUE",
    //Types
    "UINT",
    "INT",
    "LONG",
    "SHORT",
    "PTR",
    "BOOL",
    "CHAR",
    "STRUCT",
    "STRING",
    "PTR_OF_TYPE",
    "VOID",
    //Ops
    "ADD",
    "SUB",
    "MULT",
    "DIV",
    "MOD",
    "INC",
    "DEC",
    "GCMP",
    "LCMP",
    "LECMP",
    "GECMP",
    "ECMP",
    "NECMP",
    "AND",
    "BIT_AND",
    "BIT_OR",
    "NOT",
    "XOR",
    "SHL",
    "SHR",
    "DEREF",
    "CALL",
    "OFNAME",
    "AMPERSAND",
    "ARGLEA",
    "SUBSET",
    //Keywords
    "PROC",
    "END",
    "BEGIN",
    "IN",
    "OUT",
    "MSTRUCT",
    "PUSH",
    "POP",
    "SYSCALL",
    "STATIC",
    "PERSIST",
    "EXTERN",
    "GLOBAL",
    "WHILE",
    "IF",
    "ELSE",
    //Symbols
    "OPARA",
    "CPARA",
    "OSQRB",
    "CSQRB",
    "SEMICOLON",
    "DOT",
    "DEREF_FIELD",
    //Define
    "DEFU", // UINT
    "DEFI", // INT
    "DEFL", // LONG
    "DEFP", // PTR
    "DEFB", // BOOL
    "DEFC", // CHAR
    "DEFS", // STRING
    "DEFSHORT", //SHORT
    "DEFSTRUCT",//STRUCT
    "DEFP_OF_TYPE", // PTR (TYPE)
    //Temp
    "TEMP",
    "OPERATOR",
    "SPECIAL_ASSIGN_FOR_OPS_PROCESSING"
  });
  vector<string> PTR_LIST;
  unordered_map<string, Structure> STRUCTS;
  vector<string> EXTERNAL_PROCS;
  errh::FileTree Files;
  
  //UTIL
  void print_token(Token tk){
    cout << "{\n  Value: " << tk.value << ";\n  Line:  " << tk.line << ";\n  Type:  " << TYPE_NAMES[tk.type] << "\n}\n";
  }
  bool isNumber(const string& str){
    for (char const &c : str) {
      if (isdigit(c) == 0) return false;
    }
    return true;
  }
  ENUM_TYPE literal_or_var(string &val){
    if(val=="true" || val=="false")return BOOL;
    if(isNumber(val))return UINT;
    if(val.size()>=2){if(val[0] == '0' && val[1]== 'x') return UINT;}
    if(val[0]=='"'&&val[val.size()-1]=='"')
      if(val.size()<=3) return CHAR;
      else return STRING;
    return ERROR;
  }
  int lsize_of(ENUM_TYPE type){
    if(type == UINT||type == INT) return 4;
    if(type == LONG) return 8;
    if(type == SHORT) return 2;
    if(type == PTR){
      if(options::target==options::X86_I386) return 4;
      return 8;
    }
    if(type == BOOL||type == CHAR) return 1;
    return -1;
  }
  string ltrim(const string &s){
    size_t start = s.find_first_not_of(WHITESPACE);
    return (start == string::npos) ? "" : s.substr(start);
  }
  string rtrim(const string &s){
    size_t end = s.find_first_of(WHITESPACE);
    return (end == string::npos) ? s : s.substr(0, end);
  }
  string convertToString(char a){
    string s = "";
    return s + a;
  }
  vector<string> split(string &x, vector<string> &SYMBOLS, char delim = ' '){
    x = ltrim(x);
    x += delim;
    vector<string> splitted;
    string temp = "";
    bool in_string = false;
    for (int i = 0; i < x.length(); i++){
      if (x[i] == delim){
        splitted.push_back(rtrim(temp));
        temp = "";
        i++;
      }
      char y;
      while(true){
        y = x[i];
        if(x[i]=='"'){
          if(temp != "") splitted.push_back(temp);
          temp = "";
          temp += x[i];
          i++;
          while(true){
            if(i>=x.length()){
              cerr << "CAN'T FIND END OF STRING.\n";
              assert(false);
            }
            temp+=x[i];
            if(x[i]=='"'){
              i++;
              break;
            }
            i++;
          }
          splitted.push_back(temp);
          temp = "";
        }
        else if(find(SYMBOLS.begin(), SYMBOLS.end(), convertToString(y)+x[i+1]) != SYMBOLS.end()){
          if(temp!="")splitted.push_back(temp);
          temp = "";
          temp += y;
          temp += x[i+1];
          splitted.push_back(temp);
          temp = "";
          i+=2;
        }
        else if(find(SYMBOLS.begin(), SYMBOLS.end(), convertToString(y)) != SYMBOLS.end()){
          if(temp!="")splitted.push_back(temp);
          temp = "";
          temp += x[i];
          splitted.push_back(temp);
          temp = "";
          i++;
        }
        else{
          break;
        }
      }
      y = x[i];
      if(y=='#'){
        if(temp != "") splitted.push_back(temp);
        return splitted;
      }
      if(y!=' '||in_string) temp += x[i];
    }
    return splitted;
  }
  
  bool isOp(ENUM_TYPE type){
    return type>=ADD&&type<=AMPERSAND&&type!=CALL;
  }
  bool isType(ENUM_TYPE type){
    return type>=UINT&&type<=VOID;
  }
  bool isDef(ENUM_TYPE type){
    return type>=DEFU&&type<=DEFSTRUCT;
  }
  bool isType(string type){
    return type=="uint"||type=="long"||type=="int"||type=="bool"||type=="char"||type=="ptr"||type=="string"||type=="short"||type=="struct";
  }

  ENUM_TYPE defToType(ENUM_TYPE def){
    ENUM_TYPE type;
    switch(def){
      case DEFU: type = UINT;   break;
      case DEFI: type = INT;    break;
      case DEFL: type = LONG;   break;
      case DEFP: type = PTR;    break;
      case DEFB: type = BOOL;   break;
      case DEFC: type = CHAR;   break;
      case DEFS: type = STRING; break;
      case DEFSHORT: type = SHORT; break;
      case DEFSTRUCT: type = STRUCT; break;
      case DEFP_OF_TYPE: type = PTR_OF_TYPE; break;
    }
    return type;
  }

  ops::VALUE_TYPE getValueType(ENUM_TYPE type){
    return isDef(type) ?(ops::VALUE_TYPE) defToType(type) - (ops::VALUE_TYPE)1 : ((ops::VALUE_TYPE)type) - (ops::VALUE_TYPE)1;
  }

  ENUM_TYPE getEnumType(ops::VALUE_TYPE type){
    return ((ENUM_TYPE)type) + (ENUM_TYPE)1;
  }
  
  //LEXER
  class LEXER{
    public:
    
    vector <Token> Tokens;
    vector <string> Lines;
    unordered_map<string, string> VAR_STRUCT_TYPES;
    list<ops::OpSpec> *oplist;
    
    LEXER(const char* fd, list<ops::OpSpec> *_oplist): oplist(_oplist){
      FILENAME=fd;
      {
        Files.init(FILENAME, INT_MAX);
      }
      string line;
      ifstream source_code;
      source_code.open(fd);
      if(source_code.is_open()){
        while(getline(source_code,line)){
          Lines.push_back(line);
        }
        source_code.close();
      }
      else {
        fprintf(stderr, "\033[1;31mUnable to open file.\033[0m\n");
        assert(false);
      }
    }
    vector <Token> run(){
      {
        vector<Token> tks = tokenize();
        parse(tks);
      }
      pregrammer_check();
      grammer_check();
      postprocess();
      return Tokens;
    }
    
    static ENUM_TYPE defToType(ENUM_TYPE def){
      ENUM_TYPE type;
      switch(def){
        case DEFU: type = UINT;   break;
        case DEFI: type = INT;    break;
        case DEFL: type = LONG;   break;
        case DEFP: type = PTR;    break;
        case DEFB: type = BOOL;   break;
        case DEFC: type = CHAR;   break;
        case DEFS: type = STRING; break;
        case DEFSHORT: type = SHORT; break;
        case DEFSTRUCT: type = STRUCT; break;
        case DEFP_OF_TYPE: type = PTR_OF_TYPE; break;
      }
      return type;
    }

    private:

    string defToWord(ENUM_TYPE def){
      string type;
      switch(def){
        case DEFU: type = "uint";   break;
        case DEFI: type = "int";    break;
        case DEFL: type = "long";   break;
        case DEFP: type = "ptr";    break;
        case DEFB: type = "bool";   break;
        case DEFC: type = "char";   break;
        case DEFS: type = "string"; break;
        case DEFSHORT: type = "short"; break;
        case DEFSTRUCT: type = "struct"; break;
      }
      return type;
    }
    string typeToWord(ENUM_TYPE type){
      string word;
      switch(type){
        case UINT: word = "uint";     break;
        case INT: word = "int";       break;
        case LONG: word = "long";     break;
        case PTR: word = "ptr";       break;
        case BOOL: word = "bool";     break;
        case CHAR: word = "char";     break;
        case STRING: word = "string"; break;
        case SHORT: word = "short";   break;
        case STRUCT: word = "struct"; break;
      }
      return word;
    }
    int opInAmount(ENUM_TYPE op){
      int x;
      switch(op){
        case ADD:
        case SUB:
        case MULT:
        case DIV:
        case MOD:
        case GCMP:
        case LCMP:
        case LECMP:
        case GECMP:
        case ECMP:
        case NECMP:
        case AND:
        case BIT_AND:
        case BIT_OR:
        case SHL:
        case SHR:
        case XOR:
        x = 2;
        break;
        case INC:
        case DEC:
        case DEREF:
        case AMPERSAND:
        x = 1;
        break;
      }
      return x;
    }
    
    vector<Token> tokenize(){
      unordered_map<string, ENUM_TYPE> KEYWORD_MAP({
        {"end",     END},
        {"in",      IN},
        {"out",     OUT},
        {"begin",   BEGIN},
        {"static",  STATIC},
        {"persist", PERSIST},
        {"extern",  EXTERN},
        {"global",  GLOBAL},
        {"push",    PUSH},
        {"pop",     POP},
        {"syscall", SYSCALL},
        {"while",   WHILE},
        {"if",      IF},
        {"else",    ELSE},
        {"subset",  SUBSET},
        {"(",       OPARA},
        {")",       CPARA},
        {"[",       OSQRB},
        {"]",       CSQRB},
        {"+",       ADD},
        {"-",       SUB},
        {"*",       MULT},
        {"/",       DIV},
        {"%",       MOD},
        {"&",       AMPERSAND},
        {"@",       DEREF},
        {"++",      INC},
        {"--",      DEC},
        {">>",      SHR},
        {"<<",      SHL},
        {";",       SEMICOLON},
        {"<",       LCMP},
        {">",       GCMP},
        {"<=",      LECMP},
        {">=",      GECMP},
        {"=",       ECMP},
        {"!=",      NECMP},
        {"&&",      AND},
        {"-&",      BIT_AND},
        {"|",       BIT_OR},
        {"!",       NOT},
        {"^",       XOR},
        {"uint",    DEFU},
        {"long",    DEFL},
        {"int",     DEFI},
        {"bool",    DEFB},
        {"char",    DEFC},
        {"ptr",     DEFP},
        {"string",  DEFS},
        {"short",   DEFSHORT},
        {"struct",  MSTRUCT},
        {".",       DOT},
        {"proc",    PROC},
        {"::",      OFNAME},
        {"->",      DEREF_FIELD}
      });
      if(options::STAGETEXT) printf("Tokenizing...\n");
      vector<string> SYM_TABLE({"+","-","/","*","%","&","!",";;","(",")",";","[","]","@","++","--","<",">","<=",">=","=","!=","&&","<<",">>","-&","|",".","::","->","^"});
      vector<string> LIBDEFS;
      if(options::LIBC)
        LIBDEFS.push_back("LIBC");
      else if(options::OSTYPE==options::WIN32)
        LIBDEFS.push_back("OS_WIN32");
      else if(options::OSTYPE==options::LINUX_DEBIAN)
        LIBDEFS.push_back("OS_LINUX_DEBIAN");
      
      unordered_map<string, string> CONSTS;
      vector<Token> tks;
      for(int LINE_ITER = 0; LINE_ITER < Lines.size(); LINE_ITER++){
        string line = Lines[LINE_ITER];
        if(line==""||line==" "||line=="\n"||line=="\t") continue;
        int LINE_NUMBER = LINE_ITER;

        //cout << LINE_NUMBER << ": "<<line<<"\n";

        vector<string> split_line = split(line, SYM_TABLE);
        bool semicolonTR = true;
        for(int i = 0; i < split_line.size(); i++){
          string word=split_line[i];
          Token result;
          result.line=LINE_NUMBER;
          if(word==""||word==" "||word=="\n"||word=="\t"){
            continue;
          } //temp fix, idfk wth
          else if(word=="__DEBUG_READ_DURING_TOKENIZATION"){
            cout << "__DEBUG_READ_DURING_TOKENIZATION has been hit on line " << LINE_NUMBER << "\n"; 
          }
          else if(word=="__COMPILER_RQ"){
            if(split_line[++i] == "RAND_NUMBER"){
              // no docu cannot find ;-;
              if(split_line[++i] == "LONG"){ 
                //result.value = to_string(rand(UINT64_MAX));
              }
              else if(split_line[i] == "UINT" || split_line[i] == "INT"){
                result.value = to_string((unsigned long int)time);
              }
              result.type  = literal_or_var(result.value);
              tks.push_back(result);
            }
          }
          else if(literal_or_var(word)!=ERROR){
            result.value = word;
            result.type  = literal_or_var(word);
            tks.push_back(result);
          }
          else if(find(ops::opSymbolsList.begin(), ops::opSymbolsList.end(), word)!=ops::opSymbolsList.end()){
            result.value = word; 
            result.type = OPERATOR;
            tks.push_back(result);
          }
          else if(KEYWORD_MAP.find(word)!=KEYWORD_MAP.end()){
            result.type  = KEYWORD_MAP[word];
            tks.push_back(result);
          }
          else if(CONSTS.find(word)!=CONSTS.end()){
            result.value = CONSTS[word];
            result.type  = literal_or_var(CONSTS[word]);
            tks.push_back(result);
          }
          else if(word=="self"){
            result.type  = PTR;
            result.value = "self";
            tks.push_back(result);
          }
          else if(word==";;"){
            #define STOPIF \
              { \
                int scopes = 1; \
                while(++LINE_ITER){ \
                  split_line = split(Lines[LINE_ITER], SYM_TABLE); \
                  if(split_line.size()>1){ \
                    if(split_line[0]==";;"&&split_line[1]=="ifdef") scopes++; \
                    if(split_line[0]==";;"&&split_line[1]=="fi") scopes--; \
                    if(scopes == 0) break;\
                  } \
                } \
              }
            semicolonTR = false;
            if(split_line[++i]=="include"){
              if(split_line[++i]=="static"){
                string filename;
                string fd;
                int j;
                if(split_line[++i]=="<"){
                  char result[ PATH_MAX ];
                  size_t count = readlink( "/proc/self/exe", result, PATH_MAX );
                  j = count;
                  string path(result);
                  while(j > 0) if(path[--j]=='/') break;
                  path.erase(j+1);
                  while(split_line[++i]!=">") filename+=split_line[i];
                  filename+=".alio";
                  fd = path+"include/"+filename;
                }
                else if(literal_or_var(split_line[i])==STRING){
                  fd = split_line[i];
                  fd.erase(0, 1);
                  fd.pop_back();
                  filename = fd;
                }
                else{
                  cerr << "\033[1;31m"<<FILENAME<<":"<<LINE_NUMBER<<":Unknown include file input '"<<split_line[i]<<"'.\033[0m\n";
                  assert(false);
                }
                string line;
                ifstream source_code;
                source_code.open(fd.c_str());
                j = LINE_ITER+1;
                int begin=j;
                int end;
                if(source_code.is_open()){
                  while(getline(source_code,line)){
                    Lines.insert(Lines.begin()+j, line);
                    j++;
                  }
                  source_code.close();
                  end = j;
                }
                else {
                  cerr << "\033[1;31mUnable to open file '"<<filename<<"'.\033[0m\n";
                  assert(false);
                }
                Files.add_file(filename, begin, end-begin);
                LINE_ITER++;
                break;
              }
              else{
                cerr << "\033[1;31m"<<FILENAME<<":"<<LINE_NUMBER<<":Unknown include type of '"<<split_line[i]<<"'.\033[0m\n";
                assert(false);
              }
            }
            else if(split_line[i]=="define"){
              CONSTS[split_line[i+1]]=split_line[i+2];
              i+=2;
            }
            else if(split_line[i]=="def"){
              LIBDEFS.push_back(split_line[++i]);
            }
            else if(split_line[i]=="ifndef"){
              if(find(LIBDEFS.begin(), LIBDEFS.end(), split_line[++i])!=LIBDEFS.end()){
                STOPIF;
              }
            }
            else if(split_line[i]=="ifdef"){
              if(find(LIBDEFS.begin(), LIBDEFS.end(), split_line[++i])==LIBDEFS.end()){
                STOPIF;
              }
            }
            else if(split_line[i]=="setentry"){
              options::ENTRYPOINT = split_line[++i];
            }
            else if(split_line[i]=="win32kernproc"){
              EXTERNAL_PROCS.push_back(split_line[++i]);
            }
            else if(split_line[i]=="externproc"){
              EXTERNAL_PROCS.push_back(split_line[++i]);
            }
          }
          else{
            result.value = word;
            result.type  = VALUE;
            tks.push_back(result);
          }
        }
        Token semicolon;
        semicolon.type = SEMICOLON;
        semicolon.line = LINE_NUMBER;
        if(split_line.size()>0&&semicolonTR&&tks[tks.size()-1].type!=SEMICOLON) tks.push_back(semicolon);
      }
      if(options::STAGETEXT) printf("Done Tokenizing!\n");
      if(options::DEBUGMODE){
        for(lex::Token &tk : tks){
          cout << lex::TYPE_NAMES[tk.type] << " ";
          if(tk.value!=""){
            cout << "{" << tk.value << "} ";
          }
          if(tk.type==lex::SEMICOLON){
            // cout << "\n" << formatErrorInfo(Files.get_ErrorInfo(tk.line)) << "   ";
            cout << "\n";
          }
        }
        cout << "\n";
      }
      return tks;
    }

    void parse(vector<Token> tks){
      if(options::STAGETEXT) printf("Parsing...\n");
      struct PARSE_VAR{
        Token tk;
        string info = "";
        PARSE_VAR(Token _tk) {tk = _tk;}
        PARSE_VAR(){}
      };
      struct PARSE_PROC{
        string sym;
        unordered_map<string, PARSE_VAR> vars;
        PARSE_PROC(string _sym): sym(_sym){}
        PARSE_PROC(){}
      };
      struct PARSE_FUNCTIONS{
        static inline bool doesProcExist(string sym, vector<PARSE_PROC> PROCS){
          for(PARSE_PROC &proc : PROCS) if(proc.sym == sym) return true;
          return false;
        }
      };
      vector<PARSE_PROC> PROCS;
      PROCS.push_back(PARSE_PROC("__asm"));
      if(EXTERNAL_PROCS.size()!=0){
        PROCS.insert(PROCS.end(), EXTERNAL_PROCS.begin(), EXTERNAL_PROCS.end());
      }

      for(int i = 0; i < tks.size(); i++){
        Token tk = tks[i];
        Token result;
        result.line = tk.line;
        if(tk.type==PROC){
          result.type  = PROC;
          if(tks[i+2].type==OFNAME){
            result.value = tks[i+3].value;
            if(STRUCTS.find(tks[i+1].value)!=STRUCTS.end()){
              STRUCTS[tks[i+1].value].fields.push_back(result);
              result.value = tks[i+1].value+"@"+tks[i+3].value;
            }
            else if(isDef(tks[i+1].type)){
              result.value = defToWord(tks[i+1].type)+"@"+tks[i+3].value;
            }
            i+=4;
            Tokens.push_back(result);
            Token t;
            t.type = SEMICOLON;
            Tokens.push_back(t);
            t.type = IN;
            Tokens.push_back(t);
            t.type = DEFP;
            t.value = "self";
            Tokens.push_back(t);
            t.type = SEMICOLON;
            t.value = "";
            Tokens.push_back(t);
          }
          else{
            result.value = tks[++i].value;
            PROCS.push_back(PARSE_PROC(result.value));
            Tokens.push_back(result);
          }
          PROCS.push_back(PARSE_PROC(tks[i].value));
        }
        else if(tk.type==MSTRUCT){
          result.value = tks[++i].value;
          result.type  = MSTRUCT;
          Tokens.push_back(result);
          Structure s;
          s.label      = tks[i].value;
          STRUCTS[tks[i].value] = s;
        }
        else if(tk.type==lex::ENUM_TYPE::DEFP){
          result.type = tk.type;
          if (tks[++i].type == OPARA){
            result.type = DEFP_OF_TYPE;
            result.value = tks[++i].value;
            i++;
            Tokens.push_back(result);
            result.type  = defToType(result.type);
            result.value = tks[++i].value;
            PROCS.back().vars[result.value] = PARSE_VAR(result);
            PROCS.back().vars[result.value].info = tks[i-2].value;
            Tokens.push_back(result);
          }
          else{
            result.value = tks[i].value;
            result.type  = tk.type;
            Tokens.push_back(result);
            result.type  = defToType(tk.type);
            PROCS.back().vars[result.value] = PARSE_VAR(result);
          }
        }
        else if(isDef(tk.type)){
          result.value = tks[++i].value;
          result.type  = tk.type;
          Tokens.push_back(result);
          result.type  = defToType(tk.type);
          PROCS.back().vars[result.value] = PARSE_VAR(result);
        }
        else if(tk.type==DOT){
          result.value = tks[++i].value;
          result.type  = DOT;
          Tokens.push_back(result);
        }
        else if(tk.type==DEREF_FIELD){
          result.value = tks[++i].value;
          result.type  = DEREF_FIELD;
          Tokens.push_back(result);
        }
        else if(tk.type==OFNAME){
          result.value = Tokens[Tokens.size()-1].value;
          Tokens.pop_back();
          result.type  = OFNAME;
          Tokens.push_back(result);
        }
        else if(isType(tk.type)||tk.type==SEMICOLON||isOp(tk.type)||(tk.type>=END&&tk.type<=SEMICOLON)||(tk.type==OPERATOR)){
          Tokens.push_back(tk);
        }
        else if(tk.type==OPARA&&tks[i+2].type==CPARA&&isType(defToType(tks[i+1].type))){
          result.value = tks[i+3].value;
          result.type  = defToType(tks[i+1].type);
          Tokens.push_back(result);
          i+=3;
        }
        else if(PROCS.back().vars.find(tk.value)!=PROCS.back().vars.end()){
          Tokens.push_back(PROCS.back().vars[tk.value].tk);
          if(PROCS.back().vars[tk.value].info != ""){
            result.value = PROCS.back().vars[tk.value].info;
            result.type  = VALUE;
            Tokens.push_back(result);
          }
        }
        else if(PARSE_FUNCTIONS::doesProcExist(tk.value, PROCS)){
          result.value = tk.value;
          result.type  = CALL;
          Tokens.push_back(result);
        }
        else if(STRUCTS.find(tk.value)!=STRUCTS.end()){
          result.value = tk.value;
          result.type  = DEFSTRUCT;
          Tokens.push_back(result);
          result.value = tks[++i].value;
          result.type  = STRUCT;
          Tokens.push_back(result);
          PROCS.back().vars[result.value] = PARSE_VAR(result);
          VAR_STRUCT_TYPES[result.value] = tk.value;
        }
      }
      if(options::DEBUGMODE){
        for(lex::Token &tk : Tokens){
          cout << lex::TYPE_NAMES[tk.type] << " ";
          if(tk.value!=""){
            cout << "{" << tk.value << "} ";
          }
          if(tk.type==lex::SEMICOLON){
            // cout << "\n" << formatErrorInfo(Files.get_ErrorInfo(tk.line)) << "   ";
            cout << "\n";
          }
        }
        cout << "\n";
      }
      if(options::STAGETEXT) printf("Done Parsing!\n");
    }
    void pregrammer_check(){
      for(int i = 0; i < Tokens.size(); i++){
        Token tk = Tokens[i];
        switch(tk.type){
          case SEMICOLON:
            if(Tokens[i+1].type==SEMICOLON){
              Tokens.erase(Tokens.begin()+i);
            }
          break;
          case MSTRUCT:{
            int begin = i;
            string label = Tokens[i].value;
            Structure S  = STRUCTS[label];
            if(Tokens[i+1].type==SEMICOLON)i++;
            if(Tokens[++i].type!=BEGIN){
              startErrorThrow(tk);
              cerr << "Expected 'begin' in declartion of the struct '"<<label<<"'.";
              endErrorThrow();
              assert(false);
            }
            while(Tokens[++i].type!=END){
              if(isDef(Tokens[i].type)){
                S.fields.push_back(Tokens[i]);
                if(Tokens[i].type!=DEFS){
                  uint csize = lsize_of(defToType(Tokens[i].type));
                  if(csize==-1){
                    startErrorThrow(Tokens[i]);
                    cerr <<"Uknown size of field name '"<<Tokens[i].value<<"' in declartion of the struct '"<<label<<"'.";
                    endErrorThrow();
                    assert(false);
                  }
                  S.size+=csize;
                }
                else{
                  if(Tokens[i+1].type!=OSQRB||Tokens[i+3].type!=CSQRB){
                    startErrorThrow(Tokens[i]);
                    cerr << "Uknown size of field name '"<<Tokens[i].value<<"' in declartion of the struct '"<<label<<"'.";
                    endErrorThrow();
                    assert(false);
                  }
                  uint csize = stoi(Tokens[i+2].value);
                  S.size+=csize;
                  S.fields[S.fields.size()-1].value+=" "+Tokens[i+2].value;
                  i+=3;
                }
              }
            }
            STRUCTS[label] = S;
            i+=2;
            Tokens.erase(Tokens.begin()+begin, Tokens.begin()+i);
            i = begin-1;
          }
          break;
          case DEREF_FIELD:{
            if(Tokens[i-2].type!=lex::ENUM_TYPE::PTR_OF_TYPE){
              startErrorThrow(tk);
              cerr << "Trying to dereference field '"<<tk.value<<"' when '"<<Tokens[i-2].value<<"' is not a typed pointer.";
              endErrorThrow();
              assert(false);
            }

          }
          break;
          case DOT:{
            if(VAR_STRUCT_TYPES.find(Tokens[i-1].value)==VAR_STRUCT_TYPES.end()){
              if(!isType(Tokens[i-1].type)){
                startErrorThrow(tk);
                cerr << "Trying to access invalid method of '"<<tk.value<<"' on an instance of '"<<TYPE_NAMES[Tokens[i-1].type]<<"' called '"<<Tokens[i-1].value<<"'.";
                endErrorThrow();
                assert(false);
              }
              string s = Tokens[i-1].value;
              Tokens[i-1].value=typeToWord(Tokens[i-1].type)+"@"+tk.value;
              Tokens[i-1].type=CALL;
              Tokens.erase(Tokens.begin()+i);
              i--;
              Token ptr;
              ptr.type = ARGLEA;
              ptr.value = s;
              Tokens.insert(Tokens.begin()+i+2, ptr);
              break;
            }
            Structure S = STRUCTS[VAR_STRUCT_TYPES[Tokens[i-1].value]];
            Token field;
            field.type = ERROR;
            for(Token t : S.fields){
              if(t.value[t.value.size()-1]>='0'&&t.value[t.value.size()-1]<='9'){
                const char* begin = t.value.c_str();
                const char* str = begin;
                while(*str!=' '&&*str!=0)str++;
                if(*str==0){ if(t.value==tk.value) field = t; }
                else if(t.value.substr(0, (str-begin))==tk.value) field = t;
              }
              else if(t.value==tk.value) field = t;
            }
            if(field.type==ERROR){
              startErrorThrow(tk);
              cerr << "Trying to access invalid field of '"<<tk.value<<"' on an instance of '"<<S.label<<"' called '"<<Tokens[i-1].value<<"'.";
              endErrorThrow();
              assert(false);
            }
            if(field.type==PROC){
              string s = Tokens[i-1].value;
              Tokens[i-1].type=CALL;
              Tokens[i-1].value=S.label+"@"+field.value;
              Tokens.erase(Tokens.begin()+i);
              i--;
              Token ptr;
              ptr.type = ARGLEA;
              ptr.value = s;
              Tokens.insert(Tokens.begin()+i+2, ptr);
            }
            else{
              Tokens[i-1].type=defToType(field.type);
              if(Tokens[i-1].type==STRING){
                string field_name;
                const char* begin = field.value.c_str();
                const char* str = begin;
                while(*str!=' ')str++;
                Tokens[i-1].value+="."+field.value.substr(0, (str-begin));
              }
              else{
                Tokens[i-1].value+="."+field.value;
              }
              Tokens.erase(Tokens.begin()+i);
              i--;
            }
          }
          break;
          case DEFSTRUCT:{
            int begin = i;
            Structure S = STRUCTS[tk.value];
            string parent = Tokens[++i].value;
            int offset = 0;
            vector<Token> Subsets;
            for(Token &t : S.fields){
              Token tk1;
              tk1.line = tk.line;
              string field_name;
              int csize;
              if(t.type==PROC) continue;
              ENUM_TYPE type=defToType(t.type);
              if(type==STRING){
                const char* begin = t.value.c_str();
                const char* str = begin;
                while(*(str)!=' ')str++;
                field_name = parent+"."+t.value.substr(0, (str-begin));
                str++;
                csize = stoi(t.value.substr((str-begin), t.value.size()));
              }
              else{
                field_name = parent+"."+t.value;
                csize = lsize_of(type);
              }
              tk1.type = SUBSET;
              Subsets.push_back(tk1);
              tk1.type = UINT;
              tk1.value = to_string(offset);
              offset+=csize;
              Subsets.push_back(tk1);
              tk1.value = to_string(csize);
              tk1.type = UINT;
              Subsets.push_back(tk1);
              tk1.value = field_name;
              tk1.type = STRUCT;
              Subsets.push_back(tk1);
              tk1.value = parent;
              tk1.type = STRUCT;
              Subsets.push_back(tk1);
              tk1.value = "";
              tk1.type = SEMICOLON;
              Subsets.push_back(tk1);
            }
            i+=2;
            Tokens.insert(Tokens.begin()+i, Subsets.begin(), Subsets.end());
            i+=Subsets.size();
          }
          break;
          default: break;
        }
      }
    }
    void grammer_check(){
      if(options::STAGETEXT) printf("Grammer checking...\n");
      int block_tracker = 0;
      vector<string> BLOCK_STACK;
      Token prev;
      bool IN_BEGIN = false;
      // for(Token &tk : Tokens) print_token(tk);
      for(int i = 0; i < Tokens.size(); i++){
        Token tk = Tokens[i];
        switch(tk.type){
          case PROC:
            block_tracker++;
            BLOCK_STACK.push_back(tk.value);
            IN_BEGIN = true;
          break;
          case WHILE:
            block_tracker++;
            BLOCK_STACK.push_back("while");
          break;
          case IF:
            block_tracker++;
            BLOCK_STACK.push_back("if");
          break;
          case ELSE:
            if(BLOCK_STACK[BLOCK_STACK.size()-1]!="if"&&BLOCK_STACK[BLOCK_STACK.size()-1]!="else if"){
              startErrorThrow(tk);
              cerr << "An else statement following a non-if block, instead follows a '"<<BLOCK_STACK[BLOCK_STACK.size()-1]<<"' block.";
              endErrorThrow();
              assert(false);
            }
            if(Tokens[i+1].type==IF){
              BLOCK_STACK[BLOCK_STACK.size()-1]="else if";
              i++;
            }
            else{
              BLOCK_STACK[BLOCK_STACK.size()-1]="else";
            }
          break;
          case END:
            block_tracker--;
            if(block_tracker<0){
              startErrorThrow(tk);
              cerr << "Unnessasary 'end'.";
              endErrorThrow();
              assert(false);
            }
            if(IN_BEGIN){
              startErrorThrow(tk);
              cerr << "Missing begin portion in procedure '"<<BLOCK_STACK[BLOCK_STACK.size()-1]<<"'.";
              endErrorThrow();
              assert(false);
            }
            Tokens[i].value=BLOCK_STACK[block_tracker];
            BLOCK_STACK.pop_back();
          break;
          case BEGIN:
            if(!IN_BEGIN){
              startErrorThrow(tk);
              cerr << "Unnessasary 'begin'.";
              endErrorThrow();
              assert(false);
            }
            IN_BEGIN = false;
          break;
          case IN:
          case OUT:
            if(!IN_BEGIN){
              startErrorThrow(tk);
              cerr << "You can only use the keywords in and out before the begin part of procedure.";
              endErrorThrow();
              assert(false);
            }
          break;
          case ADD:
          case SUB:
          case MULT:
          case DIV:
          case XOR:
          case MOD:
          case GCMP:
          case LCMP:
          case ECMP:
          case GECMP:
          case LECMP:
          case NECMP:
          case AND:
          case SHR:
          case SHL:
          case BIT_AND:
          case BIT_OR:
            prev = Tokens[i-1];
            if(prev.type<UINT||prev.type>CHAR){
              startErrorThrow(tk);
              cerr << "First argument of '"<< TYPE_NAMES[tk.type] <<"' is an invalid type of '"<< TYPE_NAMES[prev.type] <<"'.";
              endErrorThrow();
              assert(false);
            }
            Tokens[i-1] = tk;
            Tokens[i] = prev;
            i++;
            if(Tokens[i].type<UINT||Tokens[i].type>CHAR){
              startErrorThrow(tk);
              cerr << "Second argument of '"<< TYPE_NAMES[tk.type] <<"' is an invalid type of '"<< TYPE_NAMES[Tokens[i].type] <<"'.";
              endErrorThrow();
              assert(false);
            }
          break;
          case AMPERSAND:
            i++;
            if(Tokens[i].type<UINT||Tokens[i].type>STRING){
              startErrorThrow(tk);
              cerr << "Trying to create a pointer to an invalid type of '"<< TYPE_NAMES[Tokens[i].type] <<"'.";
              endErrorThrow();
              assert(false);
            }
            if(literal_or_var(Tokens[i].value)){
              startErrorThrow(tk);
              cerr << "Trying to create a pointer to a literal value; can only create pointers to variables.";
              endErrorThrow();
              assert(false);
            }
          break;
          case DEREF:
            if(Tokens[++i].type!=PTR){
              startErrorThrow(Tokens[i]);
              print_token(Tokens[i-2]);
              print_token(Tokens[i-1]);
              print_token(Tokens[i]);
              print_token(Tokens[i+1]);
              print_token(Tokens[i+2]);
              cerr << "Trying to derefrence a non-pointer, instead of type '"<< TYPE_NAMES[Tokens[i].type] <<"'.";
              endErrorThrow();
              assert(false);
            }
          break;
        }
      }
      if(block_tracker>0){
        cerr << "\033[1;31mUnhandled block.\033[0m\n";
        assert(false);
      }
      if(options::STAGETEXT) printf("Done grammer checking!\n");
    }

   struct Bundle_OpAndOutType{
      ops::OpSpec* op = nullptr;
      ENUM_TYPE type = ERROR;
    };

    struct TKOP{
      enum ValueType {NOTHING = 0, TK, OPSYM, OP};
      ValueType valueType = TK;
      Token tkvalue = {};
      ops::OpSpec* opvalue = nullptr;
      string opstring = "";
      TKOP(){}
      TKOP(Token tk){
        tkvalue = tk;
        valueType = TK;
      }
      void print(){
        switch(valueType){
          case TK:
          cout << lex::TYPE_NAMES[tkvalue.type] << " ";
          if(tkvalue.value!=""){
            cout << "{" << tkvalue.value << "} ";
          }
          break;
          case OP:
          cout << opvalue->symbol << " ";
          break;
          case OPSYM:
          cout << opstring << " ";
          break;
          case NOTHING:
          cout << "this one is blank, yikes";
          break;
        }
      }
    };

    string FindNextOpInExpression(vector<Token> &expr, kt::Node<TKOP> *n){
      //cout << "FindNextOpInExpression\n";
      // incomplete, doesn't account for parentheses or real priority
      string outSym = "";
      int priority = 0;
      for(int i = expr.size()-1; i >= 0; i--){
        Token tk = expr[i]; 
        if(tk.value == "=assign"){
          outSym = tk.value;
          priority = 50;
        } else if(find(ops::opSymbolsList.begin(), ops::opSymbolsList.end(), tk.value) != ops::opSymbolsList.end()){
          if(priority < 2){
            outSym = tk.value;
            priority = 2;
          }
        }
      }
      if(outSym != ""){ 
        n->value.opstring = outSym; 
        n->value.valueType = TKOP::ValueType::OPSYM;
      }
      return outSym;
    }

    inline vector<Token> getAfterSymbol(string sym, vector<Token> &expr){
      vector<Token> tks;
      int i;
      for(i = 0; expr[i].value != sym && i < expr.size(); i++);
      i++;
      for(; i < expr.size(); i++) tks.push_back(expr[i]);
      return tks;
    }

    inline vector<Token> getBeforeSymbol(string sym, vector<Token> &expr){
      vector<Token> tks;
      for(int i = 0; expr[i].value != sym && i < expr.size(); i++) tks.push_back(expr[i]);
      return tks;
    }

    inline ENUM_TYPE checkIfArgIsExpr(vector<Token> &x, kt::Node<TKOP>* opnode, unsigned rightOrLeft, kt::BinaryTree<TKOP> &expressionTree){
      if(x.size() > 1){ 
        kt::Node<TKOP>* newopnode = expressionTree.push_back(TKOP());
        if(rightOrLeft == 1) expressionTree.connect_left(opnode, newopnode);
        else expressionTree.connect_right(opnode, newopnode);
        return getOutType(FindNextOpInExpression(x, newopnode), x, newopnode, expressionTree).type;
      }
      else{ 
        kt::Node<TKOP>* n = expressionTree.push_back(TKOP(x[0]));
        if(rightOrLeft == 1) expressionTree.connect_left(opnode, n);
        else expressionTree.connect_right(opnode, n);
        return x[0].type; 
      }
    }

    Bundle_OpAndOutType matchOp1(string sym, vector<Token> &expr, kt::Node<TKOP>* opnode, kt::BinaryTree<TKOP> &expressionTree){
      // possibly not updated
      vector<Token> x = getAfterSymbol(sym, expr);
      ENUM_TYPE xType = checkIfArgIsExpr(x, opnode, 1, expressionTree);

      Bundle_OpAndOutType out;
      int topSpecificity = 0;
      for(ops::OpSpec &op : *oplist){
        if(op.symbol == sym){
          if(ops::doesValueTypeQualify(&op, getValueType(xType), 1) && op.specificity > topSpecificity){
            topSpecificity = op.specificity;
            out.op = &op;
          }
        }
      } 
      if(out.op!=nullptr) {out.type = getEnumType(outTypeOfExpression (out.op, getValueType(xType), ops::VALUE_TYPE::ERROR));}
      return out;
    }

    Bundle_OpAndOutType matchOp2(string sym, vector<Token> &expr, kt::Node<TKOP>* opnode, kt::BinaryTree<TKOP> &expressionTree){
      vector<Token> x = getBeforeSymbol(sym, expr);
      ENUM_TYPE xType = checkIfArgIsExpr(x, opnode, 1, expressionTree);

      vector<Token> y = getAfterSymbol(sym, expr);
      ENUM_TYPE yType = checkIfArgIsExpr(y, opnode, 2, expressionTree);

      Bundle_OpAndOutType out;
      int topSpecificity = 0;
      for(ops::OpSpec &op : *oplist){
        if(op.symbol == sym){
          if(ops::doesValueTypeQualify(&op, getValueType(xType), 1) && ops::doesValueTypeQualify(&op, getValueType(yType), 2) && op.specificity > topSpecificity){
            topSpecificity = op.specificity;
            out.op = &op;
          }
        }
      } 
      if(out.op!=nullptr) { out.type = getEnumType(outTypeOfExpression (out.op, getValueType(xType), getValueType(yType))); }
      return out;
    }

    Bundle_OpAndOutType matchOpDeref(string sym, vector<Token> &expr, kt::Node<TKOP>* opnode, kt::BinaryTree<TKOP> &expressionTree){
      vector<Token> x = getAfterSymbol(sym, expr);
      vector<Token> y({x[0]});
      ENUM_TYPE xType = checkIfArgIsExpr(y, opnode, 1, expressionTree);
      bool isOpAssign = false;

      if(opnode->parent != nullptr){
        if(((kt::Node<TKOP>*)opnode->parent)->value.opvalue->symbol == "=assign" && ((kt::Node<TKOP>*)opnode->parent)->left == (void*) opnode)
          isOpAssign = true;
      }
      
      Bundle_OpAndOutType out;
      for(ops::OpSpec &op : *oplist){
        if(op.symbol == sym){
          if((isOpAssign && (ops::SYN_OP2 | op.synType != 0))||(!isOpAssign && (ops::SYN_OP1 | op.synType != 0))){
            cout << "hit!\n";
            out.op = &op;
          }
        }
      } 
      if(out.op!=nullptr) { out.type = ENUM_TYPE::VALUE; }
      return out;
    }

    Bundle_OpAndOutType getOutType(string sym, vector<Token> &expr, kt::Node<TKOP>* opnode, kt::BinaryTree<TKOP> &expressionTree){
      Bundle_OpAndOutType opbundle;
      
      //deadcode, reinstate when implementing the smarter thing where if there is no op2 or op1 of that symbol it is not checked
      ops::SYNTAX_TYPE op1or2;
      for(ops::OpSpec &op : *oplist){
        op1or2 |= op.synType & ( ops::SYN_BRACKET | ops::SYN_OP1 | ops::SYN_OP2 | ops::SYN_DEREF );
      }
      if(op1or2 | ops::SYN_DEREF){
        opbundle = matchOpDeref(sym, expr, opnode, expressionTree);
      }else{
        if(op1or2 | ops::SYN_OP2)                            
          opbundle = matchOp2(sym, expr, opnode, expressionTree);
        if( op1or2 | ops::SYN_OP1 && opbundle.op == nullptr) 
          opbundle = matchOp1(sym, expr, opnode, expressionTree);
      }
      opnode->value.valueType = TKOP::ValueType::OP;
      opnode->value.opvalue = opbundle.op;
      if(sym == "=assign"){
        opbundle.type = SPECIAL_ASSIGN_FOR_OPS_PROCESSING;
      }
      return opbundle;
    }

    // :D
    vector<Token> processExpression(vector<Token*> &expr){
      // to re-activate uncomment the operators in "operators.h"
      vector<Token> loadedExpr;
      for(int i = 0; i < expr.size(); i++){ 
        // add assignment symbols
        Token tk = *expr[i];
        
        bool added_assign = false;
        if(i + 1 < expr.size()){
          if((isDef(expr[i]->type) || isType(expr[i]->type)) && isType(expr[i+1]->type) && !added_assign){
            added_assign = true;
            loadedExpr.push_back(tk);
            Token tk1;
            tk1.line = tk.line;
            tk1.value = "=assign";
            tk1.type = SPECIAL_ASSIGN_FOR_OPS_PROCESSING;
            loadedExpr.push_back(tk1);
            continue;
          }
        } 
        loadedExpr.push_back(tk);
      }

      if(options::DEBUGMODE){
        cout << "loadedExpr: ";
        for(Token tk : loadedExpr){
          cout << lex::TYPE_NAMES[tk.type] << " ";
          if(tk.value!=""){
            cout << "{" << tk.value << "} ";
          }
        }
        cout << "\n";
      }

      bool throwexpr = true;
      for(Token &tk : loadedExpr) if(tk.type == ENUM_TYPE::OPERATOR) throwexpr = false;
      if(throwexpr) return {};

      kt::Node<TKOP> root;
      kt::BinaryTree<TKOP> expressionTree(root); 

      string sym = FindNextOpInExpression(loadedExpr, expressionTree.root);
      if(sym == "") return {};
      Bundle_OpAndOutType out = getOutType(sym, loadedExpr, expressionTree.root, expressionTree);
      if(out.type == ENUM_TYPE::ERROR) {
        Token tk = *expr[0];
        startErrorThrow(tk);
        cerr << "Incorrect expression.";
        endErrorThrow();
        assert(false);
      }

      // Flip Binary Tree
      kt::FlippedBinaryTree<TKOP> ftree(expressionTree);
      
      // go through flipped binary tree

      vector<kt::FlippedNode<TKOP>*> history({ftree.roots[0]});
      kt::FlippedNode<TKOP>* curnode = ftree.roots[0];
      kt::FlippedNode<TKOP>* lastnode = nullptr;
      kt::FlippedNode<TKOP>* loadedOp = nullptr;
      int loadedOpExprIndex = -1;
      vector<vector<Token>> outExprs;
      vector<Token> assignment;
      
      int last_temp_count = 0;
      int temp_count = 0;
      for(int i = 0; ; i++){
        // node processing 
        if(last_temp_count < temp_count){
          cout << "a branch was hit, PANICCCCCCCCCCCCCCCCCCCCC!!!!!!\n";
          Token tk;
          tk.line = assignment[0].line;
          tk.value = temp_count;
          tk.type = ENUM_TYPE::TEMP;
          assignment[0] = tk;
        }

        if(lastnode == loadedOp && loadedOp != nullptr){
          if(curnode->value.valueType == TKOP::OP){
            if(isDef(assignment[0].type)){
              Token tk;
              tk.line = assignment[0].line;
              tk.value = assignment[0].value;
              tk.type = defToType(assignment[0].type);
              //outExprs[loadedOpExprIndex].push_back(tk);
              outExprs[loadedOpExprIndex].insert(outExprs[loadedOpExprIndex].begin() + 2, tk);
            }
            else outExprs[loadedOpExprIndex].push_back(assignment[0]);
          }
          else if(curnode->value.valueType == TKOP::TK){
            //outExprs[loadedOpExprIndex].push_back(curnode->value.tkvalue);
            outExprs[loadedOpExprIndex].insert(outExprs[loadedOpExprIndex].begin() + 2, curnode->value.tkvalue);
          }
        }

        if(curnode->value.valueType == TKOP::OP){
          if(curnode->value.opvalue->symbol == "=assign"){
            // change to work for expressions being assigned to
            assignment.push_back(lastnode->value.tkvalue);
          }
          else if(curnode != loadedOp){ 
            outExprs.push_back({});
            loadedOpExprIndex++;
            for(Token tk : assignment) outExprs[loadedOpExprIndex].push_back(tk);
            Token opToken(curnode->value.opvalue);
            outExprs[loadedOpExprIndex].push_back(opToken);
            loadedOp = curnode;
          };
        }

        last_temp_count = temp_count;
        lastnode = curnode;

        // tree traversal
        TREE_TRAVERSAL:
        if(find(history.begin(), history.end(), (kt::FlippedNode<TKOP>*)curnode->rightparent) == history.end() && (curnode->rightparent!=nullptr)){
          curnode = (kt::FlippedNode<TKOP>*)curnode->rightparent;
          if(curnode->value.valueType==TKOP::OP && curnode->rightparent != nullptr){  
            if(((kt::FlippedNode<TKOP>*)curnode->rightparent)->value.valueType==TKOP::OP){
              temp_count++;
              history.push_back((kt::FlippedNode<TKOP>*)curnode->rightparent);
              goto TREE_TRAVERSAL;
            }
          }
        }
        else if(find(history.begin(), history.end(), (kt::FlippedNode<TKOP>*)curnode->leftparent) == history.end() && (curnode->leftparent!=nullptr)){
          curnode = (kt::FlippedNode<TKOP>*)curnode->leftparent;
          if(curnode->value.valueType==TKOP::OP && curnode->rightparent != nullptr){ 
            if(((kt::FlippedNode<TKOP>*)curnode->rightparent)->value.valueType==TKOP::OP){
              temp_count++;
              history.push_back((kt::FlippedNode<TKOP>*)curnode->rightparent);
              goto TREE_TRAVERSAL;
            }
          }
        }
        else {
          int counter = 0;
          for(kt::FlippedNode<TKOP>* n : history){
            if(n == (kt::FlippedNode<TKOP>*)curnode->child)counter++;
          }
          if(counter < 2 && (curnode->child!=nullptr)) curnode = (kt::FlippedNode<TKOP>*)curnode->child;
          else if(curnode->child==nullptr) break;
        }
        history.push_back(curnode);
        if(curnode == lastnode) break;
      }
      // cout << "\n";

      Token semicolontk;
      semicolontk.type = ENUM_TYPE::SEMICOLON;
      semicolontk.line = loadedExpr[0].line;
      //for (vector<Token> &tks : outExprs){
      for (int i = outExprs.size()-1; i >= 0; i--){
        outExprs[i].push_back(semicolontk);
        if(i != outExprs.size()-1 && isDef(outExprs[i][0].type)) 
          outExprs[i][0].type = defToType(outExprs[i][0].type);
        if(options::DEBUGMODE){
          for(Token tk : outExprs[i]){
            cout << lex::TYPE_NAMES[tk.type] << " ";
            if(tk.value!=""){
              cout << "{" << tk.value << "} ";
            }
          }
          cout << "\n";
        }
      }

      vector<Token> flattenedOutExprs;
      for(int i = outExprs.size()-1; i >= 0; i--){
        for(Token tk : outExprs[i]) flattenedOutExprs.push_back(tk);
      }

      return flattenedOutExprs;
    }



    void postprocess(){
      typedef vector<Token*> Expr;
      for(int i = 0; i < Tokens.size();){
        Expr expr;
        int beginIndex = i;
        while(Tokens[++i].type!=SEMICOLON && i < Tokens.size()) expr.push_back(&Tokens[i]);
        int endIndex = i;
        vector<Token> exprs = processExpression(expr);

        // for (Token tk : exprs){
        //   cout << lex::TYPE_NAMES[tk.type] << " ";
        //   if(tk.value!=""){
        //     cout << "{" << tk.value << "} ";
        //   }
        // }
        if(exprs.size()>0){
          // cout << "\n";
          Tokens.erase(Tokens.begin() + beginIndex+1, Tokens.begin() + endIndex + 1);
          Tokens.insert(Tokens.begin() + beginIndex+1, exprs.begin(), exprs.end());
          i = beginIndex + exprs.size();
        }



      }
    }
  };
}

#endif