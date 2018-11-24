#define _CRT_SECURE_NO_WARNINGS

#include "kmp.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <ctime>
#include <string>
#include <cstdio>

int main()
{
	const std::string pname = "pattern.txt";
	const std::string ptext = "sherlock.txt";
	std::string pat;

	std::ifstream pattern(pname);
	std::ifstream text(ptext);

	freopen("log.txt", "a", stdout);

	std::ios_base::sync_with_stdio(false);
	text.tie(nullptr);
	time_t my_time = time(nullptr);

	auto kmp_inst = kmp();
	std::cout << std::endl << "========= =========" << std::endl;
	std::cout << ctime(&my_time);
	std::cout << kmp_inst.contains_pattern(pattern, text) << std::endl;
	std::cout << ptext << std::endl;
	std::cout << clock() / 1000.0 << std::endl;
}
