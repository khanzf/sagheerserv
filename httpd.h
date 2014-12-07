#ifndef _HTTPD_H
#define _HTTPD_H

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/uio.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
//#include <pthread.h>
#include <string.h>
#include <pwd.h>
#include <fcntl.h>
#include <signal.h>
#include <fcntl.h>
#include <errno.h>
#include <dirent.h>

#define PORT	8080
#define BACKLOG	50
#define MAXBUF	1024
#define ERROR	-1

// THIS is the reason why "char" needs to be renamed to "byte"

typedef unsigned int versionnum;

typedef char methodtype;

struct clientdata_h {
	struct sockaddr_in		clientaddy;
	int				clientsock;

	char *				resource;
	char *				requested;

	methodtype			method;
	char *				url;
	versionnum	 		version[2];

	int				numheaders;

	struct accept_h	*		accept;
	int				acceptnum;
	struct acceptcharset_h *	acceptcharset;
	int				acceptcharsetnum;
	struct acceptencoding_h *	acceptencoding;
	int				acceptencodingnum;
	struct acceptlang_h *		acceptlang;
	int				acceptlangnum;
	struct acceptrange_h *		acceptrange;
	int				acceptrangenum;
	char *				from;
	char *				host;
	char *				useragent;
	int				keepalive;
	char				connecttype;
	char *				referer;

	int				port;

	int				majortype;
	int				minortype;
};

int	getrequest(int clientsock, char **fullrequest);
void	signchild_handler(int sig);
void	errorcheck(char *argument);
void	newconnection(struct clientdata_h *clientdata);
int	realmain(int argc, char **argv);

void	usage(char *cmdline);
void	connectconstructor(struct clientdata_h *clientdata);
void	dofrees(struct clientdata_h *clientdata, char *fullrequest, char **fullrequestbroken);

#endif
