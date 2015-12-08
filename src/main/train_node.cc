#include "train_node.h"
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <iostream>
#include <common_utils.h>
#include <local_task.h>

using namespace std;
using namespace lncc;


lncc::TrainNode::TrainNode(const LnContext* ln_ctx,
                           int* sample_index,
                           int sample_num,
                           const DataSet* data_set,
                           const double* ngrads,
                           double reg_L1,
                           double reg_L2):
    PredNode(0.0),
    ln_ctx_(ln_ctx),
    sample_index_(sample_index),
    sample_num_(sample_num),
    data_set_(data_set),
    ngrads_(ngrads),
    sum_ngrads_(0.0),
    sum_weights_(0.0),
    max_split_gain_(0.0),
    reg_L1_(reg_L1),
    reg_L2_(reg_L2) {
    ComputeResponse();

    FindSplit();    
}



void lncc::TrainNode::ComputeResponse() {
    sum_ngrads_ = 0.0;
    sum_weights_ = 0.0;
    for (int i = 0; i < sample_num_; i ++) {
        double weight = data_set_->weights_[sample_index_[i]];
        sum_ngrads_ += weight * ngrads_[sample_index_[i]];
        sum_weights_ += weight;
    }

    response_= ln_ctx_->learning_rate * sum_ngrads_ / (sum_weights_ + ln_ctx_->lambda);
}

double lncc::TrainNode::FindSplitByCol(const FeatureCol* fcol, double* out_split_fval) {
    double lambda = ln_ctx_->lambda;
    double max_gain = 0.0;
    int value_num = fcol->value_set_.size();
    // TODO: cache the allocated memory
    double* ngrads_by_val = new double[value_num];
    double* weights_by_val = new double[value_num];
    memset((char*)ngrads_by_val, 0, sizeof(double) * value_num);
    memset((char*)weights_by_val, 0, sizeof(double) * value_num);
    for (int i = 0; i < sample_num_; i ++) {
        int row_id = sample_index_[i];
        int fval_id = fcol->fv_val_id_[row_id];
        double weight = data_set_->weights_[row_id];
        ngrads_by_val[fval_id] += weight * ngrads_[row_id];
        weights_by_val[fval_id] += weight;
    }

    double left_ngrads = 0.0;
    double left_weights = 0.0;
    double right_ngrads = 0.0;
    double right_weights = 0.0;
    double root_gain = sum_ngrads_ * sum_ngrads_ / (sum_weights_+lambda);
    double best_gain = 0.0; // default value 0 limits that the best gain must be larger than 0.0
    int best_split_vid = -1;
    int best_split_prev_vid = -1;
    // iterate feature value from small to large
    int prev_vid = -1;
    for (int vid = 0; vid < value_num; vid ++) {
        // skip empty rows for a value
        if (weights_by_val[vid] == 0.0) {
            continue;
        }
        if (prev_vid == -1) {
            prev_vid = vid;
            continue;
        }
        left_ngrads += ngrads_by_val[prev_vid];
        left_weights += weights_by_val[prev_vid];
        right_ngrads = sum_ngrads_ - left_ngrads;
        right_weights = sum_weights_ - left_weights;
        double left_response = left_ngrads / (left_weights+lambda);
        double right_response = right_ngrads / (right_weights+lambda);
        double left_r2 = left_response * left_response;
        double right_r2 = right_response * right_response;
        double reg = reg_L1_ * (fabs(left_response) + fabs(right_response)) + reg_L2_ * (left_r2 + right_r2);
        double gain = left_r2 * (left_weights+lambda) + right_r2 * (right_weights+lambda) - root_gain - reg;

        //double reg = reg_L1_ * (fabs(left_response) + fabs(right_response)) + reg_L2_ * (left_response* left_response + right_response * right_response);
        //double gain = left_ngrads * left_ngrads / (left_weights+lambda) + right_ngrads * right_ngrads / (right_weights+lambda) - root_gain - reg;
        
#if 0
        if (fcol->fea_name_ == "MMAX") {
            lncc::log_info("aniluke: calc split for fname=%s, value=%lf, prev_value=%lf, left_ngrads=%lf, left_weights=%lf, total_ngrads=%lf, total_weight=%lf, root_gain=%lf, gain=%lf, reg=%lf\n", fcol->fea_name_.c_str(), fcol->value_set_[vid], fcol->value_set_[prev_vid], left_ngrads, left_weights, sum_ngrads_, sum_weights_, root_gain, gain, reg);
        }
#endif
        if (gain > best_gain+1e-6) {
            best_gain = gain;
            best_split_vid = vid;
            best_split_prev_vid = prev_vid;
        }
        prev_vid = vid;
    }

    delete[] ngrads_by_val;
    delete[] weights_by_val;

    if (best_split_vid == -1) {
        *out_split_fval = 0.0;
        return 0.0;
    }

    *out_split_fval = (fcol->value_set_[best_split_vid] + fcol->value_set_[best_split_prev_vid]) / 2.0;
    return best_gain;
}

namespace lncc {
    class ColumnSplitTask : public LocalTask {
    public:
        ColumnSplitTask(TrainNode* train_node, const FeatureCol* fcol):LocalTask(), train_node_(train_node), fcol_(fcol), split_fval_(0.0), gain_(0.0) {
        };
    public:
        virtual void DoTask() {
            gain_ = train_node_->FindSplitByCol( fcol_, &split_fval_);
        }
        
    private:
        TrainNode* train_node_;
        const FeatureCol* fcol_;
    public:
        double split_fval_;
        double gain_;
    };

}


bool lncc::TrainNode::FindSplit() {
    max_split_gain_ = 0.0;
    split_fid_ = -1;
    if (sample_num_ <= 1) {
        return false;
    }
#if 1
    vector<ColumnSplitTask*> col_split_tasks;
    col_split_tasks.reserve(data_set_->fmat_.size());
    
    // dispatch all feature column splitting tasks
    for (int fid = 0; fid < data_set_->fmat_.size(); fid ++) {
        const FeatureCol* fcol = &data_set_->fmat_[fid];
        ColumnSplitTask* task = new ColumnSplitTask(this, fcol);
        ln_ctx_->task_queue->push(task);
        col_split_tasks.push_back(task);
    }

    for (int fid = 0; fid < col_split_tasks.size(); fid ++) {
        col_split_tasks[fid]->Wait();
        
        double split_fval = col_split_tasks[fid]->split_fval_;
        
        double gain = col_split_tasks[fid]->gain_;
        delete col_split_tasks[fid];
        
        if (gain > max_split_gain_ + 1e-6) {
            max_split_gain_ = gain;
            split_fid_ = fid;
            split_fval_ = split_fval;
        }
    }
#endif
#if 0
    for (int fid = 0; fid < data_set_->fmat_.size(); fid ++) {
        const FeatureCol* fcol = &data_set_->fmat_[fid];
        
        double split_fval = 0.0;
        
        double gain = FindSplitByCol(fcol, &split_fval);
        
        if (gain > max_split_gain_ + 1e-6) {
            max_split_gain_ = gain;
            split_fid_ = fid;
            split_fval_ = split_fval;
        }
    }
#endif
    
    if (split_fid_ == -1) {
        max_split_gain_ = 0.0;
        split_fval_ = 0.0;
        return false;
    }
    split_fea_name_ = data_set_->fmat_[split_fid_].fea_name_;

    //printf("aniluke: find best split %d, %s, %lf, gain=%lf\n", split_fid_, split_fea_name_.c_str(), split_fval_, max_split_gain_);

    return true;
}

bool lncc::TrainNode::Split() {
    if (split_fid_ == -1) {
        return false;
    }

    int left_sid = 0;
    int right_sid = sample_num_ -1;
    const FeatureCol* fcol = &data_set_->fmat_[split_fid_];

    while (left_sid <= right_sid) {
        int row_id = sample_index_[left_sid];

        double fval = fcol->fv_[row_id];
        if (fval < split_fval_) {
            left_sid ++;
        } else {
            // swap left and right index
            int tmp = sample_index_[right_sid];
            sample_index_[right_sid] = sample_index_[left_sid];
            sample_index_[left_sid] = tmp;
            right_sid --;
            // left still keep unchanged, so in next round the new left value
            // can be compared with split value
        }
    }

    // the last left will be the begin of the right part
    // but need to test out of boundary
    int right_begin = left_sid;
    if (right_begin == sample_num_) {
        cerr << "Split failed, no samples having value larger than or equal to " << split_fval_ << endl;
        split_fid_ = -1;
        return false;
    }
    if (right_begin == 0) {
        cerr << "Split failed, no samples having value smaller than " << split_fval_ << endl;
        split_fid_ = -1;
        return false;
    }
    left_ = new TrainNode(ln_ctx_, sample_index_, right_begin, data_set_, ngrads_, reg_L1_, reg_L2_);

    right_ = new TrainNode(ln_ctx_, &sample_index_[right_begin], sample_num_ - right_begin, data_set_, ngrads_, reg_L1_, reg_L2_);
    
    return true;
}

