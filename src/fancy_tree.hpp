#pragma once

#include <string>
#include <sstream>
#include <iostream>
#include <vector>

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

// horizontal view TRUNKS
#define BIG_TRUNK "    │"
#define SMALL_TRUNK "   │"
#define LEFT_TRUNK "┌──┤"
#define RIGHT_TRUNK "└──┤"
#define BIG_SPAN "    "
#define SMALL_SPAN "   "
#define ROOT_TRUNK "──┤"

namespace fancy
{

enum view
{
	H_VIEW,
	V_VIEW
};

template <typename Node>
class tree
{

	private:

		enum
		{
			VALUE_WIDTH = 6,
			BOX_WIDTH = VALUE_WIDTH + 2,
			LEAFS_SPAN = 2,
			LEAF_PITCH = BOX_WIDTH + LEAFS_SPAN,
			LEVEL_ROWS = 4
		};

		typedef std::vector<std::vector<std::string> >  Canvas;

		/*===================Branch===================*/

		struct  Branch
		{
			Branch      *prev;
			std::string str;

			Branch(Branch *prev, const std::string &str) : prev(prev), str(str) {}
		};

		/*===================Helper Function===================*/

		// calculate tree height
		int tree_height(const Node *root) const
		{
			if (root == nullptr)
				return 0;
			int l_height = tree_height(root->FANCY_TREE_LEFT);
			int r_height = tree_height(root->FANCY_TREE_RIGHT);
			return (l_height > r_height ? l_height : r_height) + 1;
		}

		// split a string into displayed characters (UTF-8 aware)
		std::vector<std::string> glyphs(const std::string &str) const
		{
			std::vector<std::string> out;
			for (size_t i = 0; i < str.size(); i++)
			{
				if ((static_cast<unsigned char>(str[i]) & 0xC0) == 0x80 && !out.empty())
					out.back() += str[i];
				else
					out.push_back(std::string(1, str[i]));
			}
			return out;
		}

		// value centered in the box, cut with "…" when too long
		std::vector<std::string> box_label(const Node *node) const
		{
			std::ostringstream stream;
			stream << node->FANCY_TREE_VALUE;
			std::vector<std::string> text = glyphs(stream.str());
			if (text.size() > VALUE_WIDTH)
			{
				text.resize(VALUE_WIDTH - 1);
				text.push_back("…");
			}
			std::vector<std::string> label(VALUE_WIDTH, " ");
			size_t start = (VALUE_WIDTH - text.size()) / 2;
			for (size_t i = 0; i < text.size(); i++)
				label[start + i] = text[i];
			return label;
		}

		/*===================Horizontal view===================*/

		// print horizontal view of a tree
		void    showbranches(Branch *p)
		{
			if (p == nullptr)
				return;
			showbranches(p->prev);
			if (p->str == BIG_SPAN && p->prev && p->prev->str == BIG_TRUNK)
				std::cout << SMALL_SPAN;
			else if (p->str == BIG_TRUNK && p->prev && p->prev->str == BIG_TRUNK)
				std::cout << SMALL_TRUNK;
			else
				std::cout << p->str;
		}

		// fill the tree branches in a linked list
		void    print_tree_horizontal_view(const Node *root, Branch *prev, bool is_left)
		{
			if (root == nullptr)
				return;

			std::string prev_str = BIG_SPAN;
			Branch branch(prev, prev_str);

			print_tree_horizontal_view(root->FANCY_TREE_RIGHT, &branch, true);
			if (!prev)
				branch.str = ROOT_TRUNK;
			else if (is_left)
			{
				branch.str = LEFT_TRUNK;
				prev_str = BIG_TRUNK;
			}
			else
			{
				branch.str = RIGHT_TRUNK;
				prev->str = prev_str;
			}
			showbranches(&branch);
			std::cout << root->FANCY_TREE_VALUE << std::endl;
			if (prev)
				prev->str = prev_str;
			branch.str = BIG_TRUNK;
			print_tree_horizontal_view(root->FANCY_TREE_LEFT, &branch, false);
		}

		/*===================Vertical view===================*/

		// draw a horizontal line of the same character
		void    draw_line(Canvas &canvas, int y, int from, int to, const char *glyph) const
		{
			for (int x = from; x <= to; x++)
				canvas[y][x] = glyph;
		}

		// draw a node box at (y, x) and the branches to its children
		void    draw_node(Canvas &canvas, const Node *node, int y, int x, int half_gap) const
		{
			std::vector<std::string> label = box_label(node);
			int right = x + BOX_WIDTH - 1;

			canvas[y][x] = "┌";
			draw_line(canvas, y, x + 1, right - 1, "─");
			canvas[y][right] = "┐";
			canvas[y + 1][x] = "│";
			for (int i = 0; i < VALUE_WIDTH; i++)
				canvas[y + 1][x + 1 + i] = label[i];
			canvas[y + 1][right] = "│";
			canvas[y + 2][x] = "└";
			draw_line(canvas, y + 2, x + 1, right - 1, "─");
			canvas[y + 2][right] = "┘";

			if (node->FANCY_TREE_LEFT != nullptr)
			{
				int child = x - half_gap;
				int joint = child + (BOX_WIDTH - 1) / 2;
				draw_node(canvas, node->FANCY_TREE_LEFT, y + LEVEL_ROWS, child, half_gap / 2);
				canvas[y + 1][x] = "┤";
				draw_line(canvas, y + 1, joint + 1, x - 1, "─");
				canvas[y + 1][joint] = "┌";
				draw_line(canvas, y + 2, joint, joint, "│");
				draw_line(canvas, y + 3, joint, joint, "│");
				canvas[y + LEVEL_ROWS][joint] = "┴";
			}
			if (node->FANCY_TREE_RIGHT != nullptr)
			{
				int child = x + half_gap;
				int joint = child + BOX_WIDTH / 2;
				draw_node(canvas, node->FANCY_TREE_RIGHT, y + LEVEL_ROWS, child, half_gap / 2);
				canvas[y + 1][right] = "├";
				draw_line(canvas, y + 1, right + 1, joint - 1, "─");
				canvas[y + 1][joint] = "┐";
				draw_line(canvas, y + 2, joint, joint, "│");
				draw_line(canvas, y + 3, joint, joint, "│");
				canvas[y + LEVEL_ROWS][joint] = "┴";
			}
		}

		// core function to print the tree
		void    print_tree_vertical_view(const Node *root, int height)
		{
			int leafs = 1 << height;
			int width = leafs * LEAF_PITCH - LEAFS_SPAN;
			int rows = height * LEVEL_ROWS + 3;
			int half_gap = leafs * LEAF_PITCH / 4;
			Canvas canvas(rows, std::vector<std::string>(width, " "));

			draw_node(canvas, root, 0, (leafs - 1) * LEAF_PITCH / 2, half_gap);
			for (int y = 0; y < rows; y++)
			{
				int end = width;
				while (end > 0 && canvas[y][end - 1] == " ")
					end--;
				for (int x = 0; x < end; x++)
					std::cout << canvas[y][x];
				std::cout << std::endl;
			}
		}

	public:
		//constructor
		tree(){};
		//destructor
		~tree(){};

		//main function to call
		void print_tree(const Node *root, view flags)
		{
			std::cout << std::endl;
			if (!root)
			{
				std::cout << "Tree is empty!" << std::endl;
				return;
			}
			if (flags == V_VIEW)
				print_tree_vertical_view(root, tree_height(root) - 1);
			else if (flags == H_VIEW)
				print_tree_horizontal_view(root, nullptr, false);
			std::cout << std::endl;
		}

};

}

#undef BIG_TRUNK
#undef SMALL_TRUNK
#undef LEFT_TRUNK
#undef RIGHT_TRUNK
#undef BIG_SPAN
#undef SMALL_SPAN
#undef ROOT_TRUNK
