#include "kmp.h"

#include <iostream>
#include <string>
#include <sstream>
#include <functional>

kmp::kmp()
{ }

bool kmp::contains_pattern(std::istream& pattern, std::istream& text)
{
	process_kmp(pattern, text,
		[&]() { return is_found; },
		[&](const fint index) { is_found = true; });
	return is_found;
}

fint kmp::count_occurrences(std::istream& pattern, std::istream& text)
{
	process_kmp(pattern, text,
		[]() { return false; },
		[&](const fint index) { ++count; });
	return count;
}

fvector kmp::pattern_positions(std::istream& pattern, std::istream& text)
{
	process_kmp(pattern, text,
		[]() { return false; },
		[&](const fint index) { occurrences.push_back(index); });
	return occurrences;
}

bool kmp::contains_pattern(const std::string& patt, const std::string& text)
{
	auto pattern_s = std::istringstream(patt);
	auto text_s = std::istringstream(text);
	return contains_pattern(pattern_s, text_s);
}

template <typename Fret, typename Ft>
void kmp::process_kmp(std::istream& pattern, std::istream& text, 
	Fret ret_condition, Ft kmp_task)
{
	preprocess(pattern);

	symbol_t ch;
	while (text >> ch)
	{
		textqu.push_back(ch);
		if (textqu.size() > this->pattern.size())
			textqu.pop_front();

		recalc(ch, kmp_task);
		if (ret_condition()) return;
		++text_it;
	}
}

fvector kmp::prefix_function(const std::string &s)
{
	auto result = fvector(s.size());
	result.shrink_to_fit();

	result[0] = 0;
	fint k = 0;
	for (size_t i = 1; i < s.size(); ++i)
	{
		while (k > 0 && s[i] != s[k]) k = result[k - 1];
		if (s[k] == s[i]) ++k;
		result[i] = k;
	}
	return result;
}

void kmp::preprocess(std::istream& patt)
{
	patt >> pattern;
	max_pref_length = 0;
	size_t text_it = 0;
	is_found = false;
	textqu.set_capacity(pattern.size());
	pattern_prefix = pattern.size() > 0
		? prefix_function(pattern)
		: fvector(1);
}

template <typename Ft>
void kmp::recalc(char cur_text_ch, Ft &kmp_task)
{
	while (max_pref_length != 0 && cur_text_ch != pattern[max_pref_length])
		max_pref_length = pattern_prefix[max_pref_length - 1];
	if (pattern[max_pref_length] == cur_text_ch)
		++max_pref_length;
	if (max_pref_length == pattern.size())
	{
		kmp_task(fint(text_it + 1 - pattern.size()));
		max_pref_length = pattern_prefix[max_pref_length - 1];
	}
}
