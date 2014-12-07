#ifndef _MODULES_H
#define _MODULES_H

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>

#include "httpd.h"

#define M_ERR		-1 // not sure how to use this
#define M_OK		0
#define M_DECLINE	1
#define M_CONTINUE	2

struct mod_info {
	char *name;
	int (*phasefilltypes)(struct clientdata_h *, char *);
	int (*phasesendreply)(struct clientdata_h *);
	int (*phaseconvertpath) (struct clientdata_h *);
	struct mod_info *next;
};

extern struct mod_info *moduleLink;
extern unsigned int numMods;
extern char **moduleFileNames;

void addModule(char *moduleFile);
void addallModules();
int runphasefilltypes(struct clientdata_h *clientdata, char *line);
int runphasesendreply(struct clientdata_h *clientdata);
int runphaseconvertpath(struct clientdata_h *clientdata);

#endif
