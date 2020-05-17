#include <input.h>

#include <plog\Log.h>

core::tmesh_cptr io::densify(const core::tmesh_cptr& in)
{
	auto builder = core::triangle_mesh::construct();
	std::vector<float> vertices, normals, uvs;
	std::vector<uint32_t> faces, uv_faces;
	if (!in->has_uv_faces()) {
		LOGE << "Cannot densify mesh without UV faces.";
	}
	else if (in->face_count() != in->uv_face_count()) {
		LOGE << "Cannot densify mesh with unequal vertex and UV faces.";
	}
	else {
		auto count = in->face_count() * 3;
		vertices.reserve(count);
		normals.reserve(count);
		uvs.reserve(count);
		faces.reserve(count);
		uv_faces.reserve(count);
		for (int f = 0; f < count; ++f) {
			faces.emplace_back(f);
			uv_faces.emplace_back(f);
			auto geometry_index = in->faces()[f] * 3;
			auto uv_index = in->uv_faces()[f] * 2;
			vertices.emplace_back(in->vertices()[geometry_index]);
			vertices.emplace_back(in->vertices()[geometry_index + 1]);
			vertices.emplace_back(in->vertices()[geometry_index + 2]);
			normals.emplace_back(in->normals()[geometry_index]);
			normals.emplace_back(in->normals()[geometry_index + 1]);
			normals.emplace_back(in->normals()[geometry_index + 2]);
			uvs.emplace_back(in->uvs()[uv_index]);
			uvs.emplace_back(in->uvs()[uv_index + 1]);
		}
		builder
			.with_geometry(vertices, faces)
			.with_vertex_normals(normals)
			.with_vertex_uvs(uvs)
			.with_uv_faces(uv_faces);
	}
	return builder.build();
}