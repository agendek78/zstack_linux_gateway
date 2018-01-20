# Makefile for NPI Server

LIBS= -lpthread

#include path
INCLUDE= -I application\
	-I ipclib/server \
	-I ipclib/common

#compilation Option
DEFINES = -DMRDY_EARLY_FIX -DRNP_HOST -DNPI_UART=TRUE -D__DEBUG_TIME__ -DSRDY_INTERRUPT
# -D__DEBUG_TIME__HID -D__DEBUG_MUTEX__ -DNPI_HID=TRUE 

#list of object file to compile for the server
SERVER_OBJS= \
	ipclib/server/npi_lnx_ipc.c \
	ipclib/server/npi_lnx_uart.c \
	ipclib/server/npi_lnx_spi.c \
	ipclib/server/npi_lnx_i2c.c \
	ipclib/server/hal_gpio.c \
	ipclib/server/hal_i2c.c \
	ipclib/server/hal_spi.c

APP_OBJS = $(notdir $(SERVER_OBJS:%.c=%.o)) 

.PHONY: all clean 

all: npi_lnx

clean:
	@echo "********************************************************" 
	@echo "Cleaning all" 

npi_lnx: $(APP_OBJS)
	@echo linking objects $(APP_OBJS)
	@$(CC) -rdynamic -o $@ $^ $(LDFLAGS) $(LIBS)

%.o:
	@echo "Compiling $(filter %$(basename $@).c,$(SERVER_OBJS)) into $@"
	@$(CC) -c $(filter %$(basename $@).c,$(SERVER_OBJS)) $(CFLAGS) $(INCLUDE) $(DEFINES)
