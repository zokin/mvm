#include <simplify.h>
#include <meshoptimizer.h>

#include <plog\Log.h>

core::tmesh_cptr simplify::simplify_mesh(
	const core::tmesh_cptr& mesh,
	std::size_t target_index_count, 
	float target_error,
	bool sloppy
)
{	
	target_index_count = target_index_count // indices (not faces)
		? target_index_count 
		: std::size_t(mesh->face_count() * 3 * 0.2f);
	LOGI << "Simplifying mesh to " << (target_index_count / 3)
		<< " faces, from " << mesh->face_count();
	auto simplified = core::triangle_mesh::create_empty(mesh->vertex_count(), mesh->face_count());
	auto indices_size = sloppy
		? meshopt_simplifySloppy(
			simplified->face_ptr(),
			mesh->face_ptr(),
			mesh->faces_byte_size() / sizeof(uint32_t), // total indices (not faces)
			mesh->vertex_ptr(),
			mesh->vertex_count(), // total vertices (not vertex attribute values)
			3 * sizeof(float),  // vertex stride
			target_index_count // target indices (not faces)
		)
		: meshopt_simplify( // returns indices (not faces)
		simplified->face_ptr(),
		mesh->face_ptr(),
		mesh->faces_byte_size() / sizeof(uint32_t), // total indices (not faces)
		mesh->vertex_ptr(),
		mesh->vertex_count(), // total vertices (not vertex attribute values)
		3 * sizeof(float),  // vertex stride
		target_index_count, // target indices (not faces)
		target_error // as percentage of mesh span/bbox
	);

	auto vertex_size = meshopt_optimizeVertexFetch( // return vertices (not vertex attribute values)
		simplified->vertex_ptr(),
		simplified->face_ptr(),
		indices_size, // total new indices (not faces)		
		mesh->vertex_ptr(),
		mesh->vertex_count(), // total vertices (not vertex attribute values)
		3 * sizeof(float) // vertex stride
	);
	simplified->resize(vertex_size, indices_size / 3);

	//simplified->resize(mesh->vertex_count(), indices_size / 3);
	//std::copy(
	//	std::begin(mesh->vertices()), 
	//	std::end(mesh->vertices()), 
	//	std::begin(simplified->vertices())
	//);

	LOGI << "Simplified input mesh to "
		<< simplified->vertex_count() << " vertices, and "
		<< simplified->face_count() << " faces.";

	return simplified;
}