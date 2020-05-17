#pragma once

#include <map>
#include <vector>
#include <memory>

namespace core {

	class triangle_mesh {
		std::map<std::string, std::vector<float>> attributes;		
		std::map<std::string, std::vector<uint32_t>> topology;

		static const std::string VerticesKey;
		static const std::string NormalsKey;
		static const std::string UVsKey;
		static const std::string FacesKey;

		bool has_attribute(const std::string& key) const;
		const std::vector<float>& get_attribute(const std::string& key) const;
		std::vector<float>& get_attribute(const std::string& key);
		bool has_topology(const std::string& key) const;
		const std::vector<uint32_t>& get_topology(const std::string& key) const;
		std::vector<uint32_t>& get_topology(const std::string& key);

		struct constructor_t { };
	public:
		using tmesh_ptr = std::shared_ptr<triangle_mesh>;
		using tmesh_cptr = std::shared_ptr<const triangle_mesh>;
		
		explicit triangle_mesh(constructor_t);
		explicit triangle_mesh(
			const std::size_t& vertices_count,
			const std::size_t& faces_count,
			constructor_t
		);

		class builder {
			tmesh_ptr mesh;

			builder();
			
			friend class triangle_mesh;
		public:
			builder with_geometry(
				std::vector<float> vertices,
				std::vector<uint32_t> faces
			);

			builder with_vertex_normals(
				std::vector<float> normals
			);

			builder with_vertex_uvs(
				std::vector<float> uvs
			);

			builder with_uv_faces(
				std::vector<uint32_t> uv_faces
			);

			tmesh_cptr build();
		};

		const std::vector<float>& vertices() const;
		std::vector<float>& vertices();
		const std::size_t vertex_count() const;
		const std::size_t vertices_byte_size() const;
		const float* vertex_ptr() const;
		float* vertex_ptr();

		bool has_normals() const;
		const std::vector<float>& normals() const;
		std::vector<float>& normals();
		const std::size_t normal_count() const;
		const std::size_t normals_byte_size() const;
		const float* normal_ptr() const;
		float* normal_ptr();

		bool has_uvs() const;
		const std::vector<float>& uvs() const;
		std::vector<float>& uvs();
		const std::size_t uv_count() const;
		const std::size_t uvs_byte_size() const;
		const float* uv_ptr() const;
		float* uv_ptr();

		const std::vector<uint32_t>& faces() const;
		std::vector<uint32_t>& faces();
		const std::size_t face_count() const;
		const std::size_t faces_byte_size() const;
		const uint32_t* face_ptr() const;
		uint32_t* face_ptr();

		const std::vector<uint32_t>& uv_faces() const;
		std::vector<uint32_t>& uv_faces();
		const std::size_t uv_face_count() const;
		const std::size_t uv_faces_byte_size() const;
		const uint32_t* uv_face_ptr() const;
		uint32_t* uv_face_ptr();
		bool has_uv_faces() const;

		bool empty() const;
		void resize(std::size_t vertex_count, std::size_t face_count);

		static builder construct();
		static tmesh_ptr create_null();
		static tmesh_ptr create_empty(
			const std::size_t& vertices_count,
			const std::size_t& faces_count
		);
	};

	using tmesh_cptr = triangle_mesh::tmesh_cptr;
	using tmesh_ptr = triangle_mesh::tmesh_ptr;
} /// namespace core