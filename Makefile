
ARCH ?= 64

# build dir
BUILD_DIR = build
OUT_DIR = out

# Default rule
.PHONY: all
all: build

# Create build dir if it doesn't exist and run cmake 
.PHONY: build
build:
	@if [ ! -d "$(BUILD_DIR)" ]; then \
		mkdir $(BUILD_DIR); \
		echo "Created build directory"; \
	fi
	@if [ ! -d "$(OUT_DIR)" ]; then \
		mkdir $(OUT_DIR); \
		echo "Created out directory"; \
	fi
	cd $(BUILD_DIR) && cmake .. -DARCH=$(arch)

# Compile specific target
.PHONY: run
run:
	@if [ ! -d "$(BUILD_DIR)" ]; then \
		make build ARCH=$(arch); \
	fi
	cd $(BUILD_DIR) && make -s $(target)

# Clean rule
.PHONY: clean
clean:
	@if [ -d "$(BUILD_DIR)" ]; then \
		rm -rf $(BUILD_DIR); \
		echo "Cleaned up build directory"; \
	else \
		echo "No build directory to clean"; \
	fi
	@if [ -d "$(OUT_DIR)" ]; then \
		rm -rf $(OUT_DIR); \
		echo "Cleaned up out directory"; \
	else \
		echo "No out directory to clean"; \
	fi
