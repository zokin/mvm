#include <input.h>
#include <load_obj.h>
#include <load_ply.h>

#include <plog\Log.h>

core::tmesh_cptr io::load_mesh(const std::tr2::sys::path& filename)
{
	if (filename.extension() == ".obj") {
		return load_obj(filename);
	}
	else if (filename.extension() == ".ply") {
		return load_ply(filename);
	}
	else {
		LOGE << "Cannot load file (" << filename.string()
			<< "), unsupported format (" << filename.extension().string() << ")";
		return core::triangle_mesh::construct().build();
	}
}
