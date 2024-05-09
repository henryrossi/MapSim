#ifndef NOISE_H
#define NOISE_H

#include <cmath>
#include <iostream>

class perlin {
      public:
        perlin();
        /* Fractal Brownian Motion */
        float fbm_noise(float x, float y, int n_octaves);
        float noise(float x, float y);

      private:
        int p[512];

        float fade(float t);
        float lerp(float t, float a, float b);
        float grad(int hash, float x, float y);
};

#endif /* NOISE_H*/