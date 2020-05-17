#include <triangle_mesh_renderer.h>

const char* mesh_vertex_source =
"#version 330 core\n"
"in vec3 position;\n"
"in vec3 normal;\n"
""
"uniform mat4 vp;\n"
""
"out vec3 global_normal;\n"
""
"void main() {\n"
	"global_normal = normal; //TODO: remove normalization\n"
	"gl_Position = vp * vec4(position, 1.0);\n"
"}\n"	
;

const char* mesh_fragment_source =
" #version 330 core\n"
""
"in vec3 global_normal;\n"
""
"out vec4 fragColor;\n"
""
"void main() {\n"
//"//fragColor = vec4(global_normal + vec3(1.0f) / vec3(2.0f), 1.0f);\n"
	"fragColor = vec4(abs(global_normal), 1.0f);\n"
"}\n"
""
;

render::triangle_mesh_renderer::triangle_mesh_renderer()
{
	mesh_program = gl::Program(); //TODO: Fix silent shader file loading crash /w a try/catch or external check
	gl::ShaderSource dvs_source;
	dvs_source.set_source(std::string(mesh_vertex_source));
	gl::Shader dvs(gl::enums::ShaderType::kVertexShader, dvs_source);
	gl::ShaderSource dfs_source;
	dfs_source.set_source(std::string(mesh_fragment_source));
	gl::Shader dfs(gl::enums::ShaderType::kFragmentShader, dfs_source);
	mesh_program.attachShader(dvs);
	mesh_program.attachShader(dfs);
	mesh_program.link();

	gl::Use(mesh_program);
	mesh_vao = gl::VertexArray();
	gl::Bind(mesh_vao);
	mesh_vbo_pos = gl::ArrayBuffer();
	gl::Bind(mesh_vbo_pos);
	(mesh_program | "position").setup<glm::vec3>().enable();
	mesh_vbo_norm = gl::ArrayBuffer();
	gl::Bind(mesh_vbo_norm);
	(mesh_program | "normal").setup<glm::vec3>().enable();
	mesh_ibo = gl::IndexBuffer();
	gl::Unbind(mesh_vao);
	gl::Unbind(mesh_vbo_pos);
	gl::Unbind(mesh_ibo);
	gl::Unuse(mesh_program);

	gl::Use(mesh_program);
	// setup uniforms
	gl::Unuse(mesh_program);
}

render::triangle_mesh_renderer::~triangle_mesh_renderer()
{
	mesh_program = gl::Program(0);
	mesh_vao = gl::VertexArray(0);
	mesh_vbo_pos = gl::ArrayBuffer(0);
	mesh_vbo_norm = gl::ArrayBuffer(0);
	mesh_ibo = gl::IndexBuffer(0);
}

void render::triangle_mesh_renderer::render(
		const core::tmesh_cptr mesh, 
		const glm::mat4& projection_view
	)
{
	gl::Bind(mesh_vbo_pos);
	mesh_vbo_pos.data(mesh->vertices(), gl::enums::BufferUsage::kStaticDraw);
	gl::Bind(mesh_vbo_norm);
	mesh_vbo_norm.data(mesh->normals(), gl::enums::BufferUsage::kStaticDraw);
	gl::Bind(mesh_ibo);
	mesh_ibo.data(mesh->faces(), gl::enums::BufferUsage::kStaticDraw);
	gl::Use(mesh_program);
	gl::LazyUniform<glm::mat4>(mesh_program, "vp") = projection_view;
	gl::Bind(mesh_vao);
	gl::Bind(mesh_ibo);
	gl::DrawElements(gl::PrimType::kTriangles, 3 * mesh->face_count(), gl::enums::IndexType::kUnsignedInt);
	gl::Unbind(mesh_vao);
	gl::Unbind(mesh_ibo);
	gl::Unuse(mesh_program);
}