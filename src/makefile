CXXFLAGS = -O3 -std=c++17 # -Wall -Wextra
LIBS = -I /usr/local/include/boost/
SRC_DIR = ./
OUTPUT_DIR = ../bin/
TESTCASE_DIR = ../testcase/
OUTPUT_DIR_PUBLIC = ../output/
VERIFY_DIR = ../verifier/
PROGRAM_NAME = hw5

SRC = $(wildcard $(SRC_DIR)*.cpp)
OBJ = $(SRC:$(SRC_DIR)%.cpp=$(OUTPUT_DIR)%.o)
EXEC = $(OUTPUT_DIR)$(PROGRAM_NAME)

MEMORYDEBUG := true
# # MEMORYDEBUG := false
# PROFILING := false

# ifeq ($(MEMORYDEBUG), true)
# CXXFLAGS += -fsanitize=address -g -fno-omit-frame-pointer
# else
# # CXXFLAGS += -static
# endif

# Catch positional args if used like: make run_verify 1 0.15
ifneq (,$(filter run run_verify verify,$(MAKECMDGOALS)))
  INDEX := $(word 2,$(MAKECMDGOALS))
  # avoid make treating them as targets
  $(eval $(INDEX):;@:)
endif

INPUT_FILE = $(TESTCASE_DIR)public$(INDEX).txt
OUTPUT_FILE = $(OUTPUT_DIR_PUBLIC)public$(INDEX).out

$(EXEC): $(OBJ)
	@mkdir -p $(OUTPUT_DIR)
	@g++ $(CXXFLAGS) $(OBJ) -o $(EXEC)

$(OUTPUT_DIR)%.o: $(SRC_DIR)%.cpp
	@mkdir -p $(OUTPUT_DIR)
	@g++ $(CXXFLAGS) -c $< -o $@

all: $(EXEC)

clean:
	@rm -f $(OBJ) $(EXEC)

run: $(EXEC)
	$(EXEC) $(INPUT_FILE) $(OUTPUT_FILE)

verify:
	$(VERIFY_DIR)verify $(INPUT_FILE) $(OUTPUT_FILE)

run_verify: $(EXEC)
	$(EXEC) $(INPUT_FILE) $(OUTPUT_FILE)
	$(VERIFY_DIR)verify $(INPUT_FILE) $(OUTPUT_FILE)


.PHONY: all clean run verify run_verify sample
