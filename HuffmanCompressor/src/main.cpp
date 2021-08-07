#include "Compresser.h"

int main(int argc, char* argv[])
{	
	if (argc != 3) {
		cout << "Wrong number of arguments.\n" <<
			"Try 'Compresser -compress TextFile.txt'\n" <<
			"or  'Compresser -decompress TextFile.comp'";
		return 1;
	}
	if (string(argv[1]) == "-compress") {
		cout << "Compressing... \n";
		CompressWriter compressWriter(argv[2]);
		string exitCodeStr = compressWriter.compress();
		if (exitCodeStr.empty()) {
			cout << "done. ";
		}
		else {
			cout << exitCodeStr;
		}
	}
	else if (string(argv[1]) == "-decompress") {
		cout << "Decompressing...\n";
		CompressReader compressReader(argv[2]);
		string exitCodeStr = compressReader.decompress();
		if (exitCodeStr.empty()) {
			cout << "done. ";
		}
		else {
			cout << exitCodeStr;
		}
	}
}