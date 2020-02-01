#pragma once

#include "monolight.h"

#include <string>
#include <vector>
#include <utility>

#define GLEW_STATIC
#include <GL/glew.h>

#include <vulkan/vulkan.hpp>

union InternalRef {
	GLint gli;
	GLuint glu;
	VkBuffer vkbuf;
	VkShaderModule vkshd;
};

// Buffer functions
extern InternalRef (*_createBuffer)(size_t size, bool storage);
extern InternalRef (*_createBufferWithData)(const void* data, size_t size, bool storage);
extern void  (*_reallocateBuffer)(InternalRef ref, size_t newSize, bool storage);
extern void  (*_setBufferData)(InternalRef ref, size_t offset, const void* data, size_t size);
extern void  (*_destroyBuffer)(InternalRef ref);

// ShaderModule function pointers
extern InternalRef (*_createShaderModuleVF)(const char*, const char*);
extern InternalRef (*_createShaderModuleVFG)(const char*, const char*, const char*);
extern InternalRef (*_createShaderModuleSPIRV)(const unsigned int* data, size_t length);
extern void  (*_destroyShaderModule)(InternalRef ref);
extern void  (*_useShaderModule)(InternalRef ref);

// Global GPU functions
extern void  (*_setContext)(void* wnd);

namespace Monolight {

	namespace GPU {

		class Buffer
		{
		// TODO: Usage passing
		public:
			Buffer(Buffer& buf) = delete;

			// Creates a mutable buffer of specific size
			Buffer(size_t size) : Buffer(size, false) {};

			// Creates buffer of specific size
			// The buffer is immutable(not reallocateable) if storage is true
			Buffer(size_t size, bool storage) {
				ref = _createBuffer(size, storage);
			};

			// Creates a mutable buffer, and uploads data to it
			Buffer(const void* data, size_t size) : Buffer(data, size, false) {};

			// Creates buffer, and uploads data to it
			// The buffer is immutable(not reallocateable) if storage is true
			Buffer(const void* data, size_t size, bool storage) {
				ref = _createBufferWithData(data, size, storage);
			};

			~Buffer() {
				_destroyBuffer(ref);
			}

			void setData(const void* data, size_t size){
				setData(0, data, size);
			};
			void setData(size_t offset, const void* data, size_t size);

			InternalRef ref;
		};

		using BufferRef = Monolight::Ref<Buffer>;

		struct VertexDataBinding {
			uint32_t stride;
			bool perInstance;
		};
		
		struct VertexDataAttribute {
			uint32_t location;
			uint32_t binding;
			
			struct Format {
				int elems;
				enum Type {BYTE, UBYTE, SHORT, USHORT, INT, UINT, FLOAT} type;
				bool normalize;
			} format;

			uint32_t offset;
		};

		class VertexDataLayout
		{
		public:
			VertexDataLayout(std::vector<VertexDataBinding> bindings, std::vector<VertexDataAttribute> attributes)
				: bindings(bindings), attributes(attributes) {};
			
			std::vector<VertexDataBinding> bindings;
			std::vector<VertexDataAttribute> attributes;
		};

		class ShaderModule
		{
		public:
			ShaderModule(const char* vertex, const char* fragment) {
				ref = _createShaderModuleVF(vertex, fragment);
			}

			ShaderModule(const char* vertex, const char* fragment, const char* geometry) {
				ref = _createShaderModuleVFG(vertex, fragment, geometry);
			}

			~ShaderModule() {
				_destroyShaderModule(ref);
			}

			void use() {
				_useShaderModule(ref);
			}

		private:
			InternalRef ref;
		};

		extern enum GraphicsMode { MODE_HEADLESS, MODE_OPENGL, MODE_VULKAN } graphicsMode;
		extern bool graphicsInitialized;

		void init();
		bool glInit();
		bool vkInit();

		void destroy();
		std::string info();

		inline void setContext(void* wnd) {
			_setContext(wnd);
		}

		// Load shader from file
		// First the shader description file(.shd) is looked up, loaded if found
		// Otherwise shader files themselves are loaded(.vert, .frag, .geom)
		// In case of Vulkan, a .v is appended to the end of the name
		//ShaderModule createShaderModule(::std::string name);

		// === OpenGL only
		class GLVertexArrayObject
		{
		public:
			GLVertexArrayObject(VertexDataLayout& layout, std::vector<BufferRef> buffers);
			~GLVertexArrayObject();

			void bind();
			void unbind();

		private:
			GLuint ref;
			std::vector<BufferRef> bufs;
		};

		void glSetShaderModule(ShaderModule);
	}

}
