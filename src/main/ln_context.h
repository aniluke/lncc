#ifndef _LN_CONTEXT_H_
#define _LN_CONTEXT_H_

#include <common.h>
#include <msg_queue.h>
#include <base_task.h>

namespace lncc {
    struct LnContext {
        MsgQueue<BaseTask*>* task_queue;
        
        std::string train_fv;
        std::string exclude_features;

        std::string model_folder;

        int round_num;
        int node_num;
        double learning_rate;
        double sampling_rate;
        int seed;
        std::string weights;

        std::string loss_func;

        double reg_L1;
        double reg_L2;
        double lambda;
        // testing
        std::string test_fv;
    };
}

#endif
