#pragma once

#include <triangle_mesh.h>

#include <filesystem>

namespace io {
	void store_obj(const std::tr2::sys::path& filename, 
		const core::tmesh_cptr& mesh,
		const std::tr2::sys::path& texture_filename = "");
} /// namespace io