#ifndef COMP_H
#define COMP_H

#include <vector>
#include <string>
#include <cstdio>
#include <fstream>
#include <utility>
#include <cassert>
#include <iostream>
#include <unordered_map>
#include "operators.h"
#include "hex.h"

using namespace std;

namespace comp{
  stringstream ss;
  string baseptr = "rbp";
  
  enum VAR_TYPES{
    VTSTACK,
    VTDATA,
    VTBSS,
  };
  struct Variable{
    string label;
    unsigned int size;
    unsigned int byte_addr;
    VAR_TYPES type = VTSTACK;
    void* subsetOf = nullptr;
  };
  struct Procedure{
    vector<pair<string, int>> args;
    unordered_map<string, Variable> VARS;
    string label;
    string out_sym;
    unsigned int out_size;
    unsigned int bytes;
    unsigned int call_amount=0;
    unsigned int offset=0;
  };
  
  string get_reg(int size, string reg_id){
    string result;
    switch(size){
      case 8:  result = "r"+reg_id+"x"; break;
      case 4:  result = "e"+reg_id+"x"; break;
      case 2:  result = reg_id+"x";     break;
      case 1:  result = reg_id+"l";     break;
    }
    return result;
  }
  string get_reg_adv(int size, string reg_id){
    string result;
    if(reg_id=="a"||reg_id=="b"||reg_id=="c"||reg_id=="d"){
      return get_reg(size, reg_id);
    }
    else if(reg_id=="di"||reg_id=="si"||reg_id=="bp"){
      switch(size){
        case 8:  result = "r"+reg_id; break;
        case 4:  result = "e"+reg_id; break;
        case 2:  result = reg_id;     break;
        case 1:  result = reg_id+"l"; break;
      }
    }
    else if(reg_id[0]=='r'){
      switch(size){
        case 8:  result = reg_id;     break;
        case 4:  result = reg_id+"d"; break;
        case 2:  result = reg_id+"w"; break;
        case 1:  result = reg_id+"b"; break;
      }
    }
    else{
      cerr << "\033[1;31mUnregocnized register signiture of '"<<reg_id<<"'.\033[0m\n";
      assert(false);
    }
    return result;
  }
  string asm_var(Variable &var, bool get_contents){
    switch(var.type){
      case VTSTACK:
      {
        int ptr      = var.byte_addr;
        int size     = var.size;
        switch(size){
          case 1:
          ss << "byte ["<<baseptr<<"-" << ptr << "]";
          break;
          case 2:
          ss << "word ["<<baseptr<<"-" << ptr << "]";
          break;
          case 4:
          ss << "dword ["<<baseptr<<"-" << ptr << "]";
          break;
          case 8:
          ss << "qword ["<<baseptr<<"-" << ptr << "]";
          break;
          default:
          cerr << "\033[1;31mUnknown size of '"<<size<<"' for var '"<<var.label<<"'.\033[0m\n";
          assert(false);
          break;
        }
        string output = ss.str();
        ss.str("");
        return output;
      }
      break;
      case VTDATA:
      case VTBSS:{
        if(get_contents){
          int size     = var.size;
          string ptr   = var.subsetOf!=nullptr ? ((Variable*)var.subsetOf)->label : var.label;
          if(var.byte_addr != 0){
            ptr        += "+" + to_string(var.byte_addr);
          }
          switch(var.size){
            case 1:
            ss << "byte ["<< ptr << "]";
            break;
            case 2:
            ss << "word ["<< ptr << "]";
            break;
            case 4:
            ss << "dword ["<< ptr << "]";
            break;
            case 8:
            ss << "qword ["<< ptr << "]";
            break;
            default:
            cerr << "\033[1;31mUnknown size of '"<<size<<"' for var '"<<var.label<<"'.\033[0m\n";
            assert(false);
            break;
          }
          string output = ss.str();
          ss.str("");
          return output;
        }
        return var.label;
      }
      break;
      default:
      cerr << "\033[1;31mUknown var_type of value '"<<var.type<<"'.\033[0m\n";
      assert(false);
      break;
    }
  }
  bool isRegularSize(int size){
    switch(size){
      case 1:
      case 2:
      case 4:
      case 8:
      return true;
      break;
      default:
      return false;
      break;
    }
  }
  
  string DATA_string(string label, string value){
    string output = "  "+label+": db ";
    const char* str = value.c_str();
    while(*str){
      switch(*str){
        case '\\':
          str++;
          switch(*str){
            case '\\': output+=*str; break;
            case 'n': output+="\", 10, \""; break;
            case 'r': output+="\", 13, \""; break;
            case 'a': output+="\", 7, \""; break;
            case 'b': output+="\", 8, \""; break;
            case 't': output+="\", 9, \""; break;
            case 'v': output+="\", 11, \""; break;
            case 'f': output+="\", 12, \""; break;
            case 'e': output+="\", 27, \""; break;
            case '0': output+="\", 0, \""; break;
            default:
            cerr << "\033[1;31mUnrecognized escape code of '"<<*str<<"'.\033[0m\n";
            assert(false);
            break;
          }
        break;
        default:
          output+=*str;
        break;
      }
      str++;
    }
    output+=", 0\n";
    return output;
  }
  
  class COMPILER{
    public:
    list<ops::OpSpec> *oplist;
    vector<string> syscall_args;
    vector<string> syscall_sigs;
    vector<string> WORD_MAP;
    unordered_map<string, string> CMP_MAP;
    unordered_map<string, string> INV_CMP_MAP;
  
    vector<string> INTER;
    ofstream out_stream;
    unordered_map<string, Procedure> PROCS;
    
    string DATA_SECTION;
    string BSS_SECTION;
    int string_count;
    
    COMPILER(string &wf, vector<string> &_INTER, list<ops::OpSpec> *oplist){
      out_stream.open(wf.c_str());
      if(!out_stream.is_open()){
        fprintf(stderr, "\033[1;31mUnable to open file.\033[0m\n");
        assert(false);
      }
      INTER = _INTER;
      setup();
    }
    COMPILER(string &wf, const char* rf){
      vector<string> Lines;
      string line;
      ifstream source_code;
      source_code.open(rf);
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
      for(string &s : Lines){
        vector<string> symbols({";"}), split_line = lex::split(s, symbols);
        for(int i = 0; i < split_line.size(); i++) INTER.push_back(split_line[i]);
      }
      out_stream.open(wf.c_str());
      setup();
    }
    void setup(){
      syscall_args = {
        "rax",
        "rdi",
        "rsi",
        "rdx",
        "r10",
        "r8",
        "r9"
      };
      syscall_sigs = {
        "a",
        "di",
        "si",
        "d",
        "r10",
        "r8",
        "r9"
      };
      WORD_MAP = {
        "%%NA%%",
        "byte",
        "word",
        "%%NA%%",
        "dword",
        "%%NA%%",
        "%%NA%%",
        "%%NA%%",
        "qword",
      };
      CMP_MAP = {
        {">","jg"},
        {"<","jl"},
        {">=","jge"},
        {"<=","jle"},
        {"==","je"},
        {"!=","jne"}
      };
      INV_CMP_MAP = {
        {">","jle"},
        {"<","jge"},
        {">=","jl"},
        {"<=","jg"},
        {"==","jne"},
        {"!=","je"}
      };
    }
    void run(){
      string_count = 0;
      preparse();
      switch(options::target){
        case options::X86_I386:
        baseptr      = "ebp";
        syscall_args = {
          "eax",
          "ebx",
          "ecx",
          "edx",
          "esi",
          "edi",
          "ebp"
        };
        syscall_sigs = {
          "a",
          "b",
          "c",
          "d",
          "si",
          "di",
          "bp"
        };
        init_nasm();
        //to_nasmX86_I386();
        cerr << "\033[1;31mX86_I386 (ELF_32) NOT SUPPORTED\033[0m\n";
        assert(false);
        break;
        case options::X86_64:
        default:
        init_nasm();
        to_nasmX86_64();
        break;
      }
      out_stream.close();
    }
    
    private:
    
    string asm_get_value(string &value, Procedure &proc, bool get_contents){
      if(lex::literal_or_var(value) == lex::ERROR){
        Variable var = proc.VARS[value]; 
        return asm_var(var, get_contents);
      }
      else{
        if(value == "true"){
          return "1";
        }
        else if(value == "false"){
          return "0";
        }
        else if(lex::literal_or_var(value) == lex::STRING){
          string label  = "string"+to_string(++string_count);
          DATA_SECTION +=DATA_string(label, value);
          return label;
        }
        return value;
      }
    }
    
    void preparse(){
      string CP;
      for(int i=0; i<INTER.size(); i++){
        string cur = INTER[i];
        if(cur == "proc"){
          Procedure new_proc;
          new_proc.label = INTER[++i];
          CP             = INTER[i];
          new_proc.bytes = 0;
          PROCS[CP]      = new_proc;
          i++;
        }
        else if(cur == "call"){
          PROCS[CP].call_amount++;
        }
        else if(cur == "def"&&(INTER[i-1]!="static"&&INTER[i-1]!="extern"&&INTER[i-1]!="global")){
          string symbol = INTER[i+2];
          Variable new_var;
          string isize        = INTER[++i];
          int size            = stoi(isize.erase(0,1))/8;
          string sym          = INTER[++i];
          PROCS[CP].bytes    += size;
          PROCS[CP].offset   += size;
          new_var.byte_addr   = PROCS[CP].bytes;
          new_var.size        = size;
          new_var.label       = sym;
          PROCS[CP].VARS[sym] = new_var;
          i++;
        }
      }
    }
    void init_nasm(){
      out_stream <<
      "SECTION .text		; code section\n"
      "global "<<options::ENTRYPOINT<<"		  ; make label available to linker\n";
      for(string &s : lex::EXTERNAL_PROCS) out_stream << "extern " << s << "\n";
    }
    void op_init(int &size1, int &size2, string &reg1, string &reg2, int &i, Procedure &proc){
      string v1 = INTER[++i];
      string v2 = INTER[++i];
      string reg1sig = reg1 !="" ? reg1 : "a";
      string reg2sig = reg2 !="" ? reg2 : "d";
      if(size2<size1&&lex::literal_or_var(v2)==lex::ERROR){
        reg1  = get_reg(size1, reg1sig);
        reg2  = get_reg(size1, reg2sig);
        out_stream <<
        "  mov "<< reg1 <<", " << asm_get_value(v1, proc, true) << "\n"
        "  movsx "<< reg2 <<", " << asm_get_value(v2, proc, true) << "\n";
      }
      else if(size2>size1&&lex::literal_or_var(v1)==lex::ERROR){
        size1 = size2;
        reg1  = get_reg(size2, reg1sig);
        reg2  = get_reg(size2, reg2sig);
        out_stream <<
        "  movsx "<< reg1 <<", " << asm_get_value(v1, proc, true) << "\n"
        "  mov "<< reg2 <<", " << asm_get_value(v2, proc, true) << "\n";
      }
      else{
        if(lex::literal_or_var(v1)!=lex::ERROR&&size2>size1) size1 = size2;
        reg1  = get_reg(size1, reg1sig);
        reg2  = get_reg(size1, reg2sig);
        out_stream <<
        "  mov "<< reg1 <<", " << asm_get_value(v1, proc, true) << "\n"
        "  mov "<< reg2 <<", " << asm_get_value(v2, proc, true) << "\n";
      }
    }
    void to_nasmX86_64(){
      if(options::STAGETEXT)printf("Intermediate to NASM...\n");
      if(options::DEBUGMODE){
        for(string &s : INTER){
          cout << s << " ";
          if(s == ";") cout << "\n";
        }
      }
      int INTER_LINE = 0;
      string VAR_ASSIGN;
      string CP;
      DATA_SECTION = "SECTION .data:\n";
      BSS_SECTION  = "SECTION .bss\n";
      for(int i=0; i<INTER.size(); i++){
        string cur = INTER[i];
        if(cur == "proc"){
          out_stream << INTER[++i] << ":\n";
          CP = INTER[i];
          i++;
        }
        else if(cur == "end"){
          if(INTER[++i]==options::ENTRYPOINT){
            out_stream <<
            ".exit:\n"
            "  add rsp, " << PROCS[CP].offset << "\n"
            "  pop rbp\n";
            if(!options::LIBC){
              out_stream <<
              "  mov rax, 60\n"
              "  mov rdi, 0\n"
              "  syscall\n";
            }
            else{
              out_stream <<
              "  mov eax, 0\n"
              "  call exit\n";
            }
          }
          else{
            out_stream <<
            ".exit:\n";
            if(PROCS[CP].out_sym!=""){
              out_stream <<
              "  mov " << get_reg(PROCS[CP].out_size, "a") << ", " << asm_var(PROCS[CP].VARS[PROCS[CP].out_sym], true) <<"\n";
            }
            out_stream <<
            "  add rsp, " << PROCS[CP].offset << "\n"
            "  pop rbp\n"
            "  ret\n";
          }
          CP = "///NA///";
          i++;
        }
        else if(cur == "def"){
          Variable new_var;
          int size   = stoi(INTER[++i].erase(0,1))/8;
          string sym = INTER[++i];
          if(PROCS[CP].VARS.find(sym)==PROCS[CP].VARS.end()){
            PROCS[CP].bytes    += size;
            new_var.byte_addr   = PROCS[CP].bytes;
            new_var.size        = size;
            new_var.label       = sym;
            PROCS[CP].VARS[sym] = new_var;
          }
          if(INTER[++i]=="=") VAR_ASSIGN = sym;
          else if(INTER[i]=="["){
            if(!lex::isNumber(INTER[++i])){
              cerr << "\033[1;31mLooking for uint literal in array definition of '"<<sym.erase(0,1)<<"', instead found '"<<INTER[i]<<"'\033[0m\n";
              assert(false);
            }
            PROCS[CP].VARS[sym].size = size*stoi(INTER[i]);
            if(INTER[++i]!="]"){
              cerr << "\033[1;31mDid not find closing square bracket in array definition of '"<<sym.erase(0,1)<<"'.\033[0m\n";
              assert(false);
            }
            i++;
          }
        }
        else if(cur == "subset"){
          Variable new_var;
          int size            = stoi(INTER[++i].erase(0,1))/8;
          int offset          = stoi(INTER[++i]);
          string sym          = INTER[++i];
          new_var.byte_addr   = PROCS[CP].VARS[INTER[i]].type == VTDATA 
                                  || 
                                PROCS[CP].VARS[INTER[i]].type == VTBSS 
                                ? offset : PROCS[CP].VARS[INTER[++i]].byte_addr-offset;
          new_var.size        = size;
          new_var.label       = sym;
          new_var.type        = PROCS[CP].VARS[INTER[i]].type;
          new_var.subsetOf    = (void*)&PROCS[CP].VARS[INTER[i]];
          PROCS[CP].VARS[sym] = new_var;
          i++;
        }
        else if(cur == "in"){
          i++;
          Variable new_var;
          int size            = stoi(INTER[++i].erase(0,1))/8;
          string sym          = INTER[++i];
          if(PROCS[CP].VARS.find(sym)==PROCS[CP].VARS.end()){
            PROCS[CP].bytes    += size;
            new_var.byte_addr   = PROCS[CP].bytes;
            new_var.size        = size;
            new_var.label       = sym;
            PROCS[CP].VARS[sym] = new_var;
          }
          PROCS[CP].args.push_back({sym, size});
          i++;
        }
        else if(cur == "out"){
          i++;
          Variable new_var;
          int size            = stoi(INTER[++i].erase(0,1))/8;
          string sym          = INTER[++i];
          PROCS[CP].bytes    += size;
          PROCS[CP].out_size  = size;
          PROCS[CP].out_sym   = sym;
          new_var.byte_addr   = PROCS[CP].bytes;
          new_var.size        = size;
          new_var.label       = sym;
          PROCS[CP].VARS[sym] = new_var;
          i++;
        }
        else if(cur == "begin"){
          int local_storage = PROCS[CP].offset;
          local_storage    += PROCS[CP].call_amount>0 ? 16: 0;
          PROCS[CP].offset  = local_storage;
          out_stream <<
          "  push rbp\n"
          "  mov rbp, rsp\n"
          "  sub rsp, "<< local_storage <<"\n";
          for(int j = 0; j < PROCS[CP].args.size(); j++){
            pair<string, int> arg = PROCS[CP].args[j];
            out_stream <<
            "  mov " << asm_get_value(arg.first, PROCS[CP], true) << ", " << get_reg_adv(arg.second, syscall_sigs[j])<< "\n";
          }
          i++;
        }
        else if(cur == "call"){
          string proc_name = INTER[++i];
          if(INTER[++i]!="("){
            cerr << "No open paren when calling procedure '"<<proc_name<<"'.\n";
            assert(false);
          }
          if(proc_name=="__asm"){
            if(INTER[i+1]==";")i++;
            while(INTER[++i]!=")"){
              INTER[i].erase(0,1);
              INTER[i].erase(INTER[i].size()-1,1);
              out_stream << INTER[i] << "\n";
              if(INTER[i+1]==";")i++;
            }
            i++;
          }
          else{
            int j = 0;
            while(INTER[++i]!=")"){
              if(INTER[i]=="&"){
                i++;
                Variable var = PROCS[CP].VARS[INTER[i]];
                out_stream << "  lea " << syscall_args[j] << ", ";
                switch(var.type){
                  case VTSTACK:
                  {
                    out_stream <<
                    "[rbp-" << var.byte_addr << "]\n";
                  }
                  break;
                  case VTDATA:
                  case VTBSS:
                    out_stream <<
                    "["<< var.label <<"]\n";
                  break;
                }
                j++;
              }
              else{
                lex::ENUM_TYPE type = lex::literal_or_var(INTER[i]);
                if(type==lex::STRING){
                  out_stream << "  mov ";
                }
                else{
                  int size            = lex::ERROR == type ?
                                        ( (PROCS[CP].VARS[INTER[i]].type == VAR_TYPES::VTBSS 
                                          || 
                                          PROCS[CP].VARS[INTER[i]].type == VAR_TYPES::VTDATA ) 
                                          ? 8 : PROCS[CP].VARS[INTER[i]].size )
                                        :
                                          stoi(intr::size_of(type).erase(0,1))/8;
                  if(size<8&&type==lex::ERROR) out_stream << "  movsx ";
                  else out_stream << "  mov ";
                }
                out_stream << syscall_args[j] << ", " << asm_get_value(INTER[i], PROCS[CP], false) << "\n";
                j++;
              }
            }
            out_stream <<
            "  call " << proc_name << "\n";
            i++;
            if(VAR_ASSIGN!=""){
              Variable var_assign = PROCS[CP].VARS[VAR_ASSIGN];
              string outreg = get_reg(var_assign.size, "a");
              out_stream << "  mov " << asm_var(var_assign, true) << ", "<<outreg<<"\n";
              VAR_ASSIGN="";
            }
          }
        }
        else if(cur == "persist"){}
        else if(cur == "static"){
          i++;
          Variable new_var;
          int size   = stoi(INTER[++i].erase(0,1))/8;
          string sym = INTER[++i];
          if(PROCS[CP].VARS.find(sym)==PROCS[CP].VARS.end()){
            new_var.byte_addr   = 0;
            new_var.size        = size;
            //new_var.size        = 8;
            new_var.label       = PROCS[CP].label+"@"+sym;
            new_var.type        = VTBSS;
            PROCS[CP].VARS[sym] = new_var;
          }
          if(INTER[i+1]=="="){
            i++;
            PROCS[CP].VARS[sym].type = VTDATA;
            Variable var        = PROCS[CP].VARS[sym];
            string value        = INTER[++i];
            lex::ENUM_TYPE type = lex::literal_or_var(value);
            switch(type){
              case lex::CHAR:
              case lex::STRING: DATA_SECTION  +=DATA_string(var.label, value);                                break;
              /*case lex::INT:
              case lex::UINT: DATA_SECTION    += "  "+var.label+": dd "+value+"\n";                           break;
              case lex::CHAR: DATA_SECTION    += "  "+var.label+": db "+value+"\n";                           break;
              case lex::BOOL: DATA_SECTION    += "  "+var.label+": db "+asm_get_value(value, PROCS[CP], false)+"\n"; break;
              case lex::LONG: DATA_SECTION    += "  "+var.label+": dq "+value+"\n";                           break;*/
              case lex::INT: case lex::UINT:
              case lex::BOOL:
              case lex::LONG:
              DATA_SECTION += "  "+var.label+": ";
              switch(size){
                case 1: DATA_SECTION += " db "; break;
                case 4: DATA_SECTION += " dd "; break;
                case 8: DATA_SECTION += " dq "; break;
              }
              DATA_SECTION += asm_get_value(value, PROCS[CP], false)+"\n";
              break;
              case lex::ERROR:
              cerr << "\033[1;31mTrying to assign nonliteral to '"<<sym<<"' in static def.\033[0m\n";
              assert(false);
              break;
              default:
              cerr << "\033[1;31mUnhandled type in staic def of '"<<sym<<"'.\033[0m\n";
              assert(false);
              break;
            }
          }
          else{
            Variable var = PROCS[CP].VARS[sym];
            switch(size){
              case 1:  BSS_SECTION += "  "+var.label+": resb 1\n";                    break;
              case 2:  BSS_SECTION += "  "+var.label+": resw 1\n";                    break;
              case 4:  BSS_SECTION += "  "+var.label+": resd 1\n";                    break;
              case 8:  BSS_SECTION += "  "+var.label+": resq 1\n";                    break;
              default: BSS_SECTION += "  "+var.label+": resb "+to_string(size)+"\n";  break;
            }
          }
          i++;
        }
        else if(cur == "global"){
          string proc_name = "";
          if(INTER[i+1]=="::"){
            i++;
            proc_name = INTER[++i];
          }
          string var_name  = INTER[++i];
          Variable var;
          bool found = false;
          if(proc_name==""){
            for(auto &p : PROCS){
              if(p.second.VARS.find(var_name)!=p.second.VARS.end()){
                Variable v = p.second.VARS[var_name];
                if(v.type!=VTSTACK){
                  found = true;
                  var = v;
                  break;
                }
              }
            }
          }
          else{
            if(PROCS[proc_name].VARS.find(var_name)!=PROCS[proc_name].VARS.end()){
              Variable v = PROCS[proc_name].VARS[var_name];
              if(v.type!=VTSTACK){
                found = true;
                var = v;
              }
            }
          }
          if(!found){
            cerr << "\033[1;31mCould not find a static variable called '"<<var_name<<"' in global grab.\033[0m\n";
            assert(false);
          }
          PROCS[CP].VARS[var_name] = var;
          i++;
        }
        else if(cur == "extern"){
          i++;
          Variable new_var;
          int size   = stoi(INTER[++i].erase(0,1))/8;
          string sym = INTER[++i];
          new_var.byte_addr   = 0;
          new_var.size        = size;
          new_var.label       = sym.erase(0,1);
          new_var.type        = VTBSS;
          PROCS[CP].VARS[sym] = new_var;
          DATA_SECTION       += "extern "+sym+"\n";
          i++;
        }
        else if(cur == "label"){
          out_stream <<
          INTER[++i] << ":\n";
          i++;
        }
        else if(cur == "jmpc"){
          string label = INTER[++i];
          bool inverse = INTER[i+1] == "!";
          if(inverse) i++;
          string value = INTER[++i];
          if(value[0]=='?'){
            out_stream <<
            "  cmp " << asm_get_value(value, PROCS[CP], true) << ", 0\n"
            "  "<<(inverse?"je":"jne")<<" "<< label << "\n";
          }
          else{
            int size1 = stoi(INTER[++i].erase(0,1))/8;
            int size2 = stoi(INTER[++i].erase(0,1))/8;
            string reg1;
            string reg2;
            op_init(size1, size2, reg1, reg2, i, PROCS[CP]);
            out_stream <<
            "  cmp "<<reg1<<", "<<reg2<<"\n"
            "  " <<(inverse?INV_CMP_MAP[value]:CMP_MAP[value])<<" "<< label << "\n";
          }
          i++;
        }
        else if(cur == "jmp"){
          string label = INTER[++i];
          out_stream <<
          "  jmp " << label << "\n";
          i++;
        }
        else if(cur == "op"){
          ops::OpSpec* op = (ops::OpSpec*)hexToPtr(INTER[++i]);
          switch(op->asmType){
            case ops::ASM_TYPE::ASMT_SIMPLE_OP2:
              {
                int size1 = stoi(INTER[++i].erase(0,1))/8;
                int size2 = stoi(INTER[++i].erase(0,1))/8;
                string reg1;
                string reg2;
                op_init(size1, size2, reg1, reg2, i, PROCS[CP]);
                out_stream <<
                "  "<< op->instruction <<" "<<reg1<<", "<<reg2<<"\n";
                i++;
                if(VAR_ASSIGN!=""){
                  Variable var_assign = PROCS[CP].VARS[VAR_ASSIGN];
                  if(var_assign.size==size1)
                    out_stream << "  mov " << asm_var(var_assign, true) << ", "<<reg1<<"\n";
                  else if(var_assign.size<size1){
                    out_stream << "  mov " << asm_var(var_assign, true) << ", "<<get_reg(var_assign.size, "a")<<"\n";
                  }
                  else if(var_assign.size>size1){
                    string outreg = get_reg(var_assign.size, "a");
                    out_stream << "  movsx " << outreg << ", " << reg1 << "\n"
                    "  mov " << asm_var(var_assign, true) << ", "<< outreg <<"\n";
                  }
                  VAR_ASSIGN="";
                }
              }
            break;
            default:
            cerr << "ERROR: Unhandled ASM_TYPE of " << op->asmType << "\n";
            assert(false);
            break;
          }
        }
        else if(cur == "+"){
          int size1 = stoi(INTER[++i].erase(0,1))/8;
          int size2 = stoi(INTER[++i].erase(0,1))/8;
          string reg1;
          string reg2;
          op_init(size1, size2, reg1, reg2, i, PROCS[CP]);
          out_stream <<
          "  add "<<reg1<<", "<<reg2<<"\n";
          i++;
          if(VAR_ASSIGN!=""){
            Variable var_assign = PROCS[CP].VARS[VAR_ASSIGN];
            if(var_assign.size==size1)
              out_stream << "  mov " << asm_var(var_assign, true) << ", "<<reg1<<"\n";
            else if(var_assign.size<size1){
              out_stream << "  mov " << asm_var(var_assign, true) << ", "<<get_reg(var_assign.size, "a")<<"\n";
            }
            else if(var_assign.size>size1){
              string outreg = get_reg(var_assign.size, "a");
              out_stream << "  movsx " << outreg << ", " << reg1 << "\n"
              "  mov " << asm_var(var_assign, true) << ", "<< outreg <<"\n";
            }
            VAR_ASSIGN="";
          }
        }
        else if(cur == "-"){
          int size1 = stoi(INTER[++i].erase(0,1))/8;
          int size2 = stoi(INTER[++i].erase(0,1))/8;
          string reg1;
          string reg2;
          op_init(size1, size2, reg1, reg2, i, PROCS[CP]);
          out_stream <<
          "  sub "<<reg1<<", "<<reg2<<"\n";
          i++;
          if(VAR_ASSIGN!=""){
            Variable var_assign = PROCS[CP].VARS[VAR_ASSIGN];
            if(var_assign.size==size1)
              out_stream << "  mov " << asm_var(var_assign, true) << ", "<<reg1<<"\n";
            else if(var_assign.size<size1){
              out_stream << "  mov " << asm_var(var_assign, true) << ", "<<get_reg(var_assign.size, "a")<<"\n";
            }
            else if(var_assign.size>size1){
              string outreg = get_reg(var_assign.size, "a");
              out_stream << "  movsx " << outreg << ", " << reg1 << "\n"
              "  mov " << asm_var(var_assign, true) << ", "<< outreg <<"\n";
            }
            VAR_ASSIGN="";
          }
        }
        else if(cur == "*"){
          int size1 = stoi(INTER[++i].erase(0,1))/8;
          int size2 = stoi(INTER[++i].erase(0,1))/8;
          string reg1;
          string reg2;
          op_init(size1, size2, reg1, reg2, i, PROCS[CP]);
          out_stream <<
          "  mul "<<reg2<<"\n";
          i++;
          if(VAR_ASSIGN!=""){
            Variable var_assign = PROCS[CP].VARS[VAR_ASSIGN];
            if(var_assign.size==size1)
              out_stream << "  mov " << asm_var(var_assign, true) << ", "<<reg1<<"\n";
            else if(var_assign.size<size1){
              out_stream << "  mov " << asm_var(var_assign, true) << ", "<<get_reg(var_assign.size, "a")<<"\n";
            }
            else if(var_assign.size>size1){
              string outreg = get_reg(var_assign.size, "a");
              out_stream << "  movsx " << outreg << ", " << reg1 << "\n"
              "  mov " << asm_var(var_assign, true) << ", "<< outreg <<"\n";
            }
            VAR_ASSIGN="";
          }
        }
        else if(cur == "/"){
          out_stream <<
          "  xor rdx, rdx\n";
          int size1 = stoi(INTER[++i].erase(0,1))/8;
          int size2 = stoi(INTER[++i].erase(0,1))/8;
          string reg1;
          string reg2 = "b";
          op_init(size1, size2, reg1, reg2, i, PROCS[CP]);
          out_stream <<
          "  div "<<reg2<<"\n";
          i++;
          if(VAR_ASSIGN!=""){
            Variable var_assign = PROCS[CP].VARS[VAR_ASSIGN];
            if(var_assign.size==size1)
              out_stream << "  mov " << asm_var(var_assign, true) << ", "<<reg1<<"\n";
            else if(var_assign.size<size1){
              out_stream << "  mov " << asm_var(var_assign, true) << ", "<<get_reg(var_assign.size, "a")<<"\n";
            }
            else if(var_assign.size>size1){
              string outreg = get_reg(var_assign.size, "a");
              out_stream << "  movsx " << outreg << ", " << reg1 << "\n"
              "  mov " << asm_var(var_assign, true) << ", "<< outreg <<"\n";
            }
            VAR_ASSIGN="";
          }
        }
        else if(cur == "%"){
          out_stream <<
          "  xor rdx, rdx\n"
          "  xor rax, rax\n";
          int size1 = stoi(INTER[++i].erase(0,1))/8;
          int size2 = stoi(INTER[++i].erase(0,1))/8;
          string reg1;
          string reg2 = "b";
          op_init(size1, size2, reg1, reg2, i, PROCS[CP]);
          out_stream <<
          "  div "<<reg2<<"\n";
          i++;
          if(VAR_ASSIGN!=""){
            Variable var_assign = PROCS[CP].VARS[VAR_ASSIGN];
            if(var_assign.size==size1)
              out_stream << "  mov " << asm_var(var_assign, true) << ", "<<get_reg(size1, "d")<<"\n";
            else if(var_assign.size<size1){
              out_stream << "  mov " << asm_var(var_assign, true) << ", "<<get_reg(var_assign.size, "d")<<"\n";
            }
            else if(var_assign.size>size1){
              string outreg = get_reg(var_assign.size, "d");
              out_stream << "  movsx " << outreg << ", " << get_reg(size1, "d") << "\n"
              "  mov " << asm_var(var_assign, true) << ", "<< outreg <<"\n";
            }
            VAR_ASSIGN="";
          }
        }
        else if(cur == "++"){
          Variable var = PROCS[CP].VARS[INTER[++i]];
          out_stream << "  inc " << asm_var(var, true) << "\n";
          i++;
        }
        else if(cur == "--"){
          Variable var = PROCS[CP].VARS[INTER[++i]];
          out_stream << "  dec " << asm_var(var, true) << "\n";
          i++;
        }
        else if(cur == ">"){
          int size1 = stoi(INTER[++i].erase(0,1))/8;
          int size2 = stoi(INTER[++i].erase(0,1))/8;
          string reg1;
          string reg2;
          op_init(size1, size2, reg1, reg2, i, PROCS[CP]);
          out_stream <<
          "  cmp "<<reg1<<", "<<reg2<<"\n"
          "  setg al\n";
          i++;
          if(VAR_ASSIGN!=""){
            Variable var_assign = PROCS[CP].VARS[VAR_ASSIGN];
            out_stream << "  mov " << asm_var(var_assign, true) << ", "<<get_reg(var_assign.size, "a")<<"\n";
            VAR_ASSIGN="";
          }
        }
        else if(cur == "<"){
          int size1 = stoi(INTER[++i].erase(0,1))/8;
          int size2 = stoi(INTER[++i].erase(0,1))/8;
          string reg1;
          string reg2;
          op_init(size1, size2, reg1, reg2, i, PROCS[CP]);
          out_stream <<
          "  cmp "<<reg1<<", "<<reg2<<"\n"
          "  setl al\n";
          i++;
          if(VAR_ASSIGN!=""){
            Variable var_assign = PROCS[CP].VARS[VAR_ASSIGN];
            out_stream << "  mov " << asm_var(var_assign, true) << ", "<<get_reg(var_assign.size, "a")<<"\n";
            VAR_ASSIGN="";
          }
        }
        else if(cur == "=="){
          int size1 = stoi(INTER[++i].erase(0,1))/8;
          int size2 = stoi(INTER[++i].erase(0,1))/8;
          string reg1;
          string reg2;
          op_init(size1, size2, reg1, reg2, i, PROCS[CP]);
          out_stream <<
          "  cmp "<<reg1<<", "<<reg2<<"\n"
          "  sete al\n";
          i++;
          if(VAR_ASSIGN!=""){
            Variable var_assign = PROCS[CP].VARS[VAR_ASSIGN];
            out_stream << "  mov " << asm_var(var_assign, true) << ", "<<get_reg(var_assign.size, "a")<<"\n";
            VAR_ASSIGN="";
          }
        }
        else if(cur == "!="){
          int size1 = stoi(INTER[++i].erase(0,1))/8;
          int size2 = stoi(INTER[++i].erase(0,1))/8;
          string reg1;
          string reg2;
          op_init(size1, size2, reg1, reg2, i, PROCS[CP]);
          out_stream <<
          "  cmp "<<reg1<<", "<<reg2<<"\n"
          "  setne al\n";
          i++;
          if(VAR_ASSIGN!=""){
            Variable var_assign = PROCS[CP].VARS[VAR_ASSIGN];
            out_stream << "  mov " << asm_var(var_assign, true) << ", "<<get_reg(var_assign.size, "a")<<"\n";
            VAR_ASSIGN="";
          }
        }
        else if(cur == ">="){
          int size1 = stoi(INTER[++i].erase(0,1))/8;
          int size2 = stoi(INTER[++i].erase(0,1))/8;
          string reg1;
          string reg2;
          op_init(size1, size2, reg1, reg2, i, PROCS[CP]);
          out_stream <<
          "  cmp "<<reg1<<", "<<reg2<<"\n"
          "  setge al\n";
          i++;
          if(VAR_ASSIGN!=""){
            Variable var_assign = PROCS[CP].VARS[VAR_ASSIGN];
            out_stream << "  mov " << asm_var(var_assign, true) << ", "<<get_reg(var_assign.size, "a")<<"\n";
            VAR_ASSIGN="";
          }
        }
        else if(cur == "<="){
          int size1 = stoi(INTER[++i].erase(0,1))/8;
          int size2 = stoi(INTER[++i].erase(0,1))/8;
          string reg1;
          string reg2;
          op_init(size1, size2, reg1, reg2, i, PROCS[CP]);
          out_stream <<
          "  cmp "<<reg1<<", "<<reg2<<"\n"
          "  setle al\n";
          i++;
          if(VAR_ASSIGN!=""){
            Variable var_assign = PROCS[CP].VARS[VAR_ASSIGN];
            out_stream << "  mov " << asm_var(var_assign, true) << ", "<<get_reg(var_assign.size, "a")<<"\n";
            VAR_ASSIGN="";
          }
        }
        else if(cur == ">>"){
          int size1  = stoi(INTER[++i].erase(0,1))/8;
          int size2  = stoi(INTER[++i].erase(0,1))/8;
          if(VAR_ASSIGN!=""){
            Variable var = PROCS[CP].VARS[INTER[++i]];
            Variable var_assign = PROCS[CP].VARS[VAR_ASSIGN];
            string reg = get_reg(var.size, "a");
            out_stream <<
            "  mov " << reg << ", " << asm_var(var, true) << "\n"
            "  shr " << reg << ", " << INTER[++i] << "\n"
            "  mov " << asm_var(var_assign, true) << ", "<<get_reg(var_assign.size, "a")<<"\n";
            VAR_ASSIGN="";
          }
          else{
            out_stream <<
            "  shr "<<asm_var(PROCS[CP].VARS[INTER[++i]], true)<<", "<<INTER[++i]<<"\n";
          }
          i++;
        }
        else if(cur == "<<"){
          int size1  = stoi(INTER[++i].erase(0,1))/8;
          int size2  = stoi(INTER[++i].erase(0,1))/8;
          if(VAR_ASSIGN!=""){
            Variable var = PROCS[CP].VARS[INTER[++i]];
            Variable var_assign = PROCS[CP].VARS[VAR_ASSIGN];
            string reg = get_reg(var.size, "a");
            out_stream <<
            "  mov " << reg << ", " << asm_var(var, true) << "\n"
            "  shl " << reg << ", " << INTER[++i] << "\n"
            "  mov " << asm_var(var_assign, true) << ", "<<get_reg(var_assign.size, "a")<<"\n";
            VAR_ASSIGN="";
          }
          else{
            out_stream <<
            "  shl "<<asm_var(PROCS[CP].VARS[INTER[++i]], true)<<", "<<INTER[++i]<<"\n";
          }
          i++;
        }
        else if(cur == "&&"){
          int size1 = stoi(INTER[++i].erase(0,1))/8;
          int size2 = stoi(INTER[++i].erase(0,1))/8;
          string reg1;
          string reg2;
          op_init(size1, size2, reg1, reg2, i, PROCS[CP]);
          out_stream <<
          "  and "<<reg1<<", "<<reg2<<"\n"
          "  shr al, 1\n"
          "  setc al\n";
          i++;
          if(VAR_ASSIGN!=""){
            Variable var_assign = PROCS[CP].VARS[VAR_ASSIGN];
            out_stream << "  mov " << asm_var(var_assign, true) << ", "<<get_reg(var_assign.size, "a")<<"\n";
            VAR_ASSIGN="";
          }
        }
        else if(cur == "-&"){
          int size1 = stoi(INTER[++i].erase(0,1))/8;
          int size2 = stoi(INTER[++i].erase(0,1))/8;
          string reg1;
          string reg2;
          op_init(size1, size2, reg1, reg2, i, PROCS[CP]);
          out_stream <<
          "  and "<<reg1<<", "<<reg2<<"\n";
          i++;
          if(VAR_ASSIGN!=""){
            Variable var_assign = PROCS[CP].VARS[VAR_ASSIGN];
            if(var_assign.size==size1)
              out_stream << "  mov " << asm_var(var_assign, true) << ", "<<reg1<<"\n";
            else if(var_assign.size<size1){
              out_stream << "  mov " << asm_var(var_assign, true) << ", "<<get_reg(var_assign.size, "a")<<"\n";
            }
            else if(var_assign.size>size1){
              string outreg = get_reg(var_assign.size, "a");
              out_stream << "  movsx " << outreg << ", " << reg1 << "\n"
              "  mov " << asm_var(var_assign, true) << ", "<< outreg <<"\n";
            }
            VAR_ASSIGN="";
          }
        }
        else if(cur == "|"){
          int size1 = stoi(INTER[++i].erase(0,1))/8;
          int size2 = stoi(INTER[++i].erase(0,1))/8;
          string reg1;
          string reg2;
          op_init(size1, size2, reg1, reg2, i, PROCS[CP]);
          out_stream <<
          "  or "<<reg1<<", "<<reg2<<"\n";
          i++;
          if(VAR_ASSIGN!=""){
            Variable var_assign = PROCS[CP].VARS[VAR_ASSIGN];
            if(var_assign.size==size1)
              out_stream << "  mov " << asm_var(var_assign, true) << ", "<<reg1<<"\n";
            else if(var_assign.size<size1){
              out_stream << "  mov " << asm_var(var_assign, true) << ", "<<get_reg(var_assign.size, "a")<<"\n";
            }
            else if(var_assign.size>size1){
              string outreg = get_reg(var_assign.size, "a");
              out_stream << "  movsx " << outreg << ", " << reg1 << "\n"
              "  mov " << asm_var(var_assign, true) << ", "<< outreg <<"\n";
            }
            VAR_ASSIGN="";
          }
        }
        else if(cur == "^"){
          int size1 = stoi(INTER[++i].erase(0,1))/8;
          int size2 = stoi(INTER[++i].erase(0,1))/8;
          string reg1;
          string reg2;
          op_init(size1, size2, reg1, reg2, i, PROCS[CP]);
          out_stream <<
          "  xor "<<reg1<<", "<<reg2<<"\n";
          i++;
          if(VAR_ASSIGN!=""){
            Variable var_assign = PROCS[CP].VARS[VAR_ASSIGN];
            if(var_assign.size==size1)
              out_stream << "  mov " << asm_var(var_assign, true) << ", "<<reg1<<"\n";
            else if(var_assign.size<size1){
              out_stream << "  mov " << asm_var(var_assign, true) << ", "<<get_reg(var_assign.size, "a")<<"\n";
            }
            else if(var_assign.size>size1){
              string outreg = get_reg(var_assign.size, "a");
              out_stream << "  movsx " << outreg << ", " << reg1 << "\n"
              "  mov " << asm_var(var_assign, true) << ", "<< outreg <<"\n";
            }
            VAR_ASSIGN="";
          }
        }
        else if(cur == "!" && INTER[i+1][0]=='i'){
          int size1 = stoi(INTER[++i].erase(0,1))/8;
          string reg1;
          reg1  = get_reg(size1, "a");
          out_stream <<
          "  mov "<< reg1 <<", " << asm_get_value(INTER[++i], PROCS[CP], true) << "\n"
          "  not "<<reg1<<"\n";
          i++;
          if(VAR_ASSIGN!=""){
            Variable var_assign = PROCS[CP].VARS[VAR_ASSIGN];
            if(var_assign.size==size1)
              out_stream << "  mov " << asm_var(var_assign, true) << ", "<<reg1<<"\n";
            else if(var_assign.size<size1){
              out_stream << "  mov " << asm_var(var_assign, true) << ", "<<get_reg(var_assign.size, "a")<<"\n";
            }
            else if(var_assign.size>size1){
              string outreg = get_reg(var_assign.size, "a");
              out_stream << "  movsx " << outreg << ", " << reg1 << "\n"
              "  mov " << asm_var(var_assign, true) << ", "<< outreg <<"\n";
            }
            VAR_ASSIGN="";
          }
        }
        else if(cur == "&"){
          Variable var = PROCS[CP].VARS[INTER[++i]];
          switch(var.type){
            case VTSTACK:
            {
              out_stream <<
              "  lea rax, [rbp-" << var.byte_addr << "]\n";
              if(VAR_ASSIGN!=""){
                Variable var_assign = PROCS[CP].VARS[VAR_ASSIGN];
                string outreg = get_reg(var_assign.size, "a");
                out_stream << "  mov " << asm_var(var_assign, true) << ", "<<outreg<<"\n";
                VAR_ASSIGN="";
              }
            }
            break;
            case VTDATA:
            case VTBSS:
            {
              out_stream <<
              "  lea rax, ["<< var.label <<"]\n";
              if(VAR_ASSIGN!=""){
                Variable var_assign = PROCS[CP].VARS[VAR_ASSIGN];
                string outreg = get_reg(var_assign.size, "a");
                out_stream << "  mov " << asm_var(var_assign, true) << ", "<<outreg<<"\n";
                VAR_ASSIGN="";
              }
            }
            break;
          }
          i++;
        }
        else if(cur == "@"){
          int refSize = 0; 
          int offset = 0;
          if(INTER[++i] == "offset"){
            offset = stoi(INTER[++i]);
            i++;
          }
          if(INTER[i][0] == 'i'){
            refSize = stoi(INTER[i++].erase(0,1))/8;
          }
          Variable var = PROCS[CP].VARS[INTER[i]];
          int size = INTER[i+1][0]=='i' ? stoi(INTER[++i].erase(0,1))/8 : -1;
          switch(var.type){
            case VTSTACK:
            {
              if(VAR_ASSIGN!=""){
                out_stream <<
                "  mov rax, " << asm_var(var, true) << "\n";
                Variable var_assign = PROCS[CP].VARS[VAR_ASSIGN];
                if(isRegularSize(var_assign.size)){
                  string outreg = get_reg(var_assign.size, "d");
                  out_stream << "  mov " << outreg << ", ";
                  if(offset!=0) out_stream << WORD_MAP[var_assign.size]<<" [rax+"<<offset<<"]\n";
                  else out_stream << WORD_MAP[var_assign.size]<<" [rax]\n";
                  out_stream << "  mov " << asm_var(var_assign, true) << ", " << outreg<< "\n";
                  VAR_ASSIGN="";
                }
                else{
                  int ptrv = var_assign.byte_addr;
                  size = var_assign.size;
                  while(size>0){
                    if(size>=8){
                      size-=8;
                      out_stream<<
                      "  mov rdx, qword [rax+"<<size<<"]\n"
                      "  mov qword [rbp-"<<ptrv-size<<"], rdx\n";
                    }
                    else if(size>=4){
                      size-=4;
                      out_stream<<
                      "  mov edx, dword [rax+"<<size<<"]\n"
                      "  mov dword [rbp-"<<ptrv-size<<"], edx\n";
                    }
                    else if(size>=2){
                      size-=2;
                      out_stream<<
                      "  mov dx, word [rax+"<<size<<"]\n"
                      "  mov word [rbp-"<<ptrv-size<<"], dx\n";
                    }
                    else if(size==1){
                      size-=1;
                      out_stream<<
                      "  mov dl, byte [rax+"<<size<<"]\n"
                      "  mov byte [rbp-"<<ptrv-size<<"], dl\n";
                    }
                  }
                  VAR_ASSIGN="";
                }
              }
              else if(INTER[i+1]=="="){
                out_stream <<
                "  mov rax, " << asm_var(var, true) << "\n";
                i++;
                string value = INTER[++i];
                if(value[0] != '?'){
                  cout << "hit on unimplemented: " << var.label << "\n";
                  VAR_ASSIGN = "@" + var.label;
                }else{
                  string refaddress = "[rax]";
                  if(offset!=0) refaddress = "[rax+" + to_string(offset) + "]";
                  size = refSize;
                  switch(size){
                    case 1:
                    out_stream <<
                    "  mov dl, "<<asm_get_value(value, PROCS[CP], true)<<"\n"
                    "  mov byte "<<refaddress<<", dl\n";
                    break;
                    case 2:
                    out_stream <<
                    "  mov dx, "<<asm_get_value(value, PROCS[CP], true)<<"\n"
                    "  mov word "<<refaddress<<", dx\n";
                    break;
                    case 4:
                    out_stream <<
                    "  mov edx, "<<asm_get_value(value, PROCS[CP], true)<<"\n"
                    "  mov dword "<<refaddress<<", edx\n";
                    break;
                    case 8:
                    out_stream <<
                    "  mov rdx, "<<asm_get_value(value, PROCS[CP], true)<<"\n"
                    "  mov qword "<<refaddress<<", rdx\n";
                    break;
                  }
                  VAR_ASSIGN = "";
                }
              }
              else if(INTER[i+1]!=";"){
                out_stream <<
                "  mov rax, " << asm_var(var, true) << "\n";
                string value = INTER[++i];
                string refaddress = "[rax]";
                if(offset!=0) refaddress = "[rax+" + to_string(offset) + "]";
                if(size==-1) size = PROCS[CP].VARS[value].size;
                switch(size){
                  case 1:
                  out_stream <<
                  "  mov dl, "<<asm_get_value(value, PROCS[CP], true)<<"\n"
                  "  mov byte "<<refaddress<<", dl\n";
                  break;
                  case 2:
                  out_stream <<
                  "  mov dx, "<<asm_get_value(value, PROCS[CP], true)<<"\n"
                  "  mov word "<<refaddress<<", dx\n";
                  break;
                  case 4:
                  out_stream <<
                  "  mov edx, "<<asm_get_value(value, PROCS[CP], true)<<"\n"
                  "  mov dword "<<refaddress<<", edx\n";
                  break;
                  case 8:
                  out_stream <<
                  "  mov rdx, "<<asm_get_value(value, PROCS[CP], true)<<"\n"
                  "  mov qword "<<refaddress<<", rdx\n";
                  break;
                }
              }
            }
            break;
            case VTDATA:
            case VTBSS:
            {
              if(VAR_ASSIGN!=""){
                Variable var_assign = PROCS[CP].VARS[VAR_ASSIGN];
                string outreg = get_reg(var_assign.size, "a");
                out_stream << "  mov " << outreg << ", ";
                if(offset!=0) out_stream <<WORD_MAP[var_assign.size]<<" ["<<var.label<<"+"<<offset<<"]\n";
                else out_stream <<WORD_MAP[var_assign.size]<<" ["<<var.label<<"]\n";
                out_stream << "  mov " << asm_var(var_assign, true) << ", " << outreg<< "\n";
                VAR_ASSIGN="";
              }
            }
            break;
          }
          i++;
        }
        else if(cur == "pop"){
          int size = stoi(INTER[++i].erase(0,1))/8;
          out_stream <<
          "  mov " << get_reg(size, "a") << ", " << asm_get_value(INTER[++i], PROCS[CP], true) << "\n"
          "  pop rax\n";
          i++;
        }
        else if(cur == "push"){
          int size = stoi(INTER[++i].erase(0,1))/8;
          out_stream <<
          "  mov " << get_reg(size, "a") << ", " << asm_get_value(INTER[++i], PROCS[CP], true) << "\n"
          "  push rax\n";
          i++;
        }
        else if(cur == "syscall"){
          if(INTER[++i]!="("){
            cerr << "\033[1;31mNo open paren with syscall.\033[0m\n";
            assert(false);
          }
          int j = 0;
          while(INTER[++i]!=")"){
            lex::ENUM_TYPE type = lex::literal_or_var(INTER[i]);
            int size            = lex::ERROR == type ?
                                  PROCS[CP].VARS[INTER[i]].size :
                                  stoi(intr::size_of(type).erase(0,1))/8;
            if(size<8&&type==lex::ERROR) out_stream << "  movsx ";
            else out_stream << "  mov ";
            out_stream << syscall_args[j] << ", " << asm_get_value(INTER[i], PROCS[CP], false) << "\n";
            j++;
          }
          out_stream << "  syscall\n";
          i++;
          if(VAR_ASSIGN!=""){
            Variable var_assign = PROCS[CP].VARS[VAR_ASSIGN];
            out_stream <<
            "  mov "<<asm_var(var_assign, true)<<", "<<get_reg(var_assign.size, "a")<<"\n";
            VAR_ASSIGN="";
          }
        }
        else if(PROCS[CP].VARS.find(cur)!=PROCS[CP].VARS.end()){
          Variable cvar = PROCS[CP].VARS[cur];
          switch(cvar.type){
            case VTSTACK:
            if(VAR_ASSIGN!=""){
              Variable var_assign = PROCS[CP].VARS[VAR_ASSIGN];
              int size = cvar.size;
              if(size==1||size==2||size==4||size==8){
                if(cvar.size<var_assign.size){
                  out_stream<<
                  "  xor rax, rax\n";
                }
                out_stream<<
                "  mov "<<get_reg(cvar.size, "a")<<", "<<asm_var(cvar, true)<<"\n"
                "  mov "<<asm_var(var_assign, true)<<", "<<get_reg(var_assign.size, "a")<<"\n";
                VAR_ASSIGN="";
                i++;
              }
              else{
                int ptrc = cvar.byte_addr;
                int ptrv = var_assign.byte_addr;
                while(size>0){
                  if(size>=8){
                    size-=8;
                    out_stream<<
                    "  mov rax, qword [rbp-"<<ptrc-size<<"]\n"
                    "  mov qword [rbp-"<<ptrv-size<<"], rax\n";
                  }
                  else if(size>=4){
                    size-=4;
                    out_stream<<
                    "  mov eax, dword [rbp-"<<ptrc-size<<"]\n"
                    "  mov dword [rbp-"<<ptrv-size<<"], eax\n";
                  }
                  else if(size>=2){
                    size-=2;
                    out_stream<<
                    "  mov ax, word [rbp-"<<ptrc-size<<"]\n"
                    "  mov word [rbp-"<<ptrv-size<<"], ax\n";
                  }
                  else if(size==1){
                    size-=1;
                    out_stream<<
                    "  mov al, byte [rbp-"<<ptrc-size<<"]\n"
                    "  mov byte [rbp-"<<ptrv-size<<"], al\n";
                  }
                }
                VAR_ASSIGN="";
                i++;
              }
            }
            else{
              if(INTER[++i]=="=") VAR_ASSIGN = cur;
              else if(INTER[i][0]=='@'){
                cout << "hit unimplemented deref assign\n";
              }
              else{
                cerr << i << "\n"
                << "BEFORE: " << INTER[i+1] << "\n"
                << "AFTER: "  << INTER[i-1] << "\n";
                cerr << "\033[1;31mHanging variable of '"<<cur.erase(0,1)<<"' in compiler stage.\033[0m\n";
                assert(false);
              }
            }
            break;
            case VTDATA:
            case VTBSS:
            if(VAR_ASSIGN!=""){
              Variable var_assign = PROCS[CP].VARS[VAR_ASSIGN];
              int size = var_assign.size;
              if(size==1||size==2||size==4||size==8){
                out_stream<<
                "  mov "<< get_reg(cvar.size, "a") <<", "<< asm_var(cvar, true)<<"\n"
                "  mov "<< asm_var(var_assign, true)<<", "<< get_reg(var_assign.size, "a") <<"\n";
                VAR_ASSIGN="";
                i++;
              }
              else{
                int ptrc = cvar.byte_addr;
                int ptrv = var_assign.byte_addr;
                while(size>0){
                  if(size>=8){
                    size-=8;
                    out_stream<<
                    "  mov rax, qword [rbp-"<<ptrc-size<<"]\n"
                    "  mov qword [rbp-"<<ptrv-size<<"]\n";
                  }
                  else if(size>=4){
                    size-=4;
                    out_stream<<
                    "  mov eax, dword [rbp-"<<ptrc-size<<"]\n"
                    "  mov dword [rbp-"<<ptrv-size<<"]\n";
                  }
                  else if(size>=2){
                    size-=2;
                    out_stream<<
                    "  mov ax, word [rbp-"<<ptrc-size<<"]\n"
                    "  mov word [rbp-"<<ptrv-size<<"]\n";
                  }
                  else if(size==1){
                    size-=1;
                    out_stream<<
                    "  mov al, byte [rbp-"<<ptrc-size<<"]\n"
                    "  mov byte [rbp-"<<ptrv-size<<"]\n";
                  }
                }
                VAR_ASSIGN="";
                i++;
              }
              VAR_ASSIGN="";
            }
            else{
              if(INTER[++i]=="=") VAR_ASSIGN = cur;
              else if(INTER[i][0]=='@'){
                cout << "hit unimplemented deref assign\n";
              }
              else{
                cerr << i << "\n"
                << "BEFORE: " << INTER[i+1] << "\n"
                << "AFTER: "  << INTER[i-1] << "\n";
                cerr << "\033[1;31mHanging variable of '"<<cur.erase(0,1)<<"' in compiler stage.\033[0m\n";
                assert(false);
              }
            }
            break;
          }
        }
        else if(lex::literal_or_var(cur)!=lex::ERROR){
          if(VAR_ASSIGN!=""){
            Variable var = PROCS[CP].VARS[VAR_ASSIGN];
            if(lex::literal_or_var(cur)!=lex::STRING){
              out_stream <<
              "  mov "<<asm_var(var, true)<<", "<<asm_get_value(cur, PROCS[CP], true)<<"\n";
              VAR_ASSIGN="";
            }
            else{
              string value = cur.erase(0,1);
              value.erase(value.size()-1, 1);
              int size     = value.size();
              int ptrv     = var.byte_addr;
              int counter  = 0;
              string s;
              while(size-counter>0){
                if(size-counter>=8){
                  s="";
                  for(int i = counter; i < counter+8; i++){
                    s += value[i];
                  }
                  out_stream<<
                  "  mov rax, \""<<s<<"\"\n"
                  "  mov qword [rbp-"<<ptrv-counter<<"], rax\n";
                  counter+=8;
                }
                else if(size-counter>=4){
                  s="";
                  for(int i = counter; i < counter+4; i++){
                    s += value[i];
                  }
                  out_stream<<
                  "  mov eax, \""<<s<<"\"\n"
                  "  mov dword [rbp-"<<ptrv-counter<<"], eax\n";
                  counter+=4;
                }
                else if(size-counter>=2){
                  s="";
                  for(int i = counter; i < counter+2; i++){
                    s += value[i];
                  }
                  out_stream<<
                  "  mov ax, \""<<s<<"\"\n"
                  "  mov word [rbp-"<<ptrv-counter<<"], ax\n";
                  counter+=2;
                }
                else if(size-counter==1){
                  s="";
                  for(int i = counter; i < counter+1; i++){
                    s += value[i];
                  }
                  out_stream<<
                  "  mov al, \""<<s<<"\"\n"
                  "  mov byte [rbp-"<<ptrv-counter<<"], al\n";
                  counter+=1;
                }
              }
              VAR_ASSIGN="";
            }
            i++;
          }
        }
        else{
          cerr << i << "\n"
          << "BEFORE: " << INTER[i+1] << "\n"
          << "AFTER: "  << INTER[i-1] << "\n";
          cerr << "\033[1;31mUnhandled token of '"<< cur <<"' in compilation on intermediate line "<<INTER_LINE<<".\033[0m\n";
          assert(false);
        }
        if(INTER[i]==";") INTER_LINE++;
      }
      out_stream << DATA_SECTION << "\n" << BSS_SECTION << "\n";
      if(options::STAGETEXT)printf("Done converting intermediate to NASM!\n");
    }
    
  };
}

#endif