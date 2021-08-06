#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <bitset>
#include <sstream>
#include "HoffmanCypher.h"
using namespace std;

class CompressBaseAbstract {
protected:
	CompressBaseAbstract();
	CompressBaseAbstract(string _fullPathToFile);

	string fullPathToFile;
	struct FullPathToDirSplitted {
		string fileNameWithFormat;
		string fullPathToDir;
	};

	string getFileNameWithNoFormat(const string& fileNameWithFormat);
	FullPathToDirSplitted splitFullPathToFile();

	virtual string readFromFile() = 0;
	virtual void writeToFile(const string& fullPathToDir, const string& fileNameNoFormat, string& binaryString) = 0;
};


class CompressWriter : public CompressBaseAbstract {
public:
	CompressWriter(string _fullPathToFile);
	string compress();
private:
	map<char, vector<bool>> encodingTable;

	string readFromFile();
	void addFileNameToFileData(string& fileNameWithFormat, string& fileData);
	string encodeData(string& fileData);
	void addMetaData(string& binaryString);
	void writeToFile(const string& fullPathToDir, const string& fileNameNoFormat, string& binaryString);
};


class CompressReader : public CompressBaseAbstract {
public:
	CompressReader(string _fullPathToFile);

	const size_t bytesOffset = 4;
	string decompress();

private:
	map<vector<bool>, char> reverseMap(map<char, vector<bool>>&& tableCharVectorBool);
	map<char, vector<bool>> readEncodingTableFromFile();
	string readFromFile();
	string decodeData(string& fileData, map<vector<bool>, char>&& tableVectorBoolChar);
	void writeToFile(const string& fullPathToDir, const string& fileNameNoFormat, string& decodedStr);
};

