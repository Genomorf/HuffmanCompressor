#include "Compresser.h"

//-------------------------//
// Compresser abstract class
//-------------------------//
CompressBaseAbstract::CompressBaseAbstract() {}


CompressBaseAbstract::CompressBaseAbstract(string _fullPathToFile) 
	: fullPathToFile(_fullPathToFile) {}

// struct FullPathToDirSplitted {
//	 string fileNameWithFormat;
//	 string fullPathToDir;
// };
CompressBaseAbstract::FullPathToDirSplitted CompressBaseAbstract::splitFullPathToFile() const {
	int counter = 0;
	// get file name without full path
	for (int i = fullPathToFile.size(); i >= 0 && fullPathToFile[i] != '/' && fullPathToFile[i] != '\\'; --i) {
		++counter;
	}
	return { // NRVO doesn't work here
		move(fullPathToFile.substr(fullPathToFile.size() - counter + 1, counter)),  // fileNameWithFormat: out.txt
		move(fullPathToFile.substr(0, fullPathToFile.size() - counter + 1)) // fullPathToDir:  C:/Users/Alex/source/
	};
}

// remove format form fileName: out.txt ---> out
string CompressBaseAbstract::getFileNameWithNoFormat(const string& fileNameWithFormat) const {
	string fileNameNoFormat;
	for (int i = 0; fileNameWithFormat[i] != '.'; ++i) {
		fileNameNoFormat.push_back(fileNameWithFormat[i]);
	}
	return fileNameNoFormat; 
}


//-------------------------//
// Compresser writer class
//-------------------------//
CompressWriter::CompressWriter(string _fullPathToFile) :  CompressBaseAbstract(_fullPathToFile) {}

CompressWriter::CompressWriter(CompressWriter& other)
{
	this->fullPathToFile = other.fullPathToFile;
}

CompressWriter& CompressWriter::operator=(const CompressWriter& rhs)
{
	this->fullPathToFile = rhs.fullPathToFile;
	return *this;
}

string CompressWriter::readFromFile() const {
	ofstream o("Spacing.txt", ios::binary);
	ifstream inputStream(fullPathToFile);
	string tmp;
	string fileData;
	while (getline(inputStream, tmp)) { // read text from ifstream by line
		fileData += tmp += "\\n";
		o << tmp;
	}
	inputStream.close();
	o.close();
	return fileData;
}

// add fileName to the end of the text.
// decompresser will find and read it at decompresser stage
// it helps to find name of the file later
void CompressWriter::addFileNameToFileData(string& fileNameWithFormat, string& fileData) const{
	fileNameWithFormat = " " + fileNameWithFormat;
	fileData += fileNameWithFormat; // text ----> text out.txt
}

// encoding and compressing data with Hoffman algorithm ---> HoffmanCypher.cpp
string CompressWriter::encodeData(string& fileData) {
	HoffmanCypher hoffmanCypher;
	string binaryString = hoffmanCypher.encodeString(fileData);
	encodingTable = hoffmanCypher.getEncodingTable();
	return binaryString;
}

// add size of the encoded data to the beginning of the binary string.
// it helps us to cut unnescessary bytes later, because binary string
// is not always equal to 8 bits => binaryString.size() % 8 != 0.
void CompressWriter::addMetaData(string& binaryString) const {
	string size = bitset<32>(binaryString.size()).to_string();
	binaryString = size + binaryString;
}


void CompressWriter::writeToFile(const string& fullPathToDir, const string& fileNameNoFormat, string& binaryString) const {
	ofstream outputStream(fullPathToDir + fileNameNoFormat + ".comp", ios::binary);
	stringstream ss;
	string tablestr;
	// write Huffman encoding table at the beginning of the file.
	// CompressReader will find it and be able to decode binary string
	// because CompressReader doesn't have information about decoding table
	for (auto it : encodingTable) {
		ss << it.first << '\n';
		for (auto it2 : it.second) {
			ss << it2;
		}
		ss << '\n';
	}
	// sign to CompressReader that this is the end of a table
	ss << '\0';

	tablestr = ss.str();
	outputStream << tablestr;
	size_t offset = 0;
	string buf;
	buf.reserve(8);
	// write binary string to file:
	// initialy, binaryString is simple std::string of 0s and 1s.
	// need to cast it to char to reduce size from 8 bytes to 1 byte.
	// E.g.: binaryString[0-8] = 00001001. Cast it to int(decimal 9) and char.
	for (size_t i = 0; i < binaryString.size(); ++i, ++offset) {
		if (offset == 8) {
			outputStream << static_cast<char>(bitset<8>(buf).to_ulong());
			buf.clear();
			offset = 0;
		}
		buf.push_back(binaryString[i]);
	}
	if (offset) {
		while (buf.size() != 8)
			buf.push_back('0');
		outputStream << static_cast<char>(bitset<8>(buf).to_ulong());
	}
	outputStream.close();
}

// check if name is equal to text.txt (in RegEx: .+\..+)
bool isValidFileName(string const& s) {
	auto pos = s.find('.');
	return pos != string::npos && pos != 0 && pos != s.size() - 1;
}


optional<InvalidCompressReason> CompressWriter::compress() {
	if (fullPathToFile.empty()) {
		return InvalidCompressReason::PATH_TO_FILE_IS_EMPTY;
	}
	FullPathToDirSplitted fullPathToDirSplitted = splitFullPathToFile();
	if (!isValidFileName(fullPathToDirSplitted.fileNameWithFormat)) {
		return InvalidCompressReason::INVALID_FILE;
	}
	string fileNameNoFormat = getFileNameWithNoFormat(fullPathToDirSplitted.fileNameWithFormat);
	string fileData = readFromFile();
	if (fileData.empty()) {
		return InvalidCompressReason::FILE_IS_EMPTY;
	}
	addFileNameToFileData(fullPathToDirSplitted.fileNameWithFormat, fileData);
	string binaryString = encodeData(fileData);
	if (binaryString.empty()) {
		return InvalidCompressReason::FAILED_TO_ENCODE_DATA;
	}
	addMetaData(binaryString);
	writeToFile(fullPathToDirSplitted.fullPathToDir, fileNameNoFormat, binaryString);
	
	return nullopt;
}
///////////////////////////


//-------------------------//
// Compresser reader class
//-------------------------//
CompressReader::CompressReader(string _fullPathToFile) : CompressBaseAbstract(_fullPathToFile) {}

CompressReader::CompressReader(CompressReader& other){
	this->fullPathToFile = other.fullPathToFile;
}

CompressReader& CompressReader::operator=(const CompressReader& rhs){
	this->fullPathToFile = rhs.fullPathToFile;
	return *this;
}

// find Huffman encoding table at the beginnig of the file
// stop point is the "end" string 
map<char, vector<bool>> CompressReader::readEncodingTableFromFile() const {
	ifstream inputstream(fullPathToFile);
	string key = " ";
	string value = " ";
	vector<bool> tmpvec;
	int counter = 0;
	map<char, vector<bool>> tableCharVectorBool;
	while (getline(inputstream, key) && key[0] != '\0' && getline(inputstream, value)) {
		for (const auto i : value) {
			tmpvec.push_back(i == '0' ? false : true);
		}
		tableCharVectorBool[key[0]] = tmpvec;
		tmpvec = {};
	}
	inputstream.close();
	return tableCharVectorBool;
}


map<vector<bool>, char> CompressReader::reverseMap(map<char, vector<bool>>&& tableCharVectorBool) const {
	map<vector<bool>, char> tableVectorBoolChar;
	for (const auto& i : tableCharVectorBool) {
		tableVectorBoolChar[i.second] = i.first;
	}
	return tableVectorBoolChar;
}

// after encoding table file keep information about size of the data 
// and data itself.
string CompressReader::readFromFile() const {
	// get to the position after Huffman table
	ifstream inputstream(fullPathToFile, ios::binary);
	char chartmp = ' ';
	while (chartmp != '\0') {
		inputstream.get(chartmp);
	}
	// first 4 bytes after Huffman table is the size of the binaryString
	// convert it from char to bitset and to int.
	string sizeOfData;
	int counter = 0;
	while (counter < 4) {
		inputstream.get(chartmp);
		sizeOfData.push_back(chartmp);
		++counter;
	}
	string sizeOfData8bitset;
	for (const auto i : sizeOfData) {
		sizeOfData8bitset += bitset<8>(i).to_string();
	}
	const int sizeOfDataInt = bitset<32>(sizeOfData8bitset).to_ulong();
	if (!sizeOfDataInt) {
		return "";
	}
	string binaryStr;
	string strtmp;
	// after 4 bytes of size there is data itself: read it by getline
	while (getline(inputstream, strtmp)) {
		binaryStr += strtmp;
	}
	string fileData;
	fileData.reserve(sizeOfDataInt);
	for (size_t i = 0; i < binaryStr.size(); ++i) {
		fileData += bitset<8>(binaryStr[i]).to_string();
	}
	// cut unnecessary bytes at the end according to data size
	fileData = fileData.substr(0, sizeOfDataInt); 
	// finally get binaryString of 0s and 1s;
	return fileData;
}

// decode binaryString with Huffman decode algorithm and encoding table, that we found earlier
string CompressReader::decodeData(string& fileData, map<vector<bool>, char>&& tableVectorBoolChar) const {
	HoffmanCypher hc;
	string decodedString = hc.decodeString(fileData, tableVectorBoolChar);
	return decodedString;
}


void CompressReader::writeToFile(const string& fullPathToDir, const string& fileNameNoFormat, string& decodedStr) const {
	boost::replace_all(decodedStr, "\\n", "\n");
	// find fileName at the end of a decoded string:
	// it is the last word after last space of the file.
	// pop.back() " file name" from the string.
	string fileName;
	while (decodedStr.back() != ' ')
	{
		fileName.push_back(decodedStr.back());
		decodedStr.pop_back();
	}
	decodedStr.pop_back();
	reverse(fileName.begin(), fileName.end());
	// create new directory to our decompressed file
	// and create file with decompressed data
	string sysCall = "mkdir " + fileNameNoFormat;
	system(sysCall.c_str());
	ofstream output(fileNameNoFormat + "/" + fileName);
	output.write(&decodedStr[0], decodedStr.size());
	output.close();
}

// check if file's format is .comp
// we have to run decompress code only on this type if files
bool isCompFormatFile(const string& fileName) {
	string res{ "comp" };
	for (size_t i = 1; i <= res.size() && fileName[fileName.size() - i] != '.'; ++i)
	{
		if (fileName[fileName.size() - i] != res[res.size() - i]) {
			return false;
		}
	}
	return true;
}



optional<InvalidDecompressReason> CompressReader::decompress() const{
	if (fullPathToFile.empty()) {
		return InvalidDecompressReason::PATH_TO_FILE_IS_EMPTY;
	}
	FullPathToDirSplitted fullPathToDirSplitted = splitFullPathToFile();
	if (!isValidFileName(fullPathToDirSplitted.fileNameWithFormat)) {
		return InvalidDecompressReason::INVALID_FILE;
	}
	if (!isCompFormatFile(fullPathToDirSplitted.fileNameWithFormat)) {
		return InvalidDecompressReason::FILE_IS_NOT_COMP_FORMAT;
	}
	string fileNameNoFormat = getFileNameWithNoFormat(fullPathToDirSplitted.fileNameWithFormat);
	auto tableCharVectorBool = readEncodingTableFromFile();
	if (!tableCharVectorBool.size()) {
		return InvalidDecompressReason::DECODING_FAILED;
	}
	auto tableVectorBoolChar = reverseMap(move(tableCharVectorBool));
	string fileData = readFromFile();
	if (!fileData.size()) {
		return InvalidDecompressReason::FILE_IS_EMPTY;
	}
	string decodedStr = decodeData(fileData, move(tableVectorBoolChar));
	writeToFile(fullPathToDirSplitted.fullPathToDir, fileNameNoFormat, decodedStr);
	return nullopt;
}
