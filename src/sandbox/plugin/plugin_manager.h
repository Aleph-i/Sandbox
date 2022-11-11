#ifndef SANDBOX_PLUGIN_PLUGIN_MANAGER_H_
#define SANDBOX_PLUGIN_PLUGIN_MANAGER_H_

#include <vector>
#include "plugin.h"
#include "plugin_interface.h"

namespace sandbox {

class PluginManager {
public:
    PluginManager() {}
    virtual ~PluginManager() {}

    bool loadPlugin(const std::string& fileName) {
        Plugin* plugin = new Plugin(fileName);
        bool loaded = plugin->isLoaded();
        if (loaded) {
            for (int i = 0; i < interfaces.size(); i++) {
                interfaces[i]->load(plugin);
            }
            plugins.push_back(plugin);
        }
        else {
            delete plugin;
        }
        return loaded;
    }

    void addPluginInterface(PluginInterface* interface) {
        interfaces.push_back(interface);
    }

private:
    std::vector<Plugin*> plugins;
    std::vector<PluginInterface*> interfaces;
};

}

#endif