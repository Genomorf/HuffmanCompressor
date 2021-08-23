#pragma once

#include <vector>
#include <memory>

using namespace std; // ---> bad code style but for pet-project is possible

class Node {
public:
	int count = 0;
	char symbol = 0;
	shared_ptr<Node> left;
	shared_ptr<Node> right;

	friend void swap(Node& first, Node& second);

	Node();
	Node(char _symbol, int _count);
	Node(shared_ptr<Node> l, shared_ptr<Node> r);

	Node(const Node& node);
	Node& operator=(Node other);
	Node(Node&& other) noexcept;
	Node& operator=(Node&& other) noexcept;
};
