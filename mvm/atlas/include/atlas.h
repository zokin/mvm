#pragma once

#include <tuple>

#include <triangle_mesh.h>
#include <image.h>

/* PACK OPTIONS */
/// Align charts to 4x4 blocks. Also improves packing speed, since there are fewer possible chart locations to consider.
//bool blockAlign = false; 
/// Slower, but gives the best result. If false, use random chart placement.
//bool bruteForce = false; 
/// Create Atlas::image
//bool createImage = false; 
/// Unit to texel scale. e.g. a 1x1 quad with texelsPerUnit of 32 will take up approximately 32x32 texels in the atlas.
/// If 0, an estimated value will be calculated to approximately match the given resolution.
/// If resolution is also 0, the estimated value will approximately match a 1024x1024 atlas.
//float texelsPerUnit = 0.0f;
/// If 0, generate a single atlas with texelsPerUnit determining the final resolution.
/// If not 0, and texelsPerUnit is not 0, generate one or more atlases with that exact resolution.
/// If not 0, and texelsPerUnit is 0, texelsPerUnit is estimated to approximately match the resolution.
//uint32_t resolution = 0;

/* CHART OPTIONS*/
/// Weights determine chart growth. Higher weights mean higher cost for that metric.
/// Angle between face and average chart normal.
//float normalDeviationWeight = 2.0f; 
//float roundnessWeight = 0.01f;
//float straightnessWeight = 6.0f;
/// If > 1000, normal seams are fully respected.
//float normalSeamWeight = 4.0f; 
//float textureSeamWeight = 0.5f;
/// If total of all metrics * weights > maxCost, don't grow chart. Lower values result in more charts.
//float maxCost = 2.0f; 
/// Number of iterations of the chart growing and seeding phases. Higher values result in better charts.
//uint32_t maxIterations = 1; 

/* ParameterizeOptions */
/// If the custom parameterization function works with multiple boundaries, this can be set to false to improve performance.
//bool closeHoles = true; 
/// If meshes don't have T-junctions, this can be set to false to improve performance.
//bool fixTJunctions = true; 

namespace atlas {
	std::tuple<core::tmesh_cptr, core::image_cptr> create(
		const core::tmesh_cptr& mesh,
		bool pack_block_align = false,
		bool pack_brute_force = false,
		float pack_texels_per_unit = 0.0f,
		uint32_t pack_resolution = 0,
		float chart_normal_weight = 2.0f,
		float chart_roundness_weight = 0.01f,
		float chart_straightness_weight = 6.0f,
		float chart_normal_seam_weight = 4.0f,
		float chart_texture_seam_weight = 0.5f,
		float chart_max_cost = 2.0f,
		uint32_t chart_max_iterations = 1,
		bool create_image = false,
		bool verbose = true
	);
}