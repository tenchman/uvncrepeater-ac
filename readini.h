#ifndef _READINI_H_
#define _READINI_H_

#include "commondefines.h"

/* variables */
extern int viewerPort;

extern int serverPort;

extern int allowedModes;

extern int loggingLevel;

extern bool useEventInterface;

extern int allowedMode1ServerPort; 

extern int requireListedId;

extern int maxSessions;

extern int idList[];

extern int requireListedServer;

extern struct in46_addr srvListAllow[];

extern struct in46_addr srvListDeny[];

extern char ownIpAddress[];

extern char runAsUser[];

extern char eventListenerHost[];

extern int eventListenerPort;

extern bool useHttpForEventListener;

/* functions */
bool readIniFile(char *iniFilePathAndName);

#endif
