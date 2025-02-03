# Define the build directory
BUILD_DIR = build
BUILD_TYPE ?= Debug  # Default to 'Debug' if BUILD_TYPE is not defined

configure:
	@mkdir -p $(BUILD_DIR)
	@cmake build . -S . -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=${BUILD_TYPE}

build:
	@cd $(BUILD_DIR) && $(MAKE)

run:
	./$(BUILD_DIR)/kamikaze
	
clean-all:
	rm -rf $(BUILD_DIR)

# Clean the build directory
clean:
	find $(BUILD_DIR) -maxdepth 1 -type f -delete
	rm -rf $(BUILD_DIR)/bin
	rm -rf $(BUILD_DIR)/CMakeFiles
	rm -rf $(BUILD_DIR)/kamikaze
	rm -rf $(BUILD_DIR)/src
	rm -rf $(BUILD_DIR)/Testing
	rm -rf $(BUILD_DIR)/tests
	rm -rf $(BUILD_DIR)/lib
	
	
.PHONY: configure build run clean clean-all