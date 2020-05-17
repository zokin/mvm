#include <normal.h>
#include <meshoptimizer.h>

#include <math.h>

std::vector<float> compute_average_vertex_normals(
		const core::tmesh_cptr& mesh,
		bool area_weighted
	)
{
	std::vector<float> normals(mesh->vertices().size());
	for (int i = 0; i < mesh->face_count(); ++i) {
		auto index = i * 3;
		uint32_t i1 = mesh->faces()[index];
		uint32_t i2 = mesh->faces()[index + 1];
		uint32_t i3 = mesh->faces()[index + 2];
		const float* v1 = &mesh->vertex_ptr()[i1 * 3];
		const float* v2 = &mesh->vertex_ptr()[i2 * 3];
		const float* v3 = &mesh->vertex_ptr()[i3 * 3];
		float a[3], b[3], c[3];
		a[0] = v2[0] - v1[0]; a[1] = v2[1] - v1[1]; a[2] = v2[2] - v1[2];
		b[0] = v3[0] - v1[0]; b[1] = v3[1] - v1[1]; b[2] = v3[2] - v1[2];
		c[0] = a[1] * b[2] - a[2] * b[1];
		c[1] = a[2] * b[0] - a[0] * b[2];
		c[2] = a[0] * b[1] - a[1] * b[0];
		if (area_weighted) {
			float c_m = c[0] * c[0] + c[1] * c[1] + c[2] * c[2];
			c_m = std::sqrt(c_m);
			c[0] /= c_m; c[1] /= c_m; c[2] /= c_m;
		}
		float* n1 = &normals[i1 * 3];
		float* n2 = &normals[i2 * 3];
		float* n3 = &normals[i3 * 3];
		n1[0] += c[0]; n1[1] += c[1]; n1[2] += c[2];
		n2[0] += c[0]; n2[1] += c[1]; n2[2] += c[2];
		n3[0] += c[0]; n3[1] += c[1]; n3[2] += c[2];
	}
	for (int i = 0; i < mesh->vertex_count(); ++i) {
		auto index = i * 3;
		float* n = &normals[index];
		float n_m = n[0] * n[0] + n[1] * n[1] + n[2] * n[2];
		n_m = std::sqrt(n_m);
		n[0] /= n_m; n[1] /= n_m; n[2] /= n_m;
	}
	return normals;
}

core::tmesh_cptr normal::normal_estimation(
		const core::tmesh_cptr& mesh,
		bool area_weighted
	)
{
	auto normals = compute_average_vertex_normals(mesh, area_weighted);
	auto builder = core::triangle_mesh::construct();
	builder
		.with_geometry(mesh->vertices(), mesh->faces())
		.with_vertex_normals(normals);
	if (mesh->has_uvs()) {
		builder.with_vertex_uvs(mesh->uvs());
	}
	if (mesh->has_uv_faces()) {
		builder.with_uv_faces(mesh->uv_faces());
	}
	return builder.build();
}