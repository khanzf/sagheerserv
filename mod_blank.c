#include <stdio.h>

#include "modules.h"

int filltypes_mod(struct clientdata_h *clientdata, char *requestline);
int sendreply_mod(struct clientdata_h *clientdata);
int convertpath_mod(struct clientdata_h *clientdata);

static struct mod_info mod_userpath = {
	"mod_blank", // The name of your module here
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
	return M_DECLINE;
}

struct mod_info *moduleInit() {
	return &mod_userpath;
}
