#ifndef _PRED_NODE_H_
#define _PRED_NODE_H_

#include <common.h>

namespace lncc {
    class PredNode {
    public:
        // create interm node
        PredNode(PredNode* left, PredNode* right, const std::string& split_fea_name, int split_fid, double split_fval);
        // create leaf node
        PredNode(double response);
        virtual ~PredNode();

    public:
        virtual double GetResponse() const { return response_; }

        virtual bool IsLeaf() const { return split_fid_ == -1; }

        virtual void SetLeaf();

        virtual PredNode* GetLeft() const { return left_; }
        virtual PredNode* GetRight() const { return right_; }
        virtual int GetSplitFid() const { return split_fid_; }
        virtual double GetSplitFval() const { return split_fval_; }
        virtual std::string GetSplitFeaName() const { return split_fea_name_; }
        virtual void DumpNode(int depth);
        
    protected:
        PredNode* left_;
        PredNode* right_;

        double response_;
        int split_fid_;
        double split_fval_;
        std::string split_fea_name_;

    };
}

#endif
