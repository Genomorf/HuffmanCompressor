#include "HoffmanCypher.h"

HoffmanCypher::HoffmanCypher() {}

void HoffmanCypher::fillTree() {
	for (const auto& itr : symbols) {
		//Node* p = new Node(itr.first, itr.second); // key = symbol;  value = count
		//shared_ptr<Node> p (new Node(itr.first, itr.second));
		shared_ptr<Node> p = make_shared<Node>(Node(itr.first, itr.second));
		trees.emplace_back(p);
	}
}


void HoffmanCypher::fillTable() {
	while (trees.size() != 1)
	{
		sort(trees.begin(), trees.end(),
			[](shared_ptr<Node> a, shared_ptr<Node> b) { return a->count < b->count; });
		shared_ptr<Node> l = trees.front();
		trees.pop_front();
		shared_ptr<Node> r = trees.front();
		trees.pop_front();
		
		shared_ptr<Node> parent = make_shared<Node>(l, r);
		trees.emplace_back(parent);
	}
	root = move(trees.front());
}


void HoffmanCypher::BuildTable(shared_ptr<Node> root, vector<bool>& code, map<char, vector<bool>>& table) // dfs
{
	if (root->left)
	{
		code.push_back(0); 
		BuildTable(root->left, code, table);
	}

	if (root->right)
	{
		code.push_back(1); 
		BuildTable(move(root->right), code, table);
	}

	if (root->symbol)
		table[root->symbol] = code;
	if (code.size())
		code.pop_back();
}


string HoffmanCypher::encodeString(string& str) {
	for (size_t i = 0; i < str.length(); i++)
		symbols[str[i]]++;
	
	fillTree();
	fillTable();
	BuildTable(move(root), code, table);

	for (size_t i = 0; i < str.length(); i++)
		for (size_t j = 0; j < table[str[i]].size(); j++)
		{
			out += table[str[i]][j] + '0';
		}
	isEncoded = true;
	return out;
}


vector<bool> HoffmanCypher::getVectorBoolOutput(map<vector<bool>, char> _table) {
	vector<bool> res;
	if (!isEncoded) {
		throw "Encode string first!";
	}
	for (const auto i : out) {
		res.push_back(i - '0');
	}
	return res;
}


map<char, vector<bool>> HoffmanCypher::getEncodingTable() {
	return table;
}


string HoffmanCypher::decodeString(string& str, map<vector<bool>, char> _table) 
{
	string out{};
	vector<bool> code = {};
	for (size_t i = 0; i < str.length(); i++)
	{
		code.push_back(str[i] == '0' ? false : true);
		if (_table[code])
		{
			out += _table[code];
			code.clear();
		}
	}
	return out;
}
