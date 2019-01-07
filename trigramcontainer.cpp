#include "trigramcontainer.h"

#include <algorithm>

namespace qqipa {

TrigramContainer::TrigramContainer()
{

}

void TrigramContainer::reserve(size_t new_capacity)
{
    trigrams_.reserve(new_capacity);
}

std::vector<trigram_t>::iterator TrigramContainer::begin()
{
    if (!accessible) {
        makeAccessible();
    }
    return trigrams_.begin();
}

std::vector<trigram_t>::iterator TrigramContainer::end()
{
    if (!accessible) {
        makeAccessible();
    }
    return trigrams_.end();
}

void TrigramContainer::push_back(trigram_t element)
{
    trigrams_.push_back(element);
}

void TrigramContainer::resize(size_t new_size)
{
    trigrams_.resize(new_size);
}

size_t TrigramContainer::size() const
{
    return trigrams_.size();
}

void TrigramContainer::makeAccessible()
{
    if (!trigrams_.empty()) {
        std::sort(trigrams_.begin(), trigrams_.end());
        accessible = true;
    }
}

}
