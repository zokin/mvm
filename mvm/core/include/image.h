#pragma once

#include <vector>
#include <memory>

namespace core {

	struct img {
		std::vector<uint8_t> data;
		uint32_t width, height, channels;
	};

	using image_cptr = std::shared_ptr<const img>;
	using image_ptr = std::shared_ptr<img>;
}
