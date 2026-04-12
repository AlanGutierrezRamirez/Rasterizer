# Compilador
CXX = clang++

# Flags
CXXFLAGS = -std=c++17
FRAMEWORK_PATH = ./dependencies/SDL3.xcframework/macos-arm64_x86_64
HEADERS_PATH = ./dependencies/SDL3.xcframework/macos-arm64_x86_64/SDL3.framework/Headers
FRAMEWORKS = -framework SDL3
RPATH = -Wl,-rpath,@executable_path/dependencies/SDL3.xcframework/macos-arm64_x86_64

# Archivos
SRC = src/main.cpp
OUT = Rasterizer

# Regla principal
all:
	$(CXX) $(CXXFLAGS) -I$(HEADERS_PATH) -F$(FRAMEWORK_PATH) $(FRAMEWORKS) $(RPATH) $(SRC) -o $(OUT)

# Limpiar ejecutable
clean:
	rm -f $(OUT)