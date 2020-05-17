#pragma once

#include <triangle_mesh.h>

#include <glm\glm.hpp>
#include <GL\gl3w.h>
#include <oglwrap.h>

namespace render {

	class triangle_mesh_renderer final {
		gl::Program		mesh_program	= gl::Program(0);
		gl::VertexArray mesh_vao		= gl::VertexArray(0);
		gl::ArrayBuffer mesh_vbo_pos	= gl::ArrayBuffer(0);
		gl::ArrayBuffer mesh_vbo_norm	= gl::ArrayBuffer(0);
		gl::IndexBuffer mesh_ibo		= gl::IndexBuffer(0);
	public:
		triangle_mesh_renderer();
		~triangle_mesh_renderer();

		void render(
			const core::tmesh_cptr mesh,
			const glm::mat4& projection_view
		);
	};
} /// namespace render