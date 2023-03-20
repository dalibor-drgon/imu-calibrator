#ifndef _CALIBRATION_H_
#define _CALIBRATION_H_

#include "export.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    float x, y, z;
    float __ign;
} acc_calibration_entry;


float CALIBRATOR_EXPORT acc_find_sensitivity_offsets(
        float mag, 
        unsigned n, 
        const acc_calibration_entry *measurements, 
        float out_sensitivity[9], 
        float out_offset[3]);

#ifdef __cplusplus
}
#endif

#endif
