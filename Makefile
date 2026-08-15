##*******************************************************************************************************************
 # 
 # 	Author  : Burke Weston
 # 	Date    : 2026/08/14
 # 	File    : Makefile
 # 	Project : JAudio
 # 
 #******************************************************************************************************************##

COMPILER            := g++
SOURCE_FILES        := ./src/*.cpp
BUILD_NAME          := JAudio
BUILD_LOCATION      := ./bin
INCLUDE_LOCATIONS   := -I./include -I/opt/homebrew/include
LIBRARY_LOCATIONS   := -L/opt/homebrew/lib
LIBRARIES           := 
FRAMEWORK_LOCATIONS := 
FRAMEWORKS          := 
COMPILER_FLAGS      := 

.PHONY: all
all: organize build run

.PHONY: scaffold
scaffold:
	@mkdir ./src
	@mkdir ./include
	@mkdir ./bin

.PHONY: organize
organize:
	@find ./src -name "*.h" -o -name "*.hpp" | while read -r file; do 	\
		new_file=$${file/.\/src/.\/include}; 							\
		mkdir -p "$$(dirname "$$new_file")"; 							\
		mv "$$file" "$$new_file"; 										\
		echo "Moved $$file -> $$new_file"; 								\
	done

.PHONY: build
build:
	@$(COMPILER) $(SOURCE_FILES) -o $(BUILD_LOCATION)/$(BUILD_NAME) $(INCLUDE_LOCATIONS) $(LIBRARY_LOCATIONS) $(LIBRARIES) $(FRAMEWORK_LOCATIONS) $(FRAMEWORKS) $(COMPILER_FLAGS)

.PHONY: run
run:
	@$(BUILD_LOCATION)/$(BUILD_NAME)