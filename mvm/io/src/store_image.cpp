#include <output.h>

#include <stb_image_writeh.h>

#include <plog\Log.h>

void io::store_image(const std::tr2::sys::path& filename, 
	const core::image_cptr& image)
{
	LOGI << "Writing result to " << filename.string();
	if (filename.extension() == ".jpg" || filename.extension() == ".jpeg") {
		if (!stbi_write_jpg(
			filename.string().c_str(),
			image->width,
			image->height,
			image->channels,
			image->data.data(),
			75 // quality
		)) {
			LOGE << "Error writing image (" << filename.string() << ").";
		}
	}
	else if (filename.extension() == ".png") {
		if (!stbi_write_png(
			filename.string().c_str(),
			image->width,
			image->height,
			image->channels,
			image->data.data(),
			image->width * image->channels
		)) {
			LOGE << "Error writing image (" << filename.string() << ").";
		}
	}
	else {
		LOGE << "Cannot write file (" << filename.string()
			<< "), unsupported format (" << filename.extension().string() << ")";
	}	
}

static void RandomColor(uint8_t *color)
{
	for (int i = 0; i < 3; i++)
		color[i] = uint8_t((rand() % 255 + 192) * 0.5f);
}

static void SetPixel(uint8_t *dest, int destWidth, int x, int y, const uint8_t *color)
{
	uint8_t *pixel = &dest[x * 3 + y * (destWidth * 3)];
	pixel[0] = color[0];
	pixel[1] = color[1];
	pixel[2] = color[2];
}

// https://github.com/miloyip/line/blob/master/line_bresenham.c
// License: public domain.
static void RasterizeLine(uint8_t *dest, int destWidth, const int *p1, const int *p2, const uint8_t *color)
{
	const int dx = abs(p2[0] - p1[0]), sx = p1[0] < p2[0] ? 1 : -1;
	const int dy = abs(p2[1] - p1[1]), sy = p1[1] < p2[1] ? 1 : -1;
	int err = (dx > dy ? dx : -dy) / 2;
	int current[2];
	current[0] = p1[0];
	current[1] = p1[1];
	while (SetPixel(dest, destWidth, current[0], current[1], color), current[0] != p2[0] || current[1] != p2[1])
	{
		const int e2 = err;
		if (e2 > -dx) { err -= dy; current[0] += sx; }
		if (e2 < dy) { err += dx; current[1] += sy; }
	}
}

/*
https://github.com/ssloy/tinyrenderer/wiki/Lesson-2:-Triangle-rasterization-and-back-face-culling
Copyright Dmitry V. Sokolov

This software is provided 'as-is', without any express or implied warranty.
In no event will the authors be held liable for any damages arising from the use of this software.
Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it freely,
subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
*/
static void RasterizeTriangle(uint8_t *dest, int destWidth, const int *t0, const int *t1, const int *t2, const uint8_t *color)
{
	if (t0[1] > t1[1]) std::swap(t0, t1);
	if (t0[1] > t2[1]) std::swap(t0, t2);
	if (t1[1] > t2[1]) std::swap(t1, t2);
	int total_height = t2[1] - t0[1];
	for (int i = 0; i < total_height; i++) {
		bool second_half = i > t1[1] - t0[1] || t1[1] == t0[1];
		int segment_height = second_half ? t2[1] - t1[1] : t1[1] - t0[1];
		float alpha = (float)i / total_height;
		float beta = (float)(i - (second_half ? t1[1] - t0[1] : 0)) / segment_height;
		int A[2], B[2];
		for (int j = 0; j < 2; j++) {
			A[j] = int(t0[j] + (t2[j] - t0[j]) * alpha);
			B[j] = int(second_half ? t1[j] + (t2[j] - t1[j]) * beta : t0[j] + (t1[j] - t0[j]) * beta);
		}
		if (A[0] > B[0]) std::swap(A, B);
		for (int j = A[0]; j <= B[0]; j++)
			SetPixel(dest, destWidth, j, t0[1] + i, color);
	}
}

void io::store_atlas(const std::tr2::sys::path& filename,
	const core::tmesh_cptr& mesh, uint32_t width, uint32_t height)
{
	if (!mesh->has_uv_faces()) {
		LOGW << "Mesh does not have UV faces, cannot store its atlas rasterisation.";
		LOGW << "Use --atlas to calculate the UV faces";
		return;
	}
	LOGI << "Rasterizing atlas @ " << filename.string();
	const uint32_t image_buffer_size = width * height * 3;
	std::vector<uint8_t> output_triangle_image(image_buffer_size);
	const uint8_t white[] = { 255, 255, 255 };
	uint8_t *image_data = output_triangle_image.data();
	for (uint32_t j = 0; j < mesh->uv_face_count(); ++j) {
		int verts[3][2];
		for (int k = 0; k < 3; k++) {			
			auto uvs = &mesh->uv_ptr()[mesh->uv_face_ptr()[3 * j + k] * 2];
			verts[k][0] = int(uvs[0] * width);
			verts[k][1] = int(uvs[1] * height);
		}
		uint8_t color[3];
		RandomColor(color);
		RasterizeTriangle(image_data, width, verts[0], verts[1], verts[2], color);
		RasterizeLine(image_data, width, verts[0], verts[1], white);
		RasterizeLine(image_data, width, verts[1], verts[2], white);
		RasterizeLine(image_data, width, verts[2], verts[0], white);
	}
	auto img = std::make_shared<core::img>();
	img->data = output_triangle_image;
	img->width = width;
	img->height = height;
	img->channels = 3;
	io::store_image(filename, img);
}

void io::store_charts(const std::tr2::sys::path& filename,
	const core::tmesh_cptr& mesh, uint32_t width, uint32_t height)
{
	LOGI << "Rasterizing charts @ " << filename.string();
	const uint32_t image_buffer_size = width * height * 3;
	std::vector<uint8_t> output_chart_image(image_buffer_size);
	uint8_t* image_data = output_chart_image.data();
	uint8_t color[3];
	const uint8_t white[] = { 255, 255, 255 };
	RandomColor(color);
	for (uint32_t k = 0; k < mesh->face_count(); k++) {
		int verts[3][2];
		for (int l = 0; l < 3; l++) {
			auto uvs = &mesh->uv_ptr()[mesh->uv_face_ptr()[3 * k + l] * 2];			
			verts[l][0] = int(uvs[0] * width);
			verts[l][1] = int(uvs[1] * height);
		}
		RasterizeTriangle(image_data, width, verts[0], verts[1], verts[2], color);
		RasterizeLine(image_data, width, verts[0], verts[1], white);
		RasterizeLine(image_data, width, verts[1], verts[2], white);
		RasterizeLine(image_data, width, verts[2], verts[0], white);
	}
	auto img = std::make_shared<core::img>();
	img->data = output_chart_image;
	img->width = width;
	img->height = height;
	img->channels = 3;
	io::store_image(filename, img);
}