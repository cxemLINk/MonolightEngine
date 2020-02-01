#include "monolight.h"
using namespace Monolight;

#include "osbasics.h"
#include "GameWindow.h"
#include "PythonManager.h"
#include "gpuinterface.h"
#include "geometry.h"
#include <iostream>

int main()
{
	PythonManager::init();
	PythonManager::destroy();

    std::cout << "Hello World!\n";
	try {
		GameWindow wnd(L"Test");
		wnd.setSize(640, 480);
		wnd.setTitle(L"Test aaa");
		wnd.makeContextCurrent();
		GPU::init();

		std::cout << "====================" << std::endl;
		std::cout << GPU::info();
		std::cout << "====================" << std::endl;

		fixed48_16 a(2.5f), b(8.0f), c(1.001f);
		std::cout << "a = " << a.getFloat() << "; b = " << b.getFloat() << "; c = " << c.getFloat() << std::endl;
		std::cout << "a + b = " << (a + b).getFloat() << std::endl;

		wnd.setVisible(true);

		GPU::ShaderModule prog = GPU::ShaderModule(
			"#version 110 core\n"
			"in vec2 in_Pos;\n"
			"in vec3 in_Color;\n"
			"out vec3 col;\n"
			"void main(){\n"
			"	col = in_Color;\n"
			"	gl_Position = vec4(in_Pos, 0.0, 0.0);\n"
			"}\n"
			,
			"#version 110 core\n"
			"in vec3 col;\n"
			"void main(){\n"
			"	gl_FragColor = vec4(col, 1.0);\n"
			"}\n");

		prog.use();

		const float data[] = {
			 0.0f,  0.7f,  0.1f, 0.2f, 0.3f,
		    -0.5f, -0.1f,  0.4f, 0.5f, 0.6f,
			 0.5f, -0.1f,  0.7f, 0.8f, 0.9f
		};

		GPU::Buffer buf(data, sizeof(data), true);
		Ref<GPU::Buffer> bufRef(&buf);

		GPU::VertexDataLayout vertexLayout(
			{
				{ 4 * 5, false }
			},
			{
				{ 0, 0, {2, GPU::VertexDataAttribute::Format::Type::FLOAT}, 0 },
				{ 1, 0, {3, GPU::VertexDataAttribute::Format::Type::FLOAT}, 8 }
			}
		);
		
		GPU::GLVertexArrayObject vao(vertexLayout, {bufRef});

		while (!wnd.shouldExit()) {
			vao.bind();
			glDrawArrays(GL_TRIANGLES, 0, 3);
			vao.unbind();

			wnd.pollEvents();
		}

		GPU::destroy();
		std::cout << "Closed" << std::endl;
	}catch (const std::runtime_error& e) {
		std::cout << "Exception: " << e.what() << std::endl;
	}
}

