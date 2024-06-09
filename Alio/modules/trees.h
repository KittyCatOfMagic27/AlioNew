#ifndef TREES_H
#define TREES_H

#include <vector>
#include <list>
#include <unordered_map>
#include <algorithm>
#include <cassert>
#include <iostream>

using namespace std;

namespace kt{
  template <class Tx, class Ty>
  class Pair{
    Tx* left;
    Ty* right;
  };

  template <class T>
  class Node{
    public:
    T value;
    void* parent = nullptr;
    void* left = nullptr;
    void* right = nullptr; // Node<T>*
    Node<T>(T v): value(v){}
    Node<T>(){}
  };

  template<class T>
  class BinaryTree{
    public:
      
      list<Node<T>> storage;
      Node<T>* root;
      Node<T>* currentNode;
      
      BinaryTree<T>(Node<T> _root){
        storage.push_back(_root);
        root = &storage.back();
        currentNode = root;
      }
      BinaryTree<T>(){}
      
      Node<T>* push_back(Node<T> x){
        return &storage.push_back(x);
      }
      Node<T>* push_back(T x){
        Node<T> n(x);
        storage.push_back(n);
        return &storage.back();
      }

      void connect_left(Node<T>* x, Node<T>* y){
        x->left = (void*)y;
        y->parent = (void*)x;
      }
      void connect_right(Node<T>* x, Node<T>* y){
        x->right = (void*)y;
        y->parent = (void*)x;
      }

      Pair<Node<T>*, Node<T>*> get_children(Node<T>* x){
        Pair<Node<T>*, Node<T>*> p;
        p.left = (Node<T>*)(x->left);
        p.right = (Node<T>*)(x->right);
        return p;
      }
      Node<T>* get_parent(Node<T>* x){
        return (Node<T>*)x->parent;
      }
      Node<T>* get_left(Node<T>* x){
        return (Node<T>*)x->left;
      }
      Node<T>* get_right(Node<T>* x){
        return (Node<T>*)x->right;
      }
      Node<T>* get_currentNode(){
        return currentNode;
      }

      Node<T>* update_currentNode_left(T value){
        Node<T> newNode(value);
        Node<T>* n = push_back(newNode);
        connect_left(currentNode, n);
        currentNode = n;
        return currentNode;
      }

      Node<T>* update_currentNode_right(T value){
        Node<T> newNode(value);
        Node<T>* n = push_back(newNode);
        connect_right(currentNode, n);
        currentNode = n;
        return currentNode;
      }

      Node<T>* move_currentNode_parent(){
        currentNode = get_parent(currentNode);
        return currentNode;
      }

      Node<T>* specialTreeTraversal(int steps){
        Node<T>* retnode = nullptr;
        Node<T>* curnode = root;
        //while(get_left(curnode)!=nullptr) curnode = get_left(curnode);
        vector<Node<T>*> history({curnode});
        for(int i = 0; i < steps; i++){
          curnode = specialTreeTraversalStep(curnode, history);
        }
        return curnode;
      }

      private:

      Node<T>* specialTreeTraversalStep(Node<T>* curnode, vector<Node<T>*> &history){
        Node<T>* retnode = nullptr;
        if(find(history.begin(), history.end(), (Node<T>*)curnode->left) == history.end() && (curnode->left!=nullptr)){
          retnode = (Node<T>*)curnode->left;
        }
        else if(find(history.begin(), history.end(), (Node<T>*)curnode->right) == history.end() && (curnode->right!=nullptr)){
          retnode = (Node<T>*)curnode->right;
        }
        else {
          int counter = 0;
          for(Node<T>* n : history){
            if(n == (void*)curnode->parent)counter++;
          }
          if(counter < 3 && (curnode->parent!=nullptr)) retnode = (Node<T>*)curnode->parent;
        }
        history.push_back(retnode);
        return retnode;
      }
  };

  template <class T>
  class FlippedNode{
    public:
    T value;
    void* leftparent = nullptr;
    void* rightparent = nullptr;
    void* child = nullptr; // FlippedNodeNode<T>*
    FlippedNode<T>(T v): value(v){}
    FlippedNode<T>(){}
  };

  template<class T>
  class FlippedBinaryTree{
    public:
      
      list<FlippedNode<T>> storage;
      vector<FlippedNode<T>*> roots;
      FlippedBinaryTree<T>* currentNode;
      
      FlippedBinaryTree<T>(BinaryTree<T> tree){
        // if a node has no children it is a new root
         vector<Node<T>*> made_nodes;
         vector<FlippedNode<T>*> corresponding_nodes;
        recursiveNodeToFlippedNode(tree.root, made_nodes, corresponding_nodes);
      }
      // FlippedBinaryTree<T>(){}

      FlippedNode<T>* push_back(T x){
        FlippedNode<T> n(x);
        storage.push_back(n);
        return &storage.back();
      }


    private:
    //make ahahahah 
      FlippedNode<T>* recursiveNodeToFlippedNode(Node<T>* n, vector<Node<T>*> &made_nodes, vector<FlippedNode<T>*> &corresponding_nodes){
        FlippedNode<T>* fn = push_back(n->value);
        made_nodes.push_back(n);
        corresponding_nodes.push_back(fn);
        if(n->left == nullptr && n->right == nullptr){
          roots.push_back(fn);
        }
        else{
          if(n->left != nullptr){
            auto iter = find(made_nodes.begin(), made_nodes.end(), (Node<T>*)n->left);
            if(iter != made_nodes.end()) fn->leftparent = corresponding_nodes[iter-made_nodes.begin()];
            else fn->leftparent = recursiveNodeToFlippedNode((Node<T>*)n->left, made_nodes, corresponding_nodes);
          } 
          if(n->right != nullptr){
            auto iter = find(made_nodes.begin(), made_nodes.end(), (Node<T>*)n->right);
            if(iter != made_nodes.end()) fn->rightparent = corresponding_nodes[iter-made_nodes.begin()];
            else fn->rightparent = recursiveNodeToFlippedNode((Node<T>*)n->right, made_nodes, corresponding_nodes);
          }
        }
        if(n->parent != nullptr){
          auto iter = find(made_nodes.begin(), made_nodes.end(), (Node<T>*)n->parent);
          if(iter != made_nodes.end()) fn->child = corresponding_nodes[iter-made_nodes.begin()];
          else fn->child = recursiveNodeToFlippedNode((Node<T>*)n->parent, made_nodes, corresponding_nodes);
        }
        return fn;
      }
      /*
      Node<T>* push_back(Node<T> x){
        return &storage.push_back(x);
      }
      Node<T>* push_back(T x){
        Node<T> n(x);
        storage.push_back(n);
        return &storage.back();
      }

      void connect_left(Node<T>* x, Node<T>* y){
        x->left = (void*)y;
        y->parent = (void*)x;
      }
      void connect_right(Node<T>* x, Node<T>* y){
        x->right = (void*)y;
        y->parent = (void*)x;
      }

      Pair<Node<T>*, Node<T>*> get_children(Node<T>* x){
        Pair<Node<T>*, Node<T>*> p;
        p.left = (Node<T>*)(x->left);
        p.right = (Node<T>*)(x->right);
        return p;
      }
      Node<T>* get_parent(Node<T>* x){
        return (Node<T>*)x->parent;
      }
      Node<T>* get_left(Node<T>* x){
        return (Node<T>*)x->left;
      }
      Node<T>* get_right(Node<T>* x){
        return (Node<T>*)x->right;
      }
      Node<T>* get_currentNode(){
        return currentNode;
      }

      Node<T>* update_currentNode_left(T value){
        Node<T> newNode(value);
        Node<T>* n = push_back(newNode);
        connect_left(currentNode, n);
        currentNode = n;
        return currentNode;
      }

      Node<T>* update_currentNode_right(T value){
        Node<T> newNode(value);
        Node<T>* n = push_back(newNode);
        connect_right(currentNode, n);
        currentNode = n;
        return currentNode;
      }

      Node<T>* move_currentNode_parent(){
        currentNode = get_parent(currentNode);
        return currentNode;
      }

      Node<T>* specialTreeTraversal(int steps){
        Node<T>* retnode = nullptr;
        Node<T>* curnode = root;
        //while(get_left(curnode)!=nullptr) curnode = get_left(curnode);
        vector<Node<T>*> history({curnode});
        for(int i = 0; i < steps; i++){
          curnode = specialTreeTraversalStep(curnode, history);
        }
        return curnode;
      }

      private:

      Node<T>* specialTreeTraversalStep(Node<T>* curnode, vector<Node<T>*> &history){
        Node<T>* retnode = nullptr;
        if(find(history.begin(), history.end(), (Node<T>*)curnode->left) == history.end() && (curnode->left!=nullptr)){
          retnode = (Node<T>*)curnode->left;
        }
        else if(find(history.begin(), history.end(), (Node<T>*)curnode->right) == history.end() && (curnode->right!=nullptr)){
          retnode = (Node<T>*)curnode->right;
        }
        else {
          int counter = 0;
          for(Node<T>* n : history){
            if(n == (void*)curnode->parent)counter++;
          }
          if(counter < 3 && (curnode->parent!=nullptr)) retnode = (Node<T>*)curnode->parent;
        }
        history.push_back(retnode);
        return retnode;
      }
      */
  };
}
#endif