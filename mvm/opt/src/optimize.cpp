#include <optimize.h>
#include <meshoptimizer.h>

#include <plog\Log.h>

core::tmesh_cptr optimize::optimize_mesh(
		const core::tmesh_cptr& mesh
	)
{
	LOGI << "Optimizing mesh for rendering.";
	auto optimized = core::triangle_mesh::create_empty(mesh->vertex_count(), mesh->face_count());	
	/*	Vertex Remapping / Indexing */
	std::vector<uint32_t> remap(mesh->face_count() * 3);
	size_t vertex_count = meshopt_generateVertexRemap(
		remap.data(),
		mesh->face_ptr(),//NULL, 
		remap.size(), 
		mesh->vertex_ptr(),
		remap.size(),//mesh->vertex_count() produced spurious faces		
		3* sizeof(float)
	);
	meshopt_remapIndexBuffer(
		optimized->face_ptr(), 
		mesh->face_ptr(), 
		remap.size(), 
		remap.data()
	);
	meshopt_remapVertexBuffer(
		optimized->vertex_ptr(), 
		mesh->vertex_ptr(),		
		remap.size(),//vertex_count produced spurious faces
		3 * sizeof(float),
		remap.data()
	);
	optimized->resize(vertex_count, remap.size() / 3);
	/*	Vertex Cache Optimization */
	meshopt_optimizeVertexCache(
		optimized->face_ptr(), 
		optimized->face_ptr(),
		optimized->face_count() * 3,
		optimized->vertex_count()
	);
	/*	Overdraw Optimization */
	meshopt_optimizeOverdraw(
		optimized->face_ptr(),
		optimized->face_ptr(),
		optimized->face_count() * 3,
		optimized->vertex_ptr(),
		optimized->vertex_count(), 
		3 * sizeof(float), 
		1.05f
	);
	/*	Vertex Fetch Optimization */
	auto updated_vertex_count = meshopt_optimizeVertexFetch(
		optimized->vertex_ptr(),
		optimized->face_ptr(),
		optimized->face_count() * 3,
		optimized->vertex_ptr(),
		optimized->vertex_count(),
		3 * sizeof(float)
	);
	LOGW_IF(updated_vertex_count != vertex_count)
		<< "Non-unique vertex count (" << updated_vertex_count << ")"
		<< " compared to the original vertex count (" << vertex_count << ")";
	return optimized;
}

core::tmesh_cptr optimize::optimize_mesh_deinterleaved(
	const core::tmesh_cptr& mesh
	)
{
	LOGI << "Optimizing deinterleaved mesh for rendering.";
	/*	Vertex Remapping / Indexing */
	std::vector<meshopt_Stream> streams;
	streams.emplace_back(
		meshopt_Stream{ mesh->vertex_ptr(), sizeof(float) * 3, sizeof(float) * 3 }
	);
	if (mesh->has_normals()) {
		streams.emplace_back(
			meshopt_Stream{ mesh->normal_ptr(), sizeof(float) * 3, sizeof(float) * 3 }
		);
	}
	if (mesh->has_uvs()) {
		streams.emplace_back(
			meshopt_Stream{ mesh->uv_ptr(), sizeof(float) * 2, sizeof(float) * 2 }
		);
	}
	std::vector<uint32_t> remap(mesh->face_count() * 3);
	size_t vertex_count = meshopt_generateVertexRemapMulti(
		remap.data(),
		mesh->face_ptr(),//NULL, 
		remap.size(),
		remap.size(),//mesh->vertex_count() produced spurious faces		
		streams.data(),
		streams.size()
	);
	std::vector<uint32_t> optimized_indices(mesh->face_count() * 3);
	meshopt_remapIndexBuffer(
		optimized_indices.data(),
		mesh->face_ptr(),
		remap.size(),
		remap.data()
	);
	std::vector<float> vertices(mesh->vertex_count() * 3);
	meshopt_remapVertexBuffer(
		vertices.data(),
		mesh->vertex_ptr(),
		remap.size(),//vertex_count produced spurious faces
		//vertex_count,
		3 * sizeof(float),
		remap.data()
	);
	vertices.resize(vertex_count * 3);
	std::vector<float> normals, uvs;
	if (mesh->has_normals()) {
		normals.resize(mesh->normal_count() * 3);
		meshopt_remapVertexBuffer(
			normals.data(),
			mesh->normal_ptr(),
			remap.size(),//vertex_count produced spurious faces
			//vertex_count,
			3 * sizeof(float),
			remap.data()
		);
		normals.resize(vertex_count * 3);
	}
	if (mesh->has_uvs()) {
		uvs.resize(mesh->uv_count() * 2);
		meshopt_remapVertexBuffer(
			uvs.data(),
			mesh->uv_ptr(),
			remap.size(),//vertex_count produced spurious faces
			//vertex_count,
			2 * sizeof(float),
			remap.data()
		);
		uvs.resize(vertex_count * 2);
	}
	/*	Vertex Cache Optimization */
	meshopt_optimizeVertexCache(
		optimized_indices.data(),
		optimized_indices.data(),
		optimized_indices.size(),
		vertex_count
	);
	/*	Overdraw Optimization */
	meshopt_optimizeOverdraw(
		optimized_indices.data(),
		optimized_indices.data(),
		optimized_indices.size(),
		vertices.data(),
		vertex_count,
		3 * sizeof(float),
		1.05f
	);
	/*	Vertex Fetch Optimization */
	auto updated_vertex_count = meshopt_optimizeVertexFetchRemap(
		//optimized_indices.data(),
		remap.data(),
		optimized_indices.data(),
		optimized_indices.size(),
		vertex_count
	);
	meshopt_remapVertexBuffer(
		vertices.data(),
		vertices.data(),
		//remap.size(),//vertex_count produced spurious faces
		vertex_count,
		3 * sizeof(float),
		remap.data()
	);
	meshopt_remapIndexBuffer(
		optimized_indices.data(),
		optimized_indices.data(),
		remap.size(),
		remap.data()
	);
	if (mesh->has_normals()) {
		meshopt_remapVertexBuffer(
			normals.data(),
			normals.data(),
			//remap.size(),//vertex_count produced spurious faces
			vertex_count,
			3 * sizeof(float),
			remap.data()
		);
	}
	if (mesh->has_uvs()) {
		meshopt_remapVertexBuffer(
			uvs.data(),
			uvs.data(),
			//remap.size(),//vertex_count produced spurious faces
			vertex_count,
			2 * sizeof(float),
			remap.data()
		);
	}
	LOGW_IF(updated_vertex_count != vertex_count)
		<< "Non-unique vertex count (" << updated_vertex_count << ")"
		<< " compared to the original vertex count (" << vertex_count << ")";
	auto builder = core::triangle_mesh::construct()
		.with_geometry(vertices, optimized_indices);
	if (mesh->has_normals()) {
		builder = builder.with_vertex_normals(normals);
	}
	if (mesh->has_uvs()) {
		builder = builder.with_vertex_uvs(uvs);
	}
	return builder.build();
}