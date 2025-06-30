# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -Wall -g -I./src -I./crypto

# Linker flags
LDFLAGS = -ldl

# Directories
SRC_DIR = src
CRYPTO_DIR = crypto
BUILD_DIR = build
CRYPTO_BUILD_DIR = $(BUILD_DIR)/crypto

# Target executable
TARGET = encryption

# Source files
MAIN_SRC = $(SRC_DIR)/main.cpp $(SRC_DIR)/utils.cpp $(SRC_DIR)/dynamic_loader.cpp
CRYPTO_SRC = $(CRYPTO_DIR)/caesar.cpp $(CRYPTO_DIR)/playfair.cpp $(CRYPTO_DIR)/polybius.cpp

# Object files
MAIN_OBJ = $(MAIN_SRC:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)
CRYPTO_OBJ = $(CRYPTO_SRC:$(CRYPTO_DIR)/%.cpp=$(CRYPTO_BUILD_DIR)/%.o)

# Shared libraries
CRYPTO_LIBS = $(CRYPTO_BUILD_DIR)/libcaesar.so $(CRYPTO_BUILD_DIR)/libplayfair.so $(CRYPTO_BUILD_DIR)/libpolybius.so

# Default target
all: $(BUILD_DIR) $(CRYPTO_BUILD_DIR) $(CRYPTO_LIBS) $(BUILD_DIR)/$(TARGET)

# Create build directories
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(CRYPTO_BUILD_DIR):
	mkdir -p $(CRYPTO_BUILD_DIR)

# Compile main program
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compile crypto source files into shared libraries
$(CRYPTO_BUILD_DIR)/libcaesar.so: $(CRYPTO_DIR)/caesar.cpp
	$(CXX) $(CXXFLAGS) -shared -fPIC $< -o $@

$(CRYPTO_BUILD_DIR)/libplayfair.so: $(CRYPTO_DIR)/playfair.cpp
	$(CXX) $(CXXFLAGS) -shared -fPIC $< -o $@

$(CRYPTO_BUILD_DIR)/libpolybius.so: $(CRYPTO_DIR)/polybius.cpp
	$(CXX) $(CXXFLAGS) -shared -fPIC $< -o $@

# Link main program
$(BUILD_DIR)/$(TARGET): $(MAIN_OBJ)
	$(CXX) $(MAIN_OBJ) -o $@ $(LDFLAGS)

# Clean up
clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean
