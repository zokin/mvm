#include <viewpoint_renderer.h>

#include <glm\gtx\string_cast.hpp>

#include <plog\Log.h>

#include <debug_draw.hpp>

render::viewpoint_renderer::viewpoint_renderer()
{
	
}

void render::viewpoint_renderer::render(
	const core::viewpoint viewpoint,
	const glm::mat4& projection,
	const glm::mat4& view,
	const glm::vec3 color
	)
{
	auto mat = glm::transpose(viewpoint.pose);
	auto quad = dd::frustum2(&mat[0][0], &color.x, 1.0f);
	dd::axisTriad(&mat[0][0], 0.025f, 0.15f, 0, false);
}