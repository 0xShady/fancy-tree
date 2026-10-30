#include <string>
#include "test.hpp"
#include "fancy_tree.hpp"

int main()
{
	Node<std::string> root("root");
	Node<std::string> left("a long value");
	Node<std::string> right("été");
	Node<std::string> right_left("1234567");
	Node<std::string> right_right("123456");
	root._left = &left;
	root._right = &right;
	right._left = &right_left;
	right._right = &right_right;

	fancy::tree<Node<std::string> > strings;
	strings.print_tree(&root, fancy::V_VIEW);
	strings.print_tree(&root, fancy::H_VIEW);

	Node<double> single(-3.25);
	fancy::tree<Node<double> > doubles;
	doubles.print_tree(&single, fancy::V_VIEW);
	doubles.print_tree(nullptr, fancy::V_VIEW);

	Node<int> big(-12345678);
	Node<int> zero(0);
	big._left = &zero;
	fancy::tree<Node<int> > ints;
	ints.print_tree(&big, fancy::V_VIEW);
}
