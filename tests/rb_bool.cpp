#include <string>

struct entry {
	std::string	key;
	bool		is_red;
	entry		*left;
	entry		*right;

	entry(const std::string &k, bool red) : key(k), is_red(red), left(nullptr), right(nullptr) {}
};

#define FANCY_TREE_LEFT left
#define FANCY_TREE_RIGHT right
#define FANCY_TREE_VALUE key
#define FANCY_TREE_COLOR is_red
#define FANCY_TREE_RED true
#include "fancy_tree.hpp"

int	main()
{
	entry root("mango", false);
	entry apple("apple", true);
	entry zucchini("zucchini", true);
	entry kiwi("kiwi", false);
	root.left = &apple;
	root.right = &zucchini;
	apple.right = &kiwi;

	fancy::tree<entry> tree;
	tree.print_tree(&root, fancy::V_VIEW);
	tree.print_tree(&root, fancy::H_VIEW);
}
