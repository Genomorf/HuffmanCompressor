#include "Compresser.h"

//-------------------------//
// Compresser abstract class
//-------------------------//
CompressBaseAbstract::CompressBaseAbstract() {}


CompressBaseAbstract::CompressBaseAbstract(string _fullPathToFile) : fullPathToFile(_fullPathToFile) {}


CompressBaseAbstract::FullPathToDirSplitted CompressBaseAbstract::splitFullPathToFile() {
	int counter = 0;
	// get file name without full path
	for (int i = fullPathToFile.size(); i >= 0 && fullPathToFile[i] != '/' && fullPathToFile[i] != '\\'; --i) {
		++counter;
	}
	return {
		fullPathToFile.substr(fullPathToFile.size() - counter + 1, counter),  // fileNameWithFormat: out.txt
		fullPathToFile.substr(0, fullPathToFile.size() - counter + 1) // fullPathToDir:  C:/Users/Alex/source/
	};
}

// remove format form fileName: out.txt ---> out
string CompressBaseAbstract::getFileNameWithNoFormat(const string& fileNameWithFormat) {
	string fileNameNoFormat;
	for (int i = 0; fileNameWithFormat[i] != '.'; ++i) {
		fileNameNoFormat.push_back(fileNameWithFormat[i]);
	}
	return fileNameNoFormat; 
}

// pure virtual foos
string CompressBaseAbstract::readFromFile() {}
void CompressBaseAbstract::writeToFile(const string& fullPathToDir, const string& fileNameNoFormat, string& binaryString) {}
//////////////////////////////




//-------------------------//
// Compresser writer class
//-------------------------//
CompressWriter::CompressWriter(string _fullPathToFile) {
	fullPathToFile = _fullPathToFile;
}

string CompressWriter::readFromFile() {
	ifstream inputStream(fullPathToFile);
	string tmp;
	string fileData;
	while (getline(inputStream, tmp)) { // read text from ifstream by line
		fileData += tmp;
	}
	inputStream.close();
	return fileData;
}

// add fileName to the end of the text.
// decompresser will find and read it at decompresser stage
// it helps to find name of the file later
void CompressWriter::addFileNameToFileData(string& fileNameWithFormat, string& fileData) {
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
void CompressWriter::addMetaData(string& binaryString) {
	binaryString = bitset<32>(binaryString.size()).to_string() + binaryString;
}


void CompressWriter::writeToFile(const string& fullPathToDir, const string& fileNameNoFormat, string& binaryString) {
	ofstream outputStream(fullPathToDir + fileNameNoFormat + ".comp");
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
	ss << "end" << '\n' << '\0';  

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
	if (pos != string::npos && pos != 0 && pos != s.size() - 1) {
		return true;
	}
	return false;
}

//	struct FullPathToDirSplitted {
//		string fileNameWithFormat;
//		string fullPathToDir;
//	};
string CompressWriter::compress() {
	FullPathToDirSplitted fullPathToDirSplitted = splitFullPathToFile();
	if (!isValidFileName(fullPathToDirSplitted.fileNameWithFormat)) {
		return "Invalid file";
	}
	string fileNameNoFormat = getFileNameWithNoFormat(fullPathToDirSplitted.fileNameWithFormat);
	string fileData = readFromFile();
	if (!fileData.size()) {
		return "File is empty";
	}
	addFileNameToFileData(fullPathToDirSplitted.fileNameWithFormat, fileData);
	string binaryString = encodeData(fileData);
	addMetaData(binaryString);
	writeToFile(fullPathToDirSplitted.fullPathToDir, fileNameNoFormat, binaryString);
	return "";
}
///////////////////////////




//-------------------------//
// Compresser reader class
//-------------------------//

CompressReader::CompressReader(string _fullPathToFile) {
	fullPathToFile = (_fullPathToFile);
}

// find Huffman encoding table at the beginnig of the file
// stop point is the "end" string 
map<char, vector<bool>> CompressReader::readEncodingTableFromFile() {
	ifstream inputstream(fullPathToFile);
	string key = " ";
	string value = " ";
	vector<bool> tmpvec;
	map<char, vector<bool>> tableCharVectorBool;
	while (getline(inputstream, key) && key != "end" && getline(inputstream, value)) {
		for (const auto i : value) {
			tmpvec.push_back(i == '0' ? false : true);
		}
		tableCharVectorBool[key[0]] = tmpvec;
		tmpvec = {};
	}
	inputstream.close();
	return tableCharVectorBool;
}


map<vector<bool>, char> CompressReader::reverseMap(map<char, vector<bool>>&& tableCharVectorBool) {
	map<vector<bool>, char> tableVectorBoolChar;
	for (const auto& i : tableCharVectorBool) {
		tableVectorBoolChar[i.second] = i.first;
	}
	return tableVectorBoolChar;
}

// after encoding table file keep information about size of the data 
// and data itself.
string CompressReader::readFromFile() {
	// get to the position after Huffman table
	ifstream inputstream(fullPathToFile, ios::binary);
	char chartmp = ' ';
	while (chartmp != '\0') {
		inputstream.get(chartmp);
	}
	// first 4 bytes after Huffman table is the size of the binaryString
	// convert it from char to bitset and to int.
	string sizeOfData;
	sizeOfData.resize(bytesOffset);
	inputstream.read(&sizeOfData[0], bytesOffset);
	string sizeOfData8bitset;
	for (const auto i : sizeOfData) {
		sizeOfData8bitset += bitset<8>(i).to_string();
	}
	const int sizeOfDataInt = bitset<32>(sizeOfData8bitset).to_ulong();
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
string CompressReader::decodeData(string& fileData, map<vector<bool>, char>&& tableVectorBoolChar) {
	HoffmanCypher hc;
	string decodedString = hc.decodeString(fileData, tableVectorBoolChar);
	return decodedString;
}


void CompressReader::writeToFile(const string& fullPathToDir, const string& fileNameNoFormat, string& decodedStr) {
	cout << endl << "Decoded str: " << decodedStr << endl;
	// find fileName at the end of a decoded string:
	// it is the last word after last space of the file
	string fileName;
	while (decodedStr.back() != ' ')
	{
		fileName.push_back(decodedStr.back());
		decodedStr.pop_back();
	}
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


string CompressReader::decompress() {
	FullPathToDirSplitted fullPathToDirSplitted = splitFullPathToFile();
	if (!isValidFileName(fullPathToDirSplitted.fileNameWithFormat)) {
		return "Invalid file";
	}
	if (!isCompFormatFile(fullPathToDirSplitted.fileNameWithFormat)) {
		return "File is not *.comp";
	}
	string fileNameNoFormat = getFileNameWithNoFormat(fullPathToDirSplitted.fileNameWithFormat);
	auto tableCharVectorBool = readEncodingTableFromFile();
	if (!tableCharVectorBool.size()) {
		return "Decompress failed";
	}
	auto tableVectorBoolChar = reverseMap(move(tableCharVectorBool));
	string fileData = readFromFile();
	if (!fileData.size()) {
		"File is empty";
	}
	string decodedStr = decodeData(fileData, move(tableVectorBoolChar));
	writeToFile(fullPathToDirSplitted.fullPathToDir, fileNameNoFormat, decodedStr);
	return "";
}
