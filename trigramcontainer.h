#ifndef TRIGRAMCONTAINER_H
#define TRIGRAMCONTAINER_H

#include <vector>
#include <stdint.h>

namespace qqipa {

using trigram_t = uint_fast32_t;

constexpr trigram_t tbytes_mask = 0x00FFFFFF;

struct TrigramContainer
{
public:
    TrigramContainer();

    void reserve(size_t new_capacity);
    std::vector<trigram_t>::iterator begin();
    std::vector<trigram_t>::iterator end();
    void push_back(trigram_t element);
    void resize(size_t new_size);
    size_t size() const;

private:
    void makeAccessible();

private:
    bool accessible = false;
    std::vector<trigram_t> trigrams_;

};

}

#endif // TRIGRAMCONTAINER_H
