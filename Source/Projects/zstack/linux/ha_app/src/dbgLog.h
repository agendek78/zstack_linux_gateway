/*
 * dbgLog.h
 *
 *  Created on: Jan 31, 2018
 *      Author: andy
 */

#ifndef SOURCE_PROJECTS_ZSTACK_LINUX_HA_APP_SRC_DBGLOG_H_
#define SOURCE_PROJECTS_ZSTACK_LINUX_HA_APP_SRC_DBGLOG_H_

#include <stdio.h>

#define DEBUG_LOG_ENABLED
#define DEBUG_ERR_ENABLED

#ifdef DEBUG_ERR_ENABLED
#define DBG_ERR(msg, args...)  printf(msg "\n", ##args)
#else
#define DBG_ERR(msg, args...)
#endif

#ifdef DEBUG_LOG_ENABLED
#define DBG_LOG(msg, args...)  printf(msg "\n", ##args)
#else
#define DBG_LOG(msg, args...)
#endif

#endif /* SOURCE_PROJECTS_ZSTACK_LINUX_HA_APP_SRC_DBGLOG_H_ */
