#pragma once

#include <cstdlib>
#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#if defined(__unix__) || defined(__APPLE__)
# include <unistd.h>
#endif

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
#if defined(FANCY_TREE_COLOR) && !defined(FANCY_TREE_RED)
# error "FANCY_TREE_COLOR needs FANCY_TREE_RED: the value of that member that means red"
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

		struct  Cell
		{
			std::string glyph;
			bool        red;

			Cell() : glyph(" "), red(false) {}
		};

		typedef std::vector<std::vector<Cell> >  Canvas;

		struct  Box
		{
			const char *top_left;
			const char *top_right;
			const char *bottom_left;
			const char *bottom_right;
			const char *across;
			const char *side;
			const char *left_joint;
			const char *right_joint;
			const char *top_joint;
		};

		bool    _colour;

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

		/*===================Red/black===================*/

		bool    is_red(const Node *node) const
		{
#ifdef FANCY_TREE_COLOR
			return node->FANCY_TREE_COLOR == FANCY_TREE_RED;
#else
			(void)node;
			return false;
#endif
		}

		// NO_COLOR turns colours off, CLICOLOR_FORCE turns them on when piped
		static bool colour_wanted()
		{
			const char *off = std::getenv("NO_COLOR");
			if (off && *off)
				return false;
			const char *force = std::getenv("CLICOLOR_FORCE");
			if (force && *force && std::string(force) != "0")
				return true;
#if defined(__unix__) || defined(__APPLE__)
			return isatty(STDOUT_FILENO) == 1;
#else
			return false;
#endif
		}

		const Box   &box(const Node *node) const
		{
			static const Box single = {"┌", "┐", "└", "┘", "─", "│", "┤", "├", "┴"};
			static const Box twin = {"╔", "╗", "╚", "╝", "═", "║", "╢", "╟", "╧"};
			return is_red(node) && !_colour ? twin : single;
		}

		/*===================Horizontal view===================*/

		void    print_value(const Node *node) const
		{
			if (!is_red(node))
				std::cout << node->FANCY_TREE_VALUE;
			else if (_colour)
				std::cout << "\033[31m" << node->FANCY_TREE_VALUE << "\033[0m";
			else
				std::cout << node->FANCY_TREE_VALUE << " (red)";
		}


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
			print_value(root);
			std::cout << std::endl;
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
				canvas[y][x].glyph = glyph;
		}

		// draw a node box at (y, x) and the branches to its children
		void    draw_node(Canvas &canvas, const Node *node, int y, int x, int half_gap) const
		{
			std::vector<std::string> label = box_label(node);
			const Box &style = box(node);
			int right = x + BOX_WIDTH - 1;

			canvas[y][x].glyph = style.top_left;
			draw_line(canvas, y, x + 1, right - 1, style.across);
			canvas[y][right].glyph = style.top_right;
			canvas[y + 1][x].glyph = style.side;
			for (int i = 0; i < VALUE_WIDTH; i++)
				canvas[y + 1][x + 1 + i].glyph = label[i];
			canvas[y + 1][right].glyph = style.side;
			canvas[y + 2][x].glyph = style.bottom_left;
			draw_line(canvas, y + 2, x + 1, right - 1, style.across);
			canvas[y + 2][right].glyph = style.bottom_right;
			for (int row = y; row < y + 3; row++)
				for (int col = x; col <= right; col++)
					canvas[row][col].red = _colour && is_red(node);

			if (node->FANCY_TREE_LEFT != nullptr)
			{
				int child = x - half_gap;
				int joint = child + (BOX_WIDTH - 1) / 2;
				draw_node(canvas, node->FANCY_TREE_LEFT, y + LEVEL_ROWS, child, half_gap / 2);
				canvas[y + 1][x].glyph = style.left_joint;
				draw_line(canvas, y + 1, joint + 1, x - 1, "─");
				canvas[y + 1][joint].glyph = "┌";
				draw_line(canvas, y + 2, joint, joint, "│");
				draw_line(canvas, y + 3, joint, joint, "│");
				canvas[y + LEVEL_ROWS][joint].glyph = box(node->FANCY_TREE_LEFT).top_joint;
			}
			if (node->FANCY_TREE_RIGHT != nullptr)
			{
				int child = x + half_gap;
				int joint = child + BOX_WIDTH / 2;
				draw_node(canvas, node->FANCY_TREE_RIGHT, y + LEVEL_ROWS, child, half_gap / 2);
				canvas[y + 1][right].glyph = style.right_joint;
				draw_line(canvas, y + 1, right + 1, joint - 1, "─");
				canvas[y + 1][joint].glyph = "┐";
				draw_line(canvas, y + 2, joint, joint, "│");
				draw_line(canvas, y + 3, joint, joint, "│");
				canvas[y + LEVEL_ROWS][joint].glyph = box(node->FANCY_TREE_RIGHT).top_joint;
			}
		}

		// core function to print the tree
		void    print_tree_vertical_view(const Node *root, int height)
		{
			int leafs = 1 << height;
			int width = leafs * LEAF_PITCH - LEAFS_SPAN;
			int rows = height * LEVEL_ROWS + 3;
			int half_gap = leafs * LEAF_PITCH / 4;
			Canvas canvas(rows, std::vector<Cell>(width));

			draw_node(canvas, root, 0, (leafs - 1) * LEAF_PITCH / 2, half_gap);
			for (int y = 0; y < rows; y++)
			{
				int end = width;
				bool red = false;
				while (end > 0 && canvas[y][end - 1].glyph == " ")
					end--;
				for (int x = 0; x < end; x++)
				{
					if (canvas[y][x].red != red)
					{
						red = canvas[y][x].red;
						std::cout << (red ? "\033[31m" : "\033[0m");
					}
					std::cout << canvas[y][x].glyph;
				}
				if (red)
					std::cout << "\033[0m";
				std::cout << std::endl;
			}
		}

		/*===================DOT export===================*/

		static std::string dot_escape(const std::string &str)
		{
			std::string out;
			for (size_t i = 0; i < str.size(); i++)
			{
				if (str[i] == '"' || str[i] == '\\')
					out += '\\';
				if (str[i] == '\n')
					out += "\\n";
				else
					out += str[i];
			}
			return out;
		}

		std::string dot_style(const Node *node) const
		{
#ifdef FANCY_TREE_COLOR
			if (is_red(node))
				return ", style=filled, fillcolor=\"#c62828\", fontcolor=white";
			return ", style=filled, fillcolor=\"#212121\", fontcolor=white";
#else
			(void)node;
			return "";
#endif
		}

		void    dot_gap(std::ostream &out, int parent, int &next, const char *size) const
		{
			int gap = next++;
			out << "\tn" << gap << " [label=\"\", style=invis, " << size << "];\n";
			out << "\tn" << parent << " -> n" << gap << " [style=invis];\n";
		}

		// invisible nodes keep the parent centred and a lone child on its side
		int     dot_node(std::ostream &out, const Node *node, int &next) const
		{
			int id = next++;
			std::ostringstream value;

			value << node->FANCY_TREE_VALUE;
			out << "\tn" << id << " [label=\"" << dot_escape(value.str()) << "\"" << dot_style(node) << "];\n";
			if (!node->FANCY_TREE_LEFT && !node->FANCY_TREE_RIGHT)
				return id;
			dot_child(out, id, node->FANCY_TREE_LEFT, next);
			dot_gap(out, id, next, "width=0.1");
			dot_child(out, id, node->FANCY_TREE_RIGHT, next);
			return id;
		}

		void    dot_child(std::ostream &out, int parent, const Node *child, int &next) const
		{
			if (!child)
			{
				dot_gap(out, parent, next, "width=0.6");
				return;
			}
			int id = dot_node(out, child, next);
			out << "\tn" << parent << " -> n" << id << ";\n";
		}

	public:
		//constructor
		tree() : _colour(false) {};
		//destructor
		~tree(){};

		//main function to call
		void print_tree(const Node *root, view flags)
		{
#ifdef FANCY_TREE_COLOR
			_colour = colour_wanted();
#endif
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

		// Graphviz DOT, render with: dot -Tpng tree.dot -o tree.png
		void print_dot(const Node *root, std::ostream &out = std::cout) const
		{
			int next = 0;

			out << "digraph tree {\n";
			out << "\tgraph [ordering=out];\n";
			out << "\tnode [shape=box, fontname=\"monospace\"];\n";
			if (root)
				dot_node(out, root, next);
			out << "}\n";
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
