all: cmake_default

run: build
	cd build/untitled-knight-game && ./untitled-knight-game

build:
	make cmake_default

dreamcast: build_setup_dreamcast
	cmake --build build

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

build_setup_dreamcast:
	cmake -S . -B build -DPLATFORM=Dreamcast

