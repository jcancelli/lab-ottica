# https://gist.githubusercontent.com/zobayer1/7265c698d1b024bb7723bc624aeedeb3/raw/0a4ffff809271d55253ba9733ba2649babf01438/Makefile

CXX_FLAGS := -Wall -Wextra

# Project directory structure
OUT := out
SRC := src

# Build directories and output
TEST := $(OUT)/test
BUILD := build

# Include directories
INC_DIRS := $(shell find $(SRC) -type d) 
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

# Construct build output and dependency filenames
SRCS := $(shell find $(SRC) -name *.cpp)
OBJS := $(subst $(SRC)/,$(BUILD)/,$(addsuffix .o,$(basename $(SRCS))))
DEPS := $(OBJS:.o=.d)

# Test task
test: clean build-tests
	./$(TEST) $(ARGS)

# Build tests task
build-tests: clean $(TEST)

# Task producing test from built files
$(TEST): $(OBJS)
	mkdir -p $(dir $@)
	$(CXX) $(OBJS) -o $@

# Compile all cpp files
$(BUILD)/%.o: $(SRC)/%.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CXX_FLAGS) $(INC_FLAGS) -c -o $@ $<

# Clean task
.PHONY: clean
clean:
	rm -rf $(BUILD)