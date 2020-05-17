#include <output.h>
#include <store_obj.h>
#include <store_ply.h>

#include <plog\Log.h>

void io::store_mesh(const std::tr2::sys::path& filename, 
	const core::tmesh_cptr& mesh, const std::tr2::sys::path& texture_filename)
{
	if (filename.extension() == ".obj") {
		return store_obj(filename, mesh, texture_filename);
	}
	else if (filename.extension() == ".ply") {
		return store_ply(filename, mesh);
	}
	else {
		LOGW << "Unsupported extension (" << filename.extension().string()
			<< "), file saving aborted.";
	}
}
