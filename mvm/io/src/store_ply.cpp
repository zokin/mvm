#include <store_ply.h>

#include <fstream>

#include <plog\Log.h>

#include <tinyply.h>

void io::store_ply(const std::tr2::sys::path& filename, const core::tmesh_cptr& mesh)
{
	LOGI << "Writing result to " << filename.string();
	const static bool save_binary = false;
	std::filebuf fb;
	auto flags = save_binary ? std::ios::out | std::ios::binary : std::ios::out;	
	fb.open(filename, flags);
	std::ostream outstream(&fb);
	if (outstream.fail()) {
		LOGF << "Failed to open " << filename.string();
		throw std::runtime_error("failed to open " + filename.string());
	}

	tinyply::PlyFile plyfile;
	std::vector<float> vertices(mesh->vertices());
	plyfile.add_properties_to_element("vertex", { "x", "y", "z" },
		tinyply::Type::FLOAT32, 
		mesh->vertex_count(),
		reinterpret_cast<uint8_t*>(vertices.data()), 
		tinyply::Type::INVALID,
		0
	);
	if (mesh->has_normals()) {
		std::vector<float> normals(mesh->normals());
		plyfile.add_properties_to_element("vertex", { "nx", "ny", "nz" },
			tinyply::Type::FLOAT32,
			mesh->vertex_count(),
			reinterpret_cast<uint8_t*>(normals.data()),
			tinyply::Type::INVALID,
			0
		);
	}
	if (mesh->has_uvs()) {
		std::vector<float> uvs(mesh->uvs());
		plyfile.add_properties_to_element("vertex", { "u", "v" },
			tinyply::Type::FLOAT32,
			mesh->vertex_count(),
			reinterpret_cast<uint8_t*>(uvs.data()),
			tinyply::Type::INVALID,
			0
		);
	}
	std::vector<uint32_t> faces(mesh->faces());
	plyfile.add_properties_to_element("face", { "vertex_indices" },
		tinyply::Type::UINT32,
		mesh->face_count(),
		reinterpret_cast<uint8_t*>(faces.data()),
		tinyply::Type::UINT8,
		3
	);
	if (mesh->has_uv_faces()) {
		LOGW << "Ply saving does not currently support saving separate UV faces.";
		LOGW << "Consider switching to Wavefront (*.obj) format.";
	}
	plyfile.get_comments().push_back("Saved by multiview mesh tool");
	plyfile.write(outstream, save_binary);
}