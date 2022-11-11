#ifndef SANDBOX_PLUGIN_PLUGIN_INTERFACE_H_
#define SANDBOX_PLUGIN_PLUGIN_INTERFACE_H_

#include <string>
#include "plugin.h"

namespace sandbox {

class PluginInterface {
public:
    virtual ~PluginInterface() {}

    void load(Plugin* plugin) {
        typedef void load_t(PluginInterface*);
    	load_t* registerInterface = plugin->loadSymbol<load_t>("registerInterface");
        if (registerInterface) {
            registerInterface(this);
        }
    }
};

}

#endif