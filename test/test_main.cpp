#include <iostream>
#include <vector>
#include <string>
#include <map>

#include "src/third_party/json.hpp"
#include "src/utils/inc.h"
#include "src/idl/json_parser.h"
#include "src/framework/node.h"
#include "src/framework/graph.h"
#include "src/framework/topology.h"
#include "src/framework/workflow.h"
#include "src/framework/image_helper.h"
#include "src/framework/module_factory.h"

using namespace easy_dag;

class UserContext {
public:
    void set(const std::string& key, int value) {
        data_[key] = value;
    }

    int get(const std::string& key) {
        if (data_.find(key) == data_.end()) {
            return -1;
        }
        return data_[key];
    }
private:
    std::map<std::string, int> data_;
};

class ExampleTask : public BackgroundTask {
public:
    void run(Callback callback) override {
        LOG(INFO) << "ExampleTask is running.";
        callback(nullptr); 
    }
};

class ModuleA : public Node {
public:
    ModuleA() = default;
    ModuleA(const std::string& name) : Node(name) {}

    int init() override {
        LOG(INFO) << "ModuleA is init.";
        val_ = 100;
        return ErrorCode::SUCCESS;
    }

    int run(void *args = nullptr) override {
        UserContext* context = static_cast<UserContext*>(args);
        context->set("a", 1);
        LOG(INFO) << "ModuleA is running.";
        std::this_thread::sleep_for(std::chrono::milliseconds(3));
        return ErrorCode::SUCCESS;
    }

    int get_val() { return val_; }
private:
    int val_;
};

REGISTER_MODULE(ModuleA);

class ModuleB : public Node {
public:
    ModuleB() = default;
    ModuleB(const std::string& name) : Node(name) {}

    int init() override {
        return ErrorCode::SUCCESS;
    }
    int run(void *args = nullptr) override {
        UserContext* context = static_cast<UserContext*>(args);
        EXPECT_EQ(context->get("a"), 1);
        context->set("b", 2);
        LOG(INFO) << "ModuleB is running.";
        return ErrorCode::SUCCESS;
    }
};
REGISTER_MODULE(ModuleB);

class ModuleK : public Node {
public:
    ModuleK() = default;
    ModuleK(const std::string& name) : Node(name) {}

    int init() override {
        return ErrorCode::SUCCESS;
    }
    int run(void *args = nullptr) override {
        std::this_thread::sleep_for(std::chrono::milliseconds(3));
        UserContext* context = static_cast<UserContext*>(args);
        context->set("k", 2);
        LOG(INFO) << "ModuleK is running.";
        return ErrorCode::SUCCESS;
    }
};
REGISTER_MODULE(ModuleK);

class ModuleC : public Node {
public:
    ModuleC() = default;
    ModuleC(const std::string& name) : Node(name) {}
    int init() override {
        return ErrorCode::SUCCESS;
    }
    int run(void *args = nullptr) override {
        std::this_thread::sleep_for(std::chrono::milliseconds(3));
        LOG(INFO) << "ModuleC is running.";
        return ErrorCode::SUCCESS;
    }
};
REGISTER_MODULE(ModuleC);

class ModuleD : public Node {
public:
    ModuleD() = default;
    ModuleD(const std::string& name) : Node(name) {}
    int init() override {
        return ErrorCode::SUCCESS;
    }
    int run(void *args = nullptr) override {
        LOG(INFO) << "ModuleD is running.";
        return ErrorCode::SUCCESS;
    }
};
REGISTER_MODULE(ModuleD);

class ModuleE: public Node {
public:
    ModuleE() = default;
    ModuleE(const std::string& name) : Node(name) {}
    int init() override {
        return ErrorCode::SUCCESS;
    }
    int run(void *args = nullptr) override {
        LOG(INFO) << "ModuleE is running.";
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        LOG(INFO) << "ModuleE is running... 100ms";
        return ErrorCode::SUCCESS;
    }
};
REGISTER_MODULE(ModuleE);

class ModuleF: public Node {
public:
    ModuleF() = default;
    ModuleF(const std::string& name) : Node(name) {}
    int init() override {
        return ErrorCode::SUCCESS;
    }
    int run(void *args = nullptr) override {
        LOG(INFO) << "ModuleF is running.";
        return ErrorCode::SUCCESS;
    }
};
REGISTER_MODULE(ModuleF);

class ModuleG : public Node {
public:
    ModuleG() = default;
    ModuleG(const std::string& name) : Node(name) {}
    int init() override {
        return ErrorCode::SUCCESS;
    }
    int run(void* args = nullptr) override {
        LOG(INFO) << "ModuleG is running.";
        return ErrorCode::SUCCESS;
    }
};
REGISTER_MODULE(ModuleG);

class ModuleH : public Node {
public:
    ModuleH() = default;
    ModuleH(const std::string& name) : Node(name) {}
    int init() override {
        return ErrorCode::SUCCESS;
    }
    int run(void* args = nullptr) override {
        LOG(INFO) << "ModuleH is running.";
        return ErrorCode::SUCCESS;
    }
};
REGISTER_MODULE(ModuleH);

class ModuleI : public Node {
public:
    ModuleI() = default;
    ModuleI(const std::string& name) : Node(name) {}
    int init() override {
        return ErrorCode::SUCCESS;
    }
    int run(void* args = nullptr) override {
        LOG(INFO) << "ModuleI is running.";
        return ErrorCode::SUCCESS;
    }
};
REGISTER_MODULE(ModuleI);

int test_case_run_graph(){
    std::string graph_name = "test_case_run_graph";
    std::shared_ptr<Graph> graph = std::make_shared<Graph>(graph_name);
    Node* node_a = graph->create_node<ModuleA>("a");
    Node* node_b = graph->create_node<ModuleB>("b");
    Node* node_c = graph->create_node<ModuleC>("c");
    Node* node_d = graph->create_node<ModuleD>("d");

    node_a->add_downstream(node_b);
    node_d->add_downstream(node_b);
    node_a->add_downstream(node_c);
    node_b->add_downstream(node_c);
    ImageHelper::generate_image(graph->nodes(), graph->name());

    Topology topology;
    topology.set_timeout_ms(80);
    if (!topology.create(graph)) {
        LOG(ERROR) << "create graph failed";
        return -1;
    }

    EXPECT_EQ(topology.source_nodes().size(), 2);
    EXPECT_EQ(topology.sink_nodes().size(), 1);

    UserContext context;
    Workflow workflow(&topology);
    workflow.schedule(&context);

    ModuleA* module_a = dynamic_cast<ModuleA*>(node_a);
    EXPECT_EQ(module_a->get_val(), 100);
    
    // wait for background task done
    std::this_thread::sleep_for(std::chrono::seconds(3));
    EXPECT_EQ(workflow.get_result_status(), ErrorCode::SUCCESS);

    return 0;
}

int test_case_run_timeout() {
    std::string graph_name = "test_case_run_timeout";
    std::shared_ptr<Graph> graph = std::make_shared<Graph>(graph_name);
    Node* node_e = graph->create_node<ModuleE>("e");    // trigger timeout
    Node* node_f = graph->create_node<ModuleF>("f");   

    node_e->add_downstream(node_f);
    Topology topology;
    topology.set_timeout_ms(80);
    if (!topology.create(graph)) {
        LOG(ERROR) << "create graph failed";
        return -1;
    }

    UserContext context;
    Workflow workflow(&topology);
    workflow.schedule(&context);

    // wait for background task done
    std::this_thread::sleep_for(std::chrono::seconds(3));
    EXPECT_EQ(workflow.get_result_status(), ErrorCode::TIMEOUT);
    return 0;
}

int test_case_context() {
    std::string graph_name = "test_case_context";
    std::shared_ptr<Graph> graph = std::make_shared<Graph>(graph_name);
    Node* node_a = graph->create_node<ModuleA>("a");
    Node* node_b = graph->create_node<ModuleB>("b");

    node_a->add_downstream(node_b);

    Topology topology;
    topology.set_timeout_ms(80);
    if (!topology.create(graph)) {
        LOG(ERROR) << "create graph failed";
        return -1;
    }

    UserContext context;
    Workflow workflow(&topology);
    workflow.schedule(&context);

    // wait for background task done
    std::this_thread::sleep_for(std::chrono::seconds(3));
    EXPECT_EQ(workflow.get_result_status(), ErrorCode::SUCCESS);

    EXPECT_EQ(context.get("a"), 1);
    EXPECT_EQ(context.get("b"), 2);

    // reset and schedule new graph
    workflow.reset();
    std::shared_ptr<Graph> graph2 = std::make_shared<Graph>(graph_name);
    Node* node_c = graph2->create_node<ModuleC>("c");
    Node* node_d = graph2->create_node<ModuleD>("d");

    node_c->add_downstream(node_d);
    topology.set_timeout_ms(80);
    topology.create(graph2);

    UserContext context2;
    workflow.bind(&topology);
    workflow.schedule(&context2);

    // wait for background task done
    std::this_thread::sleep_for(std::chrono::seconds(3));
    EXPECT_EQ(workflow.get_result_status(), ErrorCode::SUCCESS);
    return 0;
}

int test_case_parser_expression1() {
    std::string workflow_expression = R"({
        "workflow": "test_case_parser_expression1",
        "dependencies": [
            {
                "target": "ModuleB",
                "dependencies": ["ModuleA", "ModuleD"]
            },
            {
                "target": "ModuleC",
                "dependencies": ["ModuleB"]
            }
        ]
    })";

    std::shared_ptr<Graph> graph = std::make_shared<Graph>();
    auto ret = JsonParser::parser_from_json(workflow_expression, graph);
    if (ret != 0) {
        LOG(ERROR) << "parser_from_json failed, ret : " << ret;
        return ret;
    }
    std::string graph_name = "test_case_parser_expression1";
    EXPECT_EQ_STR(graph->name(), graph_name);

    Topology topology;
    topology.set_timeout_ms(80);
    if (!topology.create(graph)) {
        LOG(ERROR) << "create graph failed";
        return -1;
    }

    EXPECT_EQ(topology.source_nodes().size(), 2);
    EXPECT_EQ(topology.sink_nodes().size(), 1);

    UserContext context;
    Workflow workflow(&topology);
    workflow.schedule(&context);

    // wait for background task done
    std::this_thread::sleep_for(std::chrono::seconds(3));
    EXPECT_EQ(workflow.get_result_status(), ErrorCode::SUCCESS);

    // 可视化graph
    ImageHelper::generate_image(graph->nodes(), graph->name());
    return 0;
}

int test_case_check_circle() {
    std::string workflow_expression = R"({
        "workflow": "test_case_circle",
        "dependencies": [
            {
                "target": "ModuleB",
                "dependencies": ["ModuleA"]
            },
            {
                "target": "ModuleC",
                "dependencies": ["ModuleB"]
            },
            {
                "target": "ModuleA",
                "dependencies": ["ModuleC"]
            }
        ]
    })";

    std::shared_ptr<Graph> graph = std::make_shared<Graph>();
    auto ret = JsonParser::parser_from_json(workflow_expression, graph);
    if (ret != 0) {
        LOG(ERROR) << "parser_from_json failed, ret : " << ret;
        return ret;
    }
    std::string graph_name = "test_case_circle";
    EXPECT_EQ_STR(graph->name(), graph_name);

    Topology topology;
    topology.set_timeout_ms(80);
    bool result = topology.create(graph);
    EXPECT_EQ(result, false);
    ImageHelper::generate_image(graph->nodes(), graph->name());
    return 0;
}

int test_case_parser_expression2() {
    std::string workflow_expression = R"({
        "workflow": "test_case_parser_expression2",
        "dependencies": [
            {
                "target": "ModuleC",
                "dependencies": ["ModuleA", "ModuleK"]
            },
            {
                "target": "ModuleD",
                "dependencies": ["ModuleA", "ModuleK", "ModuleC"]
            },
            {
                "target": "ModuleG",
                "dependencies": ["ModuleC"]
            },
            {
                "target": "ModuleH",
                "dependencies": ["ModuleD"]
            },
            {
                "target": "ModuleF",
                "dependencies": ["ModuleE"]
            },
            {
                "target": "ModuleI",
                "dependencies": ["ModuleF", "ModuleG", "ModuleH"]
            }
        ]
    })";

    std::shared_ptr<Graph> graph = std::make_shared<Graph>();
    auto ret = JsonParser::parser_from_json(workflow_expression, graph);
    if (ret != 0) {
        LOG(ERROR) << "parser_from_json failed, ret : " << ret;
        return ret;
    }
    std::string graph_name = "test_case_parser_expression2";
    EXPECT_EQ_STR(graph->name(), graph_name);

    Topology topology;
    topology.set_timeout_ms(8000);
    if (!topology.create(graph)) {
        LOG(ERROR) << "create graph failed";
        return -1;
    }

    EXPECT_EQ(topology.source_nodes().size(), 3);
    EXPECT_EQ(topology.sink_nodes().size(), 1);

    UserContext context;
    Workflow workflow(&topology);
    workflow.schedule(&context);

    // wait for background task done
    std::this_thread::sleep_for(std::chrono::seconds(3));
    EXPECT_EQ(workflow.get_result_status(), ErrorCode::SUCCESS);

    ImageHelper::generate_image(graph->nodes(), graph->name());
    return 0;
}

int test_module_register(){
    UserContext ctx;
    std::string target = "ModuleA";
    std::shared_ptr<Node> module(GREATE_MODULE(target, Node));
    module->run(&ctx);

    EXPECT_EQ(ctx.get("a"), 1);
    return 0;
}

int test_create_task() {
    auto task = std::make_shared<ExampleTask>();
    BackgroundTaskManager::getInstance().startTask(task, [task](void* result) {
        LOG(INFO) << "First callback executed";
        BackgroundTaskManager::getInstance().startTask(task, [](void* result) {
            LOG(INFO) << "Second callback executed";
        });
    });

    // wait for background task done
    std::this_thread::sleep_for(std::chrono::seconds(3));
    return 0;
}

// use after free
int test_asan() {
    const int length = 1024;
    int * p = new int[length];
    for (int i = 0; i < length; i++) {
        p[i] = i;
    }
    delete[] p;
    for (int i = 0; i < length; i++) {
        p[i] = i * 2;
    }
    return 0;
}

int main(int argc, char* argv[]) {
    LOG(INFO) << "hello dag ";
    // test_asan();
    test_create_task();
    test_module_register();
    test_case_run_graph();
    test_case_run_timeout();
    test_case_check_circle();
    test_case_context();
    test_case_parser_expression1();
    test_case_parser_expression2();
    return 0;
}