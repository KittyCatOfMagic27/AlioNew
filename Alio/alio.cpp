#include <cstdlib>
#include <cstdio>
#include <sstream>
#include <string>
#include "./modules/trees.h"
#include "./modules/options.h"
#include "./modules/error.h"
#include "./modules/lexer.h"
#include "./modules/intermediate.h"
#include "./modules/compiler.h"
#include "./modules/flags.h"
#include "./modules/operators.h"


//Functions for command line flags
namespace flags{
  void ALIO_FILE(string FILENAME){
    FILENAME.erase(FILENAME.size()-1);
    if(FILENAME.find(".alio") != string::npos){
      FILENAME.erase(FILENAME.size()-5);
      options::FILE_BASENAME = FILENAME;
    }else{
      options::FILE_BASENAME = FILENAME;
    }
  }
  
  void INTER_FILE(string FILENAME){
    FILENAME.erase(FILENAME.size()-1);
    options::INTER_FILE = FILENAME;
  }
  
  void TARGETFLAG(string TARGET){
    TARGET.erase(TARGET.size()-1);
    if(TARGET=="elf64")
      options::target = options::X86_64;
    else if(TARGET=="elf32")
      options::target = options::X86_I386;
    else{
      cerr << "\033[1;31mUnsupported target given '"<<TARGET<<"'.\033[0m\n";
      assert(false);
    }
  }
  
  void EXEC_FILE_SET(string EXEC_FILENAME){
    options::EXEC_FILE = EXEC_FILENAME;
  }
  
  void ASMFLAG(string NA){
    options::KEEP_ASM = true;
  }
  
  void DEBUGFLAG(string NA){
    options::DEBUGMODE = true;
  }
  
  void INTERMODE(string NA){
    options::INTERMODE = true;
  }
  
  void WIN32FLAG(string NA){
    options::OSTYPE = options::WIN32;
  }
  
  void LIBCFLAG(string NA){
    options::LIBC = true;
  }

  void FLAVORFLAG(string NA){
    options::FLAVORTEXT = true;
  }

  void STAGEFLAG(string NA){
    options::STAGETEXT = true;
  }

  void AUTORUNFLAG(string NA){
    options::AUTORUN = true;
  }
}

kf::FlagProxy setup_flags(){
  //Command line flags
  string help_desc = "This is the Alio help desc!\nYou can bring up this page by using the --help flag.";
  kf::FlagProxy prox(help_desc);
  
  string file_flag = "-f";
  string file_flag_desc = "Provides .alio file to compiler (Required).";
  prox.AddFlag(file_flag, flags::ALIO_FILE, file_flag_desc, 1);
  prox.MakeFlagRequired(file_flag);
  
  string inter_flag = "-i";
  string inter_flag_desc = "Switches to inter mode.";
  prox.AddFlag(inter_flag, flags::INTERMODE, inter_flag_desc, 0);
  
  string exec_flag = "-o";
  string exec_flag_desc = "Name the exec file.";
  prox.AddFlag(exec_flag, flags::EXEC_FILE_SET, exec_flag_desc, 1);
  
  string ointer_flag = "-oi";
  string ointer_flag_desc = "Puts intermediate into given file.";
  prox.AddFlag(ointer_flag, flags::INTER_FILE, ointer_flag_desc, 1);
  
  string target_flag = "-t";
  string target_flag_desc = "Provides the target to the compiler. (elf64 or elf32)";
  prox.AddFlag(target_flag, flags::TARGETFLAG, target_flag_desc, 1);
  
  string debug_flag = "-d";
  string debug_flag_desc = "Prints debug information while compiling.";
  prox.AddFlag(debug_flag, flags::DEBUGFLAG, debug_flag_desc, 0);
  
  string asm_flag = "-asm";
  string asm_flag_desc = "Keeps assembly code after compilation.";
  prox.AddFlag(asm_flag, flags::ASMFLAG, asm_flag_desc, 0);
  
  string win32_flag = "-win32";
  string win32_flag_desc = "Sets compilation to win32 mode. (Currently does not work)";
  prox.AddFlag(win32_flag, flags::WIN32FLAG, win32_flag_desc, 0);
  
  string libc_flag = "-libc";
  string libc_flag_desc = "Includes libc. (Currently does not work)";
  prox.AddFlag(libc_flag, flags::LIBCFLAG, libc_flag_desc, 0);

  string flavor_flag = "-flavor";
  string flavor_flag_desc = "Outputs flavor text during compilation.";
  prox.AddFlag(flavor_flag, flags::FLAVORFLAG, flavor_flag_desc, 0);

  string stages_flag = "-stages";
  string stages_flag_desc = "Outputs stage text during compilation.";
  prox.AddFlag(stages_flag, flags::STAGEFLAG, stages_flag_desc, 0);

  string auto_run_flag = "-autor";
  string auto_run_flag_desc = "Auto runs the program after compilation.";
  prox.AddFlag(auto_run_flag, flags::AUTORUNFLAG, auto_run_flag_desc, 0);
  
  
  prox.AddHelp("For additional info check README.md /home/$USR/ALIO/.\n(Powered by KFlags)");
  return prox;
}

int main(int argc, char** argv){
  kf::FlagProxy prox = setup_flags();
  prox.Parse(argc, argv);
  
  //Hello Alio
  if(options::FLAVORTEXT) printf("Hello to Another\n");
  
  //Files and SS
  string asm_file  = options::FILE_BASENAME+".asm";
  string alio_file = options::FILE_BASENAME+".alio";
  stringstream ss;

  //Initialize Operators
  list<ops::OpSpec> oplist;
  ops::InitOperators(oplist);
  
  //Compile Alio
  if(options::INTERMODE){
    comp::COMPILER *compiler = new comp::COMPILER(asm_file, alio_file.c_str());
    compiler->run();
    delete compiler;
  }
  else{
    lex::LEXER *lexer = new lex::LEXER(alio_file.c_str(), &oplist);
    vector<lex::Token> Tokens = lexer->run();
    delete lexer;
    intr::INTERMEDIATE *intr = new intr::INTERMEDIATE(Tokens, &oplist);
    vector<string> INTER = intr->run();
    delete intr;
    comp::COMPILER *compiler = new comp::COMPILER(asm_file, INTER, &oplist);
    compiler->run();
    delete compiler;
  }
  //Additional Clean up
  
  
  string elf_target;
  string linker_target;
  switch(options::target){
    case options::X86_I386:
    elf_target    = "elf32";
    linker_target = "elf_i386";
    break;
    case options::X86_64:
    default:
    elf_target    = "elf64";
    linker_target = "elf_x86_64";
    if(options::OSTYPE==options::WIN32){
      elf_target  = "win64";
      // linker_target = "64";
    }
    break;
  }
  //Compile nasm
  ss << "nasm -f "<<elf_target<<" -o "<< options::FILE_BASENAME << ".o " << asm_file;
  string cmd_line = ss.str();
  const char* char_line = cmd_line.c_str();
  ss.str("");
  system(char_line);
  cerr << char_line << "\n";
  
  //Link Program
  if(!options::LIBC){
    ss << "ld -m "<<linker_target<<" -e "<< options::ENTRYPOINT <<" -o " << (options::EXEC_FILE=="" ? options::FILE_BASENAME : options::EXEC_FILE) << " " << options::FILE_BASENAME << ".o ";
  }
  else{
    ss << "gcc -no-pie -Wl,-m"<<linker_target<<" -e "<< options::ENTRYPOINT <<" -o " << (options::EXEC_FILE=="" ? options::FILE_BASENAME : options::EXEC_FILE) << " " << options::FILE_BASENAME << ".o ";
  }
  cmd_line = ss.str();
  char_line = cmd_line.c_str();
  ss.str("");
  system(char_line);
  cerr << char_line << "\n";
  
  //Run
  if(options::AUTORUN){
    cout << "PROGRAM OUTPUT:\n";
    ss << "./" << (options::EXEC_FILE=="" ? options::FILE_BASENAME : options::EXEC_FILE);
    cmd_line = ss.str();
    char_line = cmd_line.c_str();
    ss.str("");
    system(char_line);
  }
  
  //Deletes .o file
  ss << "shred -u " << options::FILE_BASENAME << ".o";
  cmd_line = ss.str();
  char_line = cmd_line.c_str();
  ss.str("");
  system(char_line);
  
  //Deletes .asm file
  if(!options::KEEP_ASM){
    ss << "shred -u " << asm_file;
    cmd_line = ss.str();
    char_line = cmd_line.c_str();
    ss.str("");
    system(char_line);
  }
  

  return 0;
}