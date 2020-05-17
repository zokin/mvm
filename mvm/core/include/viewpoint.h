#pragma once

#include <memory>
#include <string>

#include <glm.hpp>

namespace core {
	struct viewpoint final {
		std::string name;
		glm::mat4 pose;
		glm::mat3 camera;
		glm::vec2 resolution;
	};

	using vp_cptr = std::shared_ptr<const viewpoint>;
	using vp_ptr = std::shared_ptr<viewpoint>;
} /// namespace core