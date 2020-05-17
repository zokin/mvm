#include <atlas.h>
#include <simplify.h>
#include <stats.h>
#include <input.h>
#include <output.h>
#include <optimize.h>
#include <normal.h>

#include <CLI\CLI.hpp>
#include <plog\Log.h>
#include <plog\Formatters\MessageOnlyFormatter.h>
#include <plog\Formatters\TxtFormatter.h>
#include <plog\Appenders\ColorConsoleAppender.h>

#include <filesystem>

int exit_code(int code = 0)
{
#ifdef _DEBUG
	return std::getchar();
#else
	return code;
#endif // DEBUG

}

int main(int argc, char** argv)
{
	std::string input_mesh_filename = std::string("");
	std::string output_mesh_filename = std::string("");
	std::string work_dir = std::string("");
	std::string log_file = std::string("");
	std::string output_texture_filename = std::string("");
	int verbose_flag = 0;

	int simplify_flag = 0;
	std::size_t target_faces = 0;
	float target_percentage = 0.2f;
	float target_error = 1e-2f;
	int sloppy_flag = 0;

	int optimize_flag = 0;

	int normals_flag = 0;
	int area_weighted_flag = 0;

	int atlas_flag = 0;
	int pack_block_align_flag = 0;
	int pack_brute_force_flag = 0;
	float pack_texels_per_unit = 0.0f;
	uint32_t pack_resolution = 1024;
	float chart_normal_weight = 2.0f;
	float chart_roundness_weight = 0.01f;
	float chart_straightness_weight = 6.0f;
	float chart_normal_seam_weight = 4.0f;
	float chart_texture_seam_weight = 0.5f;
	float chart_max_cost = 2.0f;
	uint32_t chart_max_iterations = 1;

	CLI::App app { "Multiview Mesh Optimization Tool" };
	app.add_option("--input", input_mesh_filename, "Input mesh filename")->default_val("");
	app.add_option("--output", output_mesh_filename, "Output mesh filename")->default_val("");
	app.add_option("--work_dir", work_dir, "Working directory")->default_val("");
	app.add_option("--log_file", log_file, "Local log file")->default_val("log.txt");	
	app.add_flag("--verbose", verbose_flag, "Verbose flag");
	auto simplify_option = app.add_flag("--simplify", simplify_flag, 
		"Perform simplification before optimization");
	auto target_count_option = app.add_option("--target_faces", target_faces,
		"Simplification target faces")->needs(simplify_option);
	app.add_option("--target_percentage", target_percentage,
		"Simplification target percentage of faces")->excludes(target_count_option);
	app.add_option("--target_error", target_error,
		"Simplification target error")->needs(simplify_option);
	app.add_flag("--sloppy", sloppy_flag,
		"Use sloppy simplification")->needs(simplify_option);
	auto optimize_option = app.add_flag("--optimize", optimize_flag,
		"Perform rendering optimization");
	auto normals_option = app.add_flag("--normals", normals_flag,
		"Calculate mesh normals");
	app.add_flag("--area_weighted", area_weighted_flag,
		"Area weighted normal calculation")->needs(normals_option);
	auto atlas_option = app.add_flag("--atlas", atlas_flag,
		"Create texture atlas");
	app.add_flag("--block_align", pack_block_align_flag,
		"Pack align charts to 4x4 blocks (default false). Also improves packing speed, since there are fewer possible chart locations to consider."
	)->needs(atlas_option);
	app.add_flag("--brute_force", pack_brute_force_flag,
		"Brute force packing is slower, but gives the best result (default false). If false, use random chart placement."
	)->needs(atlas_option);
	app.add_option("--texels", pack_texels_per_unit,
		"Atlas texels per unit (default 0.0), if 0 use resolution to determine.")->needs(atlas_option);
	app.add_option("--resolution", pack_resolution,
		"Atlas resolution (default 1024), if 0 use texels per unit to determine.")->needs(atlas_option);
	app.add_option("--normal_w", chart_normal_weight,
		"Normal deviation (angle between face and average chart normal) weight (default 2.0).")->needs(atlas_option);
	app.add_option("--roundness_w", chart_roundness_weight,
		"Roundness weight (default 0.01).")->needs(atlas_option);
	app.add_option("--straightness_w", chart_straightness_weight,
		"Straightness weight (default 6.0).")->needs(atlas_option);
	app.add_option("--normal_seam_w", chart_normal_seam_weight,
		"Normal seam weight, if > 1000, normal seams are fully respected (default 4.0).")->needs(atlas_option);
	app.add_option("--texture_seam_w", chart_texture_seam_weight,
		"Texture seam weight (default 0.5).")->needs(atlas_option);
	app.add_option("--max_chart_cost", chart_max_cost,
		"Lower values result in more charts (default 0.2).")->needs(atlas_option);
	app.add_option("--max_chart_iters", chart_max_iterations,
		"Maximum charting iterations (default 1)")->needs(atlas_option);
	app.add_option("--output_texture", output_texture_filename, 
		"Output texture filename")->default_val("")->needs(atlas_option);
	
	CLI11_PARSE(app, argc, argv);

	using path = std::tr2::sys::path;
	path work_path = work_dir.empty() ? std::tr2::sys::current_path() : path(work_dir);
	path log_path = work_path / log_file;

	plog::ColorConsoleAppender<plog::MessageOnlyFormatter> consoleAppender;
	plog::RollingFileAppender<plog::TxtFormatter> fileAppender(log_path.string().c_str(), 20, 1);
	if (!log_file.empty()) {
		plog::init(verbose_flag ? plog::verbose : plog::info, &consoleAppender).addAppender(&fileAppender);
		LOGI << "Logging to: " << log_path.string();
	}
	else {
		plog::init(verbose_flag ? plog::verbose : plog::info, &consoleAppender);
	}
	
	if (!std::tr2::sys::exists(input_mesh_filename)) {
		LOGF << "Input file (" << input_mesh_filename << ") does not exist.";
		return -1;
	}
	auto input_mesh = io::load_mesh(input_mesh_filename);
	LOGE_IF(input_mesh->empty()) << "Invalid (empty) mesh.";
	//auto original_stats = stats::print_stats(input_mesh, "Pre-simplification");
	auto original_stats = stats::get_stats(input_mesh);
	auto simplified_mesh = simplify_flag 
		? simplify::simplify_mesh(input_mesh, 
			target_faces ? target_faces * 3 : std::size_t(3 * target_percentage * input_mesh->face_count()), 
			target_error, (bool)sloppy_flag)
		: input_mesh;
	//original_stats = stats::print_stats(simplified_mesh, "Post-simplification");
	original_stats = stats::get_stats(simplified_mesh);
	auto optimized_mesh = optimize_flag
		? optimize::optimize_mesh(simplified_mesh)
		//? optimize::optimize_mesh_deinterleaved(uv_mapped_mesh)
		: simplified_mesh;

	auto textured_mesh = atlas_flag
		? atlas::create(optimized_mesh,
			pack_block_align_flag,
			pack_brute_force_flag,
			pack_texels_per_unit,
			pack_resolution,
			chart_normal_weight,
			chart_roundness_weight,
			chart_straightness_weight,
			chart_normal_seam_weight,
			chart_texture_seam_weight,
			chart_max_cost,
			chart_max_iterations,
			!output_texture_filename.empty(),
			verbose_flag
		)
		: std::make_tuple(simplified_mesh, std::shared_ptr<core::img>());
	auto uv_mapped_mesh = std::get<0>(textured_mesh);
	auto atlas = std::get<1>(textured_mesh);
	auto uvs_stats = stats::print_stats(uv_mapped_mesh, "Post-atlas");

	//auto optimized_mesh = optimize_flag
	//	//? optimize::optimize_mesh(simplified_mesh)
	//	? optimize::optimize_mesh_deinterleaved(uv_mapped_mesh)
	//	: uv_mapped_mesh;
	//auto optimized_stats = stats::print_stats(optimized_mesh, "Post-optimization");
	auto optimized_stats = stats::get_stats(uv_mapped_mesh);
	LOGW << optimized_stats;
	stats::print_gains(optimized_stats, original_stats);
	//stats::print_gains(optimized_stats, uvs_stats);
	/*auto uv_mapped_mesh = atlas_flag
		? atlas::create(optimized_mesh)
		: optimized_mesh;
	auto uvs_stats = stats::print_stats(uv_mapped_mesh, "Post-atlas");*/
	uv_mapped_mesh = normals_flag
		? normal::normal_estimation(uv_mapped_mesh)
		: uv_mapped_mesh;
	io::store_mesh(output_mesh_filename, uv_mapped_mesh, output_texture_filename);

	//optimized_mesh = normals_flag
	//	? normal::normal_estimation(optimized_mesh, area_weighted_flag)
	//	: optimized_mesh;	
	//io::store_mesh(output_mesh_filename, optimized_mesh);
	
	if (!output_texture_filename.empty()) {
		io::store_image(output_texture_filename, atlas);		
	}
	if (atlas_flag && !output_texture_filename.empty()) {
		auto p = std::tr2::sys::path(output_texture_filename);
		//io::store_atlas("atlas.png", uv_mapped_mesh, atlas->width, atlas->height);
		io::store_charts((p.parent_path() / p.stem()).string() + "_charts.png",
			uv_mapped_mesh, atlas->width, atlas->height);
	}

	std::cout.flush();
	return exit_code();
}