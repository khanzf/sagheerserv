#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <pwd.h>

#include "httpd.h"
#include "modules.h"
#include "config.h"

int filltypes_mod(struct clientdata_h *clientdata, char *requestline);
int sendreply_mod(struct clientdata_h *clientdata);
int convertpath_mod(struct clientdata_h *clientdata);

static struct mod_info mod_userpath = {
	"mod_userpath",
	filltypes_mod,
	sendreply_mod,
	convertpath_mod,
	NULL
};

int filltypes_mod(struct clientdata_h *clientdata, char *requestline) {
	return M_DECLINE;
}

int sendreply_mod(struct clientdata_h *clientdata) {
	return M_DECLINE;
}

int convertpath_mod(struct clientdata_h *clientdata) {
	char username[64];
	//char newpath[MAXBUF];
	struct passwd* pwent;
	int len;

	printf("\n\n\nBeginning @@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
	printf("it is %s\n", clientdata->requested);

	if (clientdata->requested[1] != '~') {
		printf("Does not use the module\n");
		printf("Ending @@@@@@@@@@@@@@@@@@@@@@@@@@@@\n\n\n");
		return M_DECLINE;
	}
	printf("DO use this module\n");

	len = 0;
	while(clientdata->requested[len+2] != '/' && clientdata->requested[len+3] != '\0') {
		len++;
	}
	printf("The length is %i\n", len);

	strncpy(username, clientdata->requested+2, len);
	username[len] = '\0';

	printf("The username is %s\n", username);

	pwent = getpwnam(username);
	snprintf(clientdata->resource, MAXBUF, "%s/%s%s", pwent->pw_dir, userpath_root, clientdata->requested+len+2);
	printf("The source is %s\n", clientdata->resource);

	printf("Ending @@@@@@@@@@@@@@@@@@@@@@@@@@@@\n\n\n");
	return M_CONTINUE;
}

struct mod_info *moduleInit() {

	userpath_root = get_string("userpath_root");
	if (userpath_root == NULL)
		configError("userpath_root");

	return &mod_userpath;
}
