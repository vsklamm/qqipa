#include "trigramcontainer.h"

#include <algorithm>

namespace qqipa {

TrigramContainer::TrigramContainer()
{

}

void TrigramContainer::reserve(size_t newCapacity)
{
    trigrams.reserve(newCapacity);
}

std::vector<trigram_t>::iterator TrigramContainer::begin()
{
    if (!accessible) {
        makeAccessible();
    }
    return trigrams.begin();
}

std::vector<trigram_t>::iterator TrigramContainer::end()
{
    if (!accessible) {
        makeAccessible();
    }
    return trigrams.end();
}

void TrigramContainer::push_back(trigram_t element)
{
    trigrams.push_back(element);
}

void TrigramContainer::resize(size_t newSize)
{
    trigrams.resize(newSize);
}

size_t TrigramContainer::size() const noexcept
{
    return trigrams.size();
}

void TrigramContainer::makeAccessible()
{
    if (!trigrams.empty()) {
        std::sort(trigrams.begin(), trigrams.end());
        accessible = true;
    }
}

}
