#ifndef _PARSE_H
#define _PARSE_H

#include <sys/select.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "httpd.h"
#include "library.h"
#include "response.h"

#define METHOD_OPTIONS	0
#define METHOD_GET	1
#define METHOD_HEAD	2
#define METHOD_POST	3
#define METHOD_PUT	4
#define METHOD_DELETE	5
#define METHOD_TRACE	6
#define METHOD_CONNECT	7

#define CT_CLOSE	0
#define CT_KEEPALIVE	1
//#define CT_ERROR	99

struct accept_h {
	char *type;
	double quality;
	int level;
};

struct acceptcharset_h {
	char *type;
	double quality;
};

struct acceptencoding_h {
	char *type;
	double quality;
};

struct acceptlang_h {
	char *type;
	double quality;
};

struct acceptrange_h {
	char *type;
	double quality;
};

methodtype getmethod(char *line);  
char * geturl(methodtype method, char *line);
void getversion(versionnum *version, char *line);
int getrequest(int clientsock, char **fullrequest);
void fillheaders(struct clientdata_h *clientdata, int number_of_lines, char **fullrequestbroken);
void gethost(struct clientdata_h *clientdata, char *line);
void getkeepalive(struct clientdata_h *clientdata, char *line);
void getuseragent(struct clientdata_h *clientdata, char *line);
void getreferer(struct clientdata_h *clientdata, char *line);
void getconnection(struct clientdata_h *clientdata, char *line);
void acceptheader(struct clientdata_h *clientdata, char *line);
void acceptcharsetheader(struct clientdata_h *clientdata, char *line);
void acceptencodingheader(struct clientdata_h *clientdata, char *line);
void acceptlangheader(struct clientdata_h *clientdata, char *line);
void getcontenttype(struct clientdata_h *clientdata);
void truepath(struct clientdata_h *clientdata);

#endif
