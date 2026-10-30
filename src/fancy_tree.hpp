#pragma once

#include <string>
#include <iostream>
#include <vector>
#include <cmath>

// Node field names, override them before including this header
#ifndef FANCY_TREE_LEFT
# define FANCY_TREE_LEFT _left
#endif
#ifndef FANCY_TREE_RIGHT
# define FANCY_TREE_RIGHT _right
#endif
#ifndef FANCY_TREE_VALUE
# define FANCY_TREE_VALUE _value
#endif

// Constants values
#define LEAFS_SPAN 2
#define NODE_WIDTH 6
#define CONTENT_LEN 4

// Flags
#define H_VIEW 0x00				// for Horizontal view
#define V_VIEW 0x01				// for Vertical view

// vertical view TRUNKS
#define TOP_LEFT_FRAME "┌───┴┐"
#define TOP_RIGHT_FRAME "┌┴───┐"
#define RIGHT_LEAF_FRAME "┌──┴─┐"
#define LEFT_LEAF_FRAME "┌─┴──┐"
#define FRAME_BOTTOM "└────┘"
#define FRAME_TOP "┌────┐"

// horizontal view TRUNKS
#define BIG_TRUNK "    │"
#define SMALL_TRUNK "   │"
#define LEFT_TRUNK "┌──┤"
#define RIGHT_TRUNK "└──┤"
#define BIG_SPAN "    "
#define SMALL_SPAN "   "
#define ROOT_TRUNK "──┤"

// frequently used
#define NEW_LINE std::endl
#define PRINT std::cout

template <typename Node>
class fancy_tree
{

	private:

		/*===================Branch===================*/

		struct	Branch
		{
			Branch		*prev;
			std::string	str;

			Branch(Branch *prev, std::string str)
			{
				this->prev = prev;
				this->str = str;
			}
		};

		/*===================Helper Function===================*/

		// fill nodes from the same level in a vector
		std::vector<Node *>	childs_vectors_filler(std::vector<std::vector<Node *> > vec, int level)
		{
			std::vector<Node *> childs;
			for (size_t i = 0; i < vec[level].size(); i++)
			{
				if (vec[level][i] == nullptr)
				{
					childs.push_back(nullptr);
					childs.push_back(nullptr);
					continue ;
				}
				if (vec[level][i]->FANCY_TREE_LEFT != nullptr)
					childs.push_back(vec[level][i]->FANCY_TREE_LEFT);
				else
					childs.push_back(nullptr);
				if (vec[level][i]->FANCY_TREE_RIGHT != nullptr)
					childs.push_back(vec[level][i]->FANCY_TREE_RIGHT);
				else
					childs.push_back(nullptr);
			}
			return childs;
		}

		// fill levels vectors into a master vector
		std::vector<std::vector<Node *> >	master_vector_filler(Node *root, size_t height)
		{
			size_t level = 0;
			std::vector<std::vector<Node *> > vec;
			std::vector<Node *> first;
			first.push_back(root);
			vec.push_back(first);
			while (level < height)
			{
				vec.push_back(childs_vectors_filler(vec, level));
				level++;
			}
			return vec;
		}

		// set width
		void    set_width(int count, std::string str)
		{
			for (int i = 0; i < count; i++)
				PRINT << str;
		}

		// calculate tree height
		int tree_height(Node *root)
		{
			if (root == nullptr)
				return 0;
			else
			{
				int l_height = tree_height(root->FANCY_TREE_LEFT);
				int r_height = tree_height(root->FANCY_TREE_RIGHT);
				if (l_height > r_height)
					return (l_height + 1);
				else
					return (r_height + 1);
			}
		}

		// calculate value length
		int		int_len(int n)
		{
			int count = 0;
			if (!n)
				return 1;
			if (n < 0)
			{
				count++;
				n = -n;
			}
			while (n != 0)
			{
				n /= 10;
				count++;
			}
			return count;
		}

		// print the content of a node
		void    print_node(Node *node, bool left_child, bool right_child)
		{
			if (!left_child)
				PRINT << "│";
			PRINT << node->FANCY_TREE_VALUE;
			int len = CONTENT_LEN - int_len(node->FANCY_TREE_VALUE);
			set_width(len, " ");
			if (!right_child)
				PRINT << "│";
		}

		// print left branch of a node
		void	print_left_branch(int len)
		{
			PRINT << "┌";
			set_width(len - 1, "─");
			PRINT << "┤";
		}

		// print right branch of a node
		void	print_right_branch(int len)
		{
			PRINT << "├";
			set_width(len - 1, "─");
			PRINT << "┐";
		}

		/*===================Horizontal view===================*/

		// print horizontal view of a tree
		void	showbranches(Branch *p)
		{
			if (p == nullptr)
				return;
			showbranches(p->prev);
		    if (p->str == BIG_SPAN && p->prev && p->prev->str == BIG_TRUNK)
		        PRINT << SMALL_SPAN;
		    else if (p->str == BIG_TRUNK && p->prev && p->prev->str == BIG_TRUNK)
		        PRINT << SMALL_TRUNK;
		    else
		    	PRINT << p->str;
		}

		// fill the tree branches in a linked list
		void	print_tree_horizontal_view(Node *root, Branch *prev, bool is_left)
		{
			if (root == nullptr)
				return;

		    std::string	prev_str = BIG_SPAN;
			Branch *branch = new Branch(prev, prev_str);

			print_tree_horizontal_view(root->FANCY_TREE_RIGHT, branch, true);
			if (!prev)
				branch->str = ROOT_TRUNK;
			else if (is_left)
			{
				branch->str = LEFT_TRUNK;
				prev_str = BIG_TRUNK;
			}
			else
			{
				branch->str = RIGHT_TRUNK;
				prev->str = prev_str;
			}
			showbranches(branch);
			PRINT << root->FANCY_TREE_VALUE << std::endl;
			if (prev)
				prev->str = prev_str;
			branch->str = BIG_TRUNK;
			print_tree_horizontal_view(root->FANCY_TREE_LEFT, branch, false);
		}

		/*===================Vertical view===================*/

		// print all the nodes of the same level
		void	print_level(std::vector<Node *> vec, int level, int height, int factor, int branch_len)
		{
			bool is_first = true;
			for (size_t i = 0; i < vec.size(); i++)
			{
				set_width(level == height ? (pow(2.0, factor) - (is_first ? 1 : LEAFS_SPAN)) : (is_first ? pow(2.0, factor) - 1 - branch_len : pow(2.0, factor + 1) - NODE_WIDTH - (branch_len * 2)), " ");
				if (is_first)
					is_first = false;
				if (vec[i] == nullptr)
				{
					set_width(level == height ? NODE_WIDTH : NODE_WIDTH + branch_len * 2, " ");
					continue;
				}
				if (vec[i]->FANCY_TREE_LEFT != nullptr)
					print_left_branch(branch_len);
				else
					set_width(level != height ? branch_len : 0, " ");
				print_node(vec[i], (vec[i]->FANCY_TREE_LEFT != nullptr), (vec[i]->FANCY_TREE_RIGHT != nullptr));
				if (vec[i]->FANCY_TREE_RIGHT != nullptr)
					print_right_branch(branch_len);
				else
					set_width(level != height ? branch_len : 0, " ");
			}
			PRINT << NEW_LINE;
		}

		// print the branches of nodes
		void	branch_printer(int len, Node *node)
		{
			if (node != nullptr && node->FANCY_TREE_LEFT != nullptr)
				PRINT << TOP_LEFT_FRAME;
			else
				set_width(NODE_WIDTH, " ");
			set_width(len - CONTENT_LEN, " ");
			if (node != nullptr && node->FANCY_TREE_RIGHT != nullptr)
				PRINT << TOP_RIGHT_FRAME;
			else
				set_width(NODE_WIDTH, " ");
		}

		// print the branches of leafs
		void	branch_printer_before_last(Node *node)
		{
			if (node != nullptr && node->FANCY_TREE_LEFT != nullptr)
				PRINT << LEFT_LEAF_FRAME;
			else
				set_width(NODE_WIDTH, " ");
			set_width(LEAFS_SPAN, " ");
			if (node != nullptr && node->FANCY_TREE_RIGHT != nullptr)
				PRINT << RIGHT_LEAF_FRAME;
			else
				set_width(NODE_WIDTH, " ");
		}

		// print down branches
		void	print_vertical_branches_mid(int branch_len, int factor, std::vector<Node *> vec)
		{
			bool is_first = true;
			for (size_t i = 0; i < vec.size(); i++)
			{
				set_width(is_first ? pow(2.0, factor) - 1 - branch_len : pow(2.0, factor + 1) - NODE_WIDTH - (branch_len * 2), " ");
				PRINT << ((vec[i] != nullptr && vec[i]->FANCY_TREE_LEFT != nullptr) ? "│" : " ");
				set_width((branch_len * 2) + CONTENT_LEN, " ");
				PRINT << ((vec[i] != nullptr && vec[i]->FANCY_TREE_RIGHT != nullptr) ? "│" : " ");
				if (is_first)
					is_first = false;
			}
			PRINT << NEW_LINE;
		}

		// print the frame of leaf nodes
		void	print_leafs_frame(std::vector<Node *> vec)
		{
			bool is_first = true;
			for (size_t i = 0; i < vec.size(); i++)
			{
				set_width(is_first ? LEAFS_SPAN + 1 : LEAFS_SPAN, " ");
				if (vec[i] != nullptr)
					PRINT << FRAME_BOTTOM;
				else
					set_width(NODE_WIDTH, " ");
				if (is_first)
					is_first = false;
			}
			PRINT << NEW_LINE;
		}

		// print top part of the frame with branches
		void	print_vertical_branches_top(int branch_len, int factor, std::vector<Node *> vec)
		{
			bool is_first = true;
			for (size_t i = 0; i < vec.size(); i++)
			{
				set_width(is_first ? pow(2.0, factor) - 1 - branch_len : pow(2.0, factor + 1) - NODE_WIDTH - (branch_len * 2), " ");
				PRINT << ((vec[i] != nullptr && vec[i]->FANCY_TREE_LEFT != nullptr) ? "│" : " ");
				set_width(branch_len - 1, " ");
				if (vec[i] != nullptr && vec[i] != nullptr)
					PRINT << FRAME_BOTTOM;
				else
					set_width(NODE_WIDTH, " ");
				set_width(branch_len - 1, " ");
				PRINT << ((vec[i] != nullptr && vec[i]->FANCY_TREE_RIGHT != nullptr) ? "│" : " ");
				if (is_first)
					is_first = false;
			}
			PRINT << NEW_LINE;
		}

		// print bottom part of the frame with branches
		void	print_vertical_branches_bottom(int branch_len, int factor, std::vector<Node *> vec, bool before_last)
		{
			bool is_first = true;
			for (size_t i = 0; i < vec.size(); i++)
			{
				if (before_last)
				{
					set_width(is_first ? pow(2.0, factor) - 3 - branch_len : pow(2.0, factor + 1) - (branch_len * 2) - 10, " ");
					branch_printer_before_last(vec[i]);
				}
				else
				{
					set_width(is_first ? pow(2.0, factor) - branch_len - 5 : pow(2.0, factor + 1) - (branch_len * 2) - 14, " ");
					branch_printer((branch_len * 2) + NODE_WIDTH, vec[i]);
				}
				if (is_first)
					is_first = false;
			}
			PRINT << NEW_LINE;
		}

		// core function to print the tree
		void    print_tree_vertical_view(std::vector<std::vector<Node *> > vec, size_t height)
		{
			for (size_t level = 0; level < vec.size(); level++)
			{
				int factor = height - level + 2;
				int branch_len = ((pow(2.0, factor) / 2 - CONTENT_LEN) == 0) ? 2 : (pow(2.0, factor) / 2) - CONTENT_LEN;
				if (level == 0)
				{
					set_width(pow(2.0, factor) - 1, " ");
					PRINT << FRAME_TOP;
					PRINT << NEW_LINE;
				}
				print_level(vec[level], level, height, factor, branch_len);
				if (level != height)
				{
					print_vertical_branches_top(branch_len, factor, vec[level]);
					print_vertical_branches_mid(branch_len, factor, vec[level]);
					print_vertical_branches_bottom(branch_len, factor, vec[level], (level == height - 1));
				}
				else
					print_leafs_frame(vec[height]);
			}
		}

	public:
		//constructor
		fancy_tree(){};
		//destructor
		~fancy_tree(){};

		//main function to call
		void print_tree(Node *root, int flags)
		{
			PRINT << NEW_LINE;
			if (!root)
			{
				PRINT << "Tree is empty!" << NEW_LINE;
				return;
			}
			size_t height = tree_height(root) - 1;
			std::vector<std::vector<Node *> > vec = master_vector_filler(root, height);
			if (flags == V_VIEW)
				print_tree_vertical_view(vec, height);
			else if (flags == H_VIEW)
				print_tree_horizontal_view(root, nullptr, false);
			PRINT << NEW_LINE;
		}

};

#undef LEAFS_SPAN
#undef NODE_WIDTH
#undef CONTENT_LEN
#undef TOP_LEFT_FRAME
#undef TOP_RIGHT_FRAME
#undef RIGHT_LEAF_FRAME
#undef LEFT_LEAF_FRAME
#undef FRAME_BOTTOM
#undef FRAME_TOP
#undef BIG_TRUNK
#undef SMALL_TRUNK
#undef LEFT_TRUNK
#undef RIGHT_TRUNK
#undef BIG_SPAN
#undef SMALL_SPAN
#undef ROOT_TRUNK
#undef NEW_LINE
#undef PRINT
