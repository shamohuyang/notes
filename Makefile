
# Makefile
CURPWD = $(PWD)

OUT_DIR ?= out
BUILD_DIR = $(CURPWD)/$(OUT_DIR)/$(TARGET)

TARGET ?= x86

ROOTFS =
ifeq ($(TARGET), ARM)
	ROOTFS=/home/cndul/work/zxq/IVI/ZXQ/external/VisionSDK/vision_sdk/linux/targetfs
	CROSS_COMPILE=arm-linux-gnueabihf-
endif

# compiler
CC = $(CROSS_COMPILE)gcc
CXX = $(CROSS_COMPILE)g++

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
APP_SRCS := \
	app/render.c \
	app/show_image.c
C_SRCS := \
	egl/egl.c \
	wayland/wayland.c \
	gles/gles.c \
	gles/shader.c \
	gles/texture.c \
	gles/draw.c \
	utils/file.c \
	cairo/cairo.c \
	gui/ui.c \
	utils/util.c \
	utils/Matrix.c
CXX_SRCS := \
	utils/obj/loader.cpp

# objs
OBJS := $(subst .c,.o,$(C_SRCS))
OBJS += $(subst .cpp,.opp,$(CXX_SRCS))
APP_OBJS := $(subst .c,.o,$(APP_SRCS))

all: apps

pre:
	@echo BUILD_DIR=$(BUILD_DIR)
	@[ -d $(BUILD_DIR) ] || mkdir $(BUILD_DIR) -p

%.o: %.c
	@echo CC $^
	@[ -d $(BUILD_DIR)/$(<D) ] || mkdir $(BUILD_DIR)/$(<D) -p
	@$(CC) -c $(INC) $< -o $(BUILD_DIR)/$@

%.opp: %.cpp
	@echo CXX $^
	@[ -d $(BUILD_DIR)/$(<D) ] || mkdir $(BUILD_DIR)/$(<D) -p
	@$(CXX) -c $(INC) $< -o $(BUILD_DIR)/$@

apps: pre $(OBJS) app
	@echo finish

app: $(APP_OBJS)
	@for a in $^; do \
		echo CCLD `basename $$a .o` && \
		cd $(BUILD_DIR) && $(CC) $$a $(OBJS) $(LIB) \
		$(LIBS) -o $(BUILD_DIR)/`basename $$a .o`; done

.PHONY: clean
clean:
	-@rm $(BUILD_DIR) -rf
