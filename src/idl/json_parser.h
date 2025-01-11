#include <string>
#include "src/framework/graph.h"
namespace easy_dag {

class JsonParser {
public:
    static int parser_from_json(const std::string& json_str, std::shared_ptr<Graph>& graph);
};
}