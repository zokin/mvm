#include <stats.h>
#include <meshoptimizer.h>

#include <plog\Log.h>

stats::performance_stats stats::print_stats(
		const core::tmesh_cptr& mesh,
		std::string prefix
	)
{
	auto stats = get_stats(mesh);
	LOGW << prefix << " " << stats;
	return stats;
}

std::ostream & stats::operator<<(std::ostream & out, const performance_stats & stats)
{
	out << "Performance Analysis:" << std::endl
		<< "\tVertex Fetch:" << std::endl
		<< "\t\tOverfetch\t\tBytes Fetched" << std::endl
		<< "\t\t" << stats.vertex_fetch_stats.overfetch
		<< "\t\t\t" << stats.vertex_fetch_stats.bytes_fetched << std::endl
		<< "\tVertex Cache:" << std::endl
		<< "\t\tACMR\t\tATVR\t\tVertices Transformed\t\tWarps Executed" << std::endl
		<< "\t\t" << stats.vertex_cache_stats.acmr
		<< "\t\t" << stats.vertex_cache_stats.atvr
		<< "\t\t\t" << stats.vertex_cache_stats.verts_transformed
		<< "\t\t\t\t" << stats.vertex_cache_stats.warps_executed << std::endl
		<< "\tOverdraw:" << std::endl
		<< "\t\tPercentage\t\tPixels Covered\t\tPixels Shaded" << std::endl
		<< "\t\t" << stats.overdraw_stats.percentage
		<< "\t\t\t\t" << stats.overdraw_stats.pixels_covered
		<< "\t\t\t" << stats.overdraw_stats.pixels_shaded << std::endl;
	return out;
}

stats::performance_stats stats::get_stats(const core::tmesh_cptr& mesh)
{
	performance_stats stats;
	auto vertex_fetch_stats = meshopt_analyzeVertexFetch(
		mesh->face_ptr(),
		mesh->faces_byte_size() / sizeof(uint32_t),
		mesh->vertex_count(),
		sizeof(float) * 3
	);
	stats.vertex_fetch_stats.bytes_fetched = vertex_fetch_stats.bytes_fetched;
	stats.vertex_fetch_stats.overfetch = vertex_fetch_stats.overfetch;
	
	auto vertex_cache_stats = meshopt_analyzeVertexCache(
		mesh->face_ptr(),
		mesh->faces_byte_size() / sizeof(uint32_t),
		mesh->vertex_count(),
		32,
		32,
		mesh->vertices_byte_size()
	);
	stats.vertex_cache_stats.acmr = vertex_cache_stats.acmr;
	stats.vertex_cache_stats.atvr = vertex_cache_stats.atvr;
	stats.vertex_cache_stats.verts_transformed = vertex_cache_stats.vertices_transformed;
	stats.vertex_cache_stats.warps_executed = vertex_cache_stats.warps_executed;

	auto overdraw_stats = meshopt_analyzeOverdraw(
		mesh->face_ptr(),
		mesh->faces_byte_size() / sizeof(uint32_t),
		mesh->vertex_ptr(),
		mesh->vertex_count(),
		sizeof(float) * 3
	);
	stats.overdraw_stats.percentage = overdraw_stats.overdraw;
	stats.overdraw_stats.pixels_covered = overdraw_stats.pixels_covered;
	stats.overdraw_stats.pixels_shaded = overdraw_stats.pixels_shaded;

	return stats;
}

void stats::print_gains(const performance_stats& optimized, const performance_stats& original)
{
	LOGW << "Optimization gains:" << std::endl
		<< "\t\tOverfetch\t\tBytes Fetched" << std::endl
		<< "\t\t" << original.vertex_fetch_stats.overfetch / optimized.vertex_fetch_stats.overfetch
		<< "\t\t\t" << float(original.vertex_fetch_stats.bytes_fetched) / float(optimized.vertex_fetch_stats.bytes_fetched) << std::endl
		<< "\tVertex Cache:" << std::endl
		<< "\t\tACMR\t\tATVR\t\tVertices Transformed\t\tWarps Executed" << std::endl
		<< "\t\t" << original.vertex_cache_stats.acmr / optimized.vertex_cache_stats.acmr
		<< "\t\t" << original.vertex_cache_stats.atvr / optimized.vertex_cache_stats.atvr
		<< "\t\t\t" << float(original.vertex_cache_stats.verts_transformed) / float(optimized.vertex_cache_stats.verts_transformed)
		<< "\t\t\t\t" << float(original.vertex_cache_stats.warps_executed) / float(optimized.vertex_cache_stats.warps_executed) << std::endl
		<< "\tOverdraw:" << std::endl
		<< "\t\tPercentage\t\tPixels Covered\t\tPixels Shaded" << std::endl
		<< "\t\t" << original.overdraw_stats.percentage / optimized.overdraw_stats.percentage
		<< "\t\t\t\t" << float(original.overdraw_stats.pixels_covered) / float(optimized.overdraw_stats.pixels_covered)
		<< "\t\t\t" << float(original.overdraw_stats.pixels_shaded) / float(optimized.overdraw_stats.pixels_shaded) << std::endl;
	;
}
