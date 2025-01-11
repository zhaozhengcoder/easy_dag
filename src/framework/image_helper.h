#pragma once
#include <memory>
#include <string>
#include "src/framework/node.h"

namespace easy_dag {

class ImageHelper {
public:
    static int generate_dot(const std::vector<std::shared_ptr<Node>>& nodes, const std::string& fileName);
    static int generate_image_from_dot(const std::string& dotFileName, const std::string& imageFileName);
    static int generate_image(const std::vector<std::shared_ptr<Node>>& nodes, std::string graph_name);
};

}