#ifndef TRIGRAMCONTAINER_H
#define TRIGRAMCONTAINER_H

#include <vector>
#include <stdint.h>

namespace qqipa {

// TODO: or includes are here

using trigram_t = uint_fast32_t;

struct TrigramContainer
{
public:
    TrigramContainer();

private:
    void makeAvailable();




public:
    std::vector<trigram_t> trigrams_;

};

}

#endif // TRIGRAMCONTAINER_H
