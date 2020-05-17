#include <atlas.h>
#include <xatlas.h>

#include <plog\Log.h>

static int plog_print(const char *format, ...)
{
	//LOGI << format;
	return 0;
}

static bool plog_progress(xatlas::ProgressCategory::Enum category, int progress, void *userData)
{
	std::string status;
	if (progress == 0) {
		status = "started";
	}
	else if (progress == 100) {
		status = "completed";
	}
	else {
		return true;
	}
	switch (category) {
	case xatlas::ProgressCategory::AddMesh:
		LOGI << "Atlas initialization " << status;
		break;
	case xatlas::ProgressCategory::ComputeCharts:
		LOGI << "Chart computation " << status;
		break;
	case xatlas::ProgressCategory::ParameterizeCharts:
		LOGI << "Chart parameterisation " << status;
		break;
	case xatlas::ProgressCategory::PackCharts:
		LOGI << "Chart packing " << status;
		break;
	case xatlas::ProgressCategory::BuildOutputMeshes:
		LOGI << "Atlas finalization " << status;
		break;
	}
	return true;
}

std::tuple<core::tmesh_cptr, core::image_cptr> atlas::create(
		const core::tmesh_cptr& mesh,
		bool pack_block_align,
		bool pack_brute_force,
		float pack_texels_per_unit,
		uint32_t pack_resolution,
		float chart_normal_weight,
		float chart_roundness_weight,
		float chart_straightness_weight,
		float chart_normal_seam_weight,
		float chart_texture_seam_weight,
		float chart_max_cost,
		uint32_t chart_max_iterations,
		bool create_image,
		bool verbose
	)
{
	xatlas::SetPrint(plog_print, verbose);
	xatlas::Atlas *atlas = xatlas::Create();
	xatlas::SetProgressCallback(atlas, plog_progress);
	
	xatlas::MeshDecl meshDecl;
	meshDecl.vertexCount = (uint32_t)mesh->vertex_count();
	meshDecl.vertexPositionData = mesh->vertex_ptr();
	meshDecl.vertexPositionStride = sizeof(float) * 3;
	meshDecl.indexCount = mesh->face_count() * 3;
	meshDecl.indexData = mesh->face_ptr();
	meshDecl.indexFormat = xatlas::IndexFormat::UInt32;

	xatlas::AddMeshError::Enum error = xatlas::AddMesh(atlas, meshDecl, 1);
	if (error != xatlas::AddMeshError::Success) {
		xatlas::Destroy(atlas);
		LOGF << "Error adding mesh (" << xatlas::StringForEnum(error) << ")";
		return std::make_tuple(mesh, std::make_shared<core::img>());
	}
	xatlas::AddMeshJoin(atlas); // Not necessary. Only called here so geometry totals are printed after the AddMesh progress indicator.
	LOGI << "Generating atlas.";
	xatlas::PackOptions pack_options;
	pack_options.bilinear = true;
	pack_options.blockAlign = pack_block_align;
	pack_options.bruteForce = pack_brute_force;
	pack_options.createImage = create_image;
	pack_options.maxChartSize = 0;
	pack_options.padding = 0;
	pack_options.texelsPerUnit = pack_texels_per_unit;
	pack_options.resolution = pack_resolution;
	xatlas::ChartOptions chart_options;
	chart_options.maxChartArea = 0.0f;
	chart_options.maxBoundaryLength = 0.0f;
	chart_options.normalDeviationWeight = chart_normal_weight;
	chart_options.roundnessWeight = chart_roundness_weight;
	chart_options.straightnessWeight = chart_straightness_weight;
	chart_options.normalSeamWeight = chart_normal_seam_weight;
	chart_options.textureSeamWeight = chart_texture_seam_weight;
	chart_options.maxCost = chart_max_cost;
	chart_options.maxIterations = chart_max_iterations;
	xatlas::Generate(atlas, chart_options, xatlas::ParameterizeOptions(), pack_options);
	LOGI << "Created " << atlas->chartCount << " charts and " << atlas->atlasCount << " atlases.";
	for (uint32_t i = 0; i < atlas->atlasCount; i++) {
		LOGI << "Atlas {" << i << "}: " << atlas->utilization[i] * 100.0f << " utilization";
	}
	LOGI << "Atlas resolution: " << atlas->width << " x " << atlas->height;
	uint32_t totalVertices = atlas->meshes[0].vertexCount;
	uint32_t totalFaces = atlas->meshes[0].indexCount / 3;	
	LOGV << "UV mapped mesh total vertices: " << totalVertices << " , triangles: " <<  totalFaces;
	if (!totalVertices && !totalFaces) {
		LOGE << "Could not create an atlas.";
		return std::make_tuple(mesh, std::make_shared<core::img>());
	}
	/*std::vector<float> vertices(3 * totalVertices);
	std::vector<float> uvs(2 * totalVertices);
	std::vector<uint32_t> faces(3 * totalFaces);
	auto uv_vertex_ptr = atlas->meshes[0].vertexArray;
	auto index_ptr = atlas->meshes[0].indexArray;
	auto original_vertex_ptr = mesh->vertex_ptr();	
	auto width = atlas->width;
	auto height = atlas->height;
	for (int i = 0; i < totalVertices; ++i) {
		auto vertex = uv_vertex_ptr[i];
		auto original_vertex_index = vertex.xref * 3;
		auto x = original_vertex_ptr[original_vertex_index + 0];
		auto y = original_vertex_ptr[original_vertex_index + 1];
		auto z = original_vertex_ptr[original_vertex_index + 2];
		auto output_vertex_index = i * 3;
		vertices[output_vertex_index + 0] = x;
		vertices[output_vertex_index + 1] = y;
		vertices[output_vertex_index + 2] = z;
		auto output_uv_index = i * 2;
		uvs[output_uv_index + 0] = vertex.uv[0] / width;
		uvs[output_uv_index + 1] = vertex.uv[1] / height;
	}
	for (int i = 0; i < totalFaces; ++i) {
		auto input_output_face_index = i * 3;
		faces[input_output_face_index + 0] = index_ptr[input_output_face_index + 0];
		faces[input_output_face_index + 1] = index_ptr[input_output_face_index + 1];
		faces[input_output_face_index + 2] = index_ptr[input_output_face_index + 2];
	}*/
	std::vector<float> uvs(2 * totalVertices);
	std::vector<uint32_t> uv_faces(3 * totalFaces);
	auto uv_vertex_ptr = atlas->meshes[0].vertexArray;
	auto index_ptr = atlas->meshes[0].indexArray;
	auto width = atlas->width;
	auto height = atlas->height;
	for (int i = 0; i < totalVertices; ++i) {
		auto vertex = uv_vertex_ptr[i];
		auto output_uv_index = i * 2;
		uvs[output_uv_index + 0] = vertex.uv[0] / width;
		uvs[output_uv_index + 1] = vertex.uv[1] / height;
	}
	for (int i = 0; i < totalFaces; ++i) {
		auto input_output_face_index = i * 3;
		uv_faces[input_output_face_index + 0] = index_ptr[input_output_face_index + 0];
		uv_faces[input_output_face_index + 1] = index_ptr[input_output_face_index + 1];
		uv_faces[input_output_face_index + 2] = index_ptr[input_output_face_index + 2];
	}
	auto img = std::make_shared<core::img>();
	if (atlas->image) {
		img->data = std::vector<uint8_t>(reinterpret_cast<uint8_t*>(atlas->image),
			reinterpret_cast<uint8_t*>(atlas->image) + atlas->width * atlas->height * 4);
		img->width = atlas->width;
		img->height = atlas->height;
		img->channels = 4;
	}
	// Cleanup.
	xatlas::Destroy(atlas);
	auto atlas_mesh_builder = core::triangle_mesh::construct();
	atlas_mesh_builder
		.with_geometry(mesh->vertices(), mesh->faces())
		.with_vertex_uvs(uvs)
		.with_uv_faces(uv_faces);
	if (mesh->has_normals()) {
		atlas_mesh_builder.with_vertex_normals(mesh->normals());
	}
	return std::make_tuple(
		/*atlas_mesh_builder
			.with_geometry(vertices, faces)
			.with_vertex_uvs(uvs)
			.build(),*/
		atlas_mesh_builder.build(),
		img
	);	
}