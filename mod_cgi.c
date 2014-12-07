#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "modules.h"
#include "config.h"
#include "parse.h"

int filltypes_mod(struct clientdata_h *clientdata, char *requestline);
int sendreply_mod(struct clientdata_h *clientdata);
int convertpath_mod(struct clientdata_h *clientdata);

void getlinedata (struct clientdata_h *clientdata, char **queryvar, char **pathvar, char **scriptvar);
void executeandsend(struct clientdata_h *clientdata, char *query, char *path, char *script);

static char *cgibin_root;
static char *cgiuser;

static struct mod_info mod_cgi = {
	"mod_cgi",
	filltypes_mod,
	sendreply_mod,
	convertpath_mod,
	NULL
};

int filltypes_mod(struct clientdata_h *clientdata, char *requestline) {
	return M_DECLINE;
}

void executeandsend(struct clientdata_h *clientdata, char *query, char *path, char *script) {

	int progoutput[2];

	pipe(progoutput);

	if (fork() == 0) {
		// child process

		char *serverprotocol;
		char *serverport;
		char *requestmethod;
		char *pathinfo;
		char *pathtranslated;
		char *querystring;
		char *remoteaddr;

		char *args[2];
		char *envs[14];

		serverprotocol = malloc(MAXBUF);
		snprintf(serverprotocol, MAXBUF, "SERVER_PROTOCOL=HTTP/%i.%i", clientdata->version[0], clientdata->version[1]);

		serverport = malloc(6);
		snprintf(serverport, 21, "SERVER_PORT=%i", config_vars.port);

		requestmethod = malloc(23);
		strcpy(requestmethod, "REQUEST_METHOD=");
		if (clientdata->method == METHOD_OPTIONS)
			strcat(requestmethod, "OPTIONS");
		else if (clientdata->method == METHOD_GET)
			strcat(requestmethod, "GET");
		else if (clientdata->method == METHOD_HEAD)
			strcat(requestmethod, "HEAD");
		else if (clientdata->method == METHOD_POST)
			strcat(requestmethod, "POST");
		else if (clientdata->method == METHOD_PUT)
			strcat(requestmethod, "PUT");
		else if (clientdata->method == METHOD_DELETE)
			strcat(requestmethod, "DELETE");
		else if (clientdata->method == METHOD_TRACE)
			strcat(requestmethod, "TRACE");
		else if (clientdata->method == METHOD_CONNECT)
			strcat(requestmethod, "CONNECT");

		pathinfo = malloc(MAXBUF);
		pathtranslated = malloc(MAXBUF);
		snprintf(pathinfo, MAXBUF, "PATH_INFO=");
		snprintf(pathtranslated, MAXBUF, "PATH_TRANSLATED=");
		if (path) {
			strncat(pathinfo, path, MAXBUF);
			strncat(pathtranslated, config_vars.wwwroot, MAXBUF);
			strncat(pathtranslated, path, MAXBUF);
		}

		querystring = malloc(MAXBUF);
		snprintf(querystring, MAXBUF, "QUERY_STRING=");
		if (query)
			strncat(querystring, query, MAXBUF);

		remoteaddr = malloc(MAXBUF);
		snprintf(remoteaddr, MAXBUF, "REMOTE_ADDR=%s", inet_ntoa(clientdata->clientaddy.sin_addr));
		
		args[0] = script;
		args[1] = NULL;

		envs[0]  = "SERVER_SOFTWARE=FarhanServ/0.0.1";
		envs[1]  = "SERVER_NAME=127.0.0.1";
		envs[2]  = "GATEWAY_INTERFACE=CGI/1.1";
		envs[3]  = serverprotocol;
		envs[4]  = serverport;
		envs[5]  = requestmethod;
		envs[6]  = pathinfo;
		envs[7]  = pathtranslated;
		envs[8]  = "SCRIPT_NAME="; // fill in the rest later
		envs[9]  = querystring;
		envs[10] = "REMOTE_HOST=";
		envs[11] = remoteaddr;
		envs[12] = "AUTH_TYPE=";
		envs[13] = "REMOTE_USER=";
		envs[14] = NULL;


		dup2(progoutput[1], fileno(stdout));
		close(progoutput[0]);

		//execve("suexec", args, NULL);
		execve(script, args, envs);
	}
	else {
		char c[MAXBUF];
		int len;

		len = snprintf(c, MAXBUF, "HTTP 200 OK\r\n");
		write(clientdata->clientsock, c, len);

		close(progoutput[1]);
		while((len = read(progoutput[0], c, MAXBUF)) != 0) {
			write(clientdata->clientsock, c, len);
			printf("%s\n", c);
		}
	}

}

int sendreply_mod(struct clientdata_h *clientdata) {
	struct stat fileinfo;
	int ret;

	printf("\n\n\n\nBeginning of the module\n");

	ret = lstat(clientdata->resource, &fileinfo);
	printf("Resource is %s\n", clientdata->resource);
	printf("The return value of lstat is %i\n", ret);
	perror("lstat");
	if (S_ISDIR(fileinfo.st_mode)==1) { // Directory
		printf("This is a directory!!!\n");
		return M_DECLINE;
	}

	if ( strncmp(clientdata->resource, cgibin_root, strlen(cgibin_root) ) ) {
		printf("Nope\n");
		printf("End of the module\n\n\n\n\n");
		return M_DECLINE;
	}
	else {
		printf("Within the module\n");
		char *queryvar;
		char *pathvar;
		char *scriptvar;

		getlinedata(clientdata, &queryvar, &pathvar, &scriptvar);

		printf("here\n");
		printf("the variables are %s %s %s\n", queryvar, pathvar, scriptvar);

		executeandsend(clientdata, queryvar, pathvar, scriptvar);

		printf("End of the module\n\n\n\n\n");
		return M_OK;
	}
}

int convertpath_mod(struct clientdata_h *clientdata) {
	return M_DECLINE;
}

void getlinedata (struct clientdata_h *clientdata, char **queryvar, char **pathvar, char **scriptvar) {
	int fd;
	int x;

	char script[1024];
	char *query;
	char *path;
	char *tmpline;
	int qe = 0;

	path = NULL;
	query = NULL;

	tmpline = strdup(clientdata->resource);

	fd = open(tmpline, O_RDONLY);
	if (fd == -1) {
		close(fd);
		printf("we have to do more stuff\n");
		for(x=strlen(tmpline);x>0;x--) {
			if (tmpline[x] == '?' || tmpline[x] == '/') {
				if (tmpline[x] == '?') {
					query = tmpline + x + 1;
					qe = 1;
				}
				memset(script, 0, 1024);
				strncpy(script, tmpline, x);
				printf("testing with \"%s\"\n", script);
				fd = open(script, O_RDONLY);
				if (fd != -1) {
					close(fd);
					break;
				}
			}
		}
		if (tmpline[x] == '/') {
			path = tmpline+x;
			if (qe == 1) {
				int y;
				printf("We have to do some trimming\n");
				for(y=0;y<strlen(path);y++) {
					if (path[y] == '?') {
						path[y] = '\0';
					}
				}
			}
		}
	}
	else {
		close(fd);
		printf("Path is right\n");
		memset(script, 0, 1024);
		strncpy(script, tmpline, strlen(tmpline) );
		query = NULL;
	}


	printf("\n\nFinal Products!!!\n\n");
	printf("script\t:\t\t\"%s\"\n", script);
	printf("query\t:\t\t\"%s\"\n", query);
	printf("pathinfo\t:\t\t\"%s\"\n", path);

	*scriptvar = script;
	*queryvar = query;
	*pathvar = path;
}

struct mod_info *moduleInit() {

	cgibin_root = get_string("cgibin_root");
	if (cgibin_root == NULL)
		configError("cgibin_root");

	cgiuser = get_string("cgiuser");
	if (cgiuser == NULL)
		configError("cgiuser");

	printf("Variables are:\n");
	printf("cgibin_root: %s\n", cgibin_root);
	printf("cgiuser    : %s\n", cgiuser);

	return &mod_cgi;
}
