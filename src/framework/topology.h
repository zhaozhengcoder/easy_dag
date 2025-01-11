#pragma once
#include <memory>
#include <string>
#include "graph.h"
#include "src/utils/inc.h"

namespace easy_dag {
class Topology {
public:
    Topology() = default;
    ~Topology() = default;

    bool create(std::shared_ptr<Graph>& graph);
    bool reset();

    /*
    * 检测环：
    * 找到graph入度=0的节点，然后把入度=0的节点从图中摘下，修改相邻节点的入度；
    * 重复上述过程，如果最后还有剩余节点节点那么就是有环；
    */
    bool check_no_circle(std::shared_ptr<Graph>& graph);

    void show_source_nodes();
    void show_sink_nodes();
    void inc_sink_counter();
    bool check_all_sinks_visited();
    void set_begin_time();
    void set_timeout_ms(std::uint32_t timeout_ms);
    bool check_timeout();
    void* context_args();
    void set_context_args(void* args);
    void set_done();

    const std::vector<Node*>& source_nodes() const { return source_nodes_; }
    const std::vector<Node*>& sink_nodes() const { return sink_nodes_; }
    const std::string& name() const { return graph_->name(); }
    void set_run_done() {run_done_ = true;}
    bool check_run_done() {return run_done_;}

    void set_result_code(int code) {result_code_ = code;}
    int result_code() {return result_code_;}

    private:
        std::shared_ptr<Graph> graph_; 
        void* context_args_{nullptr};

        std::uint32_t timeout_ms_{0};
        std::chrono::high_resolution_clock::time_point begin_time_;
        
        std::vector<Node*> source_nodes_;
        std::vector<Node*> sink_nodes_; 
        std::atomic<uint32_t> total_sink_counter_{0};  // 总的出口节点数
        std::atomic<uint32_t> sinks_counter_{0};       // 当出口的节点被执行后计数加1
        
        bool run_done_{false};
        int result_code_{ErrorCode::UNDEFINED};        // topology运行的返回值 refer errcode.h
};

}