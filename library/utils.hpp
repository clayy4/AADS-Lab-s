#ifndef UTILS_H
#define UTILS_H

#include <random>

template <typename T>
class Dice {
private:
    std::mt19937 rand_gen;
    std::uniform_real_distribution<float> dis;

public:
    Dice() = default;

    Dice(T min, T max): rand_gen(std::random_device{}()), dis(static_cast<float>(min), static_cast<float>(max)) {}

    T roll() {
        float value = dis(rand_gen);

        if constexpr (std::is_same<T, bool>::value) {
            return static_cast<bool>(value > 0.5f);
        }
        else {
            return static_cast<T>(value);
        }
    }
};

#endif // UTILS_H
