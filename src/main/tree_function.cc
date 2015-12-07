#include "tree_function.h"
#include <list>
#include <string.h>
#include <common_utils.h>

using namespace std;
using namespace lncc;
using namespace tinyxml2;

lncc::TreeFunction::TreeFunction(const LnContext* ln_ctx):
    task_queue_(ln_ctx->task_queue),
    max_node_(ln_ctx->node_num),
    root_(NULL),
    reg_L1_(ln_ctx->reg_L1),
    reg_L2_(ln_ctx->reg_L2),
    lambda_(ln_ctx->lambda){
}

static double ComputeTotalWeight(const vector<int>& sample_index, const DataSet& data_set) {
    double total_weight = 0;
    for (int i = 0; i < sample_index.size(); i ++) {
        total_weight += data_set.weights_[sample_index[i]];
    }
    return total_weight;
}

int lncc::TreeFunction::Fit(const vector<int>& sample_index, const DataSet& data_set, const vector<double>& ngrads, double learning_rate) {
    list<TrainNode*> queue;
    int* sample_index_buf = new int[sample_index.size()];
    memcpy ((char*)sample_index_buf, (char*)sample_index.data(), sizeof(int)*sample_index.size());

    double total_weight = ComputeTotalWeight(sample_index, data_set);
    double weighted_reg_L1 = reg_L1_ * total_weight;
    double weighted_reg_L2 = reg_L2_ * total_weight;
    root_ = new TrainNode(task_queue_, sample_index_buf, sample_index.size(), &data_set, ngrads.data(), learning_rate, weighted_reg_L1, weighted_reg_L2, lambda_);
    queue.push_back((TrainNode*)root_);

    while (queue.size() < max_node_) {
        // find best node that having best splitting gain
        list<TrainNode*>::iterator best_node_it = FindBestNode(queue);
        if (best_node_it == queue.end()) {
            break;
        }
        if ((*best_node_it)->Split() == false) {
            // means all nodes are leaf node
            break;
        }
        TrainNode* left = (TrainNode*)(*best_node_it)->GetLeft();
        TrainNode* right = (TrainNode*)(*best_node_it)->GetRight();
        
        queue.erase(best_node_it);
        queue.push_back(left);
        queue.push_back(right);
    }

    for (list<TrainNode*>::iterator it = queue.begin(); it != queue.end(); it++ ) {
        (*it)->SetLeaf();
    }

    delete[] sample_index_buf;

    return 0;

}

double lncc::TreeFunction::Predict(const DataSet& data_set, int row_id) {
    PredNode* node = root_;
    while (!node->IsLeaf()) {
        int fid = node->GetSplitFid();
        const FeatureCol* fcol = &data_set.fmat_[fid];
        double split_fval = node->GetSplitFval();
        double actual_fval = fcol->fv_[row_id];
        if (actual_fval < split_fval) {
            node = node->GetLeft();
        } else {
            node = node->GetRight();
        }
    }
    return node->GetResponse();
}

std::list<lncc::TrainNode*>::iterator lncc::TreeFunction::FindBestNode(list<TrainNode*>& queue) {
    double max_gain = 0.0;
    list<TrainNode*>::iterator best_node_it = queue.end();
    //log_info("aniluke: start to find best node\n");
    for(list<TrainNode*>::iterator cit = queue.begin(); cit != queue.end(); cit ++) {
        //log_info("aniluke: check node with feature=%s, gain=%lf\n", (*cit)->GetSplitFeaName().c_str(), (*cit)->GetMaxSplitGain());

        if ( (*cit)->GetMaxSplitGain() > max_gain) {
            best_node_it = cit;
            max_gain = (*cit)->GetMaxSplitGain();
        }
    }
    if (best_node_it != queue.end()) {
        //log_info("aniluke: find best node with feature=%s, gain=%lf\n", (*best_node_it)->GetSplitFeaName().c_str(), (*best_node_it)->GetMaxSplitGain());
    }
    return best_node_it;
}

static void SaveNode(lncc::TrainNode* node, XMLDocument* doc, XMLElement* root_elem) {
    XMLElement* elem = NULL;
    if (node->IsLeaf()) {
        elem = doc->NewElement("Response");
        elem->SetAttribute("value", lncc::to_string(node->GetResponse()).c_str());
        elem->SetAttribute("gain", lncc::to_string(node->GetMaxSplitGain()).c_str());
    } else {
        elem = doc->NewElement("Node");
        int fid = node->GetSplitFid();
        elem->SetAttribute("feature", node->GetSplitFeaName().c_str());
        elem->SetAttribute("fid", lncc::to_string(fid).c_str());
        elem->SetAttribute("gain", lncc::to_string(node->GetMaxSplitGain()).c_str());
        elem->SetAttribute("value", lncc::to_string(node->GetSplitFval()).c_str());
        elem->SetAttribute("response", lncc::to_string(node->GetResponse()).c_str());
        
        SaveNode((TrainNode*)node->GetLeft(), doc, elem);
        SaveNode((TrainNode*)node->GetRight(), doc, elem);
    }
    root_elem->InsertEndChild(elem);
}

void lncc::TreeFunction::SaveModel(XMLDocument* doc, XMLElement* root_elem) {
    SaveNode((TrainNode*)root_,  doc, root_elem);
}

void lncc::TreeFunction::DumpModel() {
    root_->DumpNode(0);
}

static lncc::PredNode* BuildNode(const XMLElement* node_elem) {
    if (node_elem == NULL) {
        return NULL;
    }
    PredNode* new_node = NULL;
    if (strcmp(node_elem->Name(), "Response") == 0) {
        double response = atof(node_elem->Attribute("value"));
        new_node = new PredNode(response);
    } else if (strcmp(node_elem->Name(), "Node") == 0 &&
               node_elem->FirstChildElement() &&
               node_elem->FirstChildElement()->NextSiblingElement()) {
        string split_fea_name = node_elem->Attribute("feature");
        int split_fid = atoi(node_elem->Attribute("fid"));
        double split_fval = atof(node_elem->Attribute("value"));
        const XMLElement* left_elem = node_elem->FirstChildElement();
        PredNode* left = BuildNode(left_elem);
        PredNode* right = BuildNode(left_elem->NextSiblingElement());
        new_node = new PredNode(left, right, split_fea_name, split_fid, split_fval);

    }
    return new_node;
}

lncc::TreeFunction::TreeFunction(const XMLElement* tree_elem) {
    root_ = BuildNode(tree_elem->FirstChildElement());    
}

static void CollectFeatureStatsInternal(TrainNode* node, std::vector<double>* out_feature_stats, double learning_rate) {
    if (node->IsLeaf()) 
        return;

    double gain = node->GetMaxSplitGain();
    int fid = node->GetSplitFid();
    (*out_feature_stats)[fid] += gain * learning_rate;
    CollectFeatureStatsInternal( (TrainNode*)node->GetLeft(), out_feature_stats, learning_rate);
    CollectFeatureStatsInternal( (TrainNode*)node->GetRight(), out_feature_stats, learning_rate);
}

void lncc::TreeFunction::CollectFeatureStats(std::vector<double>* out_feature_stats, double learning_rate) {
    CollectFeatureStatsInternal((TrainNode*)root_, out_feature_stats, learning_rate);
}
