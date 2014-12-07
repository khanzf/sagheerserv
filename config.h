#ifndef _CONFIG_H
#define _CONFIG_H

#include <sys/types.h>

#include <pwd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

extern unsigned int singleProcess;

struct config_file_variables {
	int port;

	char *username;
	uid_t uid;
	gid_t gid;

	char *wwwroot;

	char *dirIndex;

	char *msg200;
	char *msg403;
	char *msg404;
	char *msg400;
	char *msg501;

	char *errorroot;

	char *page403;
	char *page404;
} config_vars;

typedef unsigned char s_code;

extern char *config_default_path;

// Informational		1xx
#define STAT_CONTINUE		100
#define STAT_SWITCHPROTO	101
// Successful			2xx
#define STAT_OK			200
#define STAT_CREATED		201
#define STAT_ACCEPTED		202
#define STAT_NONAUTHITATIVE	203
#define STAT_NOCONTENT		204
#define STAT_RESETCONTENT	205
#define STAT_PARTIALCONTENT	206
// Redirection			3xx
#define STAT_MULTIPLECHOICES	300
#define STAT_MOVEDPERMANETLY	301
#define STAT_FOUND		302
#define STAT_SEEOTHER		303
#define STAT_NOTMODIFIED	304
#define STAT_USEPROXY		305
// Reserved?  IANA people are weird sometimes
#define STAT_TEMPORARYREDIRECT	307
// Client Error			4xx
#define STAT_BADREQUEST		400
#define STAT_UNAUTHORIZED	401
#define STAT_PAYMENTREQUIREMENT	402
#define STAT_FORBIDDEN		403
#define STAT_NOTFOUND		404
#define STAT_METHODNOTALLOWED	405
#define STAT_NOTACCEPTABLE	406
#define STAT_PROXYAUTHENTICATE	407
#define STAT_REQUESTTIMEOUT	408
#define STAT_CONFLICT		409
#define STAT_GONE		410
#define STAT_LENGTHREQUIRED	411
#define STAT_PRECONDITIONFAILED	412
#define STAT_ENTITYTOOLARGER	413
#define STAT_REQUESTURITOOLONG	414
#define STAT_UNSUPPORTEDMEDIA	415
#define STAT_RANGENOTSATISFY	416
#define STAT_EXCEPTIONFAIL	417
// Internal Server Error	5xx
#define STAT_INTERNALSERVERERR	500
#define STAT_NOTIMPLEMENTED	501
#define STAT_BADGATEWAY		502
#define STAT_SERVICEUNAVAILABLE	503
#define STAT_GATEWAYTIMEOUT	504
#define STAT_HTTPVERSION	505

int initialize_config(char *config_path);
int get_int(char *strname);
char *get_string(char *strname);
void set_variables();

typedef unsigned char s_type;

#define	T_UNDEF			0
#define T_APPL			1
#define T_AUDI			2
#define T_CHEM			3
#define T_IMAG			4
#define T_MESS			5
#define T_MODE			6
#define T_MULT			7
#define T_TEXT			8
#define T_VIDE			9
#define T_XCON			10

struct mimetype {
	s_type		majortype;
	char *		minortype;
	char **		extensions;
	unsigned int	num_extensions;
} * mimetypes;

int	num_types;

struct mimetype * fill_types();

void content_type(char *extension, s_type *majortype, char *minortype);
void configError(char *var);

#endif
