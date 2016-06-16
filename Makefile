
# Makefile
CURPWD = $(PWD)

OUT_DIR ?= out
BUILD_DIR = $(CURPWD)/$(OUT_DIR)/$(TARGET)

TARGET ?= x86

ROOTFS =
ifeq ($(TARGET), ARM)
	ROOTFS=/home/cndul/work/zxq/targetfs
	CROSS_COMPILE=arm-linux-gnueabihf-
endif

# compiler
CC = $(CROSS_COMPILE)gcc -g
CXX = $(CROSS_COMPILE)g++ -g

CXX_FLAGS = -std=c++11

# include
INC = -I$(ROOTFS)/usr/include -I$(CURPWD)
INC-cairo=$(ROOTFS)/usr/include/cairo
INC += -I$(INC-cairo)

# lib
LIB = -L$(ROOTFS)/usr/lib -L$(ROOTFS)/lib -L.
LIBS = -lpthread -lm -lstdc++ -lGLESv2 -lEGL -lwayland-client
LIBS-ARM = -lffi -ldrm -lpvr_wlegl -lIMGegl -lsrv_um -lpvr2d\
	-ldrm_omap -lm -lwayland-server -lgbm -ludev -lglib-2.0\
	-ldw -lelf -lz -lbz2 -lcap
LIBS-x86 = -lwayland-egl -lglapi
LIBS-cairo = -lcairo -lpng -lpixman-1 -lfreetype -lfontconfig -lexpat
LIBS += $(LIBS-cairo) $(LIBS-$(TARGET))

# src
APP_C_SRCS := \
	app/render.c \
	app/show_image.c \
	app/cube.c \
	app/cube2.c \

APP_CPP_SRCS := \
	app/ui_test.cpp \
	app/cube3.cpp \
	app/layout_test.cpp \
	app/test_project1.cpp \

C_SRCS := \
	egl/egl.c \
	wayland/wayland.c \
	gles/gles.c \
	gles/shader.c \
	gles/texture.c \
	gles/draw.c \
	utils/file.c \
	cairo/cairo.c \
	utils/util.c \
	utils/Matrix.c \
	utils/png_load.c \
	utils/blender2opengles/cube.c \

CXX_SRCS := \
	utils/obj/loader.cpp \
	gui/common.cpp \
	gui/node.cpp \
	gui/window.cpp \
	gui/native_window.cpp \
	gui/widget.cpp \
	gui/widget_image.cpp \
	gui/painter_image_png.cpp \
	common/app.cpp \
	gles/glsl_program_object.cpp \

# objs
OBJS := $(subst .c,.o,$(C_SRCS))
OBJS += $(subst .cpp,.opp,$(CXX_SRCS))
APP_C_OBJS := $(subst .c,.o,$(APP_C_SRCS))
APP_CPP_OBJS := $(subst .cpp,.opp,$(APP_CPP_SRCS))

all: apps

pre:
	@echo BUILD_DIR=$(BUILD_DIR)
	@[ -d $(BUILD_DIR) ] || mkdir $(BUILD_DIR) -p
	@[ -d $(BUILD_DIR)/bin ] || mkdir $(BUILD_DIR)/bin -p

%.o: %.c
	@echo CC $^
	@[ -d $(BUILD_DIR)/$(<D) ] || mkdir $(BUILD_DIR)/$(<D) -p
	@$(CC) -c $(INC) $< -o $(BUILD_DIR)/$@

%.opp: %.cpp
	@echo CXX $^
	@[ -d $(BUILD_DIR)/$(<D) ] || mkdir $(BUILD_DIR)/$(<D) -p
	@$(CXX) -c $(INC) $(CXX_FLAGS) $< -o $(BUILD_DIR)/$@

apps: pre $(OBJS) app_c app_cxx
	@echo finish
app_c: $(APP_C_OBJS)
	@for a in $^; do \
		echo CCLD `basename $$a .o` && \
		cd $(BUILD_DIR) && $(CC) $$a $(OBJS) $(LIB)\
		$(LIBS) -o $(BUILD_DIR)/bin/`basename $$a .o`; done
app_cxx: $(APP_CPP_OBJS)
	@for a in $^; do \
		echo CXXLD `basename $$a .opp` && \
		cd $(BUILD_DIR) && $(CXX) $$a $(OBJS) $(LIB)\
		$(LIBS) -o $(BUILD_DIR)/bin/`basename $$a .opp`; done

.PHONY: clean
clean:
	-@rm $(BUILD_DIR) -rf
