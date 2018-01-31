/*******************************************************************************
 Filename:       main.c
 Revised:        $Date: 2014-05-15 21:04:15 -0700 (Thu, 15 May 2014) $
 Revision:       $Revision: 38561 $

 Description:    Sample application to demonstrate the TI Home Automation ZigBee Gateway Linux solution

*******************************************************************************/

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/types.h>
#include <poll.h>
#include <stdbool.h>
#include <arpa/inet.h>
#include <signal.h>
#include <string.h>

#include "remoteMain.h"
#include "nwkMgrConn.h"

/*******************************************************************************
 * Constants
 ******************************************************************************/

/*******************************************************************************
 * Functions
 ******************************************************************************/

/*******************************************************************************
 * Main function
 ******************************************************************************/
int main(int argc, char **argv) 
{
  nwkMgrConnInit("127.0.0.1", 2531);

  remoteMainInit();
  return 0;
}
