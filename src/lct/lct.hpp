/// link-cut tree with splay tree as auxiliary tree

#pragma once

#include <memory>
#include <cassert>
#include <optional>

using namespace std;

struct Node {

  static constexpr int Left = 0;
  static constexpr int Right = 1;
  
  struct TyExposeRet {
    Node * node = nullptr;
    Node * last_path_parent = nullptr;
  };
  int val = 0;
  Node * ns[2] = {}; // aux. (splay tree) children links
  Node * p = nullptr; //link to parent within aux. tree
  // inter path (aux. tree) connection:
  //   either points to parent, or
  //   nullptr if current path is the top of the hierarchy
  Node * path_parent = nullptr;
  // used for re-rooting a node; need to reverse left and right child when set to true
  bool reverse = false; 
  //---helpers
  Node * & nl(){
    return ns[Left];
  }
  Node * & nr(){
    return ns[Right];
  }
  Node(int v, Node* parent): val(v), p(parent) {}
  Node(){}
  
  // make an edge between current node and child node
  // this assumes current and child node are in disjoint represented trees
  void link(Node * other){
    assert(!is_same_repr_tree(other));

    // make current node available in the top aux. tree of its represented tree
    // reroot current node
    reroot();

    // make child available in the top aux. tree of its represented tree
    other->expose();
    // since current node is rerooted, it's the node with lowest depth => left other in aux. tree is empty
    assert(!nl()); 
    // attach other node as the left other of current node in current aux. tree
    assert(!other->p);
    assert(!other->path_parent);
    nl() = other;
    other->p = this;
  }
  // cut current node and its parent, making 2 separate represented trees
  // this assumes the parent exists
  void cut(){
    //make the parent node available in the same aux. tree as the current node
    expose();

    // unlink edge between current node and parent node (left child of current node in aux. tree)
    assert(!nr());
    assert(nl());
    // parent node's path_parent is preserved
    nl()->p = nullptr;
    nl() = nullptr;
  }
  void cut(Node * other){
    reroot();
    other->cut();
  }
  // get the root of the represented tree
  Node * get_root(){
    // make root node available in the same aux. tree as the current node
    expose();
    // root is the left-most node, also make root splayed for faster access
    return min()->splay();
  }
  void propagate(){
    if(!reverse){
      return;
    }
    swap(ns[0], ns[1]);
    if(ns[0]){
      ns[0]->reverse ^= true;
    }
    if(ns[1]){
      ns[1]->reverse ^= true;
    }
    reverse = false;
  }
  Node * reroot(){
    expose();
    reverse ^= true;
    propagate();
    return this;
  }
  // - make a preferred path from the root of the represented tree to the current node
  // - current node is splayed to the top of the aux. tree
  // - the last path parent pointer is also returned
  TyExposeRet expose(){
    // unmark path to child in represented tree
    splay();
    // split right child aux. tree from current aux. tree
    this->split_aux_tree();

    Node * old_path_parent = path_parent;
    while(path_parent){
      old_path_parent = path_parent; //save this

      Node * pp = path_parent->splay();
      // split current node's path_parent's aux tree: it's right child becomes a new aux. tree, detach it
      {
	pp->split_aux_tree();
      }
      
      // merge current node's path_parent's aux. tree and current node's aux tree
      {
	// cur->path_parent's right child is empty since we just detached it earlier
	assert(!pp->nr());
	pp->attach_child(Right, this);
      }
      splay(); //this will update current node's path_parent
      // repeat the process to merge ancestor paths' aux. trees on way to root of represented tree
    }
    return TyExposeRet {
      .node = this,
      .last_path_parent = old_path_parent,
    };
  }
  // split right subtree of current aux tree into a new aux tree
  void split_aux_tree(){
    propagate();
    if(!nr()){
      return;
    }
    nr()->path_parent = this;
    nr()->p = nullptr;
    nr() = nullptr;
  }
  // attach node as right child within current aux tree
  void attach_child(int side, Node * child){
    propagate();
    ns[side] = child;
    if(child){
      child->path_parent = nullptr;
      child->p = this;
    }
  }
  bool is_same_repr_tree(Node * other){
    auto root0 = get_root();
    auto root1 = other->get_root();
    return root0 == root1;
  }
  Node * lca(Node * other){
    assert(is_same_repr_tree(other));
    other->expose();
    auto [_, last_path_parent] = expose();
    if(!last_path_parent){
      return this; //other is below current node
    }
    return last_path_parent;
  }
  int get_branch(){
    if(!p){
      return 0;
    }
    if(p->nl() == this){ //left
      return 0;
    }
    assert(p->nr() == this); //right
    return 1;
  }
  //rotate parent left/right (depending on dir) and let it become current node's (left/right) child
  void rot(int dir){ //dir: 0=>left, 1=>right
    auto pp = p->p;
    auto p_branch = p->get_branch();
    if(pp){
      pp->ns[p_branch] = this;
    }
    p->ns[!dir] = std::move(ns[dir]);
    if(p->ns[!dir]){
      p->ns[!dir]->p = p;
    }
    ns[dir] = p;
    ns[dir]->p = this;
    path_parent = p->path_parent;
    p = pp;
  }
  Node * splay(){
    while(p){
      if(p->p){
        // right-right/left-left or right-left/left-right case
        p->p->propagate();
        p->propagate();
        propagate();
        int my_dir = get_branch();
        int parent_dir = p->get_branch();
        if(my_dir == parent_dir){
          // right-right/left-left case: rotate parent first, then self
          p->rot(!parent_dir);
          rot(!my_dir);
        }else{
          // right-left/left-right case: rotate self twice
          rot(!my_dir);
          rot(!parent_dir);
        }
      }else{
        // right/left case: single rotation
        p->propagate();
        propagate();
        rot(!get_branch());
      }
    }
    return this;
  }
  Node * min(){
    propagate();
    if(nl()){
      return nl()->min();
    }
    return this;
  }
  Node * max(){
    propagate();
    if(nr()){
      return nr()->max();
    }
    return this;
  }
};
