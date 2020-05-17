#pragma once

#include <triangle_mesh.h>

#include <glm\glm.hpp>
#include <GL\gl3w.h>
#include <oglwrap.h>

namespace render {

	class atlas_renderer final {
		gl::Program		atlas_program	= gl::Program(0);
		gl::VertexArray mesh_vao		= gl::VertexArray(0);
		gl::ArrayBuffer mesh_vbo_pos	= gl::ArrayBuffer(0);
		gl::ArrayBuffer mesh_vbo_nor	= gl::ArrayBuffer(0);
		gl::ArrayBuffer mesh_vbo_uv		= gl::ArrayBuffer(0);		
		gl::IndexBuffer mesh_ibo		= gl::IndexBuffer(0);
		gl::Texture2D	color_tex		= gl::Texture2D(0);
		gl::Texture2D	attr_tex		= gl::Texture2D(0);
		gl::Renderbuffer z_buffer		= gl::Renderbuffer(0);
		gl::Framebuffer	frame_buffer	= gl::Framebuffer(0);
	public:
		atlas_renderer();
		~atlas_renderer();

		GLuint prepare_atlas_render() const;

		void create(
			const core::tmesh_cptr mesh
		);
		//void render();
	};
} /// namespace render
