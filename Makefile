CC 							:= cc -std=c99 -w -pedantic
FRAMEWORK 			:= .
SANDBOX_SOURCE 	:= app_layer
BUILD_DIR 			:= build
BIN 						:= app

FINAL_CFLAGS 		:= -I./$(FRAMEWORK) `pkg-config --cflags glfw3 cglm` `pkg-config --libs glfw3 cglm` -framework OpenGL -framework Cocoa -framework CoreVideo -framework CoreFoundation -framework IOKit -Wno-deprecated
OPT							:= -Ofast

prepare:
	mkdir build

clean_bin:
	rm $(BUILD_DIR)/$(BIN)

clean:
	rm -rf $(BUILD_DIR)

build:
	make prepare && $(CC) $(SANDBOX_SOURCE)/main.c $(FINAL_CFLAGS) $(OPT) -o $(BUILD_DIR)/$(BIN)

.PHONY:
	build clean
