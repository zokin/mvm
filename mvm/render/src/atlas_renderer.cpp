#include <atlas_renderer.h>

//#include <imgui.h>
//
//#define STB_IMAGE_WRITE_IMPLEMENTATION
//#include <../io/include/stb_image_writeh.h>

const char* atlas_vertex_source =
"#version 330 core\n"
"in vec2 uv;\n"
"in vec3 pos;\n"
"in vec3 nor;\n"
""
"out vec3 unit_pos;\n"
"out vec3 norm;\n"
""
"void main() {\n"
	"unit_pos = clamp(abs(pos / 1.0), 0.0, 1.0);\n"
	"norm = nor;\n"
	"gl_Position = vec4(uv.x * 2.0 - 1.0, uv.y * 2.0 - 1.0, 0.5, 1.0);\n"
"}\n"
;

const char* atlas_fragment_source =
" #version 330 core\n"
"in vec3 unit_pos;\n"
"in vec3 norm;\n"
""
"out vec4 fragment_color; \n"
""
"void main() {\n"
	//"fragment_color = vec4(0.6, 0.7, 0.2, 1.0);\n"
	"fragment_color = vec4(unit_pos * abs(norm), 1.0);\n"
"}\n"
""
;

render::atlas_renderer::atlas_renderer()
{
	atlas_program = gl::Program(); //TODO: Fix silent shader file loading crash /w a try/catch or external check
	gl::ShaderSource dvs_source;
	dvs_source.set_source(std::string(atlas_vertex_source));
	gl::Shader dvs(gl::enums::ShaderType::kVertexShader, dvs_source);
	gl::ShaderSource dfs_source;
	dfs_source.set_source(std::string(atlas_fragment_source));
	gl::Shader dfs(gl::enums::ShaderType::kFragmentShader, dfs_source);
	atlas_program.attachShader(dvs);
	atlas_program.attachShader(dfs);
	atlas_program.link();

	gl::Use(atlas_program);
	mesh_vao = gl::VertexArray();
	gl::Bind(mesh_vao);
	mesh_vbo_pos = gl::ArrayBuffer();
	gl::Bind(mesh_vbo_pos);
	(atlas_program | "pos").setup<glm::vec3>().enable();
	mesh_vbo_nor = gl::ArrayBuffer();
	gl::Bind(mesh_vbo_nor);
	(atlas_program | "nor").setup<glm::vec3>().enable();
	mesh_vbo_uv = gl::ArrayBuffer();
	gl::Bind(mesh_vbo_uv);
	(atlas_program | "uv").setup<glm::vec2>().enable();
	mesh_ibo = gl::IndexBuffer();
	gl::Unbind(mesh_vao);	
	gl::Unbind(mesh_vbo_uv);
	gl::Unbind(mesh_ibo);
	gl::Unuse(atlas_program);

	gl::Use(atlas_program);
	// setup uniforms
	gl::Unuse(atlas_program);

	color_tex = gl::Texture2D();// color component
	gl::Bind(color_tex);
	color_tex.storage(1, gl::enums::PixelDataInternalFormat::kRgba8, 1024, 1024);
	color_tex.magFilter(gl::enums::MagFilter::kLinear);
	color_tex.minFilter(gl::enums::MinFilter::kLinear);
	color_tex.wrapS(gl::enums::WrapMode::kClampToBorder);
	color_tex.wrapT(gl::enums::WrapMode::kClampToBorder);
	
	//attr_tex = gl::Texture2D();// attr component						
	//gl::Bind(attr_tex);
	//attr_tex.storage(1, gl::enums::PixelDataInternalFormat::kRgba32F, 1024, 1024);
	//attr_tex.magFilter(gl::enums::MagFilter::kLinear);
	//attr_tex.minFilter(gl::enums::MinFilter::kLinear);
	//attr_tex.wrapS(gl::enums::WrapMode::kClampToBorder);
	//attr_tex.wrapT(gl::enums::WrapMode::kClampToBorder);
	z_buffer = gl::Renderbuffer();// depth component
	gl::Bind(z_buffer);
	z_buffer.storage(gl::enums::PixelDataInternalFormat::kDepthComponent, 1024, 1024);
	frame_buffer = gl::Framebuffer();
	gl::Bind(frame_buffer);// attach all components to frame buffer
	frame_buffer.attachTexture(gl::enums::FramebufferAttachment::kColorAttachment0, color_tex, 0);
	//frame_buffer.attachTexture(gl::enums::FramebufferAttachment::kColorAttachment1, attr_tex, 0);
	frame_buffer.attachBuffer(gl::enums::FramebufferAttachment::kDepthAttachment, z_buffer);
	gl::Unbind(frame_buffer);
	gl::Unbind(color_tex);
	gl::Unbind(z_buffer);
}

render::atlas_renderer::~atlas_renderer()
{
	atlas_program = gl::Program(0);
	mesh_vao = gl::VertexArray(0);
	mesh_vbo_uv = gl::ArrayBuffer(0);
	mesh_vbo_pos = gl::ArrayBuffer(0);
	mesh_vbo_nor = gl::ArrayBuffer(0);
	mesh_ibo = gl::IndexBuffer(0);
	color_tex = gl::Texture2D(0);
	//attr_tex = gl::Texture2D(0);
	z_buffer = gl::Renderbuffer(0);
	frame_buffer = gl::Framebuffer(0);
}

void render::atlas_renderer::create(
	const core::tmesh_cptr mesh
)
{
	auto vp = gl::Viewport();
	gl::Disable(gl::enums::kCullFace);
	gl::Bind(frame_buffer);	
	//gl::DrawBuffers({ gl::enums::kColorAttachment0 , gl::enums::kColorAttachment1 }); // which buffers to draw
	gl::DrawBuffers({ gl::enums::kColorAttachment0 }); // which buffers to draw
	gl::Viewport(0, 0, 1024, 1024);
	gl::ClearColor(0.f, 0.f, 0.f, 1.0f);
	gl::Clear().Color().Depth();
	gl::Bind(mesh_vbo_pos);
	mesh_vbo_pos.data(mesh->vertices(), gl::enums::BufferUsage::kStaticDraw);
	gl::Bind(mesh_vbo_nor);
	mesh_vbo_nor.data(mesh->normals(), gl::enums::BufferUsage::kStaticDraw);
	gl::Bind(mesh_vbo_uv);
	mesh_vbo_uv.data(mesh->uvs(), gl::enums::BufferUsage::kStaticDraw);
	gl::Bind(mesh_ibo);
	mesh_ibo.data(mesh->uv_faces(), gl::enums::BufferUsage::kStaticDraw);
	gl::Use(atlas_program);	
	gl::Bind(mesh_vao);
	gl::Bind(mesh_ibo);
	gl::DrawElements(gl::PrimType::kTriangles, 3 * mesh->uv_face_count(), gl::enums::IndexType::kUnsignedInt);
	//std::vector<uint8_t> image(1024 * 1024 * 3);
	//gl::ReadBuffer(gl::enums::ColorBuffer::kColorAttachment0);
	//gl::ReadPixels(0, 0, 1024, 1024, gl::enums::PixelDataFormat::kRgb, gl::enums::PixelDataType::kByte,
	//	image.data());
	//stbi_write_png("test.png", 1024, 1024, 3, image.data(), 1024 * 3);	
	gl::Viewport(vp.x, vp.y, vp.z, vp.w);
	gl::Unbind(frame_buffer);
	gl::Enable(gl::enums::kCullFace);
	gl::Unbind(mesh_vao);
	gl::Unbind(mesh_ibo);
	gl::Unuse(atlas_program);
}

GLuint render::atlas_renderer::prepare_atlas_render() const
{
	gl::Bind(color_tex);
	return (GLuint)color_tex.expose(); 
}