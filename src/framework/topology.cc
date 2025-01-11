#include <queue>
#include <vector>
#include <set>
#include "src/framework/topology.h"

namespace easy_dag {
bool Topology::create(std::shared_ptr<Graph>& graph) {
    if (!check_no_circle(graph)) {
        LOG(ERROR) << "graph has circle, name: " << graph->name();
        return false;
    }
    graph_ = graph;
    auto& nodes = graph_->nodes();
    for (size_t i = 0; i < nodes.size();i++) {
        if (nodes[i]->upstream().empty()) {
            source_nodes_.push_back(nodes[i].get());
        }
        if (nodes[i]->downstream().empty()) {
            nodes[i]->set_sink_node();
            sink_nodes_.push_back(nodes[i].get());  // todo: smart ptr to pointer 
        }
    }
    total_sink_counter_.store(sink_nodes_.size());
    return true;
}


bool Topology::check_no_circle(std::shared_ptr<Graph>& graph) {
    auto& nodes = graph->nodes();
    std::vector<int32_t> indegree(nodes.size(), 0);
    std::queue<int32_t> q;
    for (size_t i = 0; i < nodes.size(); i++) {
        indegree[i] = nodes[i]->upstream().size();
        if (indegree[i] == 0) {
            q.push(i);
        }
    }

    std::set<int32_t> processed;
    while(!q.empty()) {
        int32_t front = q.front();
        q.pop();

        processed.insert(front);
        for (size_t i = 0; i < nodes.size(); i++) {
            if (processed.find(i) != processed.end()) {
                continue;
            }

            auto& upstream = nodes[i]->upstream();
            for (size_t j = 0; j < upstream.size(); j++) {
                if (nodes[front].get() == upstream[j]) {
                    indegree[i]--;
                    if (indegree[i] == 0) {
                        q.push(i);
                    }
                }
            }
        }
    }

    if (processed.size() == nodes.size()) {
        return true; // no circle
    }
    return false;
}

void Topology::show_source_nodes() {
    for (size_t i = 0; i < source_nodes_.size(); i++) {
        LOG(INFO) << "entry node: " << source_nodes_[i]->name();
    }
}

void Topology::show_sink_nodes() {
    for (size_t i = 0; i < sink_nodes_.size(); i++) {
        LOG(INFO) << "exit node: " << sink_nodes_[i]->name();
    }
}

void Topology::inc_sink_counter() {
    sinks_counter_.fetch_add(1);
}

bool Topology::check_all_sinks_visited() {
    return sinks_counter_.load() == total_sink_counter_.load();
}

void Topology::set_begin_time() {
    begin_time_ = std::chrono::high_resolution_clock::now();
}

void Topology::set_timeout_ms(std::uint32_t timeout_ms) {
    timeout_ms_ = timeout_ms;
}

bool Topology::check_timeout() {
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - begin_time_);
    return duration.count() > timeout_ms_;
}

void* Topology::context_args() {return context_args_;}
void Topology::set_context_args(void* args) {context_args_ = args;}

void Topology::set_done() {
    set_run_done();
    set_result_code(ErrorCode::SUCCESS);
}

bool Topology::reset() {
    timeout_ms_ = 0;
    begin_time_ = std::chrono::high_resolution_clock::time_point();
    graph_.reset();
    source_nodes_.clear();
    sink_nodes_.clear();
    total_sink_counter_.store(0);
    sinks_counter_.store(0);
    run_done_ = false;
    context_args_ = nullptr;
    result_code_ = ErrorCode::UNDEFINED;
    return true;
}

}