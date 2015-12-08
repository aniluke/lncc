#ifndef _TRAIN_NODE_H_
#define _TRAIN_NODE_H_

#include <common.h>
#include "data_set.h"
#include "pred_node.h"
#include <msg_queue.h>
#include <base_task.h>

namespace lncc {    
    class TrainNode : public PredNode {
    public:
        TrainNode(const LnContext* ln_ctx,
                  int* sample_index,
                  int sample_num,
                  const DataSet* data_set,
                  const double* ngrads,
                  double reg_L1,
                  double reg_L2);

        virtual ~TrainNode() {}

    public:
        // split and create child nodes
        bool Split();

        double GetMaxSplitGain() const { return max_split_gain_; }
        double FindSplitByCol(const FeatureCol* fcol, double* out_split_fval);

    private:
        void ComputeResponse();

        bool FindSplit();

    private:
        const LnContext* ln_ctx_;
        int* sample_index_;
        int sample_num_;
        const DataSet* data_set_;
        // the total ngrads, not only samples
        const double* ngrads_;
        double sum_ngrads_;
        double sum_weights_;

        double max_split_gain_;
        double reg_L1_;
        double reg_L2_;
    };
}
#endif
