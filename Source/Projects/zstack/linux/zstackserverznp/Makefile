# Makefile for ZStack Linux Server w/ZNP

# Variables for path locations
COMPONENTS = ../../../../Components
PROJECTS = ../../../../Projects

# Library to use
LIBS = -lpthread -lprotobuf-c

# Inlcude paths
INCLUDES = -I $(PROJECTS)/zstack/linux/zstackserverznp \
	-I $(PROJECTS)/zstack/linux/zstackpb\
	-I $(PROJECTS)/zstack/linux/srvwrapper \
	-I $(PROJECTS)/zstack/linux/linux_osal/include \
	-I $(PROJECTS)/zstack/linux/hal \
	-I $(PROJECTS)/zstack/linux/source \
	-I $(PROJECTS)/zstack/linux/zmac \
	-I $(COMPONENTS)/hal/include \
	-I $(COMPONENTS)/stack/nwk \
	-I $(COMPONENTS)/stack/zcl \
	-I $(COMPONENTS)/stack/sec \
	-I $(COMPONENTS)/stack/af \
	-I $(COMPONENTS)/stack/sys \
	-I $(COMPONENTS)/stack/zdo \
	-I $(COMPONENTS)/zmac \
	-I $(COMPONENTS)/mt \
	-I $(COMPONENTS)/mac/include

# Predefine
DEFINES = -DLINUX_ZNP -DZDO_COORDINATOR -D__ZNP_DEBUG__ -DLCD_SUPPORTED  \
	-DAPI_CLIENT_8BIT_LEN -DSERVER_NAME=ZSTACKZNP_SRVR \
	-D__APP_UI__

# Compilation Option
DEFINES += $(INCLUDES) -include ../hal/hal_types.h -include f8wConfig.h 

VPATH = ../zstackpb:../source:../srvwrapper:../linux_osal/common:$(COMPONENTS)/services/saddr:../hal

# List of object file to compile
SRCS= \
	osal_zstack_server_znp.c \
	zmain.c \
	znp_af.c \
	znp_misc.c \
	znp_zdo.c \
	zstack.pb-c.c \
	zstackpb.c \
	api_client.c \
	trace.c \
	main.c \
	api_server.c \
	configparser.c \
	OSAL_Memory.c \
	OSAL_PwrMgr.c \
	OSAL_Timers.c \
	OSAL.c \
	saddr.c \
	OnBoard.c \
	hal_timer.c

APP_OBJS = $(SRCS:%.c=%.o)

.PHONY: all clean

all: server_znp

clean:
	@echo "********************************************************" 
	@echo "Cleaning all" 

server_znp: $(APP_OBJS)
	@$(CC) -rdynamic -o $@ $^ $(LDFLAGS) $(LIBS)

%.o: %.c $(DEPS)
	@echo "Compiling $< into $@"
	@$(CC) -c $< $(CFLAGS) $(DEFINES)
