# Makefile for Network Manager Server

# Variables for path locations
COMPONENTS = ../../../../Components
PROJECTS = ../../../../Projects

# Library to use
LIBS = -lpthread -lprotobuf-c

# Include paths
INCLUDES = -I ../zstackpb \
	-I ../hal \
	-I . \
	-I $(COMPONENTS)/stack/zcl \
	-I $(PROJECTS)/zstack/linux/nwkmgr \
	-I $(PROJECTS)/zstack/linux/serverpb \
	-I $(PROJECTS)/zstack/linux/srvepconfig \
	-I $(PROJECTS)/zstack/linux/sdb \
	-I $(PROJECTS)/zstack/linux/srvwrapper \
	-I $(PROJECTS)/zstack/linux/hagateway \
	-I $(PROJECTS)/zstack/linux/linux_osal/include \
	-I $(PROJECTS)/zstack/linux/hal \
	-I $(PROJECTS)/zstack/linux/zmac

# Predefine
DEFINES = \
 	-D_GNU_SOURCE \
 	-D__USE_UNIX98 \
 	-DNWKMGR_SRVR \
 	-DZG_SECURE_ENABLED \
 	-DMAX_BINDING_CLUSTER_IDS \
 	-DZCL_ON_OFF \
 	-DZCL_STANDALONE \
	-DSERVER_NAME=NWK_MGR \
	-D__APP_UI__

# Compilation Option
DEFINES += $(INCLUDES) -include ../hal/hal_types.h

VPATH = ../zstackpb:../serverpb:../srvwrapper:../srvepconfig:$(COMPONENTS)/stack/zcl

# List of object file to compile
SRCS= \
	serverep.c \
	serverdefep.c \
	nwkmgrsrv.c \
	trace.c \
	nwkmgrservices.c \
	nwkmgrp2p.c \
	nwkmgr.pb-c.c \
	zstack.pb-c.c \
	server.pb-c.c \
	api_client.c \
	main.c \
	api_server.c \
	configparser.c \
	zcl_general.c \
	zcl.c \
	zcl_port.c \
	nwkmgrmemdb.c \
	nwkmgrdbfile.c

APP_OBJS = $(SRCS:%.c=%.o)

.PHONY: all clean 

all: nwkmgr_svr

clean:
	@echo "********************************************************" 
	@echo "Cleaning all" 

nwkmgr_svr: $(APP_OBJS)
	@$(CC) -rdynamic -o $@ $^ $(LDFLAGS) $(LIBS)

%.o: %.c $(DEPS)
	@echo "Compiling $< into $@"
	@$(CC) -c $< $(CFLAGS) $(DEFINES)
