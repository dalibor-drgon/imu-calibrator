#include <eigen3/Eigen/Dense>

#include "calibrator.h"
#include "optimizer.h"
#include "export.h"

using namespace std;
using namespace Eigen;


static void convert_sensitivity(float out_sensitivity[9], float sensitivity[6]) {
    out_sensitivity[0] = sensitivity[0];
    out_sensitivity[1] = sensitivity[3];
    out_sensitivity[2] = sensitivity[4];

    out_sensitivity[3] = sensitivity[3];
    out_sensitivity[4] = sensitivity[1];
    out_sensitivity[5] = sensitivity[5];

    out_sensitivity[6] = sensitivity[4];
    out_sensitivity[7] = sensitivity[5];
    out_sensitivity[8] = sensitivity[2];
}

static float calculate_cost(float mag, unsigned n, const acc_calibration_entry *measurements, const MatrixXf & sensitivity, const Vector3f & offset) {
    float cost = 0;
    for(unsigned i = 0; i < n; i++) {
        const acc_calibration_entry *cur_meas = &measurements[i];
        Vector3f meas_vec {cur_meas->x, cur_meas->y, cur_meas->z};
        Vector3f off_corrected = meas_vec - offset;
        Vector3f corrected = sensitivity * off_corrected;
#if 1
        float dist = sqrt(corrected.dot(corrected)) - mag;
        float cost_add = dist * dist;
#else
        Vector3f off_corrected_norm = off_corrected * (mag / sqrt(off_corrected.dot(off_corrected)));
        Vector3f err = corrected - off_corrected_norm;
        float cost_add = err.dot(err);
        //cout << "\t" << cost_add << "\t" << corrected.format(EigenCsvFormat) << "\t" << off_corrected_norm.format(EigenCsvFormat) << endl;
#endif
        cost += cost_add;
    }
    return cost / n; // / (mag * mag);
}

static float calculate_cost(float mag, unsigned n, const acc_calibration_entry *measurements, float params[9]) {
    MatrixXf sensitivity(3, 3);
    convert_sensitivity(sensitivity.data(), params);
    Vector3f offset;
    memcpy(offset.data(), &params[6], 3 * sizeof(*params));
    return calculate_cost(mag, n, measurements, sensitivity, offset);
}

static float calculate_cost(float mag, unsigned n, const acc_calibration_entry *measurements, float params[9], float der[9], float t) {
    float params_new[9];
    for(unsigned i = 0; i < 9; i++) {
        params_new[i] = params[i] + der[i] * t;
    }
    return calculate_cost(mag, n, measurements, params_new);
}

/// der[0..5] = sensitivty, der[6..8] = offsets; same for params
static float calculate_cost_der(float mag, unsigned n, const acc_calibration_entry *measurements, float params[9], float der[9]) {
    const float dt = mag * 0.00002;
    float params_cur[9];
    memcpy(params_cur, params, sizeof(params_cur));
    float base = calculate_cost(mag, n, measurements, params_cur);
    for(unsigned i = 0; i < 9; i++) {
        params_cur[i] += dt;
        der[i] = (calculate_cost(mag, n, measurements, params_cur) - base) / dt;
        params_cur[i] = params[i];
    }
    return base;
}


extern "C" float CALIBRATOR_EXPORT acc_find_sensitivity_offsets(
        float mag, unsigned n, const acc_calibration_entry *measurements, 
        float out_sensitivity[9], float out_offset[3]) {
    if(n < 9) {
        return INFINITY;
    }
    const float min_error = 0.001;

    float params[9] = {1.00f, 1.00f, 1.00f, 0.00f, 0.00f, 0.00f,  0.0f, 0.0f, 0.0f};
    float y2 = 0;

    for(unsigned i = 0; i < 4; i++) {
        float p0 = 0.0f;
        float p1 = -1.0f;
        float p2 = -2.0f;
        float der[9];
        float y0 = calculate_cost_der(mag, n, measurements, params, der);
        float y1 = calculate_cost(mag, n, measurements, params, der, p1);
        y2 = calculate_cost(mag, n, measurements, params, der, p2);
        for(unsigned j = 0; j < 4; j++) {
            float p3 = optimize_muller_step(p0, p1, p2, y0, y1, y2);
            float y3 = calculate_cost(mag, n, measurements, params, der, p3);
            if(isnan(y3))
                break;
            p0 = p1; p1 = p2; p2 = p3;
            y0 = y1; y1 = y2; y2 = y3;
            if(abs(p2 - p1) < min_error || abs(p2 - p0) < min_error)
                break;
        }
        for(unsigned j = 0; j < 9; j++) {
            params[j] += der[j] * p2;
        }
    }

    convert_sensitivity(out_sensitivity, params);
    memcpy(out_offset, &params[6], 3 * sizeof(*out_offset));
    return y2 / (mag * mag);
}

