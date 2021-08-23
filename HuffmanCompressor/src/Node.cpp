#include "Node.h"
#include <iostream>

Node::Node() {}

Node::Node(const Node& node) {
	this->count = node.count;
	this->symbol = node.symbol;
	this->left = node.left;
	this->right = node.right;
}

// copy and swap 
Node& Node::operator=(Node other) {
	if (this == &other) return *this;

	swap(*this, other);
	return *this;
}


Node::Node(Node&& other) noexcept {
	swap(*this, other);
}


Node& Node::operator=(Node&& other) noexcept {
	if (this == &other) return *this;

	swap(*this, other);
	return *this;
}


Node::Node(char _symbol, int _count) : symbol(_symbol), count(_count) {}


Node::Node(shared_ptr<Node> l, shared_ptr<Node> r) // create parent
{
	symbol = 0;
	left = move(l);
	right = move(r);
	count = left->count + right->count;
}


void swap(Node& first, Node& second)
{
	using std::swap;
	swap(first.left, second.left);
	swap(first.right, second.right);
	swap(first.symbol, second.symbol);
	swap(first.count, second.count);
}
