#include <iostream>

#include "noise.hpp"

int test_perlin_noise() {
        perlin p{};
        int count{};
        int high{};
        for (int x = 0; x < 1024; x++) {
                for (int y = 0; y < 1024; y++) {
                        float value = p.noise(x + 0.5, y + 0.5);
                        if (value > 1.0 || value < -1.0) {
                                count++;
                        }
                        if (value < -0.9 || value > 0.9) {
                                high++;
                        }
                }
        }
        std::cout << "Total number of noise returned outside of range was: "
                  << count << "\n";
                  std::cout << "Total number of noise returned in high range was: "
                  << high << "\n";
        return 0;
}

int main() {
        test_perlin_noise();
        return 0;
}