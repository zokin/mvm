#pragma once

#include <viewpoint.h>

#include <glm\glm.hpp>
#include <GL\gl3w.h>
#include <oglwrap.h>

namespace render {

	class viewpoint_renderer final {

	public:
		viewpoint_renderer();
		void render(
			const core::viewpoint viewpoint,
			const glm::mat4& projection,
			const glm::mat4& view,
			const glm::vec3 color
		);
	};
} /// namespace render