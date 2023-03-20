
#ifndef _OPTIMIZER_H_
#define _OPTIMIZER_H_

#ifdef __cplusplus
extern "C" {
#endif

/// Similar to Muller algorithm, this one just interpolates (or extrapolates) approximate
/// position of valley.
///
/// Arguments pi are X coordinates, yi are actual values of the optimized function at given values.
/// Returns the inter/extra-polated point p3 where the valley is probably located.
float optimize_muller_step(float p0, float p1, float p2, float y0, float y1, float y2);

#ifdef __cplusplus
}
#endif

#endif

