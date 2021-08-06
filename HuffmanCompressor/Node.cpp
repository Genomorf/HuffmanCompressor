#include "Node.h"

Node::Node() {}


Node::Node(char _symbol, int _count) : symbol(_symbol), count(_count) {}


Node::Node(shared_ptr<Node> l, shared_ptr<Node> r) // create parent
{
	symbol = 0;
	left = move(l);
	right = move(r);
	count = left->count + right->count;
}
