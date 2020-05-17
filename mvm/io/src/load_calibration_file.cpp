#include <input.h>

#include <json.hpp>

#include <fstream>

#include <plog\Log.h>

std::map<std::string, std::map<std::string, core::viewpoint>> io::load_viewpoints(
	const std::tr2::sys::path& filename)
{
	using json = nlohmann::json;
	std::ifstream ifs(filename);
	json calib;
	ifs >> calib;
	std::map<std::string, std::map<std::string, core::viewpoint>> res;
	for(auto& k : calib.items()) {
		LOGI << "Adding " << k.key() << " viewpoint.";
		/// color viewpoint
		auto c_e = k.value()["Color"]["Extrinsics"];
		auto c_i = k.value()["Color"]["Intrinsics"];
		core::viewpoint vpc;
		vpc.name = k.key();
		auto c_i_v = c_i.get<std::vector<float>>();
		vpc.camera = glm::mat3(
			glm::vec3(c_i_v[0], c_i_v[1], c_i_v[2]),
			glm::vec3(c_i_v[3], c_i_v[4], c_i_v[5]),
			glm::vec3(c_i_v[6], c_i_v[7], c_i_v[8])
		);
		auto c_e_v = c_e.get<std::vector<float>>();
		vpc.pose = glm::mat4(
			glm::vec4(c_e_v[0], c_e_v[1], c_e_v[2], c_e_v[3]),
			glm::vec4(c_e_v[4], c_e_v[5], c_e_v[6], c_e_v[7]),
			glm::vec4(c_e_v[8], c_e_v[9], c_e_v[10], c_e_v[11]),
			glm::vec4(c_e_v[12], c_e_v[13], c_e_v[14], c_e_v[15])
		);
		vpc.resolution = glm::vec2(1280.f, 720.f);
		res[k.key()]["Color"] = vpc;
		/// depth viewpoint
		auto d_e = k.value()["Depth"]["Extrinsics"];
		auto d_i = k.value()["Depth"]["Intrinsics"];
		core::viewpoint vpd;
		vpd.name = k.key();
		auto d_i_v = d_i.get<std::vector<float>>();
		vpd.camera = glm::mat3(
			glm::vec3(d_i_v[0], d_i_v[1], d_i_v[2]),
			glm::vec3(d_i_v[3], d_i_v[4], d_i_v[5]),
			glm::vec3(d_i_v[6], d_i_v[7], d_i_v[8])
		);
		auto d_e_v = d_e.get<std::vector<float>>();
		vpd.pose = glm::mat4(
			glm::vec4(d_e_v[0], d_e_v[1], d_e_v[2], d_e_v[3]),
			glm::vec4(d_e_v[4], d_e_v[5], d_e_v[6], d_e_v[7]),
			glm::vec4(d_e_v[8], d_e_v[9], d_e_v[10], d_e_v[11]),
			glm::vec4(d_e_v[12], d_e_v[13], d_e_v[14], d_e_v[15])
		);
		vpd.resolution = glm::vec2(320.f, 180.f);
		res[k.key()]["Depth"] = vpd;
	}

	return res;
}