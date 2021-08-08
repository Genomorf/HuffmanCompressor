#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <bitset>
#include <sstream>
#include "HoffmanCypher.h"
#include <filesystem>
#include <optional>
using namespace std; // ---> bad code style but for pet-project is possible

class CompressBaseAbstract {
public:
	
protected:
	string fullPathToFile;
	CompressBaseAbstract();
	CompressBaseAbstract(string _fullPathToFile);

	struct FullPathToDirSplitted {
		string fileNameWithFormat;
		string fullPathToDir;
	};

	string getFileNameWithNoFormat(const string& fileNameWithFormat);
	FullPathToDirSplitted splitFullPathToFile();

	virtual string readFromFile() = 0;
	virtual void writeToFile(const string& fullPathToDir, const string& fileNameNoFormat, string& binaryString) = 0;
};


enum class InvalidCompressReason {
	INVALID_FILE,
	FILE_IS_EMPTY,
	FAILED_TO_ENCODE_DATA
};


class CompressWriter : public CompressBaseAbstract {
public:
	
	CompressWriter();
	CompressWriter(string _fullPathToFile);
	CompressWriter(CompressWriter& other);
	CompressWriter& operator=(const CompressWriter& rhs);
	optional<InvalidCompressReason> compress();
protected:
	map<char, vector<bool>> encodingTable;

	string readFromFile();
	void addFileNameToFileData(string& fileNameWithFormat, string& fileData);
	string encodeData(string& fileData);
	void addMetaData(string& binaryString);
	void writeToFile(const string& fullPathToDir, const string& fileNameNoFormat, string& binaryString);
};


enum class InvalidDecompressReason {
	INVALID_FILE,
	FILE_IS_NOT_COMP_FORMAT,
	DECODING_FAILED,
	FILE_IS_EMPTY
};


class CompressReader : public CompressBaseAbstract {
public:
	CompressReader();
	CompressReader(string _fullPathToFile);
	CompressReader(CompressReader& other);
	CompressReader& operator=(const CompressReader& rhs);
	optional<InvalidDecompressReason> decompress();

protected:
	const size_t bytesOffset = 4;
	map<char, vector<bool>> readEncodingTableFromFile();
	map<vector<bool>, char> reverseMap(map<char, vector<bool>>&& tableCharVectorBool);
	string readFromFile();
	string decodeData(string& fileData, map<vector<bool>, char>&& tableVectorBoolChar);
	void writeToFile(const string& fullPathToDir, const string& fileNameNoFormat, string& decodedStr);
};

bool isValidFileName(string const& s);
bool isCompFormatFile(const string& fileName);