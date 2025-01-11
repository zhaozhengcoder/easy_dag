#include "workflow.h"

namespace easy_dag {

void DagTask::run(Callback callback) {
    if (!node_->run_flag_test_and_change()) {
        LOG(ERROR) << "graph[" << topology_->name() << "], node : " 
                    << node_->name() << " has run";
        return;
    }

    // 1. check timeout
    if (topology_->check_timeout()) {
        topology_->set_done();
        topology_->set_result_code(ErrorCode::TIMEOUT);
        LOG(ERROR) << "graph[" << topology_->name() << "], node : " 
                    << node_->name() << " timeout";
        return;
    }

    // 2. check run done
    if (topology_->check_run_done()) {
        return;
    }

    // 3. exec node
    node_->run(topology_->context_args());

    // 4. update 
    node_->update_upstream_counter();
    if (node_->is_sink_node()) {
        topology_->inc_sink_counter();
    }

    // 5. get and call next
    auto next_node = node_->get_callnext();
    for (size_t i = 0; i < next_node.size(); i++) {
        auto sub_task = std::make_shared<DagTask>(next_node[i], topology_);
        BackgroundTaskManager::getInstance().startTask(sub_task);
    }

    // 6. exec last sink node
    if (node_->is_sink_node() && topology_->check_all_sinks_visited()) {
        topology_->set_done();
        LOG(INFO) << "graph: [" << topology_->name() << "] run done";
    }
}
}