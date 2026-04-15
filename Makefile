# Compilador
CXX = clang++

# Flags
CXXFLAGS = -std=c++17
FRAMEWORK_PATH = ./dependencies/SDL3.xcframework/macos-arm64_x86_64
HEADERS_PATH   = ./dependencies/SDL3.xcframework/macos-arm64_x86_64/SDL3.framework/Headers
FRAMEWORKS     = -framework SDL3
RPATH          = -Wl,-rpath,@executable_path/dependencies/SDL3.xcframework/macos-arm64_x86_64

# Archivos
HEADERS = include
SRC = src/main.cpp src/draw.cpp src/zbuffer.cpp src/lights.cpp src/sphere.cpp \
      src/vector_2.cc src/vector_3.cc src/vector_4.cc \
      src/matrix_2.cc src/matrix_3.cc src/matrix_4.cc \
      src/collisions.cc
OUT = Rasterizer

# Regla principal
all:
	$(CXX) $(CXXFLAGS) -I$(HEADERS_PATH) -I$(HEADERS) -F$(FRAMEWORK_PATH) $(FRAMEWORKS) $(RPATH) $(SRC) -o $(OUT)

# Limpiar ejecutable
clean:
	rm -f $(OUT)
