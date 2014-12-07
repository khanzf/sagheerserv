#include "response.h"

void sendreply(struct clientdata_h *clientdata) {

	printf("Beginning of sendreply\n");
	int modReturn;

	truepath(clientdata);

	modReturn = runphaseconvertpath(clientdata);
	modReturn = runphasesendreply(clientdata);

	if (modReturn == M_DECLINE) {
		if (clientdata->method == METHOD_OPTIONS) {
			// yet to be implemented
			close(clientdata->clientsock);
		}
		else if (clientdata->method == METHOD_GET || clientdata->method == METHOD_HEAD) {
			//truepath(clientdata);
			send_get_reply(clientdata);
		}
		else if (clientdata->method == METHOD_POST) {
			// yet to be implemented
			close(clientdata->clientsock);
		}
		else if (clientdata->method == METHOD_PUT) {
			// yet to be implemented
			close(clientdata->clientsock);
		}
	}
	printf("End of sendreply\n");

}

void sendpage(struct clientdata_h *clientdata, FILE *filedes) {

	getcontenttype(clientdata);
	sendserverinfo(clientdata);
	sendcontenttype(clientdata);
	sendcontentlength(clientdata);
	write(clientdata->clientsock, "\r\n", 2);

	if (clientdata->method == METHOD_GET)
		sendbasiccontent(clientdata, filedes);

}

void send_get_reply(struct clientdata_h *clientdata) {
	FILE *filedes;
	struct stat fileinfo;

	printf("Beginning of send_get_reply\n");
	printf("just another check %s\n", clientdata->resource);

	//memset(replyheader, 0, MAXBUF);
	filedes = fopen(clientdata->resource, "rb");

	if (filedes != NULL) {

		lstat(clientdata->resource, &fileinfo);

		if (S_ISDIR(fileinfo.st_mode) == 0) { // if this is not a directory
			char replyheader[MAXBUF];
			int len;

			len = snprintf(replyheader, MAXBUF, "HTTP/%i.%i 200 %s\r\n", clientdata->version[0], clientdata->version[1], config_vars.msg200);
			write(clientdata->clientsock, replyheader, len);
			sendpage(clientdata, filedes);
			fclose(filedes);
		}
		else if (S_ISDIR(fileinfo.st_mode)==1) {
			send_get_reply_dir(clientdata);
			fclose(filedes);
		}
	}
	else { // error occured
		printf("about to go to send_error_page\n");
		send_error_page(clientdata);
	}
	printf("End of send_get_reply\n");
}

void sendserverinfo(struct clientdata_h *clientdata) {

	printf("Beginning of sendserverinfo\n");

	send(clientdata->clientsock, "Server: Farhan/0.0.1\r\n", 22, 0);
} 

void sendcontenttype(struct clientdata_h *clientdata) {
	char replyheader[MAXBUF];
	int len;

	printf("Beginning of sendcontenttype\n");

	//memset(replyheader, 0, MAXBUF);

	snprintf(replyheader, MAXBUF, "Content-Type: ");

	if (clientdata->majortype != -1) {
		if (mimetypes[clientdata->majortype].majortype == T_APPL) {
			len = snprintf(replyheader+14, MAXBUF, "application/%s\r\n", mimetypes[clientdata->majortype].extensions[clientdata->minortype]);
		}
		else if (mimetypes[clientdata->majortype].majortype == T_AUDI) {
			len = snprintf(replyheader+14, MAXBUF, "audio/%s\r\n", mimetypes[clientdata->majortype].extensions[clientdata->minortype]);
		}
		else if (mimetypes[clientdata->majortype].majortype == T_CHEM) {
			len = snprintf(replyheader+14, MAXBUF, "chemical/%s\r\n", mimetypes[clientdata->majortype].extensions[clientdata->minortype]);
		}
		else if (mimetypes[clientdata->majortype].majortype == T_IMAG) {
			len = snprintf(replyheader+14, MAXBUF, "image/%s\r\n", mimetypes[clientdata->majortype].extensions[clientdata->minortype]);
		}
		else if (mimetypes[clientdata->majortype].majortype == T_MESS) {
			len = snprintf(replyheader+14, MAXBUF, "message/%s\r\n", mimetypes[clientdata->majortype].extensions[clientdata->minortype]);
		}
		else if (mimetypes[clientdata->majortype].majortype == T_MODE) {
			len = snprintf(replyheader+14, MAXBUF, "model/%s\r\n", mimetypes[clientdata->majortype].extensions[clientdata->minortype]);
		}
		else if (mimetypes[clientdata->majortype].majortype == T_MULT) {
			len = snprintf(replyheader+14, MAXBUF, "model/%s\r\n", mimetypes[clientdata->majortype].extensions[clientdata->minortype]);
		}
		else if (mimetypes[clientdata->majortype].majortype == T_MULT) {
			len = snprintf(replyheader+14, MAXBUF, "multipart/%s\r\n", mimetypes[clientdata->majortype].extensions[clientdata->minortype]);
		}
		else if (mimetypes[clientdata->majortype].majortype == T_TEXT) {
			if (!memcmp( mimetypes[clientdata->majortype].extensions[clientdata->minortype], "txt\0", 4))
				len = snprintf(replyheader+14, MAXBUF, "text/plain\r\n");
			else
				len = snprintf(replyheader+14, MAXBUF, "text/%s\r\n", mimetypes[clientdata->majortype].extensions[clientdata->minortype]);
		}
		else if (mimetypes[clientdata->majortype].majortype == T_VIDE) {
			len = snprintf(replyheader+14, MAXBUF, "video/%s\r\n", mimetypes[clientdata->majortype].extensions[clientdata->minortype]);
		}
		else if (mimetypes[clientdata->majortype].majortype == T_XCON) {
			len = snprintf(replyheader+14, MAXBUF, "x-conference/%s\r\n", mimetypes[clientdata->majortype].extensions[clientdata->minortype]);
		}
	}
	else {
		len = snprintf(replyheader+14, MAXBUF, "application/octet-stream\r\n");
	}

	send(clientdata->clientsock, replyheader, len+14, 0); 
}

void sendcontentlength(struct clientdata_h *clientdata) {
	struct stat buf;
	int len;
	char buffer[MAXBUF];

	if ( lstat(clientdata->resource, &buf) != -1 ) {
		len = snprintf(buffer, MAXBUF, "Content-Length: %i\r\n", (int)buf.st_size);
		send(clientdata->clientsock, buffer, len, 0);
	}
	else
		errorcheck("lstat");
}

void sendbasiccontent(struct clientdata_h *clientdata, FILE *filedes) {
	char content[1024];
	int len;

	printf("Beginning of sendbasiccontent\n");

	do {
		len = fread(content, 1, MAXBUF, filedes);
		write(clientdata->clientsock, content, len);
	} while(len == MAXBUF);
}

void send_get_reply_dir(struct clientdata_h *clientdata) {
	char indexPage[MAXBUF]; // make this malloced once bug is fixed
	char replyheader[MAXBUF];
	int len;
	FILE *filedes;

	printf("Beginning of send_get_reply_dir\n");

	//memset(indexPage, 0, MAXBUF);
	// this should be the path root, not this...
	snprintf(indexPage, MAXBUF, "%s%s", clientdata->resource, config_vars.dirIndex);
	filedes = fopen(indexPage, "rb");
	//memset(replyheader, 0, MAXBUF);

	if (filedes == NULL && errno == ENOENT) {
		len = snprintf(replyheader, MAXBUF, "HTTP/%i.%i 200 %s\r\n", clientdata->version[0], clientdata->version[1], config_vars.msg200);
		write(clientdata->clientsock, replyheader, len);

		sendpage(clientdata, filedes);

		if (clientdata->method == METHOD_GET)
			senddirlisting(clientdata);
	}
	else {
		//free(clientdata->resource);
		//clientdata->resource = indexPage;
		clientdata->resource = realloc(clientdata->resource, strlen(indexPage)+1);
		strncpy(clientdata->resource, indexPage, strlen(indexPage)+1);
		len = snprintf(replyheader, MAXBUF, "HTTP/%i.%i 200 %s\r\n", clientdata->version[0], clientdata->version[1], config_vars.msg200);
		write(clientdata->clientsock, replyheader, len);
		getcontenttype(clientdata);

		sendserverinfo(clientdata);
		sendcontenttype(clientdata);
		sendcontentlength(clientdata);
		write(clientdata->clientsock, "\r\n", 2);

		if (clientdata->method == METHOD_GET)
			sendbasiccontent(clientdata, filedes);

		fclose(filedes);
	}
}

void send_error_page(struct clientdata_h *clientdata) {
	int len;
	char replyheader[MAXBUF];
	FILE *errorPage;

	free(clientdata->resource);
	clientdata->resource = malloc(MAXBUF);
	//memset(clientdata->resource, 0, MAXBUF);
	snprintf(clientdata->resource, MAXBUF, "%s/", config_vars.errorroot);

	//memset(replyheader, 0, MAXBUF);

	if (errno == ENOENT) {
		len = snprintf(replyheader, MAXBUF, "HTTP/%i.%i 404 %s\r\n", clientdata->version[0], clientdata->version[1], config_vars.msg404);
		strncat(clientdata->resource, config_vars.page404, strlen(config_vars.page404)+1);
	}

	else if (errno == EACCES) {
		len = snprintf(replyheader, MAXBUF, "HTTP/%i.%i 403 %s\r\n", clientdata->version[0], clientdata->version[1], config_vars.msg403);
		strncat(clientdata->resource, config_vars.page403, strlen(config_vars.page403)+1);
	}

	getcontenttype(clientdata);

	write(clientdata->clientsock, replyheader, len);
	sendserverinfo(clientdata);
	sendcontenttype(clientdata);
	write(clientdata->clientsock, "\r\n", 2);

	printf("the resource is |%s|\n", clientdata->resource);
	errorPage = fopen(clientdata->resource, "r");
	sendbasiccontent(clientdata, errorPage);
	fclose(errorPage);
}

void send_501(struct clientdata_h *clientdata, char *line1) {
	char senddata[MAXBUF];
	int len;

	len = snprintf(senddata, MAXBUF, "HTTP/%i.%i 501 %s\r\n", clientdata->version[0], clientdata->version[1], config_vars.msg501);
	write(clientdata->clientsock, senddata, len);

	len = snprintf(senddata, MAXBUF, "Content-type: text/html\r\n\r\n");
	write(clientdata->clientsock, senddata, len);

	len = snprintf(senddata, MAXBUF, "<TITLE>501 %s</TITLE>\r\n501 %s", config_vars.msg501, config_vars.msg501);
	write(clientdata->clientsock, senddata, len);

	exit(0);
}

void send_400(struct clientdata_h *clientdata, char *line1) {
	char senddata[MAXBUF];
	int len;

	len = snprintf(senddata, MAXBUF, "<TITLE>400 %s</TITLE>Error 400: %s: Your request was not understood<br>\r\n<i>%s", config_vars.msg400, config_vars.msg400, line1);
	write(clientdata->clientsock, senddata, len);

	exit(0);
}

void senddirlisting(struct clientdata_h *clientdata) {
	DIR *dir;
	struct dirent *entry;
	char buf[MAXBUF];
	int len;
	struct stat fileinfo;
	char filetmp[MAXBUF];

	printf("Beginning of senddirlisting\n");

	dir = opendir(clientdata->resource);

	// will fix later
	len = snprintf(buf, MAXBUF, "<TITLE>Directory: %s</TITLE>\r\n<PRE><CODE>\r\n", clientdata->requested); 
	send(clientdata->clientsock, buf, len, 0);

	while( (entry = readdir(dir)) ) {
		snprintf(filetmp, MAXBUF, "%s%s", clientdata->resource, entry->d_name);
		lstat(filetmp, &fileinfo);
			if (S_ISDIR(fileinfo.st_mode)==1) { // Directory
				if (clientdata->requested[1] == '\0')
					len = snprintf(buf, MAXBUF, "<I><A HREF=\"%s\">%s</I>\r\n", entry->d_name, entry->d_name);
				else
					len = snprintf(buf, MAXBUF, "<I><A HREF=\"/%s/%s\">%s</I>\r\n", clientdata->requested, entry->d_name, entry->d_name);
			}
			else { // not a directory
				if (clientdata->requested[1] == '\0')
					len = snprintf(buf, MAXBUF, "<B><A HREF=\"%s\">%s</B>\r\n", entry->d_name, entry->d_name);
				else
					len = snprintf(buf, MAXBUF, "<B><A HREF=\"%s/%s\">%s</B>\r\n", clientdata->requested, entry->d_name, entry->d_name);
					
			}
		send(clientdata->clientsock, buf, len, 0);
	}

	len = snprintf(buf, MAXBUF, "</PRE></CODE>");
	send(clientdata->clientsock, buf, len, 0);

	closedir(dir);
}
