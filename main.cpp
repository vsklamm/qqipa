#include "kmp.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <ctime>
#include <string>

int main()
{
	std::ifstream pattern("pattern.in");
	std::ifstream text("text.in");

	auto kmp_inst = kmp();
	std::cout << kmp_inst.count_occurrences(pattern, text) << std::endl;
	std::cout << clock() / 1000.0 << std::endl;

	system("pause");
}
