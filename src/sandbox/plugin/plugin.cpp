#include "plugin.h"

#include <iostream>

namespace sandbox {

Plugin::Plugin(const std::string& filePath, bool autoLoad) : filePath(filePath), loaded(false) {
    if (autoLoad) {
        load();
    }
}

Plugin::~Plugin() {

}

bool Plugin::load() {
    if (!loaded) {
            const char* error;
    #if defined(WIN32)
            _lib = LoadLibraryA(_filePath.c_str());
    #else
            dlerror();
            lib = dlopen(filePath.c_str(), RTLD_NOW);
            error = dlerror();
    #endif

            if (!lib) {
                std::cout << error << std::endl;
                return false;
            }

            loaded = true;
    }

    return loaded;
}

void Plugin::unload() {
	if (loaded) {
		const char* error;
#if defined(WIN32)
		BOOL result = FreeLibrary(_lib);
#else
		dlerror();
		int result = dlclose(lib);
		error = dlerror();
#endif
		if(result != 0) {
            std::cout << error << std::endl;
			return;
		}

		loaded = false;
	}
}

void* Plugin::loadSymbolInternal(const std::string &functionName) {
	if (loaded) {
#if defined(WIN32)
		FARPROC symbol =GetProcAddress(_lib, functionName.c_str());
		if (!symbol) {
			std::cout << "Cannot load symbol: " << functionName << " - " << "" << std::endl;
			//MinVR::Logger::getInstance().assertMessage(false, "Cannot load symbol: " + functionName + " - " + "");

			return NULL;
		}

		return symbol;
#else
		void* symbol = (void*) dlsym(lib, functionName.c_str());
		const char* dlsym_error = dlerror();
		if (dlsym_error) {
			std::cout << "Cannot load symbol: " << functionName << " - " << dlsym_error << std::endl;
			dlerror();

			return NULL;
		}

		return symbol;
#endif

	}

	return NULL;
}

}
