#include "lct.hpp"

#include <iostream>
#include <vector>

int main() {

  std::ios_base::sync_with_stdio(false);
  std::cin.tie(NULL);

  using namespace std;
  
  vector<shared_ptr<Node>> nodes;

  int n = 10;
    
  for (int i = 0; i < n; ++i) {
    nodes.push_back(make_shared<Node>());
    nodes.back()->val = i;
  }

  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < n; ++j) {
      bool ret = nodes[i]->is_same_repr_tree(nodes[j].get());
      if(i==j){
        assert(ret);
      }else{
        assert(!ret);
      }
    }
  }

  nodes[0]->link(nodes[1].get());
  nodes[1]->link(nodes[2].get());
  nodes[1]->link(nodes[3].get());
  nodes[3]->link(nodes[4].get());
  nodes[3]->link(nodes[5].get());
  nodes[2]->link(nodes[6].get());    

  assert(nodes[0]->is_same_repr_tree(nodes[1].get()));
  assert(nodes[0]->is_same_repr_tree(nodes[2].get()));
  assert(nodes[0]->is_same_repr_tree(nodes[3].get()));
  assert(nodes[2]->is_same_repr_tree(nodes[3].get()));
  assert(nodes[0]->is_same_repr_tree(nodes[4].get()));
  assert(nodes[0]->is_same_repr_tree(nodes[5].get()));
  assert(nodes[0]->is_same_repr_tree(nodes[6].get()));

  nodes[1]->cut(nodes[3].get());
  assert(!nodes[3]->is_same_repr_tree(nodes[1].get()));
  assert(nodes[3]->is_same_repr_tree(nodes[4].get()));
  assert(nodes[3]->is_same_repr_tree(nodes[5].get()));
  assert(!nodes[4]->is_same_repr_tree(nodes[6].get()));
  assert(!nodes[4]->is_same_repr_tree(nodes[0].get()));

  nodes[3]->link(nodes[1].get());

  nodes[0]->reroot();
  assert(nodes[3].get() == nodes[4]->lca(nodes[5].get()));
  assert(nodes[1].get() == nodes[2]->lca(nodes[3].get()));
  assert(nodes[1].get() == nodes[4]->lca(nodes[6].get()));

  nodes[5]->reroot();
  assert(nodes[3].get() == nodes[4]->lca(nodes[6].get()));

  nodes[3]->reroot();
  assert(nodes[3].get() == nodes[4]->lca(nodes[6].get()));
  assert(nodes[1].get() == nodes[6]->lca(nodes[1].get()));

  cout << "test complete!\n";
  return 0;
}
