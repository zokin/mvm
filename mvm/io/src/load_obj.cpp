#include <load_obj.h>
#include <tinyobj.h>

#include <iterator>

#include <plog\Log.h>

core::tmesh_cptr io::load_obj(const std::tr2::sys::path& filename)
{
	tinyobj::attrib_t attr;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warnings, errors;

	LOGE_IF(!tinyobj::LoadObj(
		&attr,
		&shapes,
		&materials,
		&warnings,
		&errors,
		filename.string().c_str(),
		filename.parent_path().string().c_str()
	))	<< errors;
	LOGW_IF(warnings.empty()) << warnings;
	LOGW_IF(shapes.size() > 1) << "Loaded file has more than one shapes. Using the first one.";
	auto& shape = shapes.front();
	LOGI << "Loaded mesh with "
		<< attr.vertices.size() / 3	
		<< " vertices, "
		<< attr.normals.size() / 3
		<< " normals, "
		<< attr.texcoords.size() / 2
		<< " uvs, "
		<< shape.mesh.indices.size() / 3
		<< " faces, "
		<< materials.size()
		<< " materials.";

	std::vector<uint32_t> vertex_indices;
	vertex_indices.reserve(shape.mesh.indices.size() / 3);
	std::transform(std::begin(shape.mesh.indices), std::end(shape.mesh.indices),
		std::back_inserter(vertex_indices),
		[](auto i) { return static_cast<uint32_t>(i.vertex_index); }
	);
	auto builder = core::triangle_mesh::construct();
	builder.with_geometry(attr.vertices, vertex_indices);
	if (!attr.normals.empty()) {
		builder.with_vertex_normals(attr.normals);
	}
	if (!attr.texcoords.empty()) {
		builder.with_vertex_uvs(attr.texcoords);
		std::vector<uint32_t> uv_indices;
		std::transform(std::begin(shape.mesh.indices), std::end(shape.mesh.indices),
			std::back_inserter(uv_indices),
			[](auto i) { return static_cast<uint32_t>(i.texcoord_index); }
		);
		builder.with_uv_faces(uv_indices);
	}
	
	return builder.build();
}
