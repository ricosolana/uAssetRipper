#include "Asset.h"
#include <yaml-cpp/yaml.h>
#include <sstream>

using bytes = std::vector<std::uint8_t>;



// convert to float16 based on the bitwise notation
float decodeFP16(uint16_t num) {
	float sign = num & 0x8000 ? -1 : 1;
	int exponent = ((num >> 10) & 0x1f) - 15;
	float mantissa = 1.f + ((float)(num & 0x3ff) / (float)(0x3ff));
	return sign * mantissa * std::pow(2, exponent);
}

// convert to float32 based on the bitwise notation
float decodeFP32(uint32_t num) {
	//float result = 0;
	//std::memcpy(&result, &num, 4);
	//return result;

	float sign = num & 0x80000000 ? -1 : 1;
	int exponent = ((num >> 23) & 0xff) - 127;
	float mantissa = 1.f + ((float)(num & 0x7fffff) / (float)(0x7fffff));
	return sign * mantissa * std::pow(2, exponent);
}



// convert a hex pair to byte
uint8_t decodeHexPairUint8(const std::string& hex, size_t offset) {
	std::uint8_t a = hex[offset], b = hex[offset + 1];

	if ('0' <= a && a <= '9')
		a -= '0';
	else if ('a' <= a && a <= 'f')
		a -= 'W'; // taking the difference of 10 (is 'W' in ascii)
	else if ('A' <= a && a <= 'F')
		a -= '7';
	else
		throw std::runtime_error("Invalid hex char");

	if ('0' <= b && b <= '9')
		b -= '0';
	else if ('a' <= b && b <= 'f')
		b -= 'W'; // taking the difference of 10 (is 'W' in ascii)
	else if ('A' <= b && b <= 'F')
		b -= '7';
	else
		throw std::runtime_error("Invalid hex char");

	return (a << 4) | b;
}



// decodes a hex encoded string to bytes
bytes decodeHex(const std::string& hex) {
	auto size = hex.size();

	if (size % 2 != 0)
		throw std::runtime_error("Invalid byte arrangement");

	bytes result;
	result.reserve(size / 2);

	for (int i = 0; i < size; i += 2) {
		result.push_back(decodeHexPairUint8(hex, i));
	}

	return result;
}



// convert a hex string into a 16 bit floating point number
// swapping is automatic
float decodeFP16(bytes &b, std::size_t offset) {
	return decodeFP16(*reinterpret_cast<std::uint16_t*>(b.data() + offset));
}

// convert a hex string into a 32 bit floating point number
// swapping is automatic
float decodeFP32(bytes &b, std::size_t offset) {
	return decodeFP32(*reinterpret_cast<std::uint32_t*>(b.data() + offset));
}

std::uint16_t decodeUint16(bytes& b, std::size_t offset) {
	return *reinterpret_cast<std::uint16_t*>(b.data() + offset);
}



void sizeAndDim(YAML::Node ch, int &outSize, int &outDim) {
	int format = ch["format"].as<int>();
	int dimension = ch["dimension"].as<int>();

	outSize = -1, outDim = -1;

	if (format == 0) {
		// float32[3]	
		outSize = sizeof(float);
		outDim = dimension;
	}
	else if (format == 1) {
		outSize = 2;
		// float16[2]
		if (dimension == 2)
			outDim = 2;
		// float16[4]
		else if (dimension == 4 || dimension == 52)
			outDim = 4;
	}
	else if (format == 2) {
		if (dimension == 4) {
			outSize = 1;
			outDim = 4;
		}
	}

	if (outSize == -1 || outDim == -1)
		throw std::runtime_error("Unsupported format detected");
}



std::vector<float> decodeVector(bytes &b, std::size_t offset, int size, int dim) {
	std::vector<float> result;

	while (dim--) {
		result.push_back(size == 2 ? decodeFP16(b, offset) : decodeFP32(b, offset));
		offset += size;
	}

	return result;
}

std::vector<float> decodePos(bytes& b, std::size_t offset, int size, int dim) {
	auto pos = decodeVector(b, offset, size, dim);
	pos[0] *= -1.f;
	return pos;
}



// read in the correct dimensioned and size type
//std::vector<float> nextVector(std::string &hex, YAML::Node ch) {
//	int size = 0, dim = 0;
//	sizeAndDim(ch, size, dim);
//
//	return nextVector(hex, size, dim);
//}



Asset::Asset(YAML::Node yaml) {

	//std::string hex = "49";
	//float f = decodeHexFP32(hex);
	//auto c = decodeHexPairUint8(hex, 0);


	const auto yaml_mesh = yaml["Mesh"];
	const auto yaml_vertexData = yaml_mesh["m_VertexData"];
	const auto yaml_subMeshes = yaml_mesh["m_SubMeshes"];

	if (yaml_subMeshes.size() > 1)
		throw std::runtime_error("Currently supports only 1 submesh");
	
	const auto indexCount = yaml_subMeshes[0]["indexCount"].as<int>();
	auto indexBytes = decodeHex(yaml_mesh["m_IndexBuffer"].as<std::string>());
	auto vertexBytes = decodeHex(yaml_vertexData["_typelessdata"].as<std::string>());
	const auto vertexCount = yaml_vertexData["m_VertexCount"].as<int>();

	const auto channels = yaml_vertexData["m_Channels"];

	int vertSize = 0, vertDim = 0;

	int chunkSize = 0;
	for (int i = 0; i < channels.size(); i++) {
		int size = 0, dim = 0;
		sizeAndDim(channels[i], size, dim);
		if (i == 0) {
			vertSize = size; vertDim = dim;
			if (vertDim != 3)
				throw std::runtime_error("Vertex is not 3 dimensions");
		}

		chunkSize += size * dim;
	}

	for (size_t i = 0; i < vertexBytes.size(); i += chunkSize) {
		auto pos = decodePos(vertexBytes, i, vertSize, vertDim);
		this->m_vertices.insert(m_vertices.end(), pos.begin(), pos.end());
	}

	for (size_t i = 0; i < indexBytes.size(); i += 2) {
		auto idx = decodeUint16(indexBytes, i);
		this->m_indexes.push_back(idx);
	}
}

std::string Asset::dumpAsObj() {
	// assumed to be a triangle because no clue how 
	// a quad could be recognized in the asset yaml

	// the code below is meant to be simple and readable
	// not efficient

	std::string result;
	result += "# generated by uAssetRipper\n";
	for (size_t vi = 0; vi < m_vertices.size(); vi+=3) {
		char buf[128];
		std::sprintf(buf, "v %.06f %.06f %.06f\n", m_vertices[vi], m_vertices[vi + 1], m_vertices[vi + 2]);
		result += buf;
	}

	result += "s off\n";

	for (size_t fi = 0; fi < m_indexes.size(); fi+=3) {
		char buf[128];
		std::sprintf(buf, "f %u %u %u\n", m_indexes[fi] + 1, m_indexes[fi + 1] + 1, m_indexes[fi + 2] + 1);
		result += buf;
	}

	return result;
}
