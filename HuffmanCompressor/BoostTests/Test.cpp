#define BOOST_TEST_MODULE mytests
#include <boost/test/included/unit_test.hpp>
#include "../../HuffmanCompressor/src/Compresser.h"
#include "../../HuffmanCompressor/src/HoffmanCypher.h"
#include <filesystem>

struct CompressWriterTestClass : public CompressWriter {
public:
	CompressWriterTestClass(string _fullPathToFile) {
		fullPathToFile = _fullPathToFile;
	}
	~CompressWriterTestClass() {}
	friend struct CompressWriterInternalFunctionsTest;
	
};
struct CompressReaderTestClass : public CompressReader {
public:
	CompressReaderTestClass(string _fullPathToFile) {
		fullPathToFile = _fullPathToFile;
	}
	~CompressReaderTestClass() {}
	friend struct CompressReaderInternalFunctionsTest;

};
BOOST_AUTO_TEST_CASE(CompressWriterInternalFunctionsTest) {
	ifstream inp("./BoostTests/Text.txt");
	BOOST_CHECK(inp.is_open() == true);
	inp.seekg(0, inp.end);
	auto inpSize = inp.tellg();
	BOOST_CHECK(inpSize > 0);
	inp.close();

	CompressWriterTestClass compTest("./BoostTests/Text.txt");

	auto fullPathToDirSplitted = compTest.splitFullPathToFile();
	BOOST_CHECK(fullPathToDirSplitted.fileNameWithFormat == "Text.txt");
	BOOST_CHECK(fullPathToDirSplitted.fullPathToDir == "./BoostTests/");

	string FileNameWithNoFormat = compTest.getFileNameWithNoFormat("Text.txt");
	BOOST_CHECK(FileNameWithNoFormat == "Text");

	string fileData = compTest.readFromFile();
	BOOST_CHECK(fileData == "string1string2string3");

	compTest.addFileNameToFileData(fullPathToDirSplitted.fileNameWithFormat, fileData);
	BOOST_CHECK(fileData == "string1string2string3 Text.txt");

	string encodedStr = compTest.encodeData(fileData);
	BOOST_CHECK(encodedStr == "0111110100000011101101100111110100000011101101110111110"
							  "10000001110111000101001100110001001111101011111001111");

	string encodedStrCopy = encodedStr;
	string encodedStrSize = bitset<32>(encodedStr.size()).to_string();
	compTest.addMetaData(encodedStr);
	BOOST_CHECK(encodedStr == encodedStrSize + encodedStrCopy);

	compTest.writeToFile(fullPathToDirSplitted.fullPathToDir, FileNameWithNoFormat, encodedStr);
	ifstream inputTest("./BoostTests/Text.comp");
	BOOST_CHECK(inputTest.is_open() == true);
	ifstream inputTestValidator("./BoostTests/TestValidator.comp");
	BOOST_CHECK(inputTestValidator.is_open() == true);
	string inputTestStr;
	string inputTestStrFull;
	string inputTestValidatorStr;
	string inputTestValidatorStrFull;
	bool isEqual = true;
	while (getline(inputTest, inputTestStr) && getline(inputTestValidator, inputTestValidatorStr)) {
		inputTestStrFull += inputTestStr;
		inputTestValidatorStrFull += inputTestValidatorStr;
		if (inputTestStr != inputTestValidatorStr) {
			isEqual = false;
			break;
		}
	}
	BOOST_CHECK(inputTestStrFull.size() > 0);
	BOOST_CHECK(inputTestValidatorStrFull.size() > 0);
	BOOST_CHECK(isEqual == true);
	inputTest.close();
	inputTestValidator.close();
}

BOOST_AUTO_TEST_CASE(CompressReaderInternalFunctionsTest) {
	ifstream inp("./BoostTests/Text.comp");
	BOOST_CHECK(inp.is_open() == true);
	inp.seekg(0, inp.end);
	auto inpSize = inp.tellg();
	BOOST_CHECK(inpSize > 0);
	inp.close();

	CompressReaderTestClass compTest("./BoostTests/Text.comp");
	//cout << "ReadFromFile: " << w.readFromFile();
	auto fullPathToDirSplitted = compTest.splitFullPathToFile();
	BOOST_CHECK(fullPathToDirSplitted.fileNameWithFormat == "Text.comp");
	BOOST_CHECK(fullPathToDirSplitted.fullPathToDir == "./BoostTests/");

	string FileNameWithNoFormat = compTest.getFileNameWithNoFormat("Text.comp");
	BOOST_CHECK(FileNameWithNoFormat == "Text");

	HoffmanCypher hc;
	string s("string1string2string3 Text.txt");
	hc.encodeString(s);
	auto hcTable = hc.getEncodingTable();
	auto table = compTest.readEncodingTableFromFile();
	BOOST_CHECK(hcTable == table);

	string fileData = compTest.readFromFile();
	BOOST_CHECK(fileData == "0111110100000011101101100111110100000011101101110111110"
							"10000001110111000101001100110001001111101011111001111");

	string decodedStr = compTest.decodeData(fileData, compTest.reverseMap(move(table)));
	BOOST_CHECK(decodedStr == "string1string2string3 Text.txt");

	compTest.writeToFile(fullPathToDirSplitted.fullPathToDir, FileNameWithNoFormat, decodedStr);
	ifstream in2("./Text/Text.txt");
	BOOST_CHECK(in2.is_open() == true);
	string tmp;
	string res;
	while (getline(in2, tmp)) {
		res += tmp;
	}
	BOOST_CHECK(res == "string1string2string3");
	in2.close();
}

BOOST_AUTO_TEST_CASE(CompressWriterCompressMethodTest) {
	CompressWriter compWriter("./BoostTests/Text");
	BOOST_CHECK(compWriter.compress() == "Invalid file");

	compWriter = CompressWriter("./BoostTests/EmptyText.txt");
	BOOST_CHECK(compWriter.compress() == "File is empty");

	compWriter = CompressWriter("./BoostTests/Text.txt");
	BOOST_CHECK(compWriter.compress() == "");

	ifstream inp("./BoostTests/Text.comp");
	inp.seekg(0, inp.end);
	BOOST_CHECK(inp.tellg() == 151);
}
BOOST_AUTO_TEST_CASE(CompressReaderDecompressMethodTest) {
	CompressReader compReader("./BoostTests/Text");
	BOOST_CHECK(compReader.decompress() == "Invalid file");

	compReader = CompressReader("./BoostTests/Text.txt");
	BOOST_CHECK(compReader.decompress() == "File is not *.comp");

	compReader = CompressReader("./BoostTests/EmptyComp.comp");
	BOOST_CHECK(compReader.decompress() == "Decompress failed");

	compReader = CompressReader("./BoostTests/EmptyCompWithTable.comp");
	BOOST_CHECK(compReader.decompress() == "File is empty");
}

BOOST_AUTO_TEST_CASE(isValidFileNameTest) {
	BOOST_CHECK(isValidFileName("txt.txt") == true);
	BOOST_CHECK(isValidFileName("txt.") == false);
	BOOST_CHECK(isValidFileName("txt") == false);
	BOOST_CHECK(isValidFileName(".txt") == false);
}

BOOST_AUTO_TEST_CASE(isCompFormatFileTest) {
	BOOST_CHECK(isCompFormatFile("txt.comp") == true);
	BOOST_CHECK(isCompFormatFile("txt.txt") == false);
	BOOST_CHECK(isCompFormatFile("txt") == false);
}