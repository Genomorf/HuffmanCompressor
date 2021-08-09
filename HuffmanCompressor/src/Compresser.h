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
#include <boost/algorithm/string/replace.hpp>
using namespace std; // ---> bad code style but for pet-project is possible

class CompressBaseAbstract {
public:
	virtual ~CompressBaseAbstract(){}
protected:
	string fullPathToFile;
	CompressBaseAbstract();
	CompressBaseAbstract(string _fullPathToFile);

	struct FullPathToDirSplitted {
		string fileNameWithFormat;
		string fullPathToDir;
	};

	string getFileNameWithNoFormat(const string& fileNameWithFormat) const;
	FullPathToDirSplitted splitFullPathToFile() const;

	virtual string readFromFile() const = 0;
	virtual void writeToFile(const string& fullPathToDir, const string& fileNameNoFormat, string& binaryString) const = 0;
};


enum class InvalidCompressReason {
	INVALID_FILE,
	FILE_IS_EMPTY,
	FAILED_TO_ENCODE_DATA
};


class CompressWriter : public CompressBaseAbstract {
public:
	CompressWriter(string _fullPathToFile);
	CompressWriter(CompressWriter& other);
	CompressWriter& operator=(const CompressWriter& rhs);
	optional<InvalidCompressReason> compress(); 
protected:
	map<char, vector<bool>> encodingTable;

	string readFromFile() const;
	void addFileNameToFileData(string& fileNameWithFormat, string& fileData) const;
	string encodeData(string& fileData);
	void addMetaData(string& binaryString) const;
	void writeToFile(const string& fullPathToDir, const string& fileNameNoFormat, string& binaryString) const;
};


enum class InvalidDecompressReason {
	INVALID_FILE,
	FILE_IS_NOT_COMP_FORMAT,
	DECODING_FAILED,
	FILE_IS_EMPTY
};


class CompressReader : public CompressBaseAbstract {
public:
	CompressReader(string _fullPathToFile);
	CompressReader(CompressReader& other);
	CompressReader& operator=(const CompressReader& rhs);
	optional<InvalidDecompressReason> decompress() const;

protected:
	const size_t bytesOffset = 4;
	map<char, vector<bool>> readEncodingTableFromFile() const;
	map<vector<bool>, char> reverseMap(map<char, vector<bool>>&& tableCharVectorBool) const;
	string readFromFile() const;
	string decodeData(string& fileData, map<vector<bool>, char>&& tableVectorBoolChar) const;
	void writeToFile(const string& fullPathToDir, const string& fileNameNoFormat, string& decodedStr) const;
};

bool isValidFileName(string const& s);
bool isCompFormatFile(const string& fileName);