#define _CRT_SECURE_NO_WARNINGS

#include "kmp.h"

#include <iostream>
#include <fstream>
#include <ctime>
#include <string>
#include <cstdio>
#include <regex>

int main()
{
	const std::string pname = "pattern.txt";
	const std::string ptext = "sherlock.txt";
	std::string pat;

	std::ifstream patterns(pname);
	std::ifstream texts(ptext);

	freopen("log.txt", "a", stdout);

	std::ios_base::sync_with_stdio(false);
	texts.tie(nullptr);
	time_t my_time = time(nullptr);

	auto kmp_inst = kmp();
	std::cout << std::endl << "========= " << "kmp" << " =========" << std::endl;
	std::cout << ctime(&my_time);

	std::cout << kmp_inst.count_occurrences(patterns, texts) << std::endl;
	patterns.close();
	patterns.open(pname);
	const auto pt = std::string((std::istreambuf_iterator<char>(patterns)),
		std::istreambuf_iterator<char>());

	/*int cnt = 0;
	const std::regex re(pt);
	for (std::string line; std::getline(texts, line); ) {
		std::smatch match;
		if (std::regex_search(line, match, re)) {
			++cnt;
		}
	}
	std::cout << cnt << std::endl;*/

	std::cout << ptext << " :for: " <<  pt << std::endl;
	std::cout << clock() / 1000.0 << std::endl;	
}
