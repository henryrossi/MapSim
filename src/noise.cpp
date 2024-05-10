#include "noise.hpp"

static int permutation[] = {
        151, 160, 137, 91,  90,  15,  131, 13,  201, 95,  96,  53,  194, 233,
        7,   225, 140, 36,  103, 30,  69,  142, 8,   99,  37,  240, 21,  10,
        23,  190, 6,   148, 247, 120, 234, 75,  0,   26,  197, 62,  94,  252,
        219, 203, 117, 35,  11,  32,  57,  177, 33,  88,  237, 149, 56,  87,
        174, 20,  125, 136, 171, 168, 68,  175, 74,  165, 71,  134, 139, 48,
        27,  166, 77,  146, 158, 231, 83,  111, 229, 122, 60,  211, 133, 230,
        220, 105, 92,  41,  55,  46,  245, 40,  244, 102, 143, 54,  65,  25,
        63,  161, 1,   216, 80,  73,  209, 76,  132, 187, 208, 89,  18,  169,
        200, 196, 135, 130, 116, 188, 159, 86,  164, 100, 109, 198, 173, 186,
        3,   64,  52,  217, 226, 250, 124, 123, 5,   202, 38,  147, 118, 126,
        255, 82,  85,  212, 207, 206, 59,  227, 47,  16,  58,  17,  182, 189,
        28,  42,  223, 183, 170, 213, 119, 248, 152, 2,   44,  154, 163, 70,
        221, 153, 101, 155, 167, 43,  172, 9,   129, 22,  39,  253, 19,  98,
        108, 110, 79,  113, 224, 232, 178, 185, 112, 104, 218, 246, 97,  228,
        251, 34,  242, 193, 238, 210, 144, 12,  191, 179, 162, 241, 81,  51,
        145, 235, 249, 14,  239, 107, 49,  192, 214, 31,  181, 199, 106, 157,
        184, 84,  204, 176, 115, 121, 50,  45,  127, 4,   150, 254, 138, 236,
        205, 93,  222, 114, 67,  29,  24,  72,  243, 141, 128, 195, 78,  66,
        215, 61,  156, 180};

void shuffle() {
        unsigned seed =
                std::chrono::system_clock::now().time_since_epoch().count();
        std::default_random_engine gen(seed);
        std::uniform_int_distribution<int> dist{0, 255};
        for (int i = 255; i > 0; i--) {
                int index = dist(gen);
                int temp = permutation[i];

                permutation[i] = permutation[index];
                permutation[index] = temp;
        }
}

perlin::perlin() {
        shuffle();
        for (int i = 0; i < 256; i++) {
                p[256 + i] = p[i] = permutation[i];
        }
}

float perlin::fbm_noise(float x, float y, int n_octaves) {
        float result = 0.0;
        float amplitude = 1.0;
        float frequency = 0.005;

        for (int octave = 0; octave < n_octaves; octave++) {
                float n = amplitude * noise(x * frequency, y * frequency);
                result += n;

                amplitude *= 0.5;
                frequency *= 2.0;
        }

        return result;
}

float perlin::noise(float x, float y) {
        int X = static_cast<int>(x) & 255;
        int Y = static_cast<int>(y) & 255;

        x -= floor(x);
        y -= floor(y);

        float u = fade(x);
        float v = fade(y);

        int tr = p[p[X + 1] + Y + 1], tl = p[p[X] + Y + 1],
            br = p[p[X + 1] + Y], bl = p[p[X] + Y];

        // clang-format off
        return lerp(v, 
                    lerp(u, grad(bl, x, y), 
                            grad(br, x - 1, y)),
                    lerp(u, grad(tl, x, y - 1), 
                            grad(tr, x - 1, y - 1)));
        // clang-format on
}

float perlin::fade(float t) {
        /*
         * Smoothing function
         * Optimized from 6t^5 - 15t^4 + 10t^3
         */
        return t * t * t * (t * (t * 6 - 15) + 10);
}

float perlin::lerp(float t, float a, float b) { return a + t * (b - a); }

float perlin::grad(int hash, float x, float y) {
        switch (hash & 0x7) {
        case 0x0:
                return x + y;
        case 0x1:
                return x;
        case 0x2:
                return x - y;
        case 0x3:
                return -y;
        case 0x4:
                return -x - y;
        case 0x5:
                return -x;
        case 0x6:
                return -x + y;
        case 0x7:
                return y;
        default:
                return 0.0;
        }

        // int h = hash & 3;
        // if (h == 0) {
        //         return (x * 1.0) + (y * 1.0);
        // } else if (h == 1) {
        //         return (x * -1.0) + (y * 1.0);
        // } else if (h == 2) {
        //         return (x * -1.0) + (y * -1.0);
        // } else {
        //         return (x * 1.0) + (y * -1.0);
        // }

        /* Revisit */
        // h = hash & 7;
        // float u = h < 4 ? x : y;
        // float v = h < 2 ? y : x;
        // return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}
