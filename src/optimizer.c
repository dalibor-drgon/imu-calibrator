
#include "optimizer.h"

float optimize_muller_step(float p0, float p1, float p2, float y0, float y1, float y2) {
    float h0 = p0-p2, h1 = p1-p2;
    float c = y2;

    float e0 = y0 - c;
    float e1 = y1 - c;

    float D = h0 * h1 * (h0 - h1);
    float D1 = e0 * h1 - e1 * h0;
    float D2 = e1 * h0 * h0 - e0 * h1 * h1;
    float a = D1 / D;
    float b = D2 / D;
    float p3 = p2 - b / (2*a);
    return p3;
}

