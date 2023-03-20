#include <eigen3/Eigen/Dense>
#include <fstream>
#include <vector>
#include <iostream>
#include <iomanip>

#include "calibrator.h"

using namespace std;
using namespace Eigen;

static void split(const string & str, double *out, unsigned cnt) {
    string::size_type start = 0;
    for(unsigned i = 0; i < cnt; i++) {
        string::size_type index = str.find("\t", start);
        string occurence = str.substr(start, index == string::npos ? string::npos : (index - start));
        //cout << occurence << endl;
        out[i] = stod(occurence);
        start = index + 1;
    }
}

static void process(double data[3]);

static void parse3(ifstream & in) {
    string line;
    while (getline(in, line)) {
        if (line == "") 
            continue;
        double arr[3];
        split(line, arr, sizeof(arr)/sizeof(*arr));
        process(arr);
        //out.push_back(array<double, 4>{arr[0], arr[1], arr[2], arr[3]});
    }
}

static vector<acc_calibration_entry> entries;

static void process(double data[3]) {
    entries.push_back(acc_calibration_entry{(float) data[0], (float) data[1], (float) data[2]});
}

int main(int argc, const char **argp) {
    cout << fixed << setprecision(6);

    float magnitude = 1.0f;
    const char *file = "/dev/stdin";
    if (argc == 2) {
    }
    ifstream in(file);
    parse3(in);

    MatrixXf sens(3,3);
    Vector3f off;
    float cost = acc_find_sensitivity_offsets(magnitude, entries.size(), entries.data(), sens.data(), off.data());

    cout << "Optimization done, normalized sum-of-squared-error is " << cost << endl;
    cout << "Sensitivity matrix: "<< endl;
    cout << sens << endl;
    cout << "Offset vector: "<< endl;
    cout << off << endl;
    cout << "Use as <measurement_corrected> = <sensitivity> * (<measurement> - <offset>)" << endl;
}

