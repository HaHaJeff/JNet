#ifndef RANDOM_H
#define RANDOM_H

#include <random>

namespace jraft {
class Random {
public:
    Random(int seed, int left, int right) : e_(seed), u_(left, right) {}
    int64_t Generate() {
        return u_(e_);
    }
private:
    std::default_random_engine e_;
    std::uniform_int_distribution<int64_t> u_;
};
}

#endif
