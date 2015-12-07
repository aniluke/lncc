#ifndef _TREE_FUNCTION_H_
#define _TREE_FUNCTION_H_

#include "base_function.h"
#include "train_node.h"
#include <ln_context.h>
#include <list>

namespace lncc {
    class TreeFunction: public BaseFunction {
    public:
        TreeFunction(const LnContext* ln_ctx);
        TreeFunction(const tinyxml2::XMLElement* tree_elem);

        virtual ~TreeFunction() { if (root_) delete root_; }

        virtual int Fit(const std::vector<int>& sample_index, const DataSet& data_set, const std::vector<double>& ngrads, double learning_rate);

        virtual double Predict(const DataSet& data_set, int row_id);

        virtual void SaveModel(tinyxml2::XMLDocument* doc, tinyxml2::XMLElement* elem);

        virtual void CollectFeatureStats(std::vector<double>* out_feature_stats, double learning_rate);

        virtual void DumpModel();


    private:
        std::list<TrainNode*>::iterator FindBestNode(std::list<TrainNode*>& queue);
    private:
        MsgQueue<BaseTask*>* task_queue_;
        int max_node_;
        PredNode* root_;
        double reg_L1_;
        double reg_L2_;
        double lambda_;
    };

}
#endif
