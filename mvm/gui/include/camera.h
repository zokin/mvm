#pragma once

#include <glm\glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <imgui.h>

#include <functional>

#ifndef M_PI
#define _USE_MATH_DEFINES
#endif
#include <cmath>

//#define WorldUpZ

namespace gui
{
	class camera final {
		int viewport_x, viewport_y, window_width, window_height;
		float aspect, field_of_view, near_clip, far_clip;
		glm::vec3 eye_position, original_eye_position, up_vector,
			original_target_position, target_position, translateDirection
			/*, rotation_up*/;
		ImVec2 prev_mouse_position;
		float zoom_speed, translation_speed, rotation_speed;
		bool is_active, is_rotating, is_panning;
		// angles
		float planeAngle;//0 <= theta <= 2pi
		float polarAngle;//0 <= phi <= pi
						 // Constants
		const int DefaultKeyboardZoomFactor = 100;
		const float DefaultRotationSpeed = 0.005f;
		const float DefaultZoomSpeed = 0.05f;
		const float DefaultTranslationSpeed = 0.005f;
		//const float DefaultDepthCenter = 2000.0f;
		const glm::vec3 DefaultCameraPosition = glm::vec3();
#ifdef WorldUpZ
		const glm::vec3 DefaultWorldUpVector = glm::vec3(0.0f, 0.0f, 1.0f);
#else
		const glm::vec3 DefaultWorldUpVector = glm::vec3(0.0f, 1.0f, 0.0f);
#endif
		//static glm::vec3 DefaultLookAtPosition = glm::vec3.UnitZ * DefaultDepthCenter;
		//const glm::vec3 DefaultLookAtPosition = glm::vec3();
		const float DefaultPolarAngle = M_PI / 2.0f;
		const float DefaultPlaneAngle = 0.0f;
		glm::mat4 perspective = glm::mat4(0.0f);
		glm::mat4 view = glm::mat4(1.0f);
	public:
		camera(glm::vec3 eye, glm::vec3 target)
			: planeAngle(DefaultPlaneAngle), polarAngle(DefaultPolarAngle),
			field_of_view(glm::radians(45.0f)), aspect(1920.0f / 1080.0f),
			is_rotating(false), is_panning(false), is_active(false),
			viewport_x(0), viewport_y(0), window_width(1920), window_height(1080),
			near_clip(0.5f), far_clip(4.0f), prev_mouse_position(ImVec2())
		{
			eye_position = eye;
			original_eye_position = eye_position;
			target_position = target;
			original_target_position = target_position;
			up_vector = DefaultWorldUpVector;
			//rotation_up = DefaultRotationUpVector;
			//target_position = DefaultLookAtPosition;
			zoom_speed = DefaultZoomSpeed;
			translation_speed = DefaultTranslationSpeed;
			rotation_speed = DefaultRotationSpeed;
		}

		//void set_perspective_matrix(const glm::mat4& matrix) {
		//	perspective = matrix;
		//}

		//void set_view_matrix(const glm::mat4& matrix) {
		//	view = matrix;
		//}

		const glm::mat4 get_perspective_matrix() const {
			//return perspective;
			return  glm::perspective(field_of_view, aspect, near_clip, far_clip);
			//return glm::perspectiveFov(field_of_view, window_width, window_height, near_clip, far_clip);
		}
		const glm::mat4 get_view_matrix() const {
			//return view;
			return glm::lookAt(eye_position, target_position, up_vector);
		}

		const glm::mat4 get_projection_view_matrix() const {
			return get_perspective_matrix() * get_view_matrix();
		};

		const float& get_rotation_speed() const { return rotation_speed; }
		void set_rotation_speed(const float speed) { rotation_speed = speed; }
		const float& get_pan_speed() const { return translation_speed; }
		void set_pan_speed(const float speed) { translation_speed = speed; }
		const float& get_zoom_speed() const { return zoom_speed; }
		void set_zoom_speed(const float speed) { zoom_speed = speed; }

		void reset()
		{
			//eye = DefaultCameraPosition;
			eye_position = original_eye_position;
			//target_position = DefaultLookAtPosition;
			target_position = original_target_position;
			up_vector = DefaultWorldUpVector;
			planeAngle = DefaultPlaneAngle;
			polarAngle = DefaultPolarAngle;
		}
		const glm::vec3& get_position() const { return eye_position; }
		void set_position(const glm::vec3& pos) { eye_position = pos; }
		const glm::vec3& get_target() const { return target_position; }
		void set_target(const glm::vec3& pos) { target_position = pos; }
		const float get_fov() const { return glm::degrees(field_of_view); }//TODO: careful reference?
		void set_fov(const float fov) { field_of_view = glm::radians(fov); }
		void get_viewport(int& loc_x, int& loc_y, int& width, int& height) const
		{
			loc_x = viewport_x;
			loc_y = viewport_y;
			width = window_width;
			height = window_height;
		}
		const std::size_t get_width() const { return window_width; }
		const std::size_t get_height() const { return window_height; }
		const float& get_aspect() const { return aspect; }
		void set_viewport(const int loc_x, const int loc_y, const int width, const int height)
		{
			viewport_x = loc_x;
			viewport_y = loc_y;
			window_width = width;
			window_height = height;
			//need to use doubles division here, it will not work otherwise and it is possible to get a zero aspect ratio with integer rounding
			aspect = double(width) / double(height);
		}
		void get_clipping(float& near_clip_distance, float& far_clip_distance) const
		{
			near_clip_distance = near_clip;
			far_clip_distance = far_clip;
		}
		void set_clipping(const float near_clip_distance, const float far_clip_distance)
		{
			near_clip = near_clip_distance;
			far_clip = far_clip_distance;
		}

		void update()
		{
			auto io = ImGui::GetIO();
			auto pos = io.MousePos;
			if (pos.x == FLT_MIN || pos.y == FLT_MIN) {
				return;
			}
			if (!io.WantCaptureMouse) {
				if (is_rotating) {
					rotate(pos);
				}
				if (is_panning) {
					translate(pos);
				}
				if (io.MouseClicked[0]) { // left click -> start rotating					
					is_rotating = true;
					is_panning = false;
					is_active = true;
				}
				else if (io.MouseReleased[0]) { // left unclick -> stop rotating					
					is_rotating = false;
					is_panning = false;
					is_active = false;
				}
				else if (io.MouseClicked[1]) { // right click -> start panning
					is_panning = true;
					is_rotating = false;
					is_active = true;
				}
				else if (io.MouseReleased[1]) { // right unclick -> stop panning
					is_panning = false;
					is_rotating = false;
					is_active = false;
				}
				else if (io.MouseWheel > 0.0f) { // wheel up -> zoom in			
					zoom(io.MouseWheel);
				}
				else if (io.MouseWheel < 0.0f) { // wheel down -> zoom out				
					zoom(io.MouseWheel);
				}
			}
			if (!io.WantCaptureKeyboard) {

			}
			if (is_active) {
				prev_mouse_position = pos;
			}
		}

		void get_matrices(glm::mat4 &P, glm::mat4 &V, glm::mat4 &M) const
		{
			P = get_perspective_matrix();
			V = get_view_matrix();
			M = glm::mat4(1.0f);
		}
	private:
		void rotate(const ImVec2& pos)
		{
#ifdef WorldUpZ
			int deltaY = pos.y - prev_mouse_position.y;// 2d proj plane Y displacement
			int deltaX = pos.x - prev_mouse_position.x;// 2d proj plane X displacement
#else
			//int deltaX = pos.y - prev_mouse_position.y;// 2d proj plane Y displacement
			//int deltaY = pos.x - prev_mouse_position.x;// 2d proj plane X displacement
			int deltaX = pos.y - prev_mouse_position.y;// 2d proj plane Y displacement
			int deltaY = pos.x - prev_mouse_position.x;// 2d proj plane X displacement
#endif
			float radius = glm::length(eye_position - target_position);// rotation Sphere radius
			float deltaTheta = deltaY * rotation_speed;// planeAngle displacement
			float deltaPhi = deltaX * rotation_speed;// polarAngle displacement
													 //planeAngle += deltaTheta; 
													 //polarAngle += deltaPhi;
			planeAngle += deltaPhi;
			polarAngle += deltaTheta;//switch x-y
									 // x = r * cos(theta) * cos(phi), y = r * sin(theta), z = r * cos(theta) * sin(phi)
			eye_position.x = target_position.x + radius * glm::cos(planeAngle) * glm::cos(polarAngle);
			eye_position.z = target_position.z + radius * glm::cos(planeAngle) * glm::sin(polarAngle);
			eye_position.y = target_position.y + radius * glm::sin(planeAngle);
			auto dir = glm::normalize(target_position - eye_position);
			auto right = glm::normalize(glm::cross(dir, DefaultWorldUpVector));
			//auto right = glm::normalize(glm::cross(dir, up_vector));
			up_vector = glm::normalize(glm::cross(right, dir));
		}

		void translate(const ImVec2& pos)
		{
			int deltaY = pos.y - prev_mouse_position.y;// 2d proj plane Y displacement
			int deltaX = pos.x - prev_mouse_position.x;// 2d proj plane X displacement
			glm::vec3 lookDirection = eye_position - target_position;
			glm::normalize(lookDirection);
			//translateDirection = glm::cross(lookDirection, up_vector);
			translateDirection = glm::normalize(glm::cross(lookDirection, up_vector));
			glm::vec3 panVector = translateDirection * translation_speed * (float)deltaX
				+ up_vector * translation_speed * (float)deltaY;// plus for natural translation , minus for reverse
			eye_position += panVector;
			target_position += panVector;
		}

		void zoom(int zoomFactor)
		{
			glm::vec3 zoomDirection = target_position - eye_position;
			glm::normalize(zoomDirection);
			eye_position += zoomDirection * (float)zoomFactor * zoom_speed;
			target_position += zoomDirection * (float)zoomFactor * zoom_speed;
		}
	};
} /// namespace gui

