#pragma once

#include <triangle_mesh.h>

#include <iostream>

namespace stats {
	
	struct performance_stats {
		struct vertex_fetch {
			std::size_t bytes_fetched;
			float overfetch;
		};
		struct vertex_cache {
			float acmr;
			float atvr;
			std::size_t verts_transformed;
			std::size_t warps_executed;
		};
		struct overdraw {
			float percentage;
			std::size_t pixels_covered;
			std::size_t pixels_shaded;
		};
		vertex_fetch vertex_fetch_stats;
		vertex_cache vertex_cache_stats;
		overdraw overdraw_stats;

		friend std::ostream& operator<< (std::ostream &out, const performance_stats& stats);
	};

	performance_stats get_stats(const core::tmesh_cptr& mesh);

	performance_stats print_stats(const core::tmesh_cptr& mesh, std::string prefix = "");

	void print_gains(const performance_stats& optimized, const performance_stats& original);
}