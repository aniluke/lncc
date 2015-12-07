#ifndef _FEATURE_COL_H_
#define _FEATURE_COL_H_

#include <common.h>
#include <vector>

namespace lncc {
    class FeatureCol {
    public:
        FeatureCol(const std::string& fea_name):fea_name_(fea_name) {}
        ~FeatureCol() {}
        
        std::size_t GetRowNum() const { return fv_.size(); }
        std::size_t GetValueNum() const { return value_set_.size(); }

        void CreateValueSet();

    public:
        std::string fea_name_;
        std::vector<double> fv_; // with size of row number
        std::vector<int> fv_val_id_; // with size of row number
        std::vector<double> value_set_; // with size of value number
    };
}
#endif
