#pragma once
#include <string>
#include <vector>
#include <deque>
#include <boost/circular_buffer.hpp>

using fint = uint32_t;
using fvector = std::vector<fint>;
using symbol_t = char;

struct kmp
{
public:
	kmp();

	bool contains_pattern(std::istream &pattern, std::istream &text);
	fint count_occurrences(std::istream &pattern, std::istream &text);
	fvector pattern_positions(std::istream &pattern, std::istream &text);
	bool contains_pattern(const std::string &pattern, const std::string &text);

private:
	template <typename Fret, typename Ft>
	void process_kmp(std::istream &pattern, std::istream &text, 
		Fret ret_condition, Ft kmp_task);

	static fvector prefix_function(const std::string &s);
	void preprocess(std::istream& patt);

	template <typename Ft>
	void recalc(char ch, Ft &kmp_task);
	inline void push_symbol(symbol_t ch);

private:
	fvector pattern_prefix;
	std::string pattern;
	// std::deque<symbol_t> textqu;
	boost::circular_buffer<symbol_t> textqu;
	fvector occurrences;
	fint count = 0;

	size_t max_pref_length = 0;
	size_t text_it = 0;
	bool is_found = false;
};
