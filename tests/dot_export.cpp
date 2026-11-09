#include <string>
#include "test.hpp"
#include "fancy_tree.hpp"

int main()
{
	Node<std::string> root("root");
	Node<std::string> quote("say \"hi\"");
	Node<std::string> slash("C:\\temp");
	Node<std::string> lone("only right");
	Node<std::string> twin("15");
	Node<std::string> twin_again("15");
	root._left = &quote;
	root._right = &slash;
	quote._right = &lone;
	slash._left = &twin;
	slash._right = &twin_again;

	fancy::tree<Node<std::string> > tree;
	tree.print_dot(&root);
	tree.print_dot(nullptr);
}
