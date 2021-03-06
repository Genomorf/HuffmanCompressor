#include "Compresser.h"

// TODO: SOLVE SPACING BUG 
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
		optional<InvalidCompressReason> compressResult = compressWriter.compress();
		if (!compressResult) {
			cout << "done. ";
			return 0;
		}
		else {
			switch (*compressResult) {
			case InvalidCompressReason::FAILED_TO_ENCODE_DATA: 
				cout << "Failed to encode data" << '\n';
				break;
			case InvalidCompressReason::FILE_IS_EMPTY:
				cout << "File is empty" << '\n';
				break;
			case InvalidCompressReason::INVALID_FILE:
				cout << "Invalid file" << '\n';
				break;
			case InvalidCompressReason::PATH_TO_FILE_IS_EMPTY:
				cout << "Path to file is empty" << '\n';
			}
			cout << "Failed to compress!\n";
		}
	}
	else if (string(argv[1]) == "-decompress") {
		cout << "Decompressing...\n";
		CompressReader compressReader(argv[2]);
		optional<InvalidDecompressReason> decompressResult = compressReader.decompress();
		if (!decompressResult) {
			cout << "done. ";
			return 0;
		}
		else {
			switch (*decompressResult)
			{
			case InvalidDecompressReason::DECODING_FAILED:
				cout << "Failed to decode file" << '\n';
				break;
			case InvalidDecompressReason::FILE_IS_EMPTY:
				cout << "File is empty" << '\n';
				break;
			case InvalidDecompressReason::FILE_IS_NOT_COMP_FORMAT:
				cout << "File is not *.comp format" << '\n';
				break;
			case InvalidDecompressReason::INVALID_FILE:
				cout << "Invalid file" << '\n';
				break;
			case InvalidDecompressReason::PATH_TO_FILE_IS_EMPTY:
				cout << "Path to file is empty" << '\n';
			}
			cout << "Failed to decompress!\n";
		}
	}
}