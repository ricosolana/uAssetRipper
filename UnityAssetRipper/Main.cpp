// UnityAssetRipper.cpp : Defines the entry point for the application.
//

#include <yaml-cpp/yaml.h>
#include <iostream>
#include <filesystem>
#include <fstream>

#include "Asset.h"

// If you cant get command line arguments to work correctly, use this instead
// and modify the paths below in main() manually, then compile
//#define STUPID_FUCKING_MSVC

namespace fs = std::filesystem;

YAML::Node parseAssetFile(std::string path) {
	std::string p = fs::path(path).filename().string();
	if (!p.ends_with("asset"))
		throw std::runtime_error("Not a Unity asset file");

	std::ifstream file(path, std::ios::in);
	if (file.is_open()) {
		std::cout << "Parsing " << path << "\n";

		std::stringstream buffer;
		buffer << file.rdbuf();
		std::string s = buffer.str();
		for (int i = 0; i < 3; i++) s = s.substr(s.find("\n") + 1);

		return YAML::Load(s.c_str());
	}
	throw std::runtime_error("Unable to open ");
}

void saveObj(std::string path, std::string &obj) {
	std::ofstream out(path, std::ios::out);
	out << obj;
	out.close();

	std::cout << "Saved output as " << path << "\n";
}

void parseAndSave(std::string path, const char* outDir) {
	Asset asset(parseAssetFile(path));
	auto obj = asset.dumpAsObj();

	auto name = fs::path(path).filename().string();
	auto outName = std::string(outDir) + name + ".obj";
	saveObj(outName, obj);
}

int main(int argc, char** argv) {

#ifdef STUPID_FUCKING_MSVC
	auto path = "C:/Users/Rico/Documents/Valheim.v0.154.1.Decomp/globalgamemanagers.assets/Assets/Mesh";
#else
	argc--;
	if (argc < 1) {
		std::cout << "Usage: <in-path> [out-dir]\n";
		exit(0);
	}
	auto path = argv[1];
#endif

	try {
#ifdef STUPID_FUCKING_MSVC
		//std::string out = argc > 1 ? argv[2] : "./ripped";
		std::string out = "./ripped";
#else
		std::string out = argc > 1 ? argv[2] : "./ripped";
#endif
		std::string outputDir = out;

		int num = 0;
		while (fs::exists(out)) {
			out = outputDir + " (" + std::to_string(num) + ")";
			num++;
		}

		outputDir = out + "/";

		// create the directory

		if (fs::is_regular_file(outputDir)) {
			throw std::runtime_error("Output must be a working directory (not a file)");
		}

		if (!fs::exists(outputDir)) {
			fs::create_directory(outputDir);
			std::cout << "Created output directory\n";
		}



		auto start = std::chrono::steady_clock::now();

		if (fs::is_regular_file(path)) {
			parseAndSave(path, outputDir.c_str());
		}
		else if (fs::is_directory(path)) {
			//std::cerr << "Directories not yet supported";
			for (auto const& entry :
				fs::recursive_directory_iterator(path)) {
				if (entry.is_regular_file()) {
					try {
						parseAndSave(entry.path().string(), outputDir.c_str());
					}
					catch (std::exception& err) {
						std::cerr << "Error: " << err.what() << " " << entry.path() << "\n";
					}
				}
			}
		}
		else {
			std::cerr << "Must specify an input file or directory\n";
		}

		auto end = std::chrono::steady_clock::now();
		auto elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

		printf("Took %.02fs", (((float)elapsedMs) / 1000.f));
	}
	catch (std::exception& e) {
		std::cerr << "Error: " << e.what() << "\n";
	}

	return 0;
}
