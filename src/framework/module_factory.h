#pragma once 

#include <map>
#include <string>

namespace easy_dag {

typedef void* (*CreateModuleFunction)(void);

class ModuleFactory {
public:
    ModuleFactory() {};

    void* CreateModule(const std::string& moduleName) {
        std::map<std::string, CreateModuleFunction>::const_iterator it = moduleMap_.find(moduleName);
        if (it == moduleMap_.end()) {
            return nullptr;
        } else {
            return it->second();  // here new a module object
        }
    }

    void RegisterModule(const std::string& moduleName, CreateModuleFunction createFunction) {
        moduleMap_.insert(std::pair<std::string, CreateModuleFunction>(moduleName, createFunction));
    }

    static ModuleFactory& GetInstance() {
        static ModuleFactory factory;
        return factory;
    }

private:
    std::map<std::string, CreateModuleFunction> moduleMap_;
};

class ModuleRegister {
public:
    ModuleRegister(const std::string& moduleName, CreateModuleFunction createFunction) {
        ModuleFactory::GetInstance().RegisterModule(moduleName, createFunction);
    }
};

#define REGISTER_MODULE(ModuleName)                      \
    ModuleName* CreateInstance##ModuleName() {          \
        return new ModuleName();                        \
    }                                                   \
    ModuleRegister g_ModuleRegister##ModuleName(        \
        #ModuleName, (CreateModuleFunction)CreateInstance##ModuleName)

#define GREATE_MODULE(moduleName, ModuleType)              \
    (ModuleType*)ModuleFactory::GetInstance().CreateModule(moduleName)

}