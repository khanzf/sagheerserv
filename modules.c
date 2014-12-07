#include "modules.h"

unsigned int numMods = 0;
struct mod_info *moduleLink = NULL;
char **moduleFileNames = NULL;

void addallModules() {
	int y;

	for(y=0;y<numMods;y++)
		addModule(moduleFileNames[y]);
}

void addModule(char *moduleFile) {
	void *handle;
	char *error;
	struct mod_info *(*loader)();
	struct mod_info *tmpModule;
	struct mod_info *newModule;

	handle = dlopen(moduleFile, RTLD_LAZY);
	if (!handle) {
		fputs(dlerror(), stderr);
		putchar('\n');
		exit(1);
	}

	loader = dlsym(handle, "moduleInit");
	if ((error = (char *)dlerror()) != NULL) {
		fputs(error, stderr);
		putchar('\n');
		exit(1);
	}

	newModule = loader();
	tmpModule = moduleLink;

	if (!tmpModule) {
		moduleLink = newModule;
	}
	else {
		do {
			if (tmpModule -> next)
				tmpModule = tmpModule -> next;
		} while(tmpModule->next);
		tmpModule->next = newModule;
	}
}

int runphasefilltypes(struct clientdata_h *clientdata, char *line) {
	struct mod_info *tmpModule;
	int retvalue;

	tmpModule = moduleLink;

	while(tmpModule) {

		retvalue = tmpModule->phasefilltypes(clientdata, line);
		if (retvalue == M_OK) {
			return M_OK;
		}
		tmpModule = tmpModule -> next;
	}

	return M_DECLINE;
}

int runphasesendreply(struct clientdata_h *clientdata) {
	struct mod_info *tmpModule;
	int retvalue;

	tmpModule = moduleLink;

	while (tmpModule) {

		retvalue = tmpModule->phasesendreply(clientdata);
		if (retvalue == M_OK) {
			return M_OK;
		}
		tmpModule = tmpModule -> next;
	}
	return M_DECLINE;
}

int runphaseconvertpath(struct clientdata_h *clientdata) {
	struct mod_info *tmpModule;
	int retvalue;

	tmpModule = moduleLink;

	while (tmpModule) {

		retvalue = tmpModule->phaseconvertpath(clientdata);
		if (retvalue == M_OK) {
			return M_OK;
		}
		tmpModule = tmpModule -> next;
	}
	return M_DECLINE;
}
