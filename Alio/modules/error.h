#ifndef ERROR_H
#define ERROR_H

#include <stdbool.h>
#include <string>
#include <vector>
#include <cassert>
#include <iostream>
#include <list>

using namespace std;

namespace errh{
  struct Split {
    void* n;
    int split;
  };

  struct FileNode {
    unsigned int start;
    unsigned int length;
    string filename;
    vector<Split> splits;
    void* parent;
  };

  struct ErrorInfo{
    int line = 0;
    string filename = "";
    bool operator==(const ErrorInfo &e){
      if(line != e.line || filename != e.filename) return false;
      return true;
    }
  };

  class FileTree{
    public: 
    FileNode root;
    list<FileNode> Files;
    FileTree(int filename, unsigned int size){
      root.filename = filename;
      root.length = size;
      root.start = 0;
      root.parent = nullptr;
    }

    void init(string filename, unsigned int size){
      root.filename = filename;
      root.length = size;
      root.start = 0;
      root.parent = nullptr;
    }

    FileTree(){}

    ErrorInfo get_ErrorInfo(int line){
      ErrorInfo errinfo;
      errinfo = __get_ErrorInfo(line, &root);
      return errinfo;
    }

    FileNode* getNodeOfLine(int line){
      FileNode* outnode;
      outnode = __getNodeOfLine(line, &root);
      return outnode;
    }

    void add_file(string filename, int start, int length){
      FileNode node;
      node.parent = getNodeOfLine(start);
      node.length = length;
      node.filename = filename;
      node.start = start;
      Files.push_back(node);
      ((FileNode*)node.parent)->splits.push_back((Split){&Files.back(), start});
    }

    private: 
    ErrorInfo __get_ErrorInfo(int line, FileNode* n){
      ErrorInfo errinfo = {};
      int lengthOfSkipped = 0;
      for(int i = 0; i < n->splits.size(); i++){
        if(n->start + line > n->splits[i].split){
          int length = lengthOf((FileNode*)n->splits[i].n);
          if(length + n->start > line) {errinfo = __get_ErrorInfo(line, (FileNode*)n->splits[i].n); break; }
          else lengthOfSkipped+=length;
        }
      }
      errinfo = errinfo == (ErrorInfo){0, ""} ? (ErrorInfo){line-lengthOfSkipped, n->filename} : errinfo;
      return errinfo;
    }

    FileNode* __getNodeOfLine(int line, FileNode* n){
      FileNode* outnode = nullptr;
      int lengthOfSkipped = 0;
      for(int i = 0; i < n->splits.size(); i++){
        if(n->start + line > n->splits[i].split){
          int length = lengthOf((FileNode*)n->splits[i].n);
          if(length + n->start > line) {outnode = __getNodeOfLine(line, (FileNode*)n->splits[i].n); break; }
          else lengthOfSkipped+=length;
        }
      }
      outnode = outnode == nullptr ? n : outnode;
      return outnode;
    }

    int lengthOf(FileNode* n){
      int sum = n->length;
      for(Split split : n->splits){
        sum += lengthOf((FileNode*)split.n);
      }
      return sum;
    }

  };

  //Get file ect.
  string formatErrorInfo(ErrorInfo err){
    string s;
    s+=err.filename+":";
    s+=to_string(err.line);
    s+=":";
    return s;
  }
  
  #define startErrorThrow(tk) \
    cerr << "\033[1;31m" << errh::formatErrorInfo(lex::Files.get_ErrorInfo(tk.line));
  
  #define endErrorThrow() \
    cerr << "\033[0m\n";
}

#endif