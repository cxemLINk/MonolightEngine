#pragma once
#include "monolight.h"
#include "gpuinterface.h"

#include "geometry.h"

namespace Monolight {
	
	class Material
	{
	public:
		Material() {};
		~Material();

	private:
		
	};

	class Mesh
	{
	public:
		Mesh();
		~Mesh();

	private:
		std::vector<std::pair<void*, Material>> vertexGroups;
		GPU::VertexDataLayout layout;
	};

	class RenderingContext3D
	{
	public:
		void drawMesh(Vec3 pos, Mesh m);
		void drawSphere(Vec3 pos, float size, int segments);
		void drawQuad(Vec3 pos, Vec3 norm, Vec2 size);


	};
}
