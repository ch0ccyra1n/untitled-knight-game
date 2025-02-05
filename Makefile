all: cmake_default

debug: cmake_default_debug

clean:
	rm -rf build

cmake_default: build_setup
	cmake --build build

cmake_default_debug: build_setup_debug
	cmake --build build

build_setup_debug:
	cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug

build_setup:
	cmake -S . -B build
