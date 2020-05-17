#pragma once

#include <filesystem>

#include <triangle_mesh.h>
#include <viewpoint.h>

#include <map>
#include <string>

namespace io {
	core::tmesh_cptr load_mesh(const std::tr2::sys::path& filename);

	core::vp_cptr load_viewpoint(const std::tr2::sys::path& filename);

	std::map<std::string, std::map<std::string, core::viewpoint>> load_viewpoints(
		const std::tr2::sys::path& filename);

	core::tmesh_cptr densify(const core::tmesh_cptr& in);

} /// namespace io