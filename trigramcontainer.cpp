#include "trigramcontainer.h"

#include <algorithm>

namespace qqipa {

TrigramContainer::TrigramContainer()
{

}

void TrigramContainer::makeAvailable()
{
    std::sort(trigrams_.begin(), trigrams_.end());
}

}

