#include "boosting_predictor.h"
#include <tinyxml2.h>
#include "tree_function.h"
#include <common_utils.h>

using namespace std;
using namespace lncc;
using namespace tinyxml2;

lncc::BoostingPredictor::~BoostingPredictor() {
    for (int i = 0; i < base_funcs_.size(); i ++) {
        delete base_funcs_[i];
    }
}

int lncc::BoostingPredictor::LoadModel() {
    string model_file = ln_ctx_->model_folder + '/';
    model_file += "model.xml";
    log_info ("Load model from %s\n", model_file.c_str());
    
    XMLDocument doc;
    if (doc.LoadFile(model_file.c_str()) != XML_SUCCESS) {
        log_error ("Failed load model: %s\n", model_file.c_str());
        return -1;
    }

    XMLElement* root_elem = doc.FirstChildElement();
    if (root_elem == NULL || strcmp(root_elem->Name(), "LnccModel") != 0) {
        log_error ("Invalid model format\n");
        return -1;
    }
    XMLElement* func_elem = root_elem->FirstChildElement();
    if (func_elem == NULL || strcmp(func_elem->Name(), "BoostingFunction") != 0) {
        log_error ("Invalid function in model\n");
        return -1;
    }
    
    XMLElement* tree_elem = func_elem->FirstChildElement();
    int ret = 0;
    while (tree_elem) {
        if (strcmp(tree_elem->Name(), "Tree") != 0) {
            log_error ("Invalid tree element\n");
            ret = -1;
            break;
        }
        BaseFunction* tree_func = new TreeFunction(tree_elem);
        base_funcs_.push_back(tree_func);
        const char* tree_id = tree_elem->Attribute("id");
        if (tree_id != NULL) {
            log_debug ("Loaded tree %s\n", tree_id);
        }
        //tree_func->DumpModel();

        tree_elem = tree_elem->NextSiblingElement();
    }

    return 0;
}


int lncc::BoostingPredictor::Predict() {
    data_set_.LoadTestingData(ln_ctx_);
    string score_filename = ln_ctx_->model_folder + "/";
    score_filename += "model.score";
    FILE* fp = fopen(score_filename.c_str(), "w");
    for (int i = 0; i < data_set_.GetRowNum(); i ++) {
        double score = 0.0;
        for (int t = 0; t < base_funcs_.size(); t ++) {
            score += base_funcs_[t]->Predict(data_set_, i);
        }
        fprintf (fp, "%lf\n", score);
    }
    fclose(fp);

    return 0;
}
