#include "rb_tree.hpp"

int	main()
{
	std::vector<rb_node> nodes;
	rb_node *root = nullptr;

	nodes.reserve(10);
	for (int i = 1; i <= 10; i++)
	{
		nodes.push_back(rb_node(i));
		insert(root, &nodes.back());
	}
	fancy::tree<rb_node> tree;
	tree.print_tree(root, fancy::V_VIEW);
	tree.print_tree(root, fancy::H_VIEW);
	tree.print_dot(root);
}
