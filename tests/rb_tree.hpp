#pragma once

#include <vector>

enum colour { RED, BLACK };

struct rb_node {
	int		_value;
	colour	_color;
	rb_node	*_left;
	rb_node	*_right;
	rb_node	*_parent;

	rb_node(int value) : _value(value), _color(RED), _left(nullptr), _right(nullptr), _parent(nullptr) {}
};

#define FANCY_TREE_COLOR _color
#define FANCY_TREE_RED RED
#include "fancy_tree.hpp"

static void	rotate_left(rb_node *&root, rb_node *x)
{
	rb_node *y = x->_right;

	x->_right = y->_left;
	if (y->_left)
		y->_left->_parent = x;
	y->_parent = x->_parent;
	if (!x->_parent)
		root = y;
	else if (x == x->_parent->_left)
		x->_parent->_left = y;
	else
		x->_parent->_right = y;
	y->_left = x;
	x->_parent = y;
}

static void	rotate_right(rb_node *&root, rb_node *x)
{
	rb_node *y = x->_left;

	x->_left = y->_right;
	if (y->_right)
		y->_right->_parent = x;
	y->_parent = x->_parent;
	if (!x->_parent)
		root = y;
	else if (x == x->_parent->_right)
		x->_parent->_right = y;
	else
		x->_parent->_left = y;
	y->_right = x;
	x->_parent = y;
}

static void	insert(rb_node *&root, rb_node *z)
{
	rb_node *parent = nullptr;

	for (rb_node *cur = root; cur; cur = z->_value < cur->_value ? cur->_left : cur->_right)
		parent = cur;
	z->_parent = parent;
	if (!parent)
		root = z;
	else if (z->_value < parent->_value)
		parent->_left = z;
	else
		parent->_right = z;
	while (z->_parent && z->_parent->_color == RED)
	{
		rb_node *p = z->_parent;
		rb_node *g = p->_parent;
		bool left = (p == g->_left);
		rb_node *uncle = left ? g->_right : g->_left;

		if (uncle && uncle->_color == RED)
		{
			p->_color = BLACK;
			uncle->_color = BLACK;
			g->_color = RED;
			z = g;
			continue;
		}
		if (z == (left ? p->_right : p->_left))
		{
			z = p;
			if (left)
				rotate_left(root, z);
			else
				rotate_right(root, z);
			p = z->_parent;
		}
		p->_color = BLACK;
		g->_color = RED;
		if (left)
			rotate_right(root, g);
		else
			rotate_left(root, g);
	}
	root->_color = BLACK;
}
