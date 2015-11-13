#include "stdafx.h"
#include "crc_32.h"
#include <windows.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <string>
#include <regex>

typedef long long count_wc;
using namespace std;

string int_to_hex(int i) { //hex output of int
	std::ostringstream oss;
	oss << "0x" << hex << i;
	return oss.str();
}

string calc_tag(string file, string BeginTag,string EndTag) { //calcuate text by template <BeginTag>TEXT<EndTag>
	string search = "<" + BeginTag + ">[^'<',(0x)]*<" + EndTag + ">"; // search string
	std::regex rx(search);
	std::smatch m;
	while (std::regex_search(file, m, rx)) { //while still find by expression
		for (auto x : m) {
			string in = x.str().erase(0, BeginTag.size() + 2); //
			in.erase(in.end() - (EndTag.size() + 3),in.end()); // delete tags
			crc_32 code;
			code.ProcessCRC((void *)in.c_str(), in.size()); // calculate CRC_32 summ
			string replacement = "<" + BeginTag + ">" +int_to_hex(code.m_crc32) + "<" + EndTag + ">"; // prepare replace string
			file = regex_replace(file, rx, replacement, regex_constants::format_first_only);
		}
	}
	return file;
}

string calc_declaration(string file) { //calcuate text by template DECLARATION(TEXT)
	for (long long i = 1; i < file.size(); i++) {
		if (file[i] == '(' && (isalpha(file[i - 1]) || ('0' <= file[i - 1] && '9' >= file[i - 1]))){ //find "'alpha'("
			int endD = file.find(')', i); //find end of tamplate
			if (!endD) break; // if wrong end
			string text = file.substr(i + 1, endD - i - 1);
			crc_32 code;
			code.ProcessCRC((void *)text.c_str(), text.size());
			text = int_to_hex(code.m_crc32);
			file.replace(i + 1, endD - i - 1, text);
		}
	}
	return file;
}

void calc_file(char *filename,int tag_count, char **tags) {
	string name(filename);
	ifstream ifs(name.c_str(), std::ifstream::binary);
	if (ifs) {
		filebuf* pbuf = ifs.rdbuf(); //read file
		long long Length = pbuf->pubseekoff(0, ifs.end, ifs.in); //get char count
		pbuf->pubseekpos(0, ifs.in);
		char* buffer = new char[Length + 1];
		pbuf->sgetn(buffer, Length);
		buffer[Length] = '\0';
		string file = string(buffer);

		for (int i = 2; i < tag_count; i += 2) //for all tags
			file = calc_tag(file, string(tags[i]), string(tags[i + 1]));
		file = calc_declaration(file);

		name = "result_" + name;
		ofstream ofs(name.c_str()); // put result in file with prefix "result_"
		ofs << file;
		ofs.close();

		delete[] buffer;
	}
	ifs.close();
}

int main(int argc, char *argv[])
{
	if (argc < 2) {
		cout << "ERROR: need filename or mask" << endl;
		exit(0);
	}
	if (argc % 2) {
		cout << "ERROR: you must sumbit PAIR of tags (BeginTag EndTag)" << endl;
		exit(0);
	}
	WIN32_FIND_DATAA FindFileData;
	HANDLE hFind;
	hFind = FindFirstFileA(argv[1], &FindFileData); // take a first file in maindir by mask
	if (hFind == INVALID_HANDLE_VALUE)
	{
		printf("FindFirstFile failed (%d)\n", GetLastError());
		system("pause");
		return 0;
	}
	else
	{
		do {
			if (!string(FindFileData.cFileName).find("result_")) continue; //exclude "result_*" files
			calc_file(FindFileData.cFileName,argc,argv); // calculate file
		} while (FindNextFileA(hFind, &FindFileData) != 0); //while still find by mask
		FindClose(hFind);
	}
	cout << "OK" << endl;
	system("pause");
	return 0;
}