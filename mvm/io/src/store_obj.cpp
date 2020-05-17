#include <store_obj.h>

#include <fstream>

#include <plog\Log.h>

void io::store_obj(
	const std::tr2::sys::path& filename, 
	const core::tmesh_cptr& mesh,
	const std::tr2::sys::path& texture_filename
)
{
	LOGI << "Writing result to " << filename.string();
	std::ofstream out(filename.string());
	out << "# Saved by multiview mesh tool" << std::endl;	
	if (!texture_filename.empty()) {		
		out << "mtllib " << filename.stem().string() + ".mtl" << std::endl;		
		out << "usemtl " << texture_filename.stem() << std::endl;
		LOGW << (filename.parent_path() / filename.stem()).string() + ".mtl";
		std::ofstream mtl((filename.parent_path() / filename.stem()).string() + ".mtl");
		mtl << "newmtl " << texture_filename.stem() << std::endl
			<< "Ka 0.200000 0.200000 0.200000" << std::endl
			<< "Kd 0.749020 0.749020 0.749020" << std::endl
			<< "Ks 1.000000 1.000000 1.000000" << std::endl
			<< "illum 2" << std::endl
			<< "Ns 0.000000" << std::endl
			<< "map_Kd " << texture_filename << std::endl;
		mtl.close();
	}
	for (int i = 0; i < mesh->vertex_count(); ++i)
	{
		auto ptr = mesh->vertex_ptr() + (i * 3);
		out << "v " 
			<< ptr[0] << " "
			<< ptr[1] << " "
			<< ptr[2] << "\n";
	}
	if (mesh->has_normals()) {
		for (int i = 0; i < mesh->normal_count(); ++i)
		{
			auto ptr = mesh->normal_ptr() + (i * 3);
			out << "vn "
				<< ptr[0] << " "
				<< ptr[1] << " "
				<< ptr[2] << "\n";
		}
	}
	if (mesh->has_uvs()) {
		for (int i = 0; i < mesh->uv_count(); ++i)
		{
			auto ptr = mesh->uv_ptr() + (i * 2);
			out << "vt "
				<< ptr[0] << " "
				<< ptr[1] << "\n";
		}
	}
	auto has_uvs = mesh->has_uvs();
	auto has_normals = mesh->has_normals();
	for (int i = 0; i < mesh->face_count(); ++i)
	{
		auto v_ptr = mesh->face_ptr() + (i * 3);
		auto t_ptr = mesh->has_uv_faces() ? mesh->uv_face_ptr() + (i * 3) : v_ptr;
		auto v_i1 = v_ptr[0] + 1;
		auto v_i2 = v_ptr[1] + 1;
		auto v_i3 = v_ptr[2] + 1;
		auto t_i1 = t_ptr[0] + 1;
		auto t_i2 = t_ptr[1] + 1;
		auto t_i3 = t_ptr[2] + 1;
		if (has_uvs && has_normals) {
			out << "f "
				<< v_i1 << "/" << t_i1 << "/" << v_i1 << " "
				<< v_i2 << "/" << t_i2 << "/" << v_i2 << " "
				<< v_i3 << "/" << t_i3 << "/" << v_i3 << "\n";
		}
		if (has_uvs && !has_normals) {
			out << "f "
				<< v_i1 << "/" << t_i1 << " "
				<< v_i2 << "/" << t_i2 << " "
				<< v_i3 << "/" << t_i3 << "\n";
		}
		if (!has_uvs && has_normals) {
			out << "f "
				<< v_i1 << "//" << v_i1 << " "
				<< v_i2 << "//" << v_i2 << " "
				<< v_i3 << "//" << v_i3 << "\n";
		}
		if (!has_uvs && !has_normals) {
			out << "f "
				<< v_i1 << " "
				<< v_i2 << " "
				<< v_i3 << "\n";
		}
	}
}