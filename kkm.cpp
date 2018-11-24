#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <ctime>

using namespace std;

vector<int> res;
int cnt = 0;

void printv(vector<int> &v)
{
	for (auto& e : v)
	{
		cout << e << ' ';
	}
	cout << endl;
}

vector<int> CalcPrefixFunction(string &s)
{
	vector<int> result = vector<int>(s.size()); // array with prefix function values
	result.shrink_to_fit();

	result[0] = 0;
	int k = 0;
	for (int i = 1; i < s.size(); ++i)
	{
		while (k > 0 && s[i] != s[k]) 
			k = result[k - 1];
		if (s[k] == s[i]) ++k;
		result[i] = k;
	}

	return result;
}

int kmp(std::istream& pattern, std::istream& text)
{
	auto p = CalcPrefixFunction(s);
	printv(p);

	while (text >> ch)
	{
		textqu.push_back(ch);
		if (textqu.size() > this->pattern.size())
			textqu.pop_front();

		recalc(ch, kmp_task);
		if (ret_condition()) return;
		++text_it;
	}

	size_t maxPrefixLength = 0;
	for (size_t i = 0; i < text.size(); ++i)
	{
		while (maxPrefixLength != 0 && text[i] != s[maxPrefixLength])
			maxPrefixLength = p[maxPrefixLength - 1];

		if (s[maxPrefixLength] == text[i]) ++maxPrefixLength;
		if (maxPrefixLength == s.size())
		{
			// int idx = i - s.size() + 1;
			++cnt;
			maxPrefixLength = p[maxPrefixLength - 1];
		}
	}

	return -1;
}

int main()
{
	const std::string pname = "pattern.txt";
	const std::string ptext = "sherlock.txt";
	std::string pat;

	std::ifstream pattern(pname);
	std::ifstream text(ptext);

	std::ios_base::sync_with_stdio(false);
	time_t my_time = time(nullptr);

	std::cout << std::endl << "========= =========" << std::endl;
	std::cout << ctime(&my_time);
	
	std::cout << cnt << std::endl;
	std::cout << clock() / 1000.0 << std::endl;

	system("pause");
}
