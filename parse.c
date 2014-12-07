#include "parse.h"
#include "modules.h"
#include "config.h"

void getcontenttype(struct clientdata_h *clientdata) {
	char *strptr;
	char *tmpstr;
	int x;
	int y;
	size_t len;

	printf("Beginning of getcontenttype\n");

	len = strlen(clientdata->resource);

	for(strptr=clientdata->resource+len-1;*strptr!='.'&&*strptr!='/';strptr--);
	strptr++;

	len = strlen(strptr) + 1;
	tmpstr = malloc(len);
	strncpy(tmpstr, strptr, len);
	tmpstr = fulllower(tmpstr);

	for(x=0;x<num_types;x++) {
		for(y=0;y<mimetypes[x].num_extensions;y++) {
			if ( !strncmp( tmpstr, mimetypes[x].extensions[y], strlen(tmpstr))) {
				clientdata->majortype = x;
				clientdata->minortype = y;
				free(tmpstr);
				return ;
			}
		}
	}
	// This means no format for the above was found

	clientdata->majortype = -1;
	clientdata->minortype = -1;
	free(tmpstr);
}

void acceptlangheader(struct clientdata_h *clientdata, char *line) {
	char *line0;
	int tilcomma;
	int tilq;
	int x;

	printf("Beginning of acceptlangheader\n");

	line0 = line + 16;
	clientdata->acceptlangnum = 1;

	for(x=0;x<strlen(line0)+1;x++)
		if (line0[x]==',')
			clientdata->acceptlangnum++;

	clientdata->acceptlang = malloc(clientdata->acceptlangnum * sizeof(struct acceptlang_h) );

	for(x=0;x<clientdata->acceptlangnum;x++) {
		if (*line0 == ' ') // problem in rfc 2616 14.1
			line0++;
		for(tilcomma=0 ; line0[tilcomma]!=',' ; tilcomma++)
			if (line0[tilcomma] == '\0')
				break;
		clientdata->acceptlang[x].type = malloc(tilcomma+1);
		strncpy(clientdata->acceptlang[x].type, line0, tilcomma);
		clientdata->acceptlang[x].type[tilcomma] = '\0';

		clientdata->acceptlang[x].quality = 0;

		for(tilq=0 ; tilq < strlen(clientdata->acceptlang[x].type); tilq++) {
			if (!strncmp(clientdata->acceptlang[x].type+tilq, ";q=", 3)) {
				clientdata->acceptlang[x].quality = atof(clientdata->acceptlang[x].type+tilq+3);
				clientdata->acceptlang[x].type[tilq] = '\0';
			}
		}
		line0 = line0 + tilcomma + 1;
	}
}


void acceptencodingheader(struct clientdata_h *clientdata, char *line) {
	char *line0;
	int tilcomma;
	int tilq;
	int x;

	printf("Beginning of acceptencodingheader\n");

	line0 = line + 16;
	clientdata->acceptencodingnum = 1;

	for(x=0;x<strlen(line0)+1;x++)
		if (line0[x]==',')
			clientdata->acceptencodingnum++;

	clientdata->acceptencoding = malloc(clientdata->acceptencodingnum * sizeof(struct acceptencoding_h) );

	for(x=0;x<clientdata->acceptencodingnum;x++) {
		if (*line0 == ' ') // problem in rfc 2616 14.1
			line0++;
		for(tilcomma=0 ; line0[tilcomma]!=',' ; tilcomma++)
			if (line0[tilcomma] == '\0')
				break;
		clientdata->acceptencoding[x].type = malloc(tilcomma+1);
		strncpy(clientdata->acceptencoding[x].type, line0, tilcomma);
		clientdata->acceptencoding[x].type[tilcomma] = '\0';

		clientdata->acceptencoding[x].quality = 0;

		for(tilq=0 ; tilq < strlen(clientdata->acceptencoding[x].type); tilq++) {
			if (!strncmp(clientdata->acceptencoding[x].type+tilq, ";q=", 3)) {
				clientdata->acceptencoding[x].quality = atof(clientdata->acceptencoding[x].type+tilq+3);
				clientdata->acceptencoding[x].type[tilq] = '\0';
			}
		}
		line0 = line0 + tilcomma + 1;
	}
}

void acceptcharsetheader(struct clientdata_h *clientdata, char *line) {
	char *line0;
	int tilcomma;
	int tilq;
	int x;

	printf("Beginning of acceptcharsetheader\n");

	line0 = line + 15;
	clientdata->acceptcharsetnum = 1;

	for(x=0;x<strlen(line0)+1;x++)
		if (line0[x]==',')
			clientdata->acceptcharsetnum++;

	clientdata->acceptcharset = malloc(clientdata->acceptcharsetnum * sizeof(struct acceptcharset_h) );

	for(x=0;x<clientdata->acceptcharsetnum;x++) {
		if (*line0 == ' ') // problem in rfc 2616 14.1
			line0++;
		for(tilcomma=0 ; line0[tilcomma]!=',' ; tilcomma++)
			if (line0[tilcomma] == '\0')
				break;
		clientdata->acceptcharset[x].type = malloc(tilcomma+1);
		strncpy(clientdata->acceptcharset[x].type, line0, tilcomma);
		clientdata->acceptcharset[x].type[tilcomma] = '\0';

		clientdata->acceptcharset[x].quality = 0;

		for(tilq=0 ; tilq < strlen(clientdata->acceptcharset[x].type); tilq++) {
			if (!strncmp(clientdata->acceptcharset[x].type+tilq, ";q=", 3)) {
				clientdata->acceptcharset[x].quality = atof(clientdata->acceptcharset[x].type+tilq+3);
				clientdata->acceptcharset[x].type[tilq] = '\0';
			}
		}
		line0 = line0 + tilcomma + 1;
	}
}

void acceptheader(struct clientdata_h *clientdata, char *line) {
	char *line0;
	int tilcomma;
	int tilq;
	int x;

	printf("Beginning of acceptheader\n");

	line0 = line + 7;
	clientdata->acceptnum = 1;

	for(x=0;x<strlen(line0)+1;x++)
		if (line0[x]==',')
			clientdata->acceptnum++;

	clientdata->accept = malloc(clientdata->acceptnum * sizeof(struct accept_h) );

	for(x=0;x<clientdata->acceptnum;x++) {
		if (*line0 == ' ') // problem in rfc 2616 14.1
			line0++;
		for(tilcomma=0 ; line0[tilcomma]!=',' ; tilcomma++)
			if (line0[tilcomma] == '\0')
				break;
		clientdata->accept[x].type = malloc(tilcomma+1);
		strncpy(clientdata->accept[x].type, line0, tilcomma);
		clientdata->accept[x].type[tilcomma] = '\0';

		clientdata->accept[x].quality = 0;
		clientdata->accept[x].level = 0;

		for(tilq=0 ; tilq < strlen(clientdata->accept[x].type); tilq++) {
			if (!strncmp(clientdata->accept[x].type+tilq, ";q=", 3)) {
				clientdata->accept[x].quality = atof(clientdata->accept[x].type+tilq+3);
				clientdata->accept[x].type[tilq] = '\0';
			}
			if (!strncmp(clientdata->accept[x].type+tilq, ";level=", 7)) {
				clientdata->accept[x].quality = atof(clientdata->accept[x].type+tilq+7);
				clientdata->accept[x].type[tilq] = '\0';
			}
		}
		line0 = line0 + tilcomma + 1;
	}
}

void getversion(versionnum *version, char *line) {
	char *line0;
	char *original;
	int x;

	printf("Beginning of getversion\n");

	line0 = malloc(strlen(line)+1);
	original = line0;
	strncpy(line0, line, strlen(line)+1);
	line0 = fullupper(line0);

	for(;strncmp(line0, "HTTP/", 5);line0++);
	line0 = line0 + 5;

	for(x=0;*(line0+x)!='.';x++);
	line0[x] = '\0';

	version[0] = atoi(line0);
	version[1] = atoi(line0+x+1);

	free(original);
}

char *geturl(methodtype method, char *line) {
	char *url; // the final url
	char *tmp; // used only for string manipulation
	int len;
	int x;

	printf("Beginning of geturl\n");

	switch(method) {
		case METHOD_OPTIONS:
			len = 8;
			break;
		case METHOD_GET:
			len = 4;
			break;
		case METHOD_HEAD:
			len = 5;
			break;
		case METHOD_POST:
			len = 5;
			break;
		case METHOD_PUT:
			len = 4;
			break;
		case METHOD_DELETE:
			len = 7;
			break;
		case METHOD_TRACE:
			len = 6;
			break;
		case METHOD_CONNECT:
			len = 8;
			break;
	}

	url = malloc(strlen(line+len) * sizeof(char)); // for the non-x86's
	strncpy(url, line+len, strlen(line+len));
	tmp = url;

	for(x=1;tmp[x]!=' ';x++) {
		if (tmp[x]=='\0')
			return NULL;
	}

	tmp[x]='\0';

	return url;
}


methodtype getmethod(char *line) {
	char *tmp;
	size_t len;

	printf("Beginning of getmethod\n");

	len = strlen(line);

	tmp = malloc(len+1);
	strncpy(tmp, line, len);

	tmp = fullupper(tmp);

	printf("Getmethod's line is %s\n", line);

	if (!strncmp(tmp, "OPTIONS", 7)) {
		free(tmp);
		return METHOD_OPTIONS;
	}
	else if(!strncmp(tmp, "GET", 3)) {
		free(tmp);
		return METHOD_GET;
	}
	else if (!strncmp(tmp, "HEAD", 4)) {
		free(tmp);
		return METHOD_HEAD;
	}
	else if (!strncmp(tmp, "POST", 4)) {
		free(tmp);
		return METHOD_POST;
	}
	else if (!strncmp(tmp, "PUT", 3)) {
		free(tmp);
		return METHOD_PUT;
	}
	else if (!strncmp(tmp, "DELETE", 6)) {
		free(tmp);
		return METHOD_DELETE;
	}
	else if (!strncmp(tmp, "TRACE", 5)) {
		free(tmp);
		return METHOD_TRACE;
	}
	else if (!strncmp(tmp, "CONNECT", 7)) {
		free(tmp);
		return METHOD_CONNECT;
	}

	free(tmp);
	return ERROR;
}


void gethost(struct clientdata_h *clientdata, char *line) {
	clientdata->host = line + 6;
}

void getkeepalive(struct clientdata_h *clientdata, char *line) {
	clientdata->keepalive = atoi(line+11);
}

void getuseragent(struct clientdata_h *clientdata, char *line) {
	clientdata->useragent = line + 12;
}

void getreferer(struct clientdata_h *clientdata, char *line) {
	clientdata->referer = line + 10;
}

void getfrom(struct clientdata_h *clientdata, char *line) {
	char *line0;

	line0 = line + 5;
	if (*line0 == ' ')
		line0++;

	clientdata->from = line0;
}

void getconnection(struct clientdata_h *clientdata, char *line) {
	char *line0;

	line0 = line + 12;

	if ( !strncmp("close", line0, 5) )
		clientdata->connecttype = CT_CLOSE;
	else if ( !strncmp("keepalive", line0, 9) )
		clientdata->connecttype = CT_KEEPALIVE;
}

void fillheaders(struct clientdata_h *clientdata, int number_of_lines, char **fullrequestbroken) {
	printf("Beginning of fillheader\n");
	char *tmpline;
	int x = 0;
	int modReturn;

	tmpline = NULL;
	modReturn = M_DECLINE;

	clientdata->method = getmethod(fullrequestbroken[0]);
	if (clientdata->method == ERROR)
		send_501(clientdata, fullrequestbroken[0]);

	clientdata->url = geturl(clientdata->method, fullrequestbroken[0]);
	if (clientdata->url == NULL)
		send_400(clientdata, fullrequestbroken[0]);

	getversion(clientdata->version, fullrequestbroken[0]);

	for(x=0;x<number_of_lines;x++) {
		tmpline = realloc(tmpline, strlen(fullrequestbroken[x])+1 );
		tmpline = strncpy(tmpline, fullrequestbroken[x], strlen(fullrequestbroken[x])+1 );
		tmpline = fullupper(tmpline);

		if (moduleLink) {
			modReturn = runphasefilltypes(clientdata, fullrequestbroken[x]);
		}

		if (modReturn == M_DECLINE) {
			if ( !strncmp("HOST:", tmpline, 5) ) {
				gethost(clientdata, fullrequestbroken[x]);
			}
			else if ( !strncmp("REFERER:", tmpline, 8) ) {
				getreferer(clientdata, fullrequestbroken[x]);
			}
			else if ( !strncmp("FROM:", tmpline, 5) ) {
				getfrom(clientdata, fullrequestbroken[x]);
			}
			else if ( !strncmp("USER-AGENT:", tmpline, 11) ) {
				getuseragent(clientdata, fullrequestbroken[x]);
			}
			else if ( !strncmp("KEEP-ALIVE:", tmpline, 11) ) {
				getkeepalive(clientdata, fullrequestbroken[x]);
			}
			else if ( !strncmp("CONNECTION:", tmpline, 11) ) {
				getconnection(clientdata, fullrequestbroken[x]);
			}
			else if ( !strncmp("ACCEPT:", tmpline, 7) ) {
				acceptheader(clientdata, fullrequestbroken[x]);
			}
			else if ( !strncmp("ACCEPT-CHARSET:", tmpline, 15) ) {
				acceptcharsetheader(clientdata, fullrequestbroken[x]);
			}
			else if ( !strncmp("ACCEPT-ENCODING:", tmpline, 16) ) {
				acceptencodingheader(clientdata, fullrequestbroken[x]);
			}
			else if ( !strncmp("ACCEPT-LANGUAGE:", tmpline, 16) ) {
				acceptlangheader(clientdata, fullrequestbroken[x]);
			}
		}

		tmpline = realloc(tmpline, 0);
	}
	free(tmpline);
}

void truepath(struct clientdata_h *clientdata) {
	char *new;
	char *tmpupper;
	char *file;
	size_t len;
	int numslash;
	int dirout;
	int x;
	int y;
	char *pathreal;
	char *finalpath; // this is what happens when you don't plan ahead

	printf("Beginning of truepath\n");

	len = strlen(clientdata->url);
	new = malloc(len+1);
	strncpy(new, clientdata->url, len+1);
	file = new;

	tmpupper = malloc( len+1 );
	strncpy(tmpupper, clientdata->url, len+1 );
	tmpupper = fullupper(tmpupper);

	if (!strncmp(tmpupper, "HTTP://", 7))
		file = file + 7;
	free(tmpupper);

	for(;*file!='/' && *file;file++);

	numslash = 0;
	dirout = 0;
	len = strlen(file);

	pathreal = malloc(len+1);
	pathreal = memset(pathreal, 0, len+1);

	for(x=0;x<len;x++)
		if (file[x] == '/')
			numslash++;

	for(x=0;x<numslash;x++) {
		if (dirout == 0 && !strncmp("/../", file, 4) ) {
			file = file + 3;
		}
		else if (dirout > 0 && !strncmp("/../", file, 4) ) {
			strncat(pathreal, "/..", 3);
			file = file + 3;
			dirout--;
		}
		else {
			for(y=1;file[y]!='/' && file[y]!= '\0';y++);
			strncat(pathreal, file, y);
			file = file + y;
			dirout++;
		}
	}

	clientdata->requested = pathreal;

	finalpath = malloc(strlen(config_vars.wwwroot) + strlen(pathreal) + 2);
	snprintf(finalpath, strlen(config_vars.wwwroot) + strlen(pathreal) + 2, "%s%s", config_vars.wwwroot,  pathreal);
	/*
	memset(finalpath, 0, strlen(config_vars.wwwroot) + strlen(pathreal)+2 );

	strncpy(finalpath, config_vars.wwwroot, strlen(config_vars.wwwroot));
	strncat(finalpath, pathreal, strlen(pathreal));
	*/

	clientdata->resource = finalpath;
	printf("The final product is |%s|\n", clientdata->resource);

	free(new);
}
