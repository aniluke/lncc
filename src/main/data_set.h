#ifndef _DATA_SET_H_
#define _DATA_SET_H_

#include <common.h>
#include <vector>
#include "feature_col.h"
#include <ln_context.h>

namespace lncc {
    class DataSet {
    public:
        DataSet();
        ~DataSet();

    public:
        int LoadTrainingData(const LnContext* ln_ctx);
        int LoadTestingData(const LnContext* ln_ctx);

        int LoadFvData(const std::string& fv_filename, 
                          const std::string& exclude_features);

        std::size_t GetRowNum() const { return row_num_; };

        void SamplingIndex(std::vector<int>* out_sample_index, double sample_rate);

    private:
        int LoadWeights(const LnContext* ln_ctx);

    public:
        std::size_t row_num_;
        std::vector<std::string> full_fea_names_;
        std::vector<std::string> used_fea_names_;
        std::vector<FeatureCol> fmat_;
        // only in training
        std::vector<double> targets_;
        std::vector<double> weights_;

    };
}

#endif
