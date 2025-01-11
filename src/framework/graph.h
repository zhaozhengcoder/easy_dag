#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <type_traits>
#include "node.h"

namespace easy_dag {
class Graph {
    using GraphNodePtrVec = std::vector<std::shared_ptr<Node>>;

    public:
        Graph():graph_name_("undefine graph") {}
        ~Graph() = default;
        Graph(std::string& name):graph_name_(name){}

        template <typename T, typename = typename std::enable_if<std::is_base_of<Node, T>::value 
                                                || std::is_same<Node, T>::value>::type>
        Node* create_node(std::string name) {
            auto nodeptr = std::make_shared<T>(name);
            if (nodeptr->init() != ErrorCode::SUCCESS) {
                LOG(ERROR) << "node init failed, name : " << name;
                return nullptr;
            }
            nodes_.push_back(nodeptr);
            return nodes_.back().get();
        }

        bool add_node(std::shared_ptr<Node>& node) {
            nodes_.push_back(node);
            return true;
        }

        const GraphNodePtrVec& nodes() { return nodes_; }
        const std::string& name() { return graph_name_; }
        void set_name(std::string& name) {graph_name_ = name;}
    private:
        GraphNodePtrVec nodes_;
        std::string graph_name_;
};

}