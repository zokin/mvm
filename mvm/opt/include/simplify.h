#pragma once

#include <triangle_mesh.h>

namespace simplify {
	core::tmesh_cptr simplify_mesh(
		const core::tmesh_cptr& mesh,
		std::size_t target_index_count = 0,
		float target_error = 1e-2,
		bool sloppy = false
	);
}