#include "feature_col.h"
#include <iostream>
#include <stdio.h>
#include <algorithm>
using namespace lncc;
using namespace std;

void lncc::FeatureCol::CreateValueSet() {
    vector<double> sorted_fv = fv_;
    std::sort(sorted_fv.begin(), sorted_fv.end());
    
    // assume fv_ size is not 0
    value_set_.push_back(sorted_fv[0]);
    for (int i = 1; i < sorted_fv.size(); i ++) {
        if (sorted_fv[i] != sorted_fv[i-1]) {
            value_set_.push_back(sorted_fv[i]);
        }
    }

    // value index for each sample

    for (int i = 0; i < fv_.size(); i ++) {
        vector<double>::iterator low = std::lower_bound(value_set_.begin(), value_set_.end(), fv_[i]);
        int value_index = int(low - value_set_.begin());
        // TODO: test
        //printf ("fv %d: val %d, %lf\n", i, value_index, value_set_[value_index]);

        fv_val_id_.push_back(value_index);
    }
}
