#include "monolight.h"
#include "gpuinterface.h"

#include "osbasics.h"
#include <stdexcept>

#if defined(WINDOWS)
#include <Windows.h>

void vkSetContext(void* pwnd)
{

}
#elif defined(LINUX)
void vkSetContext(void* pwnd)
{

}
#elif defined(MACOS)
void vkSetContext(void* pwnd)
{

}
#endif

InternalRef vkCreateShaderModule(const char* vertexSource, const char* fragmentSource)
{
	throw std::runtime_error("Shaders can only be created from source in OpenGL graphics mode!");
}



InternalRef vkCreateShaderModule(const char* vertexSource, const char* fragmentSource, const char* geometrySource)
{
	throw std::runtime_error("Shaders can only be created from source in OpenGL graphics mode!");
}

InternalRef vkCreateSPIRVModule(const unsigned int* data, size_t length)
{
	InternalRef r;
	r.vkshd = NULL;
	return r;
}

void vkDestroyShaderModule(InternalRef ref)
{

}

void vkUseShaderModule(InternalRef ref) {
	throw std::runtime_error("Can't set ShaderModule in Vulkan mode!");
}

using namespace Monolight;
bool GPU::vkInit()
{
	VkInstanceCreateInfo info;
	memset(&info, 0, sizeof(VkInstanceCreateInfo));
	info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;

	VkApplicationInfo appInfo;
	memset(&appInfo, 0, sizeof(VkApplicationInfo));
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	info.pApplicationInfo = &appInfo;

	VkInstance inst;
	VkResult res = vkCreateInstance(&info, NULL, &inst);
	if (res != VK_SUCCESS)
		return false;

	//TODO: The rest of initialization

	_createShaderModuleVF = vkCreateShaderModule;
	_createShaderModuleVFG = vkCreateShaderModule;
	_createShaderModuleSPIRV = vkCreateSPIRVModule;
	_destroyShaderModule = vkDestroyShaderModule;

	_setContext = vkSetContext;

	graphicsMode = MODE_VULKAN;
	return true;
}

