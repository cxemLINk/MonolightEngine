#include "monolight.h"
#include "gpuinterface.h"

#include "osbasics.h"
#include <stdexcept>

#include <iostream>

#define CHECK_GL_ERROR do{ \
GLenum err = glGetError(); \
if(err) std::cout << "OpenGL error: 0x" << std::hex << err << std::endl; \
}while(0)

using namespace Monolight::GPU;

#if defined(WINDOWS)
#include <Windows.h>
void setContext(void* pwnd)
{
	HWND wnd = (HWND)pwnd;
	HDC dc = GetDC(wnd);
	HGLRC ctx;
	// TODO: Save contexts
	if (true) {
		PIXELFORMATDESCRIPTOR pfd;
		pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
		pfd.nVersion = 1;
		pfd.dwFlags = PFD_DOUBLEBUFFER;
		pfd.iPixelType = PFD_TYPE_RGBA;
		pfd.cColorBits = 24;
		pfd.cAlphaBits = 0;
		pfd.cAccumBits = 24;
		pfd.cDepthBits = 24;
		pfd.cStencilBits = 8;
		pfd.iLayerType = PFD_MAIN_PLANE;

		int format = ChoosePixelFormat(dc, &pfd);
		if (format == 0)
			throw std::runtime_error("Unable to choose pixel format");

		SetPixelFormat(dc, format, &pfd);

		ctx = wglCreateContext(dc);
		if (ctx == NULL)
			throw std::runtime_error("Context creation failed!");
	}

	wglMakeCurrent(dc, ctx);
}
#elif defined(LINUX)
void setContext(void* pwnd)
{

}
#elif defined(MACOS)
void setContext(void* pwnd)
{

}
#endif

GLuint createShaderFromSource(GLenum type, const char* source)
{
	GLuint ret = glCreateShader(type);
	glShaderSource(ret, 1, &source, NULL);
	glCompileShader(ret);

	GLint res;
	glGetShaderiv(ret, GL_COMPILE_STATUS, &res);
	if (res != GL_TRUE) {
		glGetShaderiv(ret, GL_INFO_LOG_LENGTH, &res);
		res = (res > 4096) ? 4096 : res;

		char* mem = (char*)malloc(res);
		glGetShaderInfoLog(ret, res, NULL, mem);

		char buf[4096];
		snprintf(buf, sizeof(buf), "%s shader compilation failed:\n=============\n%s\n=============\n",
			((type == GL_VERTEX_SHADER) ? "Vertex" :
			((type == GL_FRAGMENT_SHADER) ? "Fragment" :
				((type == GL_GEOMETRY_SHADER) ? "Geometry" : "<Invalid>")
				)), mem);

		throw std::runtime_error(buf);
	}

	return ret;
}

void checkProgram(GLuint prog)
{
	GLint res;
	glGetProgramiv(prog, GL_LINK_STATUS, &res);
	if (res != GL_TRUE) {
		glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &res);
		char* mem = (char*)malloc(res);
		glGetProgramInfoLog(prog, res, NULL, mem);

		char buf[4096];
		snprintf(buf, sizeof(buf), "Program linking failed:\n=============\n%s\n=============\n", mem);

		throw std::runtime_error(buf);
	}
}

InternalRef createShaderModule(const char* vertexSource, const char* fragmentSource)
{
	GLuint vert = createShaderFromSource(GL_VERTEX_SHADER, vertexSource);
	GLuint frag = createShaderFromSource(GL_FRAGMENT_SHADER, fragmentSource);
	GLuint prog = glCreateProgram();

	glAttachShader(prog, vert);
	glAttachShader(prog, frag);
	glLinkProgram(prog);
	glDetachShader(prog, vert);
	glDetachShader(prog, frag);
	glDeleteShader(vert);
	glDeleteShader(frag);

	checkProgram(prog);

	InternalRef r;
	r.glu = prog;
	return r;
}

InternalRef createShaderModule(const char* vertexSource, const char* fragmentSource, const char* geometrySource)
{
	GLuint vert = createShaderFromSource(GL_VERTEX_SHADER, vertexSource);
	GLuint frag = createShaderFromSource(GL_FRAGMENT_SHADER, fragmentSource);
	GLuint geom = createShaderFromSource(GL_GEOMETRY_SHADER, geometrySource);
	GLuint prog = glCreateProgram();

	glAttachShader(prog, vert);
	glAttachShader(prog, frag);
	glAttachShader(prog, geom);
	glLinkProgram(prog);
	glDetachShader(prog, vert);
	glDetachShader(prog, frag);
	glDetachShader(prog, geom);
	glDeleteShader(vert);
	glDeleteShader(frag);
	glDeleteShader(geom);

	checkProgram(prog);

	InternalRef r;
	r.glu = prog;
	return r;
}

void destroyShaderModule(InternalRef ref)
{
	glDeleteProgram(ref.glu);
}

void useShaderModule(InternalRef ref) {
	glUseProgram(ref.glu);
}

InternalRef createBuffer(const void* data, size_t size, bool storage)
{
	GLuint buf;
	glGenBuffers(1, &buf);

	glBindBuffer(GL_COPY_WRITE_BUFFER, buf);
	if (storage)
		glBufferStorage(GL_COPY_WRITE_BUFFER, size, NULL, 0);
	else
		glBufferData(GL_COPY_WRITE_BUFFER, size, NULL, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_COPY_WRITE_BUFFER, 0);

	InternalRef r;
	r.glu = buf;
	return r;
}

InternalRef createBuffer(size_t size, bool storage) {
	return createBuffer(NULL, size, storage);
}

void reallocateBuffer(InternalRef ref, size_t newSize, bool storage) {
	throw std::runtime_error("Not implemented");
}

const size_t MAX_OFFSET = (1 << (sizeof(GLintptr) * 8)) - 1;
const size_t MAX_SIZE   = (1 << (sizeof(GLsizeiptr) * 8)) - 1;
void setBufferData(InternalRef ref, size_t offset, const void* data, size_t size)
{
	ML_ASSERT(offset <= MAX_OFFSET, "Offset too big");
	ML_ASSERT(size <= MAX_SIZE, "Size too big!");
	ML_ASSERT(data != NULL, "Data is NULL!");

	glBindBuffer(GL_COPY_WRITE_BUFFER, ref.glu);
	glBufferSubData(GL_COPY_WRITE_BUFFER, offset, size, data);
	glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
}

void destroyBuffer(InternalRef ref) {
	glDeleteBuffers(1, &(ref.glu));
}

GLVertexArrayObject::GLVertexArrayObject(VertexDataLayout& layout, std::vector<BufferRef> buffers)
{
	glGenVertexArrays(1, &ref);

	glBindVertexArray(ref);
	for (auto iter = layout.attributes.begin(); iter != layout.attributes.end(); iter++) {
		auto format = (*iter).format;

		int idx = (*iter).binding;
		if (idx < 0 || idx >= layout.bindings.size() || idx >= buffers.size())
			throw std::runtime_error("Invalid attribute binding index");
		
		auto binding = layout.bindings[idx];
		
		GLenum gl_type;
		
		switch (format.type) {
		case VertexDataAttribute::Format::BYTE:
			gl_type = GL_BYTE;
			break;
		case VertexDataAttribute::Format::UBYTE:
			gl_type = GL_UNSIGNED_BYTE;
			break;
		case VertexDataAttribute::Format::SHORT:
			gl_type = GL_SHORT;
			break;
		case VertexDataAttribute::Format::USHORT:
			gl_type = GL_UNSIGNED_SHORT;
			break;
		case VertexDataAttribute::Format::FLOAT:
			gl_type = GL_FLOAT;
			format.normalize = false;
			break;
		default:
			throw std::runtime_error("Invalid attribute format type");
		}

		glBindBuffer(GL_ARRAY_BUFFER, buffers[idx]->ref.glu);
		
		GLuint loc = (*iter).location;
		glVertexAttribPointer(loc, format.elems, gl_type,
			format.normalize, binding.stride, reinterpret_cast<const void*>((*iter).offset));
		glVertexAttribDivisor(loc, binding.perInstance ? 1 : 0);
		glEnableVertexAttribArray(loc);
		CHECK_GL_ERROR;
	}
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	CHECK_GL_ERROR;

	// Hold the references, so buffers don't get deallocated
	bufs = buffers;
}

GLVertexArrayObject::~GLVertexArrayObject() {
	glDeleteVertexArrays(1, &ref);
	//delete &bufs;
}

void GLVertexArrayObject::bind() {
	glBindVertexArray(ref);
	CHECK_GL_ERROR;
}

void GLVertexArrayObject::unbind() {
	glBindVertexArray(0);
	CHECK_GL_ERROR;
}

using namespace Monolight;
bool GPU::glInit()
{
	int res;
	if ((res = glewInit()) != GLEW_OK)
		return false;

	CHECK_GL_ERROR;

	_createBuffer = createBuffer;
	_createBufferWithData = createBuffer;
	_reallocateBuffer = reallocateBuffer;
	_setBufferData = setBufferData;
	_destroyBuffer = destroyBuffer;

	_createShaderModuleVF = createShaderModule;
	_createShaderModuleVFG = createShaderModule;
	_destroyShaderModule = destroyShaderModule;
	_useShaderModule = useShaderModule;

	_setContext = setContext;

	graphicsMode = MODE_OPENGL;
	return true;
}

