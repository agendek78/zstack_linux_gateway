PROJ_DIR = .

LIBS = -lpthread -lprotobuf-c

INCLUDE = -I$(PROJ_DIR) -I../engines -I../framework -I../../nwkmgr -I../../hagateway -I../../otaserver -I$(PROJ_DIR)/../remoteUi

SRCS = \
	./main.c \
	../framework/polling.c \
	../framework/timer_utils.c \
	../framework/socket_interface.c \
	../framework/tcp_client.c \
	../framework/user_interface.c \
	../framework/data_structures.c \
	../framework/macros.c \
	../engines/network_info_engine.c \
	../engines/device_list_engine.c \
	../engines/sensor_engine.c \
	../engines/state_reflector.c \
	../engines/attribute_engine.c \
	../engines/actions_engine.c \
	../engines/commissioning_engine.c \
	../engines/system_engine.c \
	../engines/group_scene_engine.c \
	../engines/ota_engine.c \
	../../otaserver/otasrvr.pb-c.c \
	../../nwkmgr/nwkmgr.pb-c.c \
	../../hagateway/gateway.pb-c.c \


#	trace.c \
#	zmain.c \
#	tcp_server.c \
#	remoteMain.c \

APP_OBJS = $(notdir $(SRCS:%.c=%.o)) 

.PHONY: all clean 

all: ha_main.bin

clean:
	@echo "********************************************************" 
	@echo "Cleaning all" 

ha_main.bin: $(APP_OBJS)
	@echo linking objects $(APP_OBJS)
	@$(CC) -rdynamic -o $@ $^ $(LDFLAGS) $(LIBS)

%.o:
	@echo "Compiling $(filter %$(basename $@).c,$(SRCS)) into $@"
	@$(CC) -c $(filter %$(basename $@).c,$(SRCS)) $(CFLAGS) $(INCLUDE) -include ../hal/hal_types.h

