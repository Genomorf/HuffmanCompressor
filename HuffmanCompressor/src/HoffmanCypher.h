#pragma once

#include <queue>
#include "Node.h"
#include <map>
#include <string>

using namespace std; // ---> bad code style but for pet-project is possible

class HoffmanCypher {
public:
	HoffmanCypher();
private:
	map<char, int> symbols = {};
	deque<shared_ptr<Node>> trees = {};
	vector<bool> code = {};
	map<char, vector<bool>> table = {};
	shared_ptr<Node> root;
	string out = {};
	bool isEncoded = false;
	void fillTree();
	void fillTable();
	void BuildTable(shared_ptr<Node> root, vector<bool>& code, map<char, vector<bool>>& table);
public:
	string encodeString(string& str);
	map<char, vector<bool>> getEncodingTable();
	string decodeString(string& str, map<vector<bool>, char> _table);
	vector<bool> getVectorBoolOutput(map<vector<bool>, char> _table);
};