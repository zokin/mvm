// dear imgui: standalone example application for GLFW + OpenGL 3, using programmable pipeline
// If you are new to dear imgui, see examples/README.txt and documentation at the top of imgui.cpp.
// (GLFW is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan/Metal graphics context creation, etc.)

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <stdio.h>

// About Desktop OpenGL function loaders:
//  Modern desktop OpenGL doesn't have a standard portable header file to load OpenGL function pointers.
//  Helper libraries are often used for this purpose! Here we are supporting a few common ones (gl3w, glew, glad).
//  You may use another loader/header of your choice (glext, glLoadGen, etc.), or chose to manually implement your own.
#if defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
#include <GL/gl3w.h>            // Initialize with gl3wInit()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
#include <GL/glew.h>            // Initialize with glewInit()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
#include <glad/glad.h>          // Initialize with gladLoadGL()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING2)
#define GLFW_INCLUDE_NONE       // GLFW including OpenGL headers causes ambiguity or multiple definition errors.
#include <glbinding/Binding.h>  // Initialize with glbinding::Binding::initialize()
#include <glbinding/gl/gl.h>
using namespace gl;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING3)
#define GLFW_INCLUDE_NONE       // GLFW including OpenGL headers causes ambiguity or multiple definition errors.
#include <glbinding/glbinding.h>// Initialize with glbinding::initialize()
#include <glbinding/gl/gl.h>
using namespace gl;
#else
#include IMGUI_IMPL_OPENGL_LOADER_CUSTOM
#endif

// Include glfw3.h after our OpenGL definitions
#include <GLFW/glfw3.h>

// [Win32] Our example includes a copy of glfw3.lib pre-compiled with VS2010 to maximize ease of testing and compatibility with old VS compilers.
// To link with VS2010-era libraries, VS2015+ requires linking with legacy_stdio_definitions.lib, which we do using this pragma.
// Your own project should not be affected, as you are likely to link with a newer binary of GLFW that is adequate for your version of Visual Studio.
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

#include <iostream>
#include <glm\gtx\string_cast.hpp>

#include <vector>
#include <camera.h>
#include <sample_gl_core.hpp>
#include <triangle_mesh.h>
#include <triangle_mesh_renderer.h>
#include <viewpoint_renderer.h>
#include <atlas_renderer.h>
#include <input.h>

#include <CLI\CLI.hpp>
#include <plog\Log.h>
#include <plog\Formatters\MessageOnlyFormatter.h>
#include <plog\Formatters\TxtFormatter.h>
#include <plog\Appenders\ColorConsoleAppender.h>

static void glfw_error_callback(int error, const char* description)
{
	fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

int main(int argc, char** argv)
{
	std::string input_mesh_filename = std::string("");	
	std::string work_dir = std::string("");
	std::string log_file = std::string("");
	int verbose_flag = 0;

	CLI::App app{ "Multiview Mesh Atlas App" };
	app.add_option("--input", input_mesh_filename, "Input mesh filename")->default_val("");	
	app.add_option("--work_dir", work_dir, "Working directory")->default_val("");
	app.add_option("--log_file", log_file, "Local log file")->default_val("log.txt");
	app.add_flag("--verbose", verbose_flag, "Verbose flag");
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

	// Setup window
	glfwSetErrorCallback(glfw_error_callback);
	if (!glfwInit())
		return 1;

	// GL 3.0 + GLSL 130
	const char* glsl_version = "#version 330";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
	auto width = 1280;
	auto height = 720;
	// Create window with graphics context
	GLFWwindow* window = glfwCreateWindow(width, height, "Multiview Mesh Atlas App", NULL, NULL);
	if (window == NULL)
		return 1;
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1); // Enable vsync
	bool err = gl3wInit() != 0;
	if (err) {
		fprintf(stderr, "Failed to initialize OpenGL loader!\n");
		return 1;
	}
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
																//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	//io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
	//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
																//io.ConfigViewportsNoAutoMerge = true;
																//io.ConfigViewportsNoTaskBarIcon = true;

																// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
	ImGuiStyle& style = ImGui::GetStyle();
	//if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	//{
	//	style.WindowRounding = 0.0f;
	//	style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	//}
	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

	// Load Fonts
	// - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
	// - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
	// - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
	// - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
	// - Read 'docs/FONTS.txt' for more instructions and details.
	// - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
	//io.Fonts->AddFontDefault();
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
	//ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
	//IM_ASSERT(font != NULL);

	// Our state
	bool show_demo_window = true;
	bool show_another_window = false;
	//ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
	ImGuiWindowFlags window_menu_flags = 0;
	//window_menu_flags |= ImGuiWindowFlags_::ImGuiWindowFlags_NoMove;
	window_menu_flags |= ImGuiWindowFlags_::ImGuiWindowFlags_NoScrollbar;
	window_menu_flags |= ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysAutoResize;
	//glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);
	gui::camera camera(glm::vec3(1.f, 1.f, 1.f), glm::vec3(0.f, 0.f, 0.f));
	DDRenderInterfaceCoreGL ddRenderIfaceGL(width, height);
	camera.set_viewport(0, 0, width, height);
	camera.set_fov(75.0f);
	camera.set_clipping(0.5, 8.0);
	camera.reset();
	dd::initialize(&ddRenderIfaceGL);
	ddh::printDDBuildConfig();
	render::triangle_mesh_renderer mesh_renderer;
	render::viewpoint_renderer viewpoint_renderer;
	render::atlas_renderer atlas_renderer;
	//auto mesh = io::load_mesh("from_vlad_1_optimized.ply");
	//auto mesh = io::load_mesh("from_vlad_1.obj");
	//auto mesh = io::load_mesh("leo_template_optimized.obj");	
	//auto mesh = io::load_mesh("H:\\Data\\Perfcap\\exported\\frame_0\\animated_mesh.obj");
	//auto mesh = io::load_mesh("optimized.obj");
	//auto mesh = io::load_mesh("H:\\Data\\Perfcap\\exported\\workspace\\temp_mesh_optim_output.obj");
	auto mesh = io::load_mesh("C:\\Users\\nzioulis\\Desktop\\output\\temp_mesh_optim_output.obj");
	auto dense_mesh = io::densify(mesh);
	//auto vps = io::load_viewpoints("H:\\Data\\Perfcap\\exported\\frame_0\\calib.json");
	auto vps = io::load_viewpoints("C:\\Users\\nzioulis\\Desktop\\output\\calibration.json"); 
	ImVec4 clear_color(0.1f, 0.1f, 0.1f, 0.3f);
	static std::vector<glm::vec3> colors;
	colors.emplace_back(glm::vec3(dd::colors::OrangeRed[0], dd::colors::OrangeRed[1], dd::colors::OrangeRed[2]));
	colors.emplace_back(glm::vec3(dd::colors::ForestGreen[0], dd::colors::ForestGreen[1], dd::colors::ForestGreen[2]));
	colors.emplace_back(glm::vec3(dd::colors::AliceBlue[0], dd::colors::AliceBlue[1], dd::colors::AliceBlue[2]));
	colors.emplace_back(glm::vec3(dd::colors::Magenta[0], dd::colors::Magenta[1], dd::colors::Magenta[2]));
	colors.emplace_back(glm::vec3(dd::colors::LightYellow[0], dd::colors::LightYellow[1], dd::colors::LightYellow[2]));
	colors.emplace_back(glm::vec3(dd::colors::RebeccaPurple[0], dd::colors::RebeccaPurple[1], dd::colors::RebeccaPurple[2]));
	colors.emplace_back(glm::vec3(dd::colors::Cyan[0], dd::colors::Cyan[1], dd::colors::Cyan[2]));
	colors.emplace_back(glm::vec3(dd::colors::Aquamarine[0], dd::colors::Aquamarine[1], dd::colors::Aquamarine[2]));
	colors.emplace_back(glm::vec3(dd::colors::NavajoWhite[0], dd::colors::NavajoWhite[1], dd::colors::NavajoWhite[2]));
	colors.emplace_back(glm::vec3(dd::colors::Azure[0], dd::colors::Azure[1], dd::colors::Azure[2]));
	
	while (!glfwWindowShouldClose(window))
	{		
		atlas_renderer.create(dense_mesh);
		int display_w, display_h;
		glfwGetFramebufferSize(window, &display_w, &display_h);
		gl::Viewport(display_w, display_h);
		gl::ClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
		gl::Clear().Color().Depth();
		// Poll and handle events (inputs, window resize, etc.)
		// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
		// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
		// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
		// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
		glfwPollEvents();
		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		camera.set_viewport(0, 0, display_w, display_h);
		camera.update();
		auto vp = camera.get_projection_view_matrix();
		static auto identity = glm::mat4(1.0f);
		ddRenderIfaceGL.mvpMatrix = Matrix4(
			Vector4(vp[0][0], vp[0][1], vp[0][2], vp[0][3]),
			Vector4(vp[1][0], vp[1][1], vp[1][2], vp[1][3]),
			Vector4(vp[2][0], vp[2][1], vp[2][2], vp[2][3]),
			Vector4(vp[3][0], vp[3][1], vp[3][2], vp[3][3])
		);
		dd::sphere(&camera.get_target().x, dd::colors::White, 0.005f);
		dd::axisTriad(&identity[0][0], 0.025, 0.15, 0, false);
		dd::xzSquareGrid(-15.0f, 15.0f, -0.82f, 0.5f, dd::colors::Green, 0, false); // -0.82 is floow in our structure
		
		mesh_renderer.render(mesh, vp);
		int index = 0;
		for (auto& kvp : vps) {
			viewpoint_renderer.render(kvp.second["Depth"], camera.get_projection_view_matrix(), 
				camera.get_view_matrix(), colors[index++]);
		}

		//ImGui::Image(static_cast<void*>(&color_tex_id), ImVec2(512, 512));
		//auto calibs = calibration_result->get_value();
		//if (calibs.method() > 0 && calibs.results().size() > 0 && show_frustrum) {
		//	std::size_t index = 0;
		//	for (auto quad_render : quad_renderers) {
		//		auto color = gui::color_provider::from_id(index);
		//		auto name = quad_render->dev_info().get_name();
		//		auto it = calibs.results().find(name);
		//		if (it != calibs.results().end()) {
		//			auto pose = it->second;
		//			auto mat = pose.matrix();
		//			auto quad = dd::frustum2(mat.data(), &color.x, frustrum_scale);
		//			dd::axisTriad(mat.data(), 0.025f, 0.15f, 0, false);
		//			dd::projectedText(name.c_str(), pose.translation().data(),
		//				&color.x, glm::value_ptr(vp), 45, 30,
		//				camera->get_width(), camera->get_height());
		//			quad_render->render(index, vp, quad);
		//		}
		//		index++;
		//	}
		//}
	

		// Rendering
		dd::flush(ddh::getTimeMilliseconds(), dd::FlushAll);
		
		auto atlas_id = atlas_renderer.prepare_atlas_render();
		ImGui::Begin("Atlas");
		ImGui::Image((void*)atlas_id, ImVec2(512, 512));
		ImGui::End();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		//// Update and Render additional Platform Windows
		//// (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
		////  For this specific demo app we could also call glfwMakeContextCurrent(window) directly)
		//if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		//{
		//	GLFWwindow* backup_current_context = glfwGetCurrentContext();
		//	ImGui::UpdatePlatformWindows();
		//	ImGui::RenderPlatformWindowsDefault();
		//	glfwMakeContextCurrent(backup_current_context);
		//}

		glfwSwapBuffers(window);
	}
		
	// Cleanup
	mesh_renderer.~triangle_mesh_renderer();
	atlas_renderer.~atlas_renderer();
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}
