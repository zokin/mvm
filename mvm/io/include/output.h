#pragma once

#include <filesystem>

#include <image.h>
#include <triangle_mesh.h>

namespace io {
	void store_mesh(const std::tr2::sys::path& filename, 
		const core::tmesh_cptr& mesh, 
		const std::tr2::sys::path& texture_filename = "");

	void store_image(const std::tr2::sys::path& filename, 
		const core::image_cptr& image);

	void store_atlas(const std::tr2::sys::path& filename,
		const core::tmesh_cptr& mesh, uint32_t width, uint32_t height);

	void store_charts(const std::tr2::sys::path& filename,
		const core::tmesh_cptr& mesh, uint32_t width, uint32_t height);
} /// namespace io