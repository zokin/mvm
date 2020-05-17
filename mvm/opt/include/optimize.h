#pragma once

#include <triangle_mesh.h>

namespace optimize {
	core::tmesh_cptr optimize_mesh(
		const core::tmesh_cptr& mesh
	);

	core::tmesh_cptr optimize_mesh_deinterleaved(
		const core::tmesh_cptr& mesh
	);
}