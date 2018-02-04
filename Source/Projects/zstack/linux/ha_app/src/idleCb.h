/*
 * idleCb.h
 *
 *  Created on: Feb 5, 2018
 *      Author: andy
 */

#ifndef SOURCE_PROJECTS_ZSTACK_LINUX_HA_APP_SRC_IDLECB_H_
#define SOURCE_PROJECTS_ZSTACK_LINUX_HA_APP_SRC_IDLECB_H_

#include <stdint.h>
#include <stdbool.h>
#include "dbgLog.h"

typedef void (* idleCbFn_t)(bool timed_out, void * arg);


void idleCbInit(void);
void idleCbCall(bool timeout);

#ifdef DEBUG_LOG_ENABLED

int idleCbRegister(idleCbFn_t cb, void *arg, const char *name);
void idleCbUnregister(const char *name);

#define IDLE_CB_REG(cb, arg, name)    idleCbRegister(cb, arg, name)
#define IDLE_CB_UNREG(name)  idleCbUnregister(name)

#else

int idleCbRegister(idleCbFn_t cb, void *arg);
void idleCbUnregister(void);

#define IDLE_CB_REG(cb, arg, name)  idleCbRegister(cb, arg)
#define IDLE_CB_UNREG(name)  idleCbUnregister()

#endif

#endif /* SOURCE_PROJECTS_ZSTACK_LINUX_HA_APP_SRC_IDLECB_H_ */
