# ========================
# 기본 설정
# ========================
CXX := arm-linux-gnueabihf-g++ 
CXXFLAGS := -std=c++17 -Wall -Wextra -Iinclude
LDFLAGS := -pthread
SRC_DIR := src
INC_DIR := include
BIN_DIR := bin
TARGET := $(BIN_DIR)/main

# ========================
# 소스 및 오브젝트 목록
# ========================
SRCS := $(wildcard $(SRC_DIR)/*.cpp)
OBJS := $(SRCS:$(SRC_DIR)/%.cpp=$(BIN_DIR)/%.o)

# ========================
# 기본 빌드 규칙
# ========================
all: $(TARGET)

# 실행 파일 생성
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# 오브젝트 파일 생성
$(BIN_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# ========================
# 기타 명령
# ========================
.PHONY: clean run

# 컴파일된 파일 제거
clean:
	rm -rf $(BIN_DIR)/*.o $(TARGET)

# 실행
run: $(TARGET)
	./$(TARGET)
