# Makefile for Gateway Server

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
	-I $(PROJECTS)/zstack/linux/hagateway \
	-I $(PROJECTS)/zstack/linux/nwkmgr \
	-I $(PROJECTS)/zstack/linux/srvwrapper \
	-I $(PROJECTS)/zstack/linux/serverpb \
	-I $(PROJECTS)/zstack/linux/srvepconfig \
	-I $(PROJECTS)/zstack/linux/linux_osal/include \
	-I $(PROJECTS)/zstack/linux/hal \
	-I $(PROJECTS)/zstack/linux/zmac 

# Predefine
DEFINES = \
	-DZG_SECURE_ENABLED \
	-DMAX_BINDING_CLUSTER_IDS \
	-DZCL_GROUPS \
	-DZCL_SCENES \
	-DZCL_ON_OFF \
	-DZCL_STANDALONE \
	-DZCL_READ \
	-DZCL_WRITE \
	-DZCL_DISCOVER \
	-DZCL_ALARMS \
	-DZCL_REPORT \
	-DZCL_IDENTIFY \
	-DZCL_LEVEL_CTRL \
	-DZCL_POLL_CONTROL \
	-DZCL_HVAC_CLUSTER \
	-DZCL_WINDOWCOVERING \
	-DZCL_DOORLOCK \
	-DZCL_ACE \
	-DZCL_ZONE \
	-DSERVER_NAME=GATEWAY \
	-D__APP_UI__

# Compilation Option
DEFINES += $(INCLUDES) -include ../hal/hal_types.h

VPATH = ../zstackpb:../serverpb:../srvwrapper:../srvepconfig:$(COMPONENTS)/stack/zcl

SRCS = main.c \
	zstack.pb-c.c \
	server.pb-c.c \
	api_client.c \
	api_server.c \
	configparser.c \
	serverep.c \
	serverdefep.c \
	gatewaysrvr.c \
	gateway.pb-c.c \
	zcl_port.c \
	zcl_general.c \
	zcl_poll_control.c \
	zcl_lighting.c \
	zcl_closures.c \
	zcl_hvac.c \
	zcl_ss.c \
	zcl.c \
	trace.c \
	gatewayservices.c \
	gatewayp2p.c \
	aps_groups.c \
	
APP_OBJS = $(SRCS:%.c=%.o)

.PHONY: all clean 

all: gateway_svr

clean:
	@echo "********************************************************" 
	@echo "Cleaning all" 

gateway_svr: $(APP_OBJS)
	@$(CC) -rdynamic -o $@ $^ $(LDFLAGS) $(LIBS)

%.o: %.c $(DEPS)
	@echo "Compiling $< into $@"
	@$(CC) -c $< $(CFLAGS) $(DEFINES)
