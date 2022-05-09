#pragma once

#include <string>
#include <iostream>
#include <vector>
#include <yaml-cpp/yaml.h>

class Asset {
	//std::vector<std::byte> m_vertexBytes;
	std::vector<float> m_vertices;
	std::vector<uint16_t> m_indexes;

public:
	Asset(YAML::Node doc);
	std::string dumpAsObj();
};