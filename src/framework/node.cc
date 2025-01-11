#include "node.h"

namespace easy_dag {

Node::Node() = default;

Node::Node(const std::string& name) : name_(name) {}

Node::~Node() {}

void Node::update_upstream_counter() {
    for (size_t i = 0; i < downstream_.size(); i++) {
        downstream_[i]->upstream_counter_.fetch_sub(1);
    }
}

std::vector<Node*> Node::get_callnext() {
    std::vector<Node*> ret;
    for (size_t i = 0; i < downstream_.size(); i++) {
        if (downstream_[i]->upstream_counter_.load() == 0) {
            ret.push_back(downstream_[i]);
        }
    }
    return ret;
}

void Node::set_name(std::string& name) {
    name_ = name;
}

std::string Node::name() {
    return name_;
}

bool Node::add_downstream(Node* node) {
    if (node == this) {
        return false;
    }
    downstream_.push_back(node);
    node->upstream_.push_back(this);
    node->upstream_counter_.fetch_add(1);
    return true;
}

const std::vector<Node*>& Node::downstream() {
    return downstream_;
}

const std::vector<Node*>& Node::upstream() {
    return upstream_;
}

void Node::set_sink_node() {
    is_sink_node_ = true;
}

bool Node::is_sink_node() {
    return is_sink_node_;
}

bool Node::run_flag_test_and_change() {
    bool expected = false;
    if (!run_flag_.compare_exchange_weak(expected
            , true, std::memory_order_seq_cst)) {
        return false;
    }
    return true;
}

}