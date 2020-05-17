#include <load_ply.h>
#include <tinyply.h>

#include <fstream>

#include <plog\Log.h>

struct memory_buffer : public std::streambuf
{
	char * p_start{ nullptr };
	char * p_end{ nullptr };
	size_t size;

	memory_buffer(char const * first_elem, size_t size)
		: p_start(const_cast<char*>(first_elem)), p_end(p_start + size), size(size)
	{
		setg(p_start, p_start, p_end);
	}

	pos_type seekoff(off_type off, std::ios_base::seekdir dir, std::ios_base::openmode which) override
	{
		if (dir == std::ios_base::cur) gbump(static_cast<int>(off));
		else setg(p_start, (dir == std::ios_base::beg ? p_start : p_end) + off, p_end);
		return gptr() - p_start;
	}

	pos_type seekpos(pos_type pos, std::ios_base::openmode which) override
	{
		return seekoff(pos, std::ios_base::beg, which);
	}
};

struct memory_stream : virtual memory_buffer, public std::istream
{
	memory_stream(char const * first_elem, size_t size)
		: memory_buffer(first_elem, size), std::istream(static_cast<std::streambuf*>(this)) {}
};

inline std::vector<uint8_t> read_file_binary(const std::string& filename)
{
	std::ifstream file(filename, std::ios::binary);
	std::vector<uint8_t> fileBufferBytes;
	if (file.is_open())
	{
		file.seekg(0, std::ios::end);
		size_t sizeBytes = file.tellg();
		file.seekg(0, std::ios::beg);
		fileBufferBytes.resize(sizeBytes);
		if (file.read((char*)fileBufferBytes.data(), sizeBytes)) return fileBufferBytes;
	}
	else {
		LOGF << "Cloud not open file: " << filename;
		throw std::runtime_error("Could not open file: " + filename);
	}
	return fileBufferBytes;
}

core::tmesh_cptr io::load_ply(const std::tr2::sys::path& filename, bool preload)
{
	std::unique_ptr<std::istream> file_stream;
	std::vector<uint8_t> byte_buffer;
	auto builder = core::triangle_mesh::construct();
	try
	{
		// For most files < 1gb, pre-loading the entire file upfront and wrapping it into a 
		// stream is a net win for parsing speed, about 40% faster. 
		if (preload)
		{
			byte_buffer = read_file_binary(filename.string());
			file_stream.reset(new memory_stream((char*)byte_buffer.data(), byte_buffer.size()));
		}
		else
		{
			file_stream.reset(new std::ifstream(filename.string(), std::ios::binary));
		}

		if (!file_stream || file_stream->fail()) {
			LOGF << "Cloud not open file: " << filename.string();
			throw std::runtime_error("Could not open file: " + filename.string());
		}

		tinyply::PlyFile file;
		file.parse_header(*file_stream);

		LOGV << "[ply_header] Type: " << (file.is_binary_file() ? "binary" : "ascii");
		for (const auto & c : file.get_comments()) {
			LOGV << "\t[ply_header] Comment: " << c;
		}
		for (const auto & c : file.get_info()) {
			LOGV << "\t[ply_header] Info: " << c;
		}

		for (const auto & e : file.get_elements())
		{
			LOGV << "\t[ply_header] element: " << e.name << " (" << e.size << ")";
			for (const auto & p : e.properties)
			{
				LOGV << "\t[ply_header] \tproperty: " << p.name << " (type=" << tinyply::PropertyTable[p.propertyType].str << ")";
				if (p.isList) LOGV << " (list_type=" << tinyply::PropertyTable[p.listType].str << ")";				
			}
		}

		// Because most people have their own mesh types, tinyply treats parsed data as structured/typed byte buffers. 
		// See examples below on how to marry your own application-specific data structures with this one. 
		std::shared_ptr<tinyply::PlyData> vertices, normals, colors, texcoords, faces, tripstrip;

		// The header information can be used to programmatically extract properties on elements
		// known to exist in the header prior to reading the data. For brevity of this sample, properties 
		// like vertex position are hard-coded: 
		try { vertices = file.request_properties_from_element("vertex", { "x", "y", "z" }); }
		catch (const std::exception & e) { LOGW << "tinyply exception: " << e.what(); }

		try { normals = file.request_properties_from_element("vertex", { "nx", "ny", "nz" }); }
		catch (const std::exception & e) { LOGW << "tinyply exception: " << e.what(); }

		//try { colors = file.request_properties_from_element("vertex", { "red", "green", "blue", "alpha" }); }
		//catch (const std::exception & e) { LOGE << "tinyply exception: " << e.what(); }

		//try { colors = file.request_properties_from_element("vertex", { "r", "g", "b", "a" }); }
		//catch (const std::exception & e) { LOGE << "tinyply exception: " << e.what(); }

		try { texcoords = file.request_properties_from_element("vertex", { "u", "v" }); }
		catch (const std::exception & e) { LOGW << "tinyply exception: " << e.what(); }

		// Providing a list size hint (the last argument) is a 2x performance improvement. If you have 
		// arbitrary ply files, it is best to leave this 0. 
		try { faces = file.request_properties_from_element("face", { "vertex_indices" }, 3); }
		catch (const std::exception & e) { LOGW << "tinyply exception: " << e.what(); }

		// Tristrips must always be read with a 0 list size hint (unless you know exactly how many elements
		// are specifically in the file, which is unlikely); 
		//try { tripstrip = file.request_properties_from_element("tristrips", { "vertex_indices" }, 0); }
		//catch (const std::exception & e) { LOGE << "tinyply exception: " << e.what(); }

		file.read(*file_stream);

		if (vertices)   LOGV << "\tRead " << vertices->count << " total vertices ";
		if (normals)    LOGV << "\tRead " << normals->count << " total vertex normals ";
		if (colors)     LOGV << "\tRead " << colors->count << " total vertex colors ";
		if (texcoords)  LOGV << "\tRead " << texcoords->count << " total vertex texcoords ";
		if (faces)      LOGV << "\tRead " << faces->count << " total faces (triangles) ";
		if (tripstrip)  LOGV << "\tRead " << (tripstrip->buffer.size_bytes() / tinyply::PropertyTable[tripstrip->t].stride) << " total indicies (tristrip) ";		

		{
			const size_t numVerticesBytes = vertices->buffer.size_bytes();
			std::vector<float> verts(vertices->count * 3);
			std::memcpy(verts.data(), vertices->buffer.get(), numVerticesBytes);
			const size_t numIndicesBytes = faces->buffer.size_bytes();
			std::vector<uint32_t> indices(faces->count * 3);
			std::memcpy(indices.data(), faces->buffer.get(), numIndicesBytes);

			builder.with_geometry(verts, indices);
			if (normals) {
				const size_t numNormalsBytes = normals->buffer.size_bytes();
				std::vector<float> norms(normals->count * 3);
				std::memcpy(norms.data(), normals->buffer.get(), numNormalsBytes);
				builder.with_vertex_normals(norms);
			}
			if (texcoords) {
				const size_t numUVsBytes = texcoords->buffer.size_bytes();
				std::vector<float> uvs(texcoords->count * 2);
				std::memcpy(uvs.data(), texcoords->buffer.get(), numUVsBytes);
				builder.with_vertex_uvs(uvs);
			}
		}
	}
	catch (const std::exception & e)
	{
		LOGE << "Caught tinyply exception: " << e.what() << std::endl;
	}
	return builder.build();
}
