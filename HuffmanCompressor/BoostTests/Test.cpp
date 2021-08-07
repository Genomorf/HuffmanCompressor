#define BOOST_TEST_MODULE mytests
#include <boost/test/included/unit_test.hpp>
#include "../../HuffmanCompressor/src/Compresser.h"
#include <filesystem>

struct CompressWriterTestClass : public CompressWriter {
public:
	CompressWriterTestClass(string _fullPathToFile) {
		fullPathToFile = _fullPathToFile;
	}
	~CompressWriterTestClass() {}
	friend struct CompressWriterTest;
	
};
struct CompressReaderTestClass : public CompressReader {
public:
	CompressReaderTestClass(string _fullPathToFile) {
		fullPathToFile = _fullPathToFile;
	}
	~CompressReaderTestClass() {}
	friend struct CompressReaderTest;

};
BOOST_AUTO_TEST_CASE(CompressWriterTest) {
	CompressWriterTestClass compTest("./BoostTests/Text.txt");

	auto fullPathToDirSplitted = compTest.splitFullPathToFile();
	BOOST_CHECK(fullPathToDirSplitted.fileNameWithFormat == "Text.txt");
	BOOST_CHECK(fullPathToDirSplitted.fullPathToDir == "./BoostTests/");

	string FileNameWithNoFormat = compTest.getFileNameWithNoFormat("Test.txt");
	BOOST_CHECK(FileNameWithNoFormat == "Test");

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
	ifstream inputTest("./BoostTests/Test.comp");
	ifstream inputTestValidator("./BoostTests/TestValidator.comp");
	string inputTestStr;
	string inputTestValidatorStr;
	bool isEqual = true;
	while (getline(inputTest, inputTestStr) && getline(inputTestValidator, inputTestValidatorStr)) {
		if (inputTestStr != inputTestValidatorStr) {
			isEqual = false;
			break;
		}
	}
	BOOST_CHECK(isEqual == true);
}

BOOST_AUTO_TEST_CASE(CompressReaderTest) {
	CompressTestClass compWriterTest("./BoostTests/Text.txt");
	//cout << "ReadFromFile: " << w.readFromFile();
	

}