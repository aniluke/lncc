#ifndef _BASE_FUNCTION_H_
#define _BASE_FUNCTION_H_

#include <common.h>
#include "data_set.h"
#include <tinyxml2.h>

namespace lncc {
    class BaseFunction {
    public:
        virtual ~BaseFunction() {};

        virtual int Fit(const std::vector<int>& sample_index, const DataSet& data_set, const std::vector<double>& targets, double learning_rate) = 0;

        virtual double Predict(const DataSet& data_set, int row_id) = 0;

        virtual void SaveModel(tinyxml2::XMLDocument* doc, tinyxml2::XMLElement* elem) = 0;

        virtual void CollectFeatureStats(std::vector<double>* out_feature_stats, double learning_rate) = 0;

        virtual void DumpModel() = 0;
    };
}
#endif
