#include <cstdio>
#include <fstream>
#include <string>
#include <unistd.h>
#include "rb_tree.hpp"

int	main(int argc, char **argv)
{
	bool dot = argc > 1 && std::string(argv[1]) == "dot";
	std::vector<rb_node> nodes;
	rb_node *root = nullptr;
	fancy::tree<rb_node> tree;

	nodes.reserve(8);
	for (int i = 1; i <= 8; i++)
	{
		nodes.push_back(rb_node(i));
		insert(root, &nodes.back());
		if (dot)
		{
			char name[32];
			std::snprintf(name, sizeof(name), "step_%02d.dot", i);
			std::ofstream file(name);
			tree.print_dot(root, file);
			continue;
		}
		std::cout << "\033[2J\033[H" << "insert " << i << std::endl;
		tree.print_tree(root, fancy::V_VIEW);
		usleep(1300000);
	}
}
