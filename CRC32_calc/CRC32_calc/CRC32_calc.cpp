#include "stdafx.h"
#include "crc_32.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <string>
#include <regex>
#include <vector>

typedef long long count_wc;
using namespace std;

vector<string> BeginTags;
vector<string> EndTags;

string int_to_hex(int i) {
	std::ostringstream oss;
	oss << "0x" << hex << i;
	return oss.str();
}

string calc_tag(string file, string BeginTag,string EndTag) {
	string search = "<" + BeginTag + ">[^'<',(0x)]*<" + EndTag + ">";
	std::regex rx(search);
	std::smatch m;
	while (std::regex_search(file, m, rx)) {
		for (auto x : m) {
			string in = x.str().erase(0, BeginTag.size() + 2);
			in.erase(in.end() - (EndTag.size() + 3),in.end());
			crc_32 code;
			code.ProcessCRC((void *)in.c_str(), in.size());
			string replacement = "<" + BeginTag + ">" +int_to_hex(code.m_crc32) + "<" + EndTag + ">";
			file = regex_replace(file, rx, replacement, regex_constants::format_first_only);
		}
	}
	return file;
}

string calc_declaration(string file) {
	for (long long i = 1; i < file.size(); i++) {
		if (file[i] == '(' && (isalpha(file[i - 1]) || ('0'<= file[i - 1] && '9'>= file[i-1]))) {
			int end = file.find(')',i);
			string text = file.substr(i+1, end - i -1 );
			//cout << text << endl;
			crc_32 code;
			code.ProcessCRC((void *)text.c_str(), text.size());
			text = int_to_hex(code.m_crc32);
			file.replace(i + 1, end - i - 1, text);
		}
	}
	return file;
}

int main(int argc, char **argv)
{
	if (argc < 2) {
		cout << "ERROR: need filename or mask" << endl;
		exit(0);
	}
	if (argc % 2) {
		cout << "ERROR: you must sumbit PAIR of tags (BeginTag EndTag)" << endl;
		exit(0);
	}
	ifstream ifs(argv[1], std::ifstream::binary);
	if (ifs) {
		filebuf* pbuf = ifs.rdbuf(); //read file
		long long Length = pbuf->pubseekoff(0, ifs.end, ifs.in); //get char count
		pbuf->pubseekpos(0, ifs.in);
		char* buffer = new char[Length+1];
		pbuf->sgetn(buffer, Length);
		buffer[Length] = '\0';
		string file = string(buffer);
		for (int i = 2; i < argc; i+=2)
			file = calc_tag(file,string(argv[i]), string(argv[i+1]));
		file = calc_declaration(file);
		cout << file << endl;
	}
	system("pause");
	return 0;
}