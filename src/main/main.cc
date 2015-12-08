#include <iostream>
#include <fstream>
#include "common.h"
#include "gradient_boosting_learner.h"
#include <string.h>
#include <stdlib.h>
#include "ln_config.h"
#include "ln_context.h"
#include "boosting_predictor.h"
#include <common_utils.h>
#include <getopt.h>
#include <worker.h>

using namespace std;
using namespace lncc;

Learner* CreateLearner(const LnConfig& config, int thread, LnContext* ln_ctx) {

    ln_ctx->train_fv = config.GetString("train_fv", "");
    ln_ctx->model_folder = config.GetString("model_folder", "");
    ln_ctx->round_num = config.GetInt("round_num", 1);
    ln_ctx->node_num = config.GetInt("node_num", 2);
    ln_ctx->learning_rate = config.GetDouble("learning_rate", 1.0);
    ln_ctx->sampling_rate = config.GetDouble("sampling_rate", 1.0);
    ln_ctx->seed = config.GetInt("seed", 32598);
    srand(ln_ctx->seed);
    
    ln_ctx->exclude_features = config.GetString("exclude_features", "");
    ln_ctx->weights = config.GetString("weights", "");
    ln_ctx->loss_func = config.GetString("loss_func", "least_square");
    ln_ctx->reg_L1 = config.GetDouble("reg_L1", 0.0);
    ln_ctx->reg_L2 = config.GetDouble("reg_L2", 0.0);
    ln_ctx->lambda = config.GetDouble("lambda", 1.0);
    ln_ctx->thread = thread;

    log_info("Learning context:\n");
    log_info("train_fv = %s\n", ln_ctx->train_fv.c_str());
    log_info("model_folder = %s\n", ln_ctx->model_folder.c_str());
    log_info("round_num = %d\n", ln_ctx->round_num);
    log_info("node_num = %d\n", ln_ctx->node_num);
    log_info("learning_rate = %lf\n", ln_ctx->learning_rate);
    log_info("sampling_rate = %lf\n", ln_ctx->sampling_rate);
    log_info("seed = %d\n", ln_ctx->seed);
    log_info("weights = %s\n", ln_ctx->weights.c_str());
    log_info("loss_func = %s\n", ln_ctx->loss_func.c_str());
    log_info("reg_L1 = %lf\n", ln_ctx->reg_L1);
    log_info("reg_L2 = %lf\n", ln_ctx->reg_L2);
    log_info("lambda = %lf\n", ln_ctx->lambda);
    
    return new GradientBoostingLearner(ln_ctx);
}

Predictor* CreatePredictor(const LnConfig& config, LnContext* ln_ctx) {
    ln_ctx->test_fv = config.GetString("test_fv", "");
    ln_ctx->model_folder = config.GetString("model_folder", "");
    ln_ctx->exclude_features = config.GetString("exclude_features", "");
    ln_ctx->test_steps = config.GetInt("test_steps", 0);
    return new BoostingPredictor(ln_ctx);
}

static MsgQueue<BaseTask*> g_global_queue(0); // TODO: set max num?
static vector<Worker*> g_workers;

void CreateWorkers(int thread_num) {
    if (thread_num <= 0) {
        thread_num = 1;
    }
    log_info ("Worker num: %d\n", thread_num);
    
    for (int i = 0; i < thread_num; i ++) {
        Worker* worker = new Worker(&g_global_queue);
        worker->Start();
        g_workers.push_back(worker);
    }    
}

int main(int argc, char** argv) {
    lncc::init_log_level(lncc::E_LOG_INFO);
    
    std::string task;
    std::string cfg_filename;
    int thread_num = 0;
    
    extern char* optarg;
    extern int optind;

    int carg = 0;
    int long_index = 0;
    static struct option long_options[] = {
        {"task", required_argument, 0, 1001},
        {"thread", required_argument, 0, 1002}
    };
    
    while ((carg = getopt_long(argc, argv, "", long_options, &long_index)) != -1) {
        switch(carg) {
        case 1001:
            task = optarg;
            log_info("Task=%s\n", task.c_str());
            break;
        case 1002:
            thread_num = atoi(optarg);
            break;
        case '?':
            log_error("wrong argument\n");
            break;
        }
    }
    if (optind == argc - 1) {
        cfg_filename = argv[argc - 1];
    }

    if (cfg_filename.size() == 0) {
        log_error("missing config file\n");
        return -1;
    }
    bool do_training = false;
    bool do_testing = false;
    if (task.empty()) {
        do_training = true;
        do_testing = true;
    } else if (task == "train") {
        do_training = true;
    } else if (task == "test") {
        do_testing = true;
    }
    
    LnConfig ln_config;
    int ret = ln_config.LoadConfig(cfg_filename);
    if (ret != 0) {
        log_error("parse config failed\n");
        return -1;
    }

    // create workers
    CreateWorkers(thread_num);
    
    LnContext ln_ctx;
    ln_ctx.task_queue = &g_global_queue;
    
    if (do_training) {
        log_info("Start training\n");
        double t1 = lncc::get_time_precise();
        Learner* learner = CreateLearner(ln_config, thread_num, &ln_ctx);
        learner->Train();
        string mkdir_cmd = "mkdir ";
        mkdir_cmd += ln_ctx.model_folder;
        system(mkdir_cmd.c_str());
        
        learner->SaveModel();
        learner->PrintFeatureStats();
        double t2 = lncc::get_time_precise();
        log_info("Training done, used %lf sec\n", t2 - t1);
    }

    if (do_testing) {
        log_info("Start Testing\n");
        double t1 = lncc::get_time_precise();
        Predictor* predictor = CreatePredictor(ln_config, &ln_ctx);
        predictor->LoadModel();
        predictor->Predict();
        double t2 = lncc::get_time_precise();
        log_info("Testing Done, used %lf sec\n", t2 - t1);
    }

    return 0;
}
