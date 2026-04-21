CXX = clang++
CXXFLAGS_COMMON = -std=c++17
CXXFLAGS_DEBUG   = $(CXXFLAGS_COMMON) -O0 -g
CXXFLAGS_RELEASE = $(CXXFLAGS_COMMON) -O2 -ffast-math -DNDEBUG

FRAMEWORK_PATH = ./dependencies/SDL3.xcframework/macos-arm64_x86_64
HEADERS_PATH   = ./dependencies/SDL3.xcframework/macos-arm64_x86_64/SDL3.framework/Headers
FRAMEWORKS     = -framework SDL3
RPATH          = -Wl,-rpath,@executable_path/dependencies/SDL3.xcframework/macos-arm64_x86_64

HEADERS = include
SRC = src/main.cpp src/draw.cpp src/zbuffer.cpp src/light.cpp src/sphere.cpp src/camera.cpp \
      src/vector_2.cc src/vector_3.cc src/vector_4.cc \
      src/matrix_2.cc src/matrix_3.cc src/matrix_4.cc \
      src/collisions.cc
OUT = Rasterizer

all: release

release:
	$(CXX) $(CXXFLAGS_RELEASE) -I$(HEADERS_PATH) -I$(HEADERS) -F$(FRAMEWORK_PATH) $(FRAMEWORKS) $(RPATH) $(SRC) -o $(OUT)

debug:
	$(CXX) $(CXXFLAGS_DEBUG) -I$(HEADERS_PATH) -I$(HEADERS) -F$(FRAMEWORK_PATH) $(FRAMEWORKS) $(RPATH) $(SRC) -o $(OUT)

clean:
	rm -f $(OUT)