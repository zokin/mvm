#pragma once

#include <filesystem>

#include <triangle_mesh.h>

namespace io {
	core::tmesh_cptr load_ply(const std::tr2::sys::path& filename, bool preload=true);
} /// na