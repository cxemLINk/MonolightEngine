#include "monolight.h"
#include "gpuinterface.h"

#include "osbasics.h"
#include <stdexcept>

#include <iostream>
#include <string_view>

// Buffer functions
InternalRef(*_createBuffer)(size_t size, bool storage);
InternalRef(*_createBufferWithData)(const void* data, size_t size, bool storage);
void  (*_reallocateBuffer)(InternalRef ref, size_t newSize, bool storage);
void  (*_setBufferData)(InternalRef ref, size_t offset, const void* data, size_t size);
void  (*_destroyBuffer)(InternalRef ref);

// ShaderModule function pointers
InternalRef(*_createShaderModuleVF)(const char*, const char*);
InternalRef(*_createShaderModuleVFG)(const char*, const char*, const char*);
InternalRef(*_createShaderModuleSPIRV)(const unsigned int* data, size_t length);
void  (*_destroyShaderModule)(InternalRef ref);
void  (*_useShaderModule)(InternalRef ref);

// Global GPU functions
// Workaround, since GLEW doesn't wanna initialize until we have a window context
void setContext(void* pwnd);
void  (*_setContext)(void* wnd) = setContext;


namespace Monolight {
	namespace GPU {
		bool graphicsInitialized = false;
		enum GraphicsMode graphicsMode;
	}
}

using namespace Monolight;

std::string modeString()
{
	switch (GPU::graphicsMode)
	{
	case GPU::MODE_HEADLESS:
		return "Headless";
	case GPU::MODE_OPENGL:
		return "OpenGL";
	case GPU::MODE_VULKAN:
		return "Vulkan";
	}

	return "<INVALID>";
}





template<std::string const& FName, typename R>
R notSupportedFunc(...) {
	throw std::runtime_error("GPUInterface function "+FName+" not supported in "+modeString()+" mode");
}

void GPU::init()
{
	if (graphicsInitialized)
		return;

	if (GPU::vkInit()) {
		std::cout << "Initialized Vulkan" << std::endl;
	}
	else if (GPU::glInit()) {
		std::cout << "Initialized GL" << std::endl;
	}
	else {
		std::cout << "Could not initialize GL or Vulkan" << std::endl;
		graphicsMode = MODE_HEADLESS;
	}

	graphicsInitialized = true;
}

void GPU::destroy()
{
	if (!graphicsInitialized)
		return;

	// TODO: Deinitialization
	switch (graphicsMode) {
	case MODE_VULKAN:
		break;

	case MODE_OPENGL:
		break;

	case MODE_HEADLESS:
		break;
	}

	graphicsInitialized = false;
}

#define GLEW_STATIC
#include <gl/glew.h>
std::string GPU::info()
{
	const char* mode, * renderer, * version, * vendor;
	std::string extensions = "N/A";
	renderer = version = vendor = "N/A";
	mode = modeString().c_str();

	switch (graphicsMode) {
	case MODE_HEADLESS:
		mode = "Headless";
		renderer = "None";
		version = "0.0";
		vendor = "None";
		extensions = "None";
		break;

	case MODE_OPENGL:
		mode = "OpenGL";
		renderer = (const char*)glGetString(GL_RENDERER);
		version = (const char*)glGetString(GL_VERSION);
		vendor = (const char*)glGetString(GL_VENDOR);
		
		{
			std::string_view list((const char*)glGetString(GL_EXTENSIONS));
			extensions = "";
			size_t i = 0;
			while (i < list.length()) {
				i = list.find_first_not_of(' ', i);
				size_t next = list.find(' ', i);
				extensions += list.substr(i, next - i);
				extensions += '\n';
				i = next + 1;
			}

			//extensions = (const char*)glGetString(GL_EXTENSIONS);
		}
		break;

	case MODE_VULKAN:
		mode = "Vulkan";
		break;
	}

	char buf[8192];
	snprintf(buf, sizeof(buf), "Graphics:\n\tMode: %s\n\tRenderer: %s %s\n\tVendor: %s\nExtensions:\n%s\n",
		mode, renderer, version, vendor, extensions.c_str());

	return std::string(buf);
}


