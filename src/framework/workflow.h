#pragma once

#include <vector>
#include "node.h"
#include "topology.h"
#include "src/utils/inc.h"

namespace easy_dag {

class DagTask : public BackgroundTask {
public:
    DagTask(Node* node, Topology* topology){
        node_ = node;
        topology_ = topology;
    }

    void run(Callback callback = nullptr) override;

private:
    Node* node_;
    Topology* topology_;
};

class Workflow {
public:
    Workflow(Topology* topology) {topology_ = topology;}
    ~Workflow() {}

    bool reset() {
        return topology_->reset();
    }

    void bind(Topology* topology) {topology_ = topology;}

    template <typename T>
    void schedule(T* args) {
        LOG(INFO) << "run graph: [" << topology_->name() << "]";
        topology_->set_begin_time();
        topology_->set_context_args(args);

        BackgroundTaskManager& manager = BackgroundTaskManager::getInstance();
        auto& entry_nodes = topology_->source_nodes();
        for (size_t i = 0; i < entry_nodes.size(); i++) {
            auto task = std::make_shared<DagTask>(entry_nodes[i], topology_);
            manager.startTask(task);
        }
    }

    int get_result_status() {
        return topology_->result_code();
    }

public:
    Topology* topology_;
};
}