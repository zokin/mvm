#pragma once

#include <filesystem>

#include <triangle_mesh.h>

namespace io {
	core::tmesh_cptr load_obj(const std::tr2::sys::path& filename);
} /// na