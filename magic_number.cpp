#include "magic_number.h"

#include <algorithm>

namespace magic_numbers {

magic_number::magic_number(std::string name_, std::vector<char> number_)
    : name(name_), number(number_)
{

}

bool magic_number::find(const char *buffer)
{
    return std::equal(number.begin(), number.end(), buffer);
}

}
