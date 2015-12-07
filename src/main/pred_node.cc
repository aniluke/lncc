#include "pred_node.h"
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <iostream>
#include <common_utils.h>

using namespace std;
using namespace lncc;

lncc::PredNode::PredNode(double response):
    left_(NULL),
    right_(NULL),
    response_(response),
    split_fid_(-1),
    split_fval_(0.0),
    split_fea_name_() {

}

lncc::PredNode::PredNode(PredNode* left, PredNode* right, const std::string& split_fea_name,
                         int split_fid, double split_fval):
    left_(left),
    right_(right),
    response_(0.0),
    split_fid_(split_fid),
    split_fval_(split_fval),
    split_fea_name_(split_fea_name) {

}

lncc::PredNode::~PredNode() {
    if (left_) {
        delete left_;
    }
    if (right_) {
        delete right_;
    }
}

void lncc::PredNode::SetLeaf() {
    split_fid_ = -1;
    if (left_) {
        delete left_;
        left_ = NULL;
    }
    if (right_) {
        delete right_;
        right_ = NULL;
    }
}

void lncc::PredNode::DumpNode(int depth) {
    for(int i = 0; i < depth; i ++) {
        printf ("\t");
    }
    if (IsLeaf()) {
        printf("response=%lf\n", response_);
        return;
    }

    printf("%s (%d) < %lf\n", split_fea_name_.c_str(), split_fid_, split_fval_);
    left_->DumpNode(depth + 1);
    right_->DumpNode(depth + 1);
}

