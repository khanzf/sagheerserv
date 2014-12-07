#include "httpd.h"
#include "library.h"
#include "config.h"
#include "modules.h"
#include "parse.h"
#include "response.h"

int getrequest(int clientsock, char **fullrequest) {
	char *fullstring;
	char buffer[MAXBUF];
	int len;
	int full_len;
	struct timeval timer;
	fd_set readfds;

	printf("Beginning of getrequest\n");

	full_len = 0;
	fullstring = NULL;

	timer.tv_sec = 30;
	timer.tv_usec = 0;

	FD_ZERO(&readfds);
	FD_SET(clientsock, &readfds);

	do {
		select(clientsock+1, &readfds, NULL, NULL, &timer);

		if (FD_ISSET(clientsock, &readfds)) {
			len = recv(clientsock, buffer, MAXBUF, 0);
			fullstring = realloc(fullstring, full_len+len);
			memcpy(fullstring+full_len, buffer, len);
			full_len = full_len + len;
		}
		else {
			exit(-1);
		}
	} while (strncmp(fullstring+full_len-4, "\r\n\r\n", 4) != 0);

	*fullrequest = fullstring;

	return full_len;
}

void signchild_handler(int sig) {
	printf("Beginning of signal handler\n");
	while(waitpid(-1, NULL, WNOHANG) > 0);
} // taken entirely out of beej's

void errorcheck(char *argument) {
	printf("Beginning of errorcheck\n");
	perror(argument);
	exit(1);
}

// will add in later insha Allah
void usage(char *cmdline) {
	printf("Usage: %s [-ubcm]\n", cmdline);
	printf("  -u displays the usage\n");
	printf("  -b executes the server in the background\n");
	printf("  -c specifies the configuration file (default is httpd.conf)\n");
	printf("  -m [modulePath] loads module in with file name modulePath\n");
	printf("  -s single process only.  Will exit after 1 connection (used for testing)\n");
	exit(0);
}

void connectconstruct(struct clientdata_h *clientdata) {
	clientdata->acceptnum = 0;
	clientdata->acceptcharsetnum = 0;
	clientdata->acceptencodingnum = 0;
	clientdata->acceptlangnum = 0;
	clientdata->acceptrangenum = 0;

	clientdata->connecttype = CT_KEEPALIVE;
}

void newconnection(struct clientdata_h *clientdata) {
	char *fullrequest;
	char *temp;
	char **fullrequestbroken;
	int requestlength;
	int number_of_lines;
	int x; // loop control variable
	int y; // another loop variable

	/*
	number_of_lines = -1;
	y = 0;
	*/

	printf("\nBeginning of newconnection\n");

	do {

	number_of_lines = -1;
	y = 0;

	connectconstruct(clientdata);

	requestlength = getrequest(clientdata->clientsock, &fullrequest);
	printf("The request |%s|\n", fullrequest);

	// count the number of "\r\n"'s
	for(x=0;x<requestlength-1;x++)
		if (fullrequest[x] == '\r' && fullrequest[x+1] == '\n')
			number_of_lines++;
	fullrequestbroken = malloc(sizeof(char *) * (number_of_lines+1) );

	temp = fullrequest;
	for(x=0;x<requestlength-1;x++) {
		if (fullrequest[x] == '\r' && fullrequest[x+1] == '\n') {
			fullrequestbroken[y] = temp;
			fullrequest[x] = '\0';
			fullrequest[x+1] = '\0';
			temp = fullrequest+x+2;
			y++;
		}
	}

	fillheaders(clientdata, number_of_lines, fullrequestbroken);
	sendreply(clientdata);

	dofrees(clientdata, fullrequest, fullrequestbroken);

	} while (clientdata->connecttype == CT_KEEPALIVE);

} 

void dofrees(struct clientdata_h *clientdata, char *fullrequest, char **fullrequestbroken) {
	printf("clientdata->url\n");
	free(clientdata->url);
	printf("clientdata->resource\n");
	free(clientdata->resource);
	printf("clientdata->requested\n");
	free(clientdata->requested);

	if (clientdata->acceptnum != 0) {
		printf("clientdata->accept\n");
		free(clientdata->accept);
	}
	if (clientdata->acceptcharsetnum != 0) {
		printf("clientdata->acceptcharset\n");
		free(clientdata->acceptcharset);
	}

	if (clientdata->acceptencodingnum != 0) {
		printf("clientdata->acceptencoding\n");
		free(clientdata->acceptencoding);
	}

	if (clientdata->acceptlangnum != 0) {
		printf("clientdata->acceptlang\n");
		free(clientdata->acceptlang);
	}

	if (clientdata->acceptrangenum != 0) {
		printf("clientdata->acceptrange\n");
		free(clientdata->acceptrange);
	}

	printf("clientdata->fullrequest\n");
	free(fullrequest);
	printf("clientdata->fullrequestbroken\n");
	free(fullrequestbroken);
}

int realmain(int argc, char **argv) {
	int listensock; // this socket listens for connetions
	int clientsock; // the socket of the clients
	int rv; // return value checker 
	int yes = 1; // i won't lie.  have no idea what this does
	int sin_size; // same deal

	struct sockaddr_in myaddr;
	struct sockaddr_in remoteaddr;

	struct clientdata_h clientdata;

	struct sigaction sa;

	set_variables();
	mimetypes = fill_types();

	addallModules();

	sa.sa_handler = signchild_handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	sigaction(SIGCHLD, &sa, NULL);

	listensock = socket(AF_INET, SOCK_STREAM, 0);
	if (listensock == -1)
		errorcheck(argv[0]);

	rv = setsockopt(listensock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
	if (rv == -1)
		errorcheck("setsockopt");

	memset(&myaddr, 0x00, sizeof(struct sockaddr_in));
	myaddr.sin_family = AF_INET;
	myaddr.sin_addr.s_addr = INADDR_ANY;
	myaddr.sin_port = htons(config_vars.port);

	rv = bind(listensock, (struct sockaddr *)&myaddr, sizeof(myaddr));
	if (rv == -1)
		errorcheck(argv[0]);

	rv = listen(listensock, BACKLOG);
	if (rv == -1)
		errorcheck(argv[0]);

	printf("Going into the main loop\n");

	while(1) {
		//clientdata = malloc(sizeof(struct clientdata_h));
		sin_size = sizeof(struct sockaddr_in);
		clientsock = accept(listensock, (struct sockaddr *)&remoteaddr, &sin_size);
		printf("\n\nGOT A NEW CONNECTION\n");
		// just got a new connection

		clientdata.clientaddy = remoteaddr;
		clientdata.clientsock = clientsock;

		if (singleProcess == 0) {
			if (!fork()) {
				close(listensock);
				newconnection(&clientdata);
				printf("@@@@@ Before the exit @@@@@\n");
				exit(0);
			}
		}
		else if(singleProcess == 1) {
			close(listensock);
			newconnection(&clientdata);
			close(clientsock);
			//free(clientdata);
			printf("@@@@@ Before the exit @@@@@\n");
			exit(0);
		}
		close(clientsock);
		//free(clientdata);
	}
}
