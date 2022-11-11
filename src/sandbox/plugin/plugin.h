#ifndef SANDBOX_PLUGIN_PLUGIN_H_
#define SANDBOX_PLUGIN_PLUGIN_H_

#include <string>

#if defined(WIN32)
	#include <Windows.h>
	typedef HMODULE LibHandleType;
#else
	#include <dlfcn.h>
	typedef void * LibHandleType;
#endif

namespace sandbox {

class Plugin {
public:
    Plugin(const std::string& filePath, bool autoLoad = true);
	virtual ~Plugin();

	bool load();
	void unload();

	bool isLoaded() { return loaded; }

	template<typename TSig>
	TSig * loadSymbol(const std::string &functionName)
	{
		return reinterpret_cast<TSig *>(loadSymbolInternal(functionName));
	}

protected:
	void* loadSymbolInternal(const std::string &functionName);


private:
	std::string filePath;
	bool loaded;
	LibHandleType lib;
};

}

#endif