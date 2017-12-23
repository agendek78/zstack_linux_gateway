# Makefile for Gateway Server

# Check to see if environment variables have been set
ifndef PROTOINC	
$(info Need to export the PROTOINC variable with the path to protobuf-c include directory)
ERROR_FLAG = 1
endif

ifndef PROTOLIB
$(info Need to export the PROTOLIB variable with the path to protobuf-c lib directory)
ERROR_FLAG = 1
endif

#ifndef TCLIB
#$(info Need to export the TCLIB variable with the path to the ti-sdk toolchain lib directory)
#ERROR_FLAG = 1
#endif

ifeq ($(ERROR_FLAG), 1)
$(error Stopping)
endif

PROJ_DIR = .

# ARM Protobuf directories - Fill these in with the name of your protobuf-c directory
PROTOC_INC_DIR = $(PROTOINC)
PROTOC_LIB_PATH = -L$(PROTOLIB)

# Compilation tools - make sure to change the directory if its not called ti-sdk-am... 
ARM_LIB_PATH = -L$(TCLIB)
CC_arm = arm-linux-gnueabihf-gcc
CC_x86 = gcc
CC_arago = arm-arago-linux-gnueabi-gcc
CC_mipsel = mipsel-openwrt-linux-uclibc-gcc

# Variables for path locations
COMPONENTS = ../../../../Components
PROJECTS = ../../../../Projects

# Compilation output
OBJS = out

# Temporary output for lib generation
TEMP = temp

# Library to use
LIBS_x86= -lpthread -lprotobuf-c
LIBS_arm= -lpthread -lprotobuf-c
LIBS_mipsel= -lpthread -lprotobuf-c

# Include paths
INCLUDES = -I ../zstackpb \
	-I ../hal \
	-I $(PROJ_DIR) \
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
DEFINES = -g \
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


#INCLUDES+= -I/mnt/work/mt7688/openWrtLinkit/staging_dir/target-mipsel_24kec+dsp_uClibc-0.9.33.2/usr/include \
#                -I/mnt/work/mt7688/openWrtLinkit/staging_dir/target-mipsel_24kec+dsp_uClibc-0.9.33.2/include \
#                -I/mnt/work/mt7688/openWrtLinkit/staging_dir/toolchain-mipsel_24kec+dsp_gcc-4.8-linaro_uClibc-0.9.33.2/usr/include \
#                -I/mnt/work/mt7688/openWrtLinkit/staging_dir/toolchain-mipsel_24kec+dsp_gcc-4.8-linaro_uClibc-0.9.33.2/include \
#				-I$(TCLIB)/../include

# Compilation Option
COMPILO_FLAGS_x86 = "-Wall $(INCLUDES) $(DEFINES) -include ../hal/hal_types.h" 
COMPILO_FLAGS_arm = "-Wall $(INCLUDES) $(DEFINES) -include ../hal/hal_types.h -mcpu=cortex-a8 -march=armv7-a -I $(PROTOC_INC_DIR)"
COMPILO_FLAGS_mipsel = "-Wall $(INCLUDES) -I $(PROTOC_INC_DIR) $(DEFINES) -include ../hal/hal_types.h -pipe -mno-branch-likely -mips32r2 -mtune=24kec -mdsp -fno-caller-saves -fhonour-copts -Wno-error=unused-but-set-variable -Wno-error=unused-result -msoft-float"

VPATH = $(PROJ_DIR):../zstackpb:../serverpb:../srvwrapper:../srvepconfig:$(COMPONENTS)/stack/zcl

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

.PHONY: all clean lib create_output arch-all-x86 arch-all-arm arch-all-mipsel clean_obj clean_obj2

all: \
	create_output \
	arch-all-mipsel

create_output:
	@echo "********************************************************" 
	@echo "Check existance of output folder" 
	-if test ! -d $(OBJS); then mkdir $(OBJS); fi

arch-all-x86: 
	@echo "********************************************************" 
	@echo "COMPILING APP FOR x86" 
	@$(MAKE) COMPILO=$(CC_x86) COMPILO_FLAGS=$(COMPILO_FLAGS_x86) exec_app_x86

arch-all-arm: 
	@echo "********************************************************" 
	@echo "COMPILING APP FOR ARM" 
	@$(MAKE) COMPILO=$(CC_arm) COMPILO_FLAGS=$(COMPILO_FLAGS_arm) exec_app_arm

arch-all-mipsel: 
	@echo "********************************************************" 
	@echo "COMPILING APP FOR mipsel" 
	@$(MAKE) COMPILO=$(CC_mipsel) COMPILO_FLAGS=$(COMPILO_FLAGS_mipsel) exec_app_mipsel


arch-all-arago: 
	@echo "********************************************************" 
	@echo "COMPILING APP FOR ARM ARAGO" 
	@$(MAKE) COMPILO=$(CC_arago) COMPILO_FLAGS=$(COMPILO_FLAGS_arm) exec_app_arm_arago

exec_app_x86: $(OBJS)/GATEWAY_SRVR_x86

exec_app_arm: $(OBJS)/GATEWAY_SRVR_arm

exec_app_mipsel: $(OBJS)/GATEWAY_SRVR_mipsel

exec_app_arm_arago: $(OBJS)/GATEWAY_SRVR_arago

clean_obj:
	@echo "********************************************************" 
	@echo "Cleaning Existing object" 
	-if test -d $(OBJS); then rm -f $(OBJS)/*.o; fi

clean_obj2:
	@echo "********************************************************" 
	@echo "Cleaning Existing object" 
	-if test -d $(OBJS); then rm -f $(OBJS)/*.o; fi

clean:
	@echo "********************************************************" 
	@echo "Cleaning all" 
	-if test -d $(OBJS); then rm -rf $(OBJS); fi
	-rm -rf *.o

$(OBJS)/GATEWAY_SRVR_arm: $(APP_OBJS)
	@echo "Building target" $@ "..."
	@$(COMPILO) -rdynamic -o $@ $(addprefix $(OBJS)/,$(APP_OBJS)) $(ARM_LIB_PATH) $(PROTOC_LIB_PATH) $(LIBS_arm) 
	@if [ -n "$(EXTRACT_FILES)" ]; then echo $(MAKEFILE_LIST) | $(EXTRACT_FILES) >> project_file_list.txt; fi
	@if [ -n "$(EXTRACT_FILES)" ]; then sort -u -V project_file_list.txt | grep -v /usr/ >actual_specific_project_file_list.txt; fi
	@if [ -n "$(EXTRACT_FILES)" ]; then sort -u -V project_file_list.txt | grep /usr/ >actual_standard_project_file_list.txt; fi
	@echo "********************************************************" 

$(OBJS)/GATEWAY_SRVR_mipsel: $(APP_OBJS)
	@echo "Building mipsel target" $@ "..."
	@$(COMPILO) -rdynamic -o $@ $(addprefix $(OBJS)/,$(APP_OBJS)) $(MIPSEL_LIB_PATH) $(PROTOC_LIB_PATH) $(LIBS_mipsel) 
	@if [ -n "$(EXTRACT_FILES)" ]; then echo $(MAKEFILE_LIST) | $(EXTRACT_FILES) >> project_file_list.txt; fi
	@if [ -n "$(EXTRACT_FILES)" ]; then sort -u -V project_file_list.txt | grep -v /usr/ >actual_specific_project_file_list.txt; fi
	@if [ -n "$(EXTRACT_FILES)" ]; then sort -u -V project_file_list.txt | grep /usr/ >actual_standard_project_file_list.txt; fi
	@echo "********************************************************" 


$(OBJS)/GATEWAY_SRVR_x86: $(APP_OBJS)
	@echo "Building target" $@ "..."
	@$(COMPILO) -rdynamic -o $@ $(addprefix $(OBJS)/,$(APP_OBJS)) $(LIBS_x86)
	@if [ -n "$(EXTRACT_FILES)" ]; then echo $(MAKEFILE_LIST) | $(EXTRACT_FILES) >> project_file_list.txt; fi
	@if [ -n "$(EXTRACT_FILES)" ]; then sort -u -V project_file_list.txt | grep -v /usr/ >actual_specific_project_file_list.txt; fi
	@if [ -n "$(EXTRACT_FILES)" ]; then sort -u -V project_file_list.txt | grep /usr/ >actual_standard_project_file_list.txt; fi
	@echo "********************************************************" 

$(OBJS)/GATEWAY_SRVR_arago: $(APP_OBJS)
	@echo "Building target" $@ "..."
	@$(COMPILO) -rdynamic -o $@ $(addprefix $(OBJS)/,$(APP_OBJS)) $(ARM_LIB_PATH) $(PROTOC_LIB_PATH) $(LIBS_arm) 
	@if [ -n "$(EXTRACT_FILES)" ]; then echo $(MAKEFILE_LIST) | $(EXTRACT_FILES) >> project_file_list.txt; fi
	@if [ -n "$(EXTRACT_FILES)" ]; then sort -u -V project_file_list.txt | grep -v /usr/ >actual_specific_project_file_list.txt; fi
	@if [ -n "$(EXTRACT_FILES)" ]; then sort -u -V project_file_list.txt | grep /usr/ >actual_standard_project_file_list.txt; fi
	@echo "********************************************************"

%.o: %.c 
	@echo "Compiling $< into $@"
	@$(COMPILO) -c $(COMPILO_FLAGS) $< -o $(OBJS)/$@
