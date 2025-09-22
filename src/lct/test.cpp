#include "lct.hpp"

#include <iostream>
#include <vector>

int main() {

  std::ios_base::sync_with_stdio(false);
  std::cin.tie(NULL);

  using namespace std;

  int n = 10;
  vector<shared_ptr<Node>> nodes;  
  
  for (int i = 0; i < n; ++i) {
    nodes.push_back(make_shared<Node>(i));
  }

  // Test: Initially all nodes are isolated
  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < n; ++j) {
      bool ret = nodes[i]->is_connected(nodes[j].get());
      if(i==j){
        assert(ret);
      }else{
        assert(!ret);
      }
    }
  }

  // Build tree:
  //     0
  //     |
  //     1
  //    / \
  //   2   3
  //   |   / \
  //   6  4   5
  
  nodes[0]->link(nodes[1].get());
  assert(nodes[0]->sum == 1);
  
  nodes[1]->link(nodes[2].get());
  assert(nodes[0]->reroot()->sum == 3);
  
  nodes[1]->link(nodes[3].get());
  assert(nodes[0]->reroot()->sum == 6);
  
  nodes[3]->link(nodes[4].get());
  assert(nodes[0]->reroot()->sum == 10);
  
  nodes[3]->link(nodes[5].get());
  assert(nodes[0]->reroot()->sum == 15);
  
  nodes[2]->link(nodes[6].get());
  assert(nodes[0]->reroot()->sum == 21);
  assert(nodes[3]->reroot()->sum == 21);

  // Test connectivity
  assert(nodes[0]->is_connected(nodes[1].get()));
  assert(nodes[0]->is_connected(nodes[2].get()));
  assert(nodes[0]->is_connected(nodes[3].get()));
  assert(nodes[2]->is_connected(nodes[3].get()));
  assert(nodes[0]->is_connected(nodes[4].get()));
  assert(nodes[0]->is_connected(nodes[5].get()));
  assert(nodes[0]->is_connected(nodes[6].get()));

  // Cut edge 1-3:
  //     0           3
  //     |          / \
  //     1         4   5
  //     |
  //     2
  //     |
  //     6

  nodes[1]->cut(nodes[3].get());

  assert(nodes[1]->sum == 9);
  assert(nodes[3]->sum == 12);
  
  assert(!nodes[3]->is_connected(nodes[1].get()));
  assert(nodes[3]->is_connected(nodes[4].get()));
  assert(nodes[3]->is_connected(nodes[5].get()));
  assert(!nodes[4]->is_connected(nodes[6].get()));
  assert(!nodes[4]->is_connected(nodes[0].get()));

  assert(nodes[6]->reroot()->sum == 9);  // 0+1+2+6
  assert(nodes[4]->reroot()->sum == 12); // 3+4+5

  // Re-link 3-1:
  //     0
  //     |
  //     1
  //    / \
  //   2   3
  //   |   / \
  //   6  4   5
  nodes[3]->link(nodes[1].get());

  assert(nodes[1]->reroot()->sum == 21);
  assert(nodes[2]->reroot()->sum == 21);
  
  // Test LCA with root at 0
  nodes[0]->reroot();
  assert(nodes[0].get() == nodes[1]->lca(nodes[0].get()));
  assert(nodes[0].get() == nodes[0]->lca(nodes[1].get()));
  assert(nodes[3].get() == nodes[4]->lca(nodes[5].get()));
  assert(nodes[1].get() == nodes[2]->lca(nodes[3].get()));
  assert(nodes[1].get() == nodes[4]->lca(nodes[6].get()));

  // Test LCA with root at 5
  nodes[5]->reroot();
  assert(nodes[3].get() == nodes[4]->lca(nodes[6].get()));

  // Test LCA with root at 3
  nodes[3]->reroot();
  assert(nodes[3].get() == nodes[4]->lca(nodes[6].get()));
  assert(nodes[1].get() == nodes[6]->lca(nodes[1].get()));

  cout << "test complete!\n";
  return 0;
}
