#include "gradient_boosting_learner.h"
#include "data_set.h"
#include "loss_function_factory.h"
#include "tree_function.h"
#include <iostream>
#include <stdio.h>
#include <tinyxml2.h>
#include <common_utils.h>
#include <algorithm>
#include <math.h>

using namespace lncc;
using namespace std;
using namespace tinyxml2;

lncc::GradientBoostingLearner::~GradientBoostingLearner() {
    for (int i = 0; i < base_funcs_.size(); i++) {
        delete base_funcs_[i];
    }
}

int GradientBoostingLearner::Train() {
    // load data
    data_set_.LoadTrainingData(ln_ctx_);
    int row_num = data_set_.GetRowNum();
    log_info("Finish loading training data, row num=%d, feature num=%d\n", row_num, data_set_.fmat_.size());

    vector<double> predictions(row_num, 0.0);
    LossFunction* loss_func = LossFunctionFactory::CreateLossFunction(ln_ctx_->loss_func);

    base_funcs_.clear();
    double t1 = 0, t2 = 0;
    for (int i = 0; i < ln_ctx_->round_num; i ++) {
        // gradient
        t1 = lncc::get_time_precise();
        vector<double> ngrads(row_num, 0.0);
        double loss_stat = loss_func->ComputeNegativeGradient(&ngrads, data_set_.targets_, predictions);
        t2 = lncc::get_time_precise();
        //log_info("aniluke: compute gradient used %lf sec\n", t2 - t1);
        
        // sampling data
        t1 = lncc::get_time_precise();
        vector<int> sample_index;
        data_set_.SamplingIndex(&sample_index, ln_ctx_->sampling_rate);
        t2 = lncc::get_time_precise();
        //log_info("aniluke: sampling used %lf sec, size %d\n", t2 - t1, sample_index.size());
        
        // grow a tree
        t1 = lncc::get_time_precise();
        BaseFunction* tree = new TreeFunction(ln_ctx_);
        tree->Fit(sample_index, data_set_, ngrads, ln_ctx_->learning_rate);
        t2 = lncc::get_time_precise();
        //log_info("aniluke: grow tree used %lf sec\n", t2 - t1);

        // update predictions
        t1 = lncc::get_time_precise();
        for(int s = 0; s < row_num; s ++) {
            predictions[s] += tree->Predict(data_set_, s);
            //printf("pred %d, %lf\n", s, predictions[s]);
        }
        t2 = lncc::get_time_precise();
        //log_info("aniluke: update prediction used %lf sec\n", t2 - t1);
        
        base_funcs_.push_back(tree);
        
        log_info("created tree: %d\tmse: %lf\n", i, loss_stat);
    }

    delete loss_func;

#if 0
    Formatter f(ln_ctx.model_file);
    f.BeginWrite();
    for(int i = 0; i < base_funcs.size(); i ++) {
        base_funcs[i]->WriteModel(f);
    }
    f.EndWrite();
#endif

    return 0;
}

int lncc::GradientBoostingLearner::SaveModel() {
    string output_file = ln_ctx_->model_folder + '/';
    output_file += "model.xml";
    log_info("Save model to %s\n", output_file.c_str());

    XMLDocument doc;
    XMLElement* root_elem = doc.NewElement("LnccModel");
    doc.InsertFirstChild(root_elem);
    XMLElement* func_elem = doc.NewElement("BoostingFunction");
    root_elem->InsertFirstChild(func_elem);
    for(int i = 0; i < base_funcs_.size(); i ++) {
        log_debug("tree %d\n", i);
        XMLElement* tree_elem = doc.NewElement("Tree");
        tree_elem->SetAttribute("id", lncc::to_string(i).c_str());
        func_elem->InsertEndChild(tree_elem);

        base_funcs_[i]->SaveModel(&doc, tree_elem);
    }

    if (doc.SaveFile(output_file.c_str()) != 0) {
        fprintf(stderr, "save model file error\n");
        return -1;
    }

    return 0;
}

static bool FeatureImpComp(const pair<int, double>& left, const pair<int, double>& right) {
    return left.second > right.second;
}

int lncc::GradientBoostingLearner::PrintFeatureStats() {
    // save feature stats
    string fea_imp_path = ln_ctx_->model_folder + '/';
    fea_imp_path += "feature.rank";
    log_info("Dump feature importance to %s\n", fea_imp_path.c_str());

    vector<double> feature_stats(data_set_.fmat_.size(), 0.0);
    for (int i = 0; i < base_funcs_.size(); i ++) {
        base_funcs_[i]->CollectFeatureStats(&feature_stats, ln_ctx_->learning_rate);
    }

    vector<pair<int,double> > feature_id_stats;
    for (int i = 0; i < feature_stats.size(); i ++) {
        feature_id_stats.push_back(make_pair(i, feature_stats[i]));
    }
    //cout << "fmat size: " << data_set_.fmat_.size() << endl;
    //cout << "feature_stats: " << feature_stats.size() << endl;
    //cout << "feature id stats: " << feature_id_stats.size() << endl;

    sort(feature_id_stats.begin(), feature_id_stats.end(), FeatureImpComp);
    double max_imp = feature_id_stats[0].second;
    // dump
    FILE* fp = fopen(fea_imp_path.c_str(), "w");
    fprintf(fp, "RANK\tFEATURE\tIMP\n");
    for (int i = 0; i < feature_id_stats.size(); i ++) {
        double imp = sqrt(feature_id_stats[i].second) / sqrt(max_imp) * 100.0;
        fprintf(fp, "%d\t%s\t%lf\n", i, data_set_.fmat_[feature_id_stats[i].first].fea_name_.c_str(), imp);
    }
    fclose(fp);
}
