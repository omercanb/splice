#include <chrono>
#include "types.h"
#include "exceptions.h"
#include "finally.h"
#include "truthy.h"
#include "iter.h"
#include "tuple.h"
#include "ptr.h"
#include "slice.h"
#include "list.h"
#include "strops.h"
#include "bytes.h"
#include "dict.h"
#include "set.h"
#include "file.h"
#include "print.h"
#include "scalars.h"
#include "mathops.h"
#include "builtins.h"
using namespace py;
class Tree;

void binary_trees();
void __init_module__();

class Tree {
  public:
    ptr<Tree> tree;
    std::optional<ptr<Tree>> left;
    std::optional<ptr<Tree>> right;

    Tree(_int depth) { __init__(depth); }

    void __init__(_int depth);
    _int check();
};

void Tree::__init__(_int depth) {
    this->tree = ptr(new Tree(0LL));
    if (to_bool(((depth == 0LL)))) {
        this->left = std::nullopt;
        this->right = std::nullopt;
    } else {
        this->left = ptr(new Tree((depth - 1LL)));
        this->right = ptr(new Tree((depth - 1LL)));
    }
}

_int Tree::check() {
    if (to_bool(((this->left is not std::nullopt)))) {
        if (!(to_bool(((this->right is not std::nullopt))))) throw AssertionError("");
        return ((1LL + this->left->check()) + this->right->check());
    } else {
        return 1LL;
    }
}

void __init_module__() {
}

void binary_trees() {
    _int min_depth;
    _int max_depth;
    _int stretch_depth;
    ptr<Tree> long_lived_tree;
    _int d;
    auto iterations;
    _int check;
    _int i;
    min_depth = 4LL;
    max_depth = 10LL;
    stretch_depth = (max_depth + 1LL);
    print(str("stretch tree of depth {} check: {}").format(stretch_depth, ptr(new Tree(stretch_depth))->check()));
    long_lived_tree = ptr(new Tree(max_depth));
    _int __stop_0 = stretch_depth;
    for (d = min_depth; d < __stop_0; d += 2) {
        iterations = pow(2LL, ((max_depth + min_depth) - d));
        check = 0LL;
        auto __stop_1 = (iterations + 1LL);
        for (i = 1LL; i < __stop_1; ++i) {
            check += ptr(new Tree(d))->check();
        }
        print(str("{} trees of depth {} check: {}").format(iterations, d, check));
    }
    print(str("long lived tree of depth {} check: {}").format(max_depth, long_lived_tree->check()));
}


    int main() {
        __init_module__();
        auto t0 = std::chrono::steady_clock::now();
        binary_trees(); // Call the benchmarked function
        auto t1 = std::chrono::steady_clock::now();
        auto s = std::chrono::duration<double>(t1 - t0).count() ;
        std::cout << "elapsed: " << s << '\n';
        return 0;
    }
    