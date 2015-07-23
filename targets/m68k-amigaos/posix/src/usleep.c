/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2005
 *
 * $Id: usleep.c,v 1.1 2005/10/16 18:57:46 phx Exp $
 */

#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#pragma amiga-align
#include <dos/dos.h>
#include <devices/timer.h>
#include <proto/exec.h>
#pragma default-align

static struct MsgPort *timerport = NULL;
static struct timerequest *timerio = NULL;


void _INIT_5_microtimer(void)
{
  if (timerport = CreateMsgPort()) {
    if (timerio = (struct timerequest *)
        CreateIORequest(timerport,sizeof(struct timerequest))) {
      if (OpenDevice(TIMERNAME,UNIT_MICROHZ,
                     (struct IORequest *)timerio,0) == 0) {
        usleep(1);
        return;
      }
      DeleteIORequest(&timerio->tr_node);
      timerio = NULL;
    }
    DeleteMsgPort(timerport);
    timerport = NULL;
  }
  exit(RETURN_FAIL);
}


void _EXIT_5_microtimer(void)
{
  if (timerport) {
    if (timerio) {
      if (!CheckIO((struct IORequest *)timerio)) {
        AbortIO((struct IORequest *)timerio);
        WaitIO((struct IORequest *)timerio);
      }
      CloseDevice((struct IORequest *)timerio);
      DeleteIORequest(&timerio->tr_node);
    }
    DeleteMsgPort(timerport);
  }
}


int usleep(useconds_t usec)
{
  if (usec >= 1000000) {
    errno = EINVAL;
    return -1;
  }
  timerio->tr_node.io_Command = TR_ADDREQUEST;
  timerio->tr_time.tv_secs = 0;
  timerio->tr_time.tv_micro = usec;
  SendIO(&timerio->tr_node);
  errno = 0;
  WaitIO(&timerio->tr_node);
  return 0;
}
