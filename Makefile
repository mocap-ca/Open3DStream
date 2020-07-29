# Convenience Makefile to automatically build out the CMake project and build it
# with either MSVC or with the currently set compiler on Linux (usually gcc/g++).
# Requirements: Make, CMake, and MSVC or gcc/g++ or clang


all: release

build:
	$(shell mkdir build)

build/debug: build
	$(shell mkdir build/debug)

build/release: build
	$(shell mkdir build/release)


###################################### Windows ################################
ifeq ($(OS),Windows_NT)

.PHONY: debug
debug: build/debug
	cd build/debug; cmake ../ -DCMAKE_BUILD_TYPE=Debug
	cd build/debug; cmake --build . --config Debug --target flatc
	cd build/debug; cmake --build . --config Debug

.PHONY: release
release: build/release
	cd build/release; cmake ../../ -DCMAKE_BUILD_TYPE=Release
	cd build/release; cmake --build . --config Release --target flatc
	cd build/release; cmake --build . --config Release

clean:
	rm -rf build

.PHONY: format
format:
	find src -iname "*.h" | xargs clang-format -i
	find src -iname "*.cpp" | xargs clang-format -i
	find src -iname "*.hpp" | xargs clang-format -i

else
UNAME_S := $(shell uname -s)
###################################### Linux ##################################
ifeq ($(UNAME_S),Linux)


.PHONY: release
release: build/release
	mkdir -p build/release
	cd build/release; cmake ../../ -DCMAKE_BUILD_TYPE=Release
	cd build/release; $(MAKE)

.PHONY: debug
debug: build/debug
	mkdir -p build/debug
	cd build/debug; cmake ../../ -DCMAKE_BUILD_TYPE=Debug
	cd build/debug; $(MAKE)

.PHONY: format
format:
	find src -iname "*.h" | xargs clang-format-6.0 -i
	find src -iname "*.cpp" | xargs clang-format-6.0 -i
	find src -iname "*.hpp" | xargs clang-format-6.0 -i

endif
endif
