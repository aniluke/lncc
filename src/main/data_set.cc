#include "data_set.h"
#include <common_utils.h>
#include <set>
#include <algorithm>
#include <fstream>
#include <iostream>
using namespace std;
using namespace lncc;

lncc::DataSet::DataSet():
    row_num_(0)    
{
    
}

lncc::DataSet::~DataSet() {
}

static void ExtractUsedFeatures(vector<string>& used_fea_names, 
                                        vector<bool>& full_fea_used_states, 
                                        const vector<string>& full_fea_names, 
                                        const string& exclude_features) {
    set<string> exclude_fea_set;
    vector<string> exclude_fea_vec;
    lncc::str_split(exclude_features, ',',exclude_fea_vec);
    for( vector<string>::iterator it = exclude_fea_vec.begin(); it != exclude_fea_vec.end(); it ++) {
        exclude_fea_set.insert(lncc::toupper(*it));
    }
    exclude_fea_set.insert("LABEL");

    for (vector<string>::const_iterator it = full_fea_names.begin(); it != full_fea_names.end(); it ++) {
        if (exclude_fea_set.find(*it) == exclude_fea_set.end()) {
            used_fea_names.push_back(*it);
            full_fea_used_states.push_back(true);
        } else {
            full_fea_used_states.push_back(false);
        }
    }
}

int lncc::DataSet::LoadTrainingData(const LnContext* ln_ctx) {
    int ret = LoadFvData(ln_ctx->train_fv, ln_ctx->exclude_features);
    if (ret != 0)
        return ret;

    // generate value set for each feature
    for (int i = 0; i < fmat_.size(); i ++) {
        fmat_[i].CreateValueSet();
    }

    // load weight
    if (ln_ctx->weights.size() == 0) {
        log_info("Use default weights\n");
        for (int i = 0; i < row_num_; i ++) {
            weights_.push_back(1.0);
        }
        return ret;
    }

    log_info("Load weights from %s\n", ln_ctx->weights.c_str());
    ret = load_vector(weights_, ln_ctx->weights);
    if (ret != 0)
        return ret;
    if (weights_.size() != row_num_) {
        log_error("Weights is not same count as samples: %d, %d\n", weights_.size(), row_num_);
        return -1;
    }

    return 0;
}

int lncc::DataSet::LoadTestingData(const LnContext* ln_ctx) {
    int ret = LoadFvData(ln_ctx->test_fv, ln_ctx->exclude_features);
    if (ret != 0 )
        return ret;
    return 0;
}

int lncc::DataSet::LoadFvData(const string& fv_filename, const string& exclude_features) {
    log_info("Load fv from %s\n", fv_filename.c_str());

    ifstream fv_ifs(fv_filename.c_str());
    if (fv_ifs.fail() || fv_ifs.eof()) {
        log_error("failed open fv file or it is empty: %s\n", fv_filename.c_str());
        return -1;
    }

    // read header
    std::string line;
    std::getline(fv_ifs, line);

    lncc::str_split(line, '\t', full_fea_names_);
    if (full_fea_names_.size() < 2) {
        fv_ifs.close();
        log_error("invalid column num: %d\n", full_fea_names_.size());
        return -1;
    }
    for (int i = 0; i < full_fea_names_.size(); i ++) {
        full_fea_names_[i] = lncc::toupper(full_fea_names_[i]);
        //cout << "feature: " << full_fea_names_[i] << endl;
    }

    vector<bool> fea_used_states;
    ExtractUsedFeatures(used_fea_names_, fea_used_states, full_fea_names_, exclude_features);
    log_info("full feature size: %d\n", full_fea_names_.size());
    log_info("used feature size: %d\n", used_fea_names_.size());

    // init samples features column
    int i = 0;
    for (i = 0; i < used_fea_names_.size(); i ++) {
        fmat_.push_back(FeatureCol(used_fea_names_[i]));
    }

    // load samples
    row_num_ = 0;
    while (!fv_ifs.eof()) {
        std::getline(fv_ifs, line);
        if (line.size() == 0)
            break;

        vector<string> fv;
        lncc::str_split(line, '\t', fv);
        if (fv.size() != full_fea_names_.size()) {
            log_error("invalid sample that feature num is wrong: %d, %d\n", fv.size(), full_fea_names_.size());
            break;
        }
        int fid = 0;
        for (i = 0; i < fv.size(); i ++) {
            if (full_fea_names_[i] == "LABEL") {
                targets_.push_back( atof(fv[i].c_str()) );
            } else if (fea_used_states[i]) {
                fmat_[fid].fv_.push_back( atof(fv[i].c_str()) );
                fid ++;
            }
        }
        row_num_ ++;
    }

    fv_ifs.close();
    if (row_num_ == 0) {
        log_error("row num is empty\n");
        return -1;
    }

    return 0;
}


void lncc::DataSet::SamplingIndex(std::vector<int>* out_sample_index, double sample_rate) {
    out_sample_index->clear();
    if (sample_rate == 1.0) {
        for (int i = 0; i < row_num_; i ++) {
            out_sample_index->push_back(i);
        }
        return;
    }
#if 0
    vector<int> row_index;
    for (int i = 0; i < row_num_; i ++) {
        row_index.push_back(i);
    }

    for (int i = 0; i < row_num_; i ++) {
        if ((double)out_sample_index->size() / (double)row_num_ > sample_rate)
            break;

        int range = row_num_ - i;
        double r = (double)random() / (double)RAND_MAX * (range - 1);
        int rid = i + int(r);
        //printf ("aniluke: sample index: %d, %d\n", rid, row_index[rid]);
        out_sample_index->push_back(row_index[rid]);
        // replace the selected row with the un-selected row
        // if i == rid, it's still OK because in next iteration i will be +1.
        row_index[rid] = row_index[i];
    }
#endif
    double bound = row_num_ * sample_rate;
    for (int i = 0; i < row_num_; i ++) {
        if (rand() % row_num_ < bound) {
            out_sample_index->push_back(i);
        }
    }
}

