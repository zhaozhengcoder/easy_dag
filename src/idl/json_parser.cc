#include <string>
#include <map>
#include <string>
#include <memory>
#include "src/third_party/json.hpp"
#include "src/framework/graph.h"
#include "src/framework/node.h"
#include "json_parser.h"
#include "src/framework/module_factory.h"

namespace easy_dag {
int JsonParser::parser_from_json(const std::string& json_str, std::shared_ptr<Graph>& graph) {
    nlohmann::json data;
    try {
        data = nlohmann::json::parse(json_str);
    } catch (const nlohmann::json::parse_error& e) {
        LOG(ERROR) << "json parser error" << e.what();
        return INVALID_JSON;
    } catch (...) {
        LOG(ERROR) << "json parser failed";
        return INVALID_JSON;
    }

    if (data.find("workflow") == data.end()) {
        LOG(ERROR) << "workflow key not found in json data";
        return INVALID_JSON_DATA;
    }
    std::string wf_name = data["workflow"];
    graph->set_name(wf_name);

    std::unordered_map<std::string, std::shared_ptr<Node>> node_map;
    if (data.find("dependencies") != data.end()) {
        for (const auto& dep : data["dependencies"]) {
            if (dep.find("target")!= dep.end()) {
                std::string target = dep["target"];
                if (node_map.find(target) == node_map.end()) {
                    Node * node = GREATE_MODULE(target, Node);
                    if (node == nullptr) {
                        LOG(INFO) << "create node failed, name : " << target;
                        return ErrorCode::UNKNOWN_MODULE;
                    }
                    std::shared_ptr<Node> node_ptr(node);
                    int ret = node_ptr->init();
                    if (ret!= ErrorCode::SUCCESS) {
                        LOG(ERROR) << "node init failed, name : " << target << ", ret : " << ret;
                        return ret;
                    }
                    node_ptr->set_name(target);
                    node_map[target] = node_ptr;
                }

                if (dep.find("dependencies")!= dep.end()) {
                    for (const auto& source : dep["dependencies"]) {
                        std::string source_name = source;
                        if (node_map.find(source_name) == node_map.end()) {
                            Node * node = GREATE_MODULE(source_name, Node);
                            if (node == nullptr) {
                                LOG(INFO) << "create node failed, name : " << target;
                                return ErrorCode::UNKNOWN_MODULE;
                            }
                            std::shared_ptr<Node> node_ptr(node);
                            int ret = node_ptr->init();
                            if (ret!= ErrorCode::SUCCESS) {
                                LOG(ERROR) << "node init failed, name : " << target << ", ret : " << ret;
                                return ret;
                            }
                            node_ptr->set_name(source_name);
                            node_map[source_name] = node_ptr;
                        }
                        node_map[source_name]->add_downstream(node_map[target].get());
                    }
                } else {
                    LOG(ERROR) << "dependencies key not found in a dependency entry";
                    return INVALID_JSON_DATA;
                }
            } else {
                LOG(ERROR) << "target key not found in a dependency entry";
                return INVALID_JSON_DATA;
            }
        }
    } else {
        LOG(ERROR) << "dependencies key not found in json data";
        return INVALID_JSON_DATA;
    }

    for (auto& iter : node_map) {
        graph->add_node(iter.second);
    }
    return 0;
}

}