#pragma once
#include <vector>
#include <string>
#include <atomic>
#include "src/utils/inc.h"

namespace easy_dag {

class Node {
public:
    Node();
    Node(const std::string& name);
    virtual ~Node();

    virtual int init() = 0;
    virtual int run(void *args = nullptr) = 0;

    void update_upstream_counter();
    std::vector<Node*> get_callnext();
    void set_name(std::string& name);
    std::string name();
    bool add_downstream(Node* node);
    const std::vector<Node*>& downstream();
    const std::vector<Node*>& upstream();
    void set_sink_node();
    bool is_sink_node();
    bool run_flag_test_and_change();

private:
    std::string name_;
    std::vector<Node*> downstream_;
    std::vector<Node*> upstream_;
    bool is_sink_node_{false};

    // 动态数据
    // 待执行的上游节点数, 当依赖的上游被执行后计数减1
    std::atomic<uint32_t> upstream_counter_{0};
    std::atomic<bool> run_flag_{false};
};

}