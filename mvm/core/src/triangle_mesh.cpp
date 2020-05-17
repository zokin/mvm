#include <triangle_mesh.h>

#pragma region Construction
core::triangle_mesh::builder core::triangle_mesh::construct()
{
	return core::triangle_mesh::builder();
}

core::triangle_mesh::triangle_mesh(constructor_t)
{
}

core::triangle_mesh::triangle_mesh(
	const std::size_t& vertices_count,
	const std::size_t& faces_count,
	constructor_t
)
{
	attributes[VerticesKey].resize(vertices_count * 3);
	topology[VerticesKey].resize(faces_count * 3);
}

core::triangle_mesh::tmesh_ptr core::triangle_mesh::create_null()
{
	return std::make_shared<core::triangle_mesh>(constructor_t());
}

core::triangle_mesh::tmesh_ptr core::triangle_mesh::create_empty(
	const std::size_t& vertices_count,
	const std::size_t& faces_count
)
{
	return std::make_shared<triangle_mesh>(vertices_count, faces_count, constructor_t());
}

void core::triangle_mesh::resize(std::size_t vertex_count, std::size_t face_count)
{
	get_attribute(VerticesKey).resize(vertex_count * 3);
	get_topology(VerticesKey).resize(face_count * 3);
}
#pragma endregion

#pragma region Attributes
const std::vector<float>& core::triangle_mesh::get_attribute(const std::string& key) const
{
	return attributes.at(key);
}

std::vector<float>& core::triangle_mesh::get_attribute(const std::string& key)
{
	return attributes.at(key);
}

bool core::triangle_mesh::has_attribute(const std::string& key) const
{
	return attributes.find(key) != attributes.end();
}

bool core::triangle_mesh::empty() const
{
	auto it = attributes.find(VerticesKey);
	return it == attributes.end() || it->second.size() == 0;
}

const std::vector<float>& core::triangle_mesh::vertices() const
{
	return get_attribute(VerticesKey);
}

std::vector<float>& core::triangle_mesh::vertices()
{
	return get_attribute(VerticesKey);
}

const std::size_t core::triangle_mesh::vertex_count() const
{
	return get_attribute(VerticesKey).size() / 3;
}

const std::size_t core::triangle_mesh::vertices_byte_size() const
{
	return get_attribute(VerticesKey).size() * sizeof(float);
}

const float* core::triangle_mesh::vertex_ptr() const
{
	return get_attribute(VerticesKey).data();
}

float* core::triangle_mesh::vertex_ptr()
{
	return get_attribute(VerticesKey).data();
}

bool core::triangle_mesh::has_normals() const
{
	return has_attribute(NormalsKey);
}

const std::vector<float>& core::triangle_mesh::normals() const
{
	return get_attribute(NormalsKey);
}

std::vector<float>& core::triangle_mesh::normals()
{
	return get_attribute(NormalsKey);
}

const std::size_t core::triangle_mesh::normal_count() const
{
	return get_attribute(NormalsKey).size() / 3;
}

const std::size_t core::triangle_mesh::normals_byte_size() const
{
	return get_attribute(NormalsKey).size() * sizeof(float);
}

const float* core::triangle_mesh::normal_ptr() const
{
	return get_attribute(NormalsKey).data();
}

float* core::triangle_mesh::normal_ptr()
{
	return get_attribute(NormalsKey).data();
}

bool core::triangle_mesh::has_uvs() const
{
	return has_attribute(UVsKey);
}

const std::vector<float>& core::triangle_mesh::uvs() const
{
	return get_attribute(UVsKey);
}

std::vector<float>& core::triangle_mesh::uvs()
{
	return get_attribute(UVsKey);
}

const std::size_t core::triangle_mesh::uv_count() const
{
	return get_attribute(UVsKey).size() / 2;
}

const std::size_t core::triangle_mesh::uvs_byte_size() const
{
	return get_attribute(UVsKey).size() * sizeof(float);
}

const float* core::triangle_mesh::uv_ptr() const
{
	return get_attribute(UVsKey).data();
}

float* core::triangle_mesh::uv_ptr()
{
	return get_attribute(UVsKey).data();
}
#pragma endregion

#pragma region Topology
const std::vector<uint32_t>& core::triangle_mesh::get_topology(const std::string& key) const
{
	return topology.at(key);
}

std::vector<uint32_t>& core::triangle_mesh::get_topology(const std::string& key)
{
	return topology.at(key);
}

bool core::triangle_mesh::has_topology(const std::string& key) const
{
	return topology.find(key) != topology.end();
}

const std::vector<uint32_t>& core::triangle_mesh::faces() const
{
	return get_topology(VerticesKey);
}

std::vector<uint32_t>& core::triangle_mesh::faces()
{
	return get_topology(VerticesKey);
}

const std::size_t core::triangle_mesh::face_count() const
{
	return get_topology(VerticesKey).size() / 3;
}

const std::size_t core::triangle_mesh::faces_byte_size() const
{
	return get_topology(VerticesKey).size() * sizeof(uint32_t);
}

const uint32_t* core::triangle_mesh::face_ptr() const
{
	return get_topology(VerticesKey).data();
}

uint32_t* core::triangle_mesh::face_ptr()
{
	return get_topology(VerticesKey).data();
}

const std::vector<uint32_t>& core::triangle_mesh::uv_faces() const
{
	return get_topology(UVsKey);
}

std::vector<uint32_t>& core::triangle_mesh::uv_faces()
{
	return get_topology(UVsKey);
}

const std::size_t core::triangle_mesh::uv_face_count() const
{
	return get_topology(UVsKey).size() / 3;
}

const std::size_t core::triangle_mesh::uv_faces_byte_size() const
{
	return get_topology(UVsKey).size() * sizeof(uint32_t);
}

const uint32_t* core::triangle_mesh::uv_face_ptr() const
{
	return get_topology(UVsKey).data();
}

uint32_t* core::triangle_mesh::uv_face_ptr()
{
	return get_topology(UVsKey).data();
}

bool core::triangle_mesh::has_uv_faces() const
{
	return has_topology(UVsKey);
}
#pragma endregion

#pragma region Builder
core::triangle_mesh::builder::builder()	
{
	mesh = core::triangle_mesh::create_null();
}

core::triangle_mesh::builder core::triangle_mesh::builder::with_geometry(
	std::vector<float> vertices,
	std::vector<uint32_t> faces
)
{
	mesh->attributes[VerticesKey] = vertices;
	mesh->topology[VerticesKey] = faces;
	return *this;
}

core::triangle_mesh::builder core::triangle_mesh::builder::with_vertex_normals(
	std::vector<float> normals
)
{
	mesh->attributes[NormalsKey] = normals;
	return *this;
}

core::triangle_mesh::builder core::triangle_mesh::builder::with_vertex_uvs(
	std::vector<float> uvs
)
{
	mesh->attributes[UVsKey] = uvs;
	return *this;
}

core::triangle_mesh::builder core::triangle_mesh::builder::with_uv_faces(
	std::vector<uint32_t> uv_faces
)
{
	mesh->topology[UVsKey] = uv_faces;
	return *this;
}

core::triangle_mesh::tmesh_cptr core::triangle_mesh::builder::build()
{
	return mesh;
}
#pragma endregion

const std::string core::triangle_mesh::VerticesKey = "vertices";
const std::string core::triangle_mesh::NormalsKey = "normals";
const std::string core::triangle_mesh::UVsKey = "uvs";
const std::string core::triangle_mesh::FacesKey = "faces";