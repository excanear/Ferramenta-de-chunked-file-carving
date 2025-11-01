# Compilador
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic -O2
DEBUG_FLAGS = -g -O0 -DDEBUG

# Diretórios
SRC_DIR = src
INCLUDE_DIR = include
BUILD_DIR = build
OUTPUT_DIR = output

# Arquivos
SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS = $(SOURCES:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)
TARGET = chunked_carver

# Regra padrão
all: $(TARGET)

# Criar executável
$(TARGET): $(OBJECTS) | $(OUTPUT_DIR)
	$(CXX) $(OBJECTS) -o $(TARGET)

# Compilar objetos
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

# Criar diretórios
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(OUTPUT_DIR):
	mkdir -p $(OUTPUT_DIR)

# Build de debug
debug: CXXFLAGS += $(DEBUG_FLAGS)
debug: $(TARGET)

# Limpeza
clean:
	rm -rf $(BUILD_DIR) $(TARGET)

# Limpeza completa
distclean: clean
	rm -rf $(OUTPUT_DIR)

# Instalar (copia para /usr/local/bin)
install: $(TARGET)
	cp $(TARGET) /usr/local/bin/

# Regras que não são arquivos
.PHONY: all debug clean distclean install