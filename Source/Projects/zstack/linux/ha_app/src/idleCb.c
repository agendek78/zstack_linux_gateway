/*
 * idleCb.c
 *
 *  Created on: Feb 5, 2018
 *      Author: andy
 */

#include "idleCb.h"
#include "dbgLog.h"

#include <string.h>
#include <stdlib.h>

typedef struct _idleCbElement
{
  idleCbFn_t            idle_cb;
  void                  *idle_cb_arg;
  struct _idleCbElement *next;
} idleCbElement_t;

#define IDLE_CB (idle_cb_list == NULL ? NULL : idle_cb_list->idle_cb)
#define IDLE_CB_ARG (idle_cb_list == NULL ? NULL : idle_cb_list->idle_cb_arg)

static idleCbElement_t *idle_cb_list = NULL;

void idleCbInit(void)
{
  if (idle_cb_list != NULL)
  {
    if (idle_cb_list->next == NULL) //if the current entry is the first in the list
    {
      IDLE_CB(false, IDLE_CB_ARG);
    }
  }
}

#ifdef DEBUG_LOG_ENABLED
int idleCbRegister(idleCbFn_t cb, void *arg, const char *name)
#else
int idleCbRegister(idleCbFn_t cb, void *arg)
#endif
{
  idleCbElement_t *current_idle_cb_entry_ptr = (idleCbElement_t *) malloc(sizeof(idleCbElement_t));
  if (current_idle_cb_entry_ptr != NULL)
  {
    current_idle_cb_entry_ptr->idle_cb = cb;
    current_idle_cb_entry_ptr->idle_cb_arg = arg;
    current_idle_cb_entry_ptr->next = NULL;

    if (idle_cb_list == NULL)
    {
      idle_cb_list = current_idle_cb_entry_ptr;
    }
    else
    {
      idleCbElement_t *tmp = idle_cb_list;
      while(tmp->next != NULL)
      {
        tmp = tmp->next;
      }

      tmp->next = current_idle_cb_entry_ptr;
    }
    DBG_LOG("Registered idle callback %s", name);
  }
  else
  {
#ifdef DEBUG_LOG_ENABLED
    DBG_ERR("Failed to create new idle_cb entry for func %s", name);
#else
    DBG_ERR("Failed to create new idle_cb entry");
#endif
    return -1;
  }

  return 0;
}

#ifdef DEBUG_LOG_ENABLED
void idleCbUnregister(const char *name)
#else
void idleCbUnregister(void)
#endif
{
  DBG_LOG("Unregistering state machine %s", name);

  if (idle_cb_list != NULL)
  {
    idleCbElement_t *temp_idle_cb_element = idle_cb_list;

    idle_cb_list = idle_cb_list->next;
    free(temp_idle_cb_element);
  }

  if (IDLE_CB != NULL)
  {
    IDLE_CB(false, IDLE_CB_ARG);
  }
}

void idleCbCall(bool timeout)
{
  if (idle_cb_list != NULL)
  {
    if (idle_cb_list->next == NULL) //if the current entry is the first in the list
    {
      IDLE_CB(timeout, IDLE_CB_ARG);
    }
  }
}
