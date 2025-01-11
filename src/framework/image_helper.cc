#include <vector>
#include <string>
#include <fstream>
#include "src/utils/inc.h"
#include "src/framework/node.h"
#include "src/framework/image_helper.h"

namespace easy_dag {

int ImageHelper::generate_dot(const std::vector<std::shared_ptr<Node>>& nodes, const std::string& fileName) {
    std::ofstream dotFile(fileName.c_str());
    if (dotFile.is_open()) {
        dotFile << "digraph NodeGraph {" << std::endl;
        for (const auto& node : nodes) {
            dotFile << "    \"" << node->name() << "\"" << std::endl;
            for (const auto& upstreamNode : node->upstream()) {
                dotFile << "    \"" << upstreamNode->name() << "\" -> \"" << node->name() << "\"" << std::endl;
            }
        }
        dotFile << "}" << std::endl;
        dotFile.close();
    } else {
        LOG(ERROR) << "generate dot file " << fileName << " failed";
    }
    return 0;
}

int ImageHelper::generate_image_from_dot(const std::string& dot_file, const std::string& image_file) {
    std::string command = "dot -Tpng " + dot_file + " -o " + image_file;
    int result = std::system(command.c_str());
    if (result!= 0) {
        LOG(ERROR) << "generate dot file to png failed";
    }
    std::string del_dot = "rm " + dot_file;
    result = std::system(del_dot.c_str());
    return 0;
}

int ImageHelper::generate_image(const std::vector<std::shared_ptr<Node>>& nodes, std::string graph_name) {
    static const std::string dot = ".dot";
    static const std::string png = ".png";

    std::string graph_dot = graph_name + dot;
    std::string graph_png = graph_name + png;
    int ret = ImageHelper::generate_dot(nodes, graph_dot);
    if (ret != 0) {
        LOG(ERROR) << "generate_dot failed, ret = " << ret;
        return ret;
    }
    ret = ImageHelper::generate_image_from_dot(graph_dot, graph_png);
    if (ret != 0){
        LOG(ERROR) << "generate_image_from_dot failed, ret = " << ret;
        return ret;
    }
    return 0;
}

}