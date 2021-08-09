#define BOOST_TEST_MODULE CompressClassesTests
#include <boost/test/included/unit_test.hpp>
#include "../../HuffmanCompressor/src/Compresser.h"
#include "../../HuffmanCompressor/src/HoffmanCypher.h"
#include <filesystem>

struct CompressWriterTestClass : public CompressWriter {
public:
	CompressWriterTestClass(string _fullPathToFile) : CompressWriter(_fullPathToFile) {}
	~CompressWriterTestClass() {}
	friend struct CompressWriterInternalFunctionsTest;
	
};
struct CompressReaderTestClass : public CompressReader {
public:
	CompressReaderTestClass(string _fullPathToFile) : CompressReader(_fullPathToFile) {}

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
	BOOST_CHECK(fileData == "string1\\nstring2\\nstring3\\n");

	compTest.addFileNameToFileData(fullPathToDirSplitted.fileNameWithFormat, fileData);
	BOOST_CHECK(fileData == "string1\\nstring2\\nstring3\\n Text.txt");
	string encodedStr = compTest.encodeData(fileData);
	BOOST_CHECK(encodedStr == "001110000111110111100111010011010011100001111101111001111100110100111"
							  "000011111011110100001001101011001000101000101110011011100101110");

	string encodedStrCopy = encodedStr;
	string encodedStrSize = bitset<32>(encodedStr.size()).to_string();
	compTest.addMetaData(encodedStr);
	BOOST_CHECK(encodedStr == encodedStrSize + encodedStrCopy);

	compTest.writeToFile(fullPathToDirSplitted.fullPathToDir, FileNameWithNoFormat, encodedStr);
	ifstream inputTest("./BoostTests/Text.comp");
	BOOST_CHECK(inputTest.is_open() == true);
	ifstream inputTestValidator("./BoostTests/test_validators/TestValidator.comp");
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
	string s("string1\\nstring2\\nstring3\\n Text.txt");
	hc.encodeString(s);
	auto hcTable = hc.getEncodingTable();
	auto table = compTest.readEncodingTableFromFile();
	BOOST_CHECK(hcTable == table);

	string fileData = compTest.readFromFile();
	BOOST_CHECK(fileData == "0011100001111101111001110100110100111000011111011110011111001101001110"
							"00011111011110100001001101011001000101000101110011011100101110");

	string decodedStr = compTest.decodeData(fileData, compTest.reverseMap(move(table)));
	BOOST_CHECK(decodedStr == "string1\\nstring2\\nstring3\\n Text.txt");

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
	auto opt = compWriter.compress();
	BOOST_CHECK(*opt == InvalidCompressReason::INVALID_FILE);

	compWriter = CompressWriter("./BoostTests/test_validators/EmptyText.txt");
	opt = compWriter.compress();
	BOOST_CHECK(*opt == InvalidCompressReason::FILE_IS_EMPTY);

	compWriter = CompressWriter("./BoostTests/Text.txt");
	opt = compWriter.compress();
	BOOST_CHECK(!opt);

	ifstream inp("./BoostTests/Text.comp");
	inp.seekg(0, inp.end);
	BOOST_CHECK(inp.tellg() == 129);
}
BOOST_AUTO_TEST_CASE(CompressReaderDecompressMethodTest) {
	CompressReader compReader("./BoostTests/Text");
	auto opt = compReader.decompress();
	BOOST_CHECK(*opt == InvalidDecompressReason::INVALID_FILE);

	compReader = CompressReader("./BoostTests/Text.txt");
	opt = compReader.decompress();
	BOOST_CHECK(*opt == InvalidDecompressReason::FILE_IS_NOT_COMP_FORMAT);

	compReader = CompressReader("./BoostTests/test_validators/EmptyComp.comp");
	opt = compReader.decompress();
	BOOST_CHECK(*opt == InvalidDecompressReason::DECODING_FAILED);

	compReader = CompressReader("./BoostTests/test_validators/EmptyCompWithTable.comp");
	opt = compReader.decompress();
	BOOST_CHECK(*opt == InvalidDecompressReason::FILE_IS_EMPTY);
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