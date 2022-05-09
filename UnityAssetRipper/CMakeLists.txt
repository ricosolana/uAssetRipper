# CMakeList.txt : CMake project for UnityAssetRipper, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
include(get_cpm)

#find_package(yaml-cpp)
#CPMAddPackage(
#	NAME yaml-cpp
#	GITHUB_REPOSITORY jbeder/yaml-cpp
#	VERSION yaml-cpp-0.7.0
#	OPTIONS 
#	"YAML_CPP_BUILD_TESTS" OFF
#	"YAML_CPP_BUILD_TOOLS" OFF
#	"YAML_CPP_BUILD_CONTRIB" OFF
#)
#target_include_directories(yaml-cpp PUBLIC ${yaml-cpp_SOURCE_DIR}/src)

CPMAddPackage("gh:jbeder/yaml-cpp#yaml-cpp-0.7.0@0.7.0")

# Add source to this project's executable.
add_executable (UnityAssetRipper "Main.cpp"  "Asset.h" "Asset.cpp")
target_compile_features(UnityAssetRipper PRIVATE cxx_std_20)

target_link_libraries(UnityAssetRipper
	yaml-cpp
)

# TODO: Add tests and install targets if needed.
