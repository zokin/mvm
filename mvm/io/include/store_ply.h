#pragma once

#include <triangle_mesh.h>

#include <filesystem>

namespace io {
	void store_ply(const std::tr2::sys::path& filename, const core::tmesh_cptr& mesh);
} /// namespace io