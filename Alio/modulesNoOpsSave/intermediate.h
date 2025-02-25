#ifndef INTER_H
#define INTER_H

#include <iterator>
#include <vector>
#include <string>
#include <cstdio>
#include <cassert>
#include <iostream>
#include "lexer.h"

using namespace std;

namespace intr{
  inline bool DEF_ASSIGN(lex::Token tk){
    return (tk.type>=lex::UINT&&tk.type<=lex::AMPERSAND) ||tk.type==lex::SYSCALL;
  }
  
  string size_of(lex::ENUM_TYPE type){
    if(type == lex::UINT) return "i32";
    if(type == lex::INT) return "i32";
    if(type == lex::LONG) return "i64";
    if(type == lex::SHORT) return "i16";
    if(type == lex::PTR || type == lex::PTR_OF_TYPE){
      if(options::target==options::X86_I386) return "i32";
      return "i64";
    }
    if(type == lex::BOOL) return "i8";
    if(type == lex::CHAR) return "i8";
    return "UKNOWN_SIZE";
  }

  size_t bytes_in(lex::ENUM_TYPE type){
    if(type == lex::UINT) return 4;
    if(type == lex::INT) return 4;
    if(type == lex::LONG) return 8;
    if(type == lex::SHORT) return 2;
    if(type == lex::PTR || type == lex::PTR_OF_TYPE){
      if(options::target==options::X86_I386) return 4;
      return 8;
    }
    if(type == lex::BOOL) return 1;
    if(type == lex::CHAR) return 1;
    return 0;
  }
  
  struct fn_node{
    string label;
    vector<fn_node*> children;
  };
  
  class INTERMEDIATE{
    public:
    
    vector<lex::Token> Tokens;
    vector<string> INTER;
    
    INTERMEDIATE(vector <lex::Token> &_Tokens){
      Tokens = _Tokens;
    }
    
    vector<string> run(){
      tks_to_inter();
      deadcode_elim();
      grammer_check();
      if(options::INTER_FILE!="") output_inter(options::INTER_FILE);
      return INTER;
    }
    
    private:
    
    void output_inter(string &wf){
      ofstream out_stream;
      out_stream.open(wf.c_str());
      for(string &s : INTER){
        out_stream << s << " ";
        if(s == ";") out_stream << "\n";
      }
    }
    
    void setup_op(lex::Token &tk1, lex::Token &tk2){
      INTER.push_back(size_of(tk1.type));
      INTER.push_back(size_of(tk2.type));
      if(lex::literal_or_var(tk1.value)!=lex::ERROR) INTER.push_back(tk1.value);
      else INTER.push_back("?"+tk1.value);
      if(lex::literal_or_var(tk2.value)!=lex::ERROR) INTER.push_back(tk2.value);
      else INTER.push_back("?"+tk2.value);
    }
    void tks_to_inter(){
      printf("Generate intermediate representation...\n");
      lex::Token tk1;
      lex::Token tk2;
      string prev;
      if(options::DEBUGMODE){
        for(lex::Token &tk : Tokens){
          cout << lex::TYPE_NAMES[tk.type] << " ";
          if(tk.value!=""){
            cout << "{"<< tk.value << "} ";
          }
          if(tk.type==lex::SEMICOLON){
            cout << "\n";
          }
        }
      }
      int ESC_COUNTER = 0;
      vector<string> BLOCK_STACK;
      for(int i = 0; i<Tokens.size(); i++){
        lex::Token tk = Tokens[i];
        switch(tk.type){
          case lex::PROC:
          INTER.push_back("proc");
          INTER.push_back(tk.value);
          INTER.push_back(";");
          break;
          case lex::END:
          INTER.push_back("end");
          INTER.push_back(tk.value);
          INTER.push_back(";");
          if(BLOCK_STACK.size()>0){
            if(BLOCK_STACK[BLOCK_STACK.size()-1]=="else if"&&tk.value=="else if"){
              INTER.pop_back();
              INTER.push_back(BLOCK_STACK[BLOCK_STACK.size()-2]);
              INTER.push_back("1");
              INTER.push_back(";");
              BLOCK_STACK.pop_back();
              BLOCK_STACK.pop_back();
            }
            else if(BLOCK_STACK[BLOCK_STACK.size()-1]=="if"&&tk.value=="if"){
              BLOCK_STACK.pop_back();
              BLOCK_STACK.pop_back();
            }
            else if(BLOCK_STACK[BLOCK_STACK.size()-1]=="else"&&tk.value=="else"){
              INTER.pop_back();
              INTER.push_back(BLOCK_STACK[BLOCK_STACK.size()-2]);
              INTER.push_back(";");
              BLOCK_STACK.pop_back();
              BLOCK_STACK.pop_back();
            }
          }
          break;
          case lex::BEGIN:
          INTER.push_back("begin");
          break;
          case lex::IN:
          INTER.push_back("in");
          break;
          case lex::OUT:
          INTER.push_back("out");
          break;
          case lex::PUSH:
          INTER.push_back("push");
          INTER.push_back(size_of(Tokens[i+1].type));
          if(lex::literal_or_var(Tokens[i+1].value)==lex::ERROR) INTER.push_back("?"+Tokens[i+1].value);
          else INTER.push_back(Tokens[i+1].value);
          INTER.push_back(";");
          i++;
          break;
          case lex::POP:
          INTER.push_back("pop");
          INTER.push_back(size_of(Tokens[i+1].type));
          if(lex::literal_or_var(Tokens[i+1].value)==lex::ERROR) INTER.push_back("?"+Tokens[i+1].value);
          else INTER.push_back(Tokens[i+1].value);
          INTER.push_back(";");
          i++;
          break;
          case lex::SUBSET:
          INTER.push_back("subset");
          INTER.push_back("i"+to_string(stoi(Tokens[i+2].value)*8));
          INTER.push_back(Tokens[i+1].value);
          INTER.push_back("?"+Tokens[i+3].value);
          INTER.push_back("?"+Tokens[i+4].value);
          INTER.push_back(";");
          i+=4;
          break;
          case lex::SYSCALL:
          INTER.push_back("syscall");
          break;
          case lex::OPARA:
          INTER.push_back("(");
          break;
          case lex::CPARA:
          INTER.push_back(")");
          break;
          case lex::CALL:
          INTER.push_back("call");
          INTER.push_back(tk.value);
          break;
          case lex::PTR_OF_TYPE:
          i++;
          case lex::UINT:
          case lex::INT:
          case lex::LONG:
          case lex::PTR:
          case lex::BOOL:
          case lex::CHAR:
          case lex::STRING:
          case lex::SHORT:
          case lex::STRUCT:
          if(lex::literal_or_var(tk.value)==lex::ERROR) INTER.push_back("?"+tk.value);
          else INTER.push_back(tk.value);
          break;
          case lex::DEFS:
          INTER.push_back("def");
          if(Tokens[i+1].type!=lex::OSQRB){
            INTER.push_back("i"+to_string((Tokens[i+1].value.size()-2)*8));
            INTER.push_back("?"+tk.value);
            if(DEF_ASSIGN(Tokens[i+1])) INTER.push_back("=");
            else if(Tokens[i+1].type==lex::OSQRB) continue;
            else INTER.push_back(";");
          }
          else{
            i++;
            INTER.push_back("i"+to_string(stoi(Tokens[++i].value)*8));
            i++;
            INTER.push_back("?"+tk.value);
            if(DEF_ASSIGN(Tokens[i+1])) INTER.push_back("=");
            else INTER.push_back(";");
          }
          break;
          case lex::DEFSTRUCT:
          INTER.push_back("def");
          INTER.push_back("i"+to_string(lex::STRUCTS[tk.value].size*8));
          INTER.push_back("?"+Tokens[++i].value);
          INTER.push_back(";");
          break;
          case lex::DEFU:
          case lex::DEFI:
          INTER.push_back("def");
          INTER.push_back("i32");
          INTER.push_back("?"+tk.value);
          if(DEF_ASSIGN(Tokens[i+1])) INTER.push_back("=");
          else if(Tokens[i+1].type==lex::OSQRB) continue;
          else INTER.push_back(";");
          break;
          case lex::DEFP_OF_TYPE:
          tk.value = Tokens[++i].value;
          case lex::DEFP:
          case lex::DEFL:
          INTER.push_back("def");
          if(options::target==options::X86_I386) INTER.push_back("i32");
          else INTER.push_back("i64");
          INTER.push_back("?"+tk.value);
          if(DEF_ASSIGN(Tokens[i+1])) INTER.push_back("=");
          else if(Tokens[i+1].type==lex::OSQRB) continue;
          else INTER.push_back(";");
          break;
          case lex::DEFC:
          case lex::DEFB:
          INTER.push_back("def");
          INTER.push_back("i8");
          INTER.push_back("?"+tk.value);
          if(DEF_ASSIGN(Tokens[i+1])) INTER.push_back("=");
          else if(Tokens[i+1].type==lex::OSQRB) continue;
          else INTER.push_back(";");
          break;
          case lex::DEFSHORT:
          INTER.push_back("def");
          INTER.push_back("i16");
          INTER.push_back("?"+tk.value);
          if(DEF_ASSIGN(Tokens[i+1])) INTER.push_back("=");
          else if(Tokens[i+1].type==lex::OSQRB) continue;
          else INTER.push_back(";");
          break;
          case lex::ADD:
          tk1 = Tokens[++i];
          tk2 = Tokens[++i];
          INTER.push_back("+");
          setup_op(tk1, tk2);
          break;
          case lex::SUB:
          tk1 = Tokens[++i];
          tk2 = Tokens[++i];
          INTER.push_back("-");
          setup_op(tk1, tk2);
          break;
          case lex::MULT:
          tk1 = Tokens[++i];
          tk2 = Tokens[++i];
          INTER.push_back("*");
          setup_op(tk1, tk2);
          break;
          case lex::DIV:
          tk1 = Tokens[++i];
          tk2 = Tokens[++i];
          INTER.push_back("/");
          setup_op(tk1, tk2);
          break;
          case lex::MOD:
          tk1 = Tokens[++i];
          tk2 = Tokens[++i];
          INTER.push_back("%");
          setup_op(tk1, tk2);
          break;
          case lex::ECMP:
          tk1 = Tokens[++i];
          tk2 = Tokens[++i];
          INTER.push_back("==");
          setup_op(tk1, tk2);
          break;
          case lex::LCMP:
          tk1 = Tokens[++i];
          tk2 = Tokens[++i];
          INTER.push_back("<");
          setup_op(tk1, tk2);
          break;
          case lex::GCMP:
          tk1 = Tokens[++i];
          tk2 = Tokens[++i];
          INTER.push_back(">");
          setup_op(tk1, tk2);
          break;
          case lex::GECMP:
          tk1 = Tokens[++i];
          tk2 = Tokens[++i];
          INTER.push_back(">=");
          setup_op(tk1, tk2);
          break;
          case lex::LECMP:
          tk1 = Tokens[++i];
          tk2 = Tokens[++i];
          INTER.push_back("<=");
          setup_op(tk1, tk2);
          break;
          case lex::NECMP:
          tk1 = Tokens[++i];
          tk2 = Tokens[++i];
          INTER.push_back("!=");
          setup_op(tk1, tk2);
          break;
          case lex::AND:
          tk1 = Tokens[++i];
          tk2 = Tokens[++i];
          INTER.push_back("&&");
          setup_op(tk1, tk2);
          break;
          case lex::BIT_AND:
          tk1 = Tokens[++i];
          tk2 = Tokens[++i];
          INTER.push_back("-&");
          setup_op(tk1, tk2);
          break;
          case lex::BIT_OR:
          tk1 = Tokens[++i];
          tk2 = Tokens[++i];
          INTER.push_back("|");
          setup_op(tk1, tk2);
          break;
          case lex::NOT:
          tk1 = Tokens[++i];
          INTER.push_back("!");
          INTER.push_back(size_of(tk1.type));
          INTER.push_back("?"+tk1.value);
          break;
          case lex::XOR:
          tk1 = Tokens[++i];
          tk2 = Tokens[++i];
          INTER.push_back("^");
          setup_op(tk1, tk2);
          break;
          case lex::SHR:
          tk1 = Tokens[++i];
          tk2 = Tokens[++i];
          INTER.push_back(">>");
          setup_op(tk1, tk2);
          break;
          case lex::SHL:
          tk1 = Tokens[++i];
          tk2 = Tokens[++i];
          INTER.push_back("<<");
          setup_op(tk1, tk2);
          break;
          case lex::AMPERSAND:
          tk1 = Tokens[++i];
          INTER.push_back("&");
          INTER.push_back("?"+tk1.value);
          break;
          case lex::DEREF_FIELD:
          tk1 = Tokens[i-2];
          INTER.pop_back();
          INTER.push_back("@");
          INTER.push_back("offset");
          {
            string stuctName = Tokens[i-1].value;
            //calculate offset
            unsigned int bytes = 0;
            int j = 0;
            for ( ; j < lex::STRUCTS[stuctName].fields.size(); j++){
              if(lex::STRUCTS[stuctName].fields[j].value==tk.value) break;
              bytes += bytes_in(lex::LEXER::defToType(lex::STRUCTS[stuctName].fields[j].type));
            }
            INTER.push_back(to_string(bytes));
            INTER.push_back(size_of(lex::LEXER::defToType(lex::STRUCTS[stuctName].fields[j].type)));
          }
          INTER.push_back("?"+tk1.value);
          if(DEF_ASSIGN(Tokens[i+1])) INTER.push_back("=");
          else INTER.push_back(";");
          break;
          case lex::DEREF:
          tk1 = Tokens[++i];
          INTER.push_back("@");
          INTER.push_back("?"+tk1.value);
          if(lex::isType(Tokens[i+1].type)) INTER.push_back(size_of(Tokens[i+1].type));
          //if(DEF_ASSIGN(Tokens[i+1])) INTER.push_back("=");
          else INTER.push_back(";");
          break;
          case lex::INC:
          prev = INTER[INTER.size()-1];
          INTER[INTER.size()-1] = "++";
          INTER.push_back(prev);
          break;
          case lex::DEC:
          prev = INTER[INTER.size()-1];
          INTER[INTER.size()-1] = "--";
          INTER.push_back(prev);
          break;
          case lex::ARGLEA:
          INTER.push_back("&");
          INTER.push_back("?"+tk.value);
          break;
          case lex::OFNAME:
          INTER.push_back("::");
          INTER.push_back(tk.value);
          break;
          case lex::SEMICOLON:
          if(INTER[INTER.size()-1]!=";") INTER.push_back(";");
          break;
          case lex::OSQRB:
          INTER.push_back("[");
          break;
          case lex::CSQRB:
          INTER.push_back("]");
          break;
          case lex::STATIC:
          INTER.push_back("static");
          break;
          case lex::PERSIST:
          INTER.push_back("persist");
          break;
          case lex::EXTERN:
          INTER.push_back("extern");
          break;
          case lex::GLOBAL:
          INTER.push_back("global");
          break;
          case lex::WHILE:
          INTER.push_back("while");
          break;
          case lex::IF:
          INTER.push_back("if");
          BLOCK_STACK.push_back(to_string(++ESC_COUNTER));
          BLOCK_STACK.push_back("if");
          break;
          case lex::ELSE:
          if(Tokens[i+1].type!=lex::IF){
            INTER.push_back("end");
            INTER.push_back(BLOCK_STACK[BLOCK_STACK.size()-1]);
            if(BLOCK_STACK[BLOCK_STACK.size()-1]!="if") INTER.push_back(BLOCK_STACK[BLOCK_STACK.size()-2]);
            if(BLOCK_STACK[BLOCK_STACK.size()-1]=="else if") INTER.push_back("0");
            BLOCK_STACK[BLOCK_STACK.size()-1]="else";
            INTER.push_back(";");
            INTER.push_back("else");
          }
          else{
            INTER.push_back("end");
            INTER.push_back(BLOCK_STACK[BLOCK_STACK.size()-1]);
            if(BLOCK_STACK[BLOCK_STACK.size()-1]!="if") INTER.push_back(BLOCK_STACK[BLOCK_STACK.size()-2]);
            if(BLOCK_STACK[BLOCK_STACK.size()-1]=="else if") INTER.push_back("0");
            BLOCK_STACK[BLOCK_STACK.size()-1]="else if";
            INTER.push_back(";");
            INTER.push_back("else if");
            i++;
          }
          break;
          default:
          startErrorThrow(tk);
          cerr << "Unhandled token type in intermediate '"<<lex::TYPE_NAMES[tk.type]<<"'.";
          endErrorThrow();
          assert(false);
          break;
        }
      }
      printf("Done generating intermediate representation!\n");
    }
    
    void parse_tree(string &current, unordered_map<string, fn_node> &tree, vector<string> &list){
      fn_node node = tree[current];
      if(options::DEBUGMODE){
        cout << "CURRENT: "<<current<<"\n";
        cout << "CHILD AMOUNT: " << node.children.size() << "\n";
      }
      if(node.children.size()<=1&&node.children[0]==nullptr){
        return;
      }
      for(fn_node* &n : node.children){
        if(options::DEBUGMODE) cout << "    CHILD: "<<n->label<<"\n";
        if(find(list.begin(), list.end(), n->label)==list.end()){
          list.push_back(n->label);
          if(n->children.size()>0&&n->children[0]!=nullptr){
            parse_tree(n->label, tree, list);
            if(options::DEBUGMODE) cout << "\n";
          }
        }
      }
    }
    void printfn_node(fn_node node){
      cout <<node.label<< " fn_node{\n";
      cout << "    int children_size = " << node.children.size() << ";\n";
      cout << "    vector<*fn_node> children = {";
      if(node.children[0]!=nullptr){
        for(fn_node* &n : node.children){
          cout << n->label << ", ";
        }
      }
      cout << "};\n};\n";
    }
    void deadcode_elim(){
      unordered_map<string, fn_node> tree;
      vector<string> fns;
      fn_node* CP;
      vector<string> list;
      for(int i = 0; i < INTER.size(); i++){
        if(INTER[i]=="proc"){
          fn_node proc;
          proc.label       = INTER[i+1];
          proc.children.push_back(nullptr);
          tree[INTER[i+1]] = proc;
          CP               = &tree[INTER[i+1]];
          fns.push_back(INTER[i+1]);
          if(options::DEBUGMODE) printfn_node(tree[INTER[i+1]]);
          if(i!=0) if(INTER[i-1] == "persist"){ list.push_back(proc.label); }
          i+=2;
        }
        else if(INTER[i]=="call" && find(fns.begin(), fns.end(), INTER[i+1]) != fns.end()){
          vector<fn_node*>* children = &CP->children;
          if(CP->children[0]==nullptr) children->pop_back();
          bool found = false;
          for(fn_node* &n : *children){
            if(n->label==INTER[i+1]){
              found = true;
              break;
            }
          }
          if(!found) children->push_back(&tree[INTER[i+1]]);
          i+=2;
        }
      }
      string main = options::ENTRYPOINT;
      list.push_back(main);
      parse_tree(main, tree, list);
      //First clean up
      tree.clear();
      for(string &s : list){
        fns.erase(find(fns.begin(), fns.end(), s));
      }
      int begin = 0;
      string current;
      int i = 0;
      while(i<INTER.size()){
        if(INTER[i]=="proc"&&find(fns.begin(), fns.end(), INTER[i+1])!=fns.end()){
          begin = i;
          current = INTER[i+1];
          i+=3;
        }
        if(INTER[i]=="end"&&INTER[i+1]==current){
          i+=3;
          INTER.erase(INTER.begin()+begin, INTER.begin()+i);
          i=begin-1;
        }
        i++;
      }
      //Second clean up
      list.clear();
      fns.clear();
    }
    
    void grammer_check(){
      int WHILE_COUNT = 0;
      int IF_COUNT    = 0;
      int ESC_COUNT   = 0;
      int ELIF_COUNT  = 0;
      for(int i = 0; i < INTER.size()-1; i++){
        string s = INTER[i];
        if(s==";"&&INTER[i+1][0]=='?'){
          INTER.insert(INTER.begin()+i+2, "=");
          i++;
        }
      }
      for(int i = 0; i < INTER.size()-1; i++){
        string s = INTER[i];
        if(s=="while"){
          //Erase and get info
          int beginning = i;
          if(INTER[++i]!="("){
            cerr << "\033[1;31mNo open paren with while dec.\033[0m\n";
            assert(false);
          }
          vector<string> expression;
          while(INTER[++i]!=")") expression.push_back(INTER[i]);
          i++;
          INTER.erase(INTER.begin()+beginning,INTER.begin()+i+1);
          i = beginning;
          //Set up the beginning of the while loop
          vector<string> insertv;
          insertv.push_back("jmp");
          insertv.push_back(".whilecmp"+to_string(++WHILE_COUNT));
          insertv.push_back(";");
          insertv.push_back("label");
          insertv.push_back(".while"+to_string(WHILE_COUNT));
          insertv.push_back(";");
          INTER.insert(INTER.begin()+i,insertv.begin(), insertv.end());
          i+=insertv.size()-1;
          //Find End
          int bc = 1;
          while(true){
            if(INTER[++i]=="while") bc++;
            else if(INTER[i]=="end"&&INTER[i+1]=="while"){
              if(--bc==0) break;
              i++;
            }
          }
          INTER.erase(INTER.begin()+i,INTER.begin()+i+3);
          //Setup end
          insertv.clear();
          insertv.push_back("label");
          insertv.push_back(".whilecmp"+to_string(WHILE_COUNT));
          insertv.push_back(";");
          if(expression.size()==1){
            insertv.push_back("jmpc");
            insertv.push_back(".while"+to_string(WHILE_COUNT));
            insertv.push_back(expression[0]);
            insertv.push_back(";");
          }
          else{
            insertv.push_back("jmpc");
            insertv.push_back(".while"+to_string(WHILE_COUNT));
            for(string &s : expression){
              insertv.push_back(s);
            }
            insertv.push_back(";");
          }
          INTER.insert(INTER.begin()+i,insertv.begin(), insertv.end());
          i = beginning+1;
        }
        else if(s=="if"){
          //Erase and get info
          int beginning = i;
          if(INTER[++i]!="("){
            cerr << "\033[1;31mNo open paren with if dec.\033[0m\n";
            assert(false);
          }
          vector<string> expression;
          while(INTER[++i]!=")") expression.push_back(INTER[i]);
          i++;
          INTER.erase(INTER.begin()+beginning,INTER.begin()+i+1);
          i = beginning;
          //Set up the beginning of the if
          vector<string> insertv;
          if(expression.size()==1){
            insertv.push_back("jmpc");
            insertv.push_back(".if"+to_string(++IF_COUNT));
            insertv.push_back("!");
            insertv.push_back(expression[0]);
            insertv.push_back(";");
          }
          else{
            insertv.push_back("jmpc");
            insertv.push_back(".if"+to_string(++IF_COUNT));
            insertv.push_back("!");
            for(string &s : expression){
              insertv.push_back(s);
            }
            insertv.push_back(";");
          }
          INTER.insert(INTER.begin()+i,insertv.begin(), insertv.end());
          i+=insertv.size()-1;
          //Find End
          int bc = 1;
          while(true){
            if(i+1>=INTER.size()){
              cerr << "\033[1;31mDid not find end of if.\033[0m\n";
              assert(false);
            }
            else if(INTER[++i]=="if") bc++;
            else if(INTER[i]=="end"&&INTER[i+1]=="if"){
              if(--bc==0) break;
              i++;
            }
          }
          INTER.erase(INTER.begin()+i,INTER.begin()+i+3);
          //Setup end
          insertv.clear();
          insertv.push_back("label");
          insertv.push_back(".if"+to_string(IF_COUNT));
          insertv.push_back(";");
          INTER.insert(INTER.begin()+i,insertv.begin(), insertv.end());
          i = beginning+1;
        }
        else if(s=="else"){
          int beginning = i;
          INTER.erase(INTER.begin()+i,INTER.begin()+i+2);
          vector<string> insertv;
          insertv.push_back("jmp");
          insertv.push_back(".escape");
          insertv.push_back(";");
          i-=3;
          int escape_index = i+1;
          INTER.insert(INTER.begin()+i, insertv.begin(), insertv.end());
          int bc = 1;
          string esc;
          while(true){
            if(i+1>=INTER.size()){
              cerr << "\033[1;31mDid not find end of else.\033[0m\n";
              assert(false);
            }
            else if(INTER[++i]=="else") bc++;
            else if(INTER[i]=="end"&&INTER[i+1]=="else"){
              if(--bc==0){
                esc = INTER[i+2];
                break;
              }
              i++;
            }
          }
          INTER[escape_index]+=esc;
          INTER.erase(INTER.begin()+i,INTER.begin()+i+4);
          insertv.clear();
          insertv.push_back("label");
          insertv.push_back(".escape"+esc);
          insertv.push_back(";");
          INTER.insert(INTER.begin()+i,insertv.begin(), insertv.end());
          i = beginning+1;
        }
        else if(s=="else if"){
          //Save start
          int beginning = i;
          //Place escape
          vector<string> insertv;
          insertv.push_back("jmp");
          insertv.push_back(".escape");
          insertv.push_back(";");
          i-=3;
          int escape_index = i+1;
          INTER.insert(INTER.begin()+i, insertv.begin(), insertv.end());
          insertv.clear();
          beginning+=3;
          i = beginning;
          //Erase and get info
          if(INTER[++i]!="("){
            cerr << "\033[1;31mNo open paren with else if dec.\033[0m\n";
            assert(false);
          }
          vector<string> expression;
          while(INTER[++i]!=")") expression.push_back(INTER[i]);
          i++;
          INTER.erase(INTER.begin()+beginning,INTER.begin()+i+1);
          i = beginning;
          //Set up the beginning of the else if
          if(expression.size()==1){
            insertv.push_back("jmpc");
            insertv.push_back(".elif"+to_string(++ELIF_COUNT));
            insertv.push_back("!");
            insertv.push_back(expression[0]);
            insertv.push_back(";");
          }
          else{
            insertv.push_back("jmpc");
            insertv.push_back(".elif"+to_string(++ELIF_COUNT));
            insertv.push_back("!");
            for(string &s : expression){
              insertv.push_back(s);
            }
            insertv.push_back(";");
          }
          INTER.insert(INTER.begin()+i,insertv.begin(), insertv.end());
          //else part
          int bc = 1;
          string esc;
          bool last;
          while(true){
            if(i+1>=INTER.size()){
              cerr << "\033[1;31mDid not find end of else if.\033[0m\n";
              assert(false);
            }
            else if(INTER[++i]=="else if") bc++;
            else if(INTER[i]=="end"&&INTER[i+1]=="else if"){
              if(--bc==0){
                esc = INTER[i+2];
                last = INTER[i+3]=="1";
                break;
              }
              i++;
            }
          }
          INTER[escape_index]+=esc;
          INTER.erase(INTER.begin()+i,INTER.begin()+i+5);
          insertv.clear();
          if(last){
            insertv.push_back("label");
            insertv.push_back(".escape"+esc);
            insertv.push_back(";");
          }
          insertv.push_back("label");
          insertv.push_back(".elif"+to_string(ELIF_COUNT));
          insertv.push_back(";");
          INTER.insert(INTER.begin()+i,insertv.begin(), insertv.end());
          i = beginning;
        }
      }
    }
  };
  
}

#endif