/*
	Someone please give me some better configuration file code!
	I was never good at writing this sort of code.  Telling me HOW
	to write it would be good enough!  email me at
	fkhani (at) gmu.edu
*/ 

#include "config.h"

unsigned int singleProcess = 0;

char *config_default_path = "httpd.conf";

FILE *configfile;

void configError(char *var) {
	fprintf(stderr, "Unable to find %s in %s\nExit -1\n", var, config_default_path);
	exit(-1);
}

void set_variables() {
	if (initialize_config(NULL)) {
		fprintf(stderr, "Unable to open %s\nExit -1\n", config_default_path);
		exit(-1);
	}

	config_vars.port = get_int("port");
	if (config_vars.port == -1)
		configError("port");

	config_vars.wwwroot = get_string("wwwroot");
	if (config_vars.wwwroot == NULL)
		configError("wwwroot");

	config_vars.msg200 = get_string("msg200");
	if (config_vars.msg200 == NULL)
		configError("msg200");

	config_vars.msg400 = get_string("msg400");
	if (config_vars.msg400 == NULL)
		configError("msg400");

	config_vars.msg404 = get_string("msg404");
	if (config_vars.msg404 == NULL)
		configError("msg404");

	config_vars.msg403 = get_string("msg403");
	if (config_vars.msg403 ==  NULL)
		configError("msg403");

	config_vars.msg501 = get_string("msg501");
	if (config_vars.msg501 == NULL)
		configError("msg501");

	config_vars.dirIndex = get_string("dirIndex");
	if (config_vars.dirIndex == NULL)
		configError("dirIndex");

	config_vars.errorroot = get_string("errorroot");
	if (config_vars.errorroot == NULL)
		configError("errorroot");

	config_vars.page403 = get_string("page403");
	if (config_vars.page403 == NULL)
		configError("page403");

	config_vars.page404 = get_string("page404");
	if (config_vars.page404 == NULL)
		configError("page404");

}

int initialize_config(char *config_path) {
	if (config_path != NULL) {
		configfile = fopen(config_path, "r");
	}
	else {
		configfile = fopen(config_default_path, "r");
	}

	if (configfile == NULL)
		return -1;
	return 0;
}

int get_int(char *strname) {
	char line[1024];
	char *tmp;
	int port;
	int len;

	(void)fseek(configfile, 0L, SEEK_SET);
	len = strlen(strname);


	tmp = malloc(len+1);
	strncpy(tmp, strname, len);
	tmp[len] = ' ';

	while(fgets(line, 1024, configfile)) {
		if (!strncmp(tmp, line, len+1)) {
			port = atoi(line+len+1);
			free(tmp);
			return port;
		}
	}
	free(tmp);
	return -1;
}


char *get_string(char *strname) {
	char *tmp;
	char line[1024];
	char *line0;
	int len;
	int linelen;

	len = strlen(strname);

	(void)fseek(configfile, 0, SEEK_SET);
	tmp = malloc(len+3);
	strncpy(tmp, strname, len);
	tmp[len] = ' ';
	tmp[len+1] = '\0';

	while( fgets(line, 1024, configfile) ) {
		linelen = strlen(line);
		if ( ! strncmp(line, tmp, len+1) ) {
			line0 = malloc(linelen);
			strncpy(line0, line+len+1, (linelen-len)-1 );

			line0[(linelen-len)-2] = '\0';
			free(tmp);
			return line0;
		}
	}

	return NULL;
}

struct mimetype *fill_types() {
	FILE			*mimefile;
	char			*line;
	char			*ptr;
	int			counter;
	//int			num_types;
	int			spaces;
	//struct mimetype *	mimetypes;

	spaces = 0;
	num_types = 0;
	mimefile = fopen("mime.types", "r");
	line = malloc(1024);
	mimetypes = malloc(1);

	while ( (fgets(line, 1024, mimefile)) ) {
		if (line[0] == '#')
			continue;

		line[strlen(line)-1] = '\0';

		num_types++;
		mimetypes = realloc(mimetypes, num_types * sizeof(struct mimetype));

		if ( !strncmp(line, "application/", 12) ) {
			mimetypes[num_types-1].majortype = T_APPL;
			ptr = line + 12;
		}
		else if ( !strncmp(line, "audio/", 6) ) {
			mimetypes[num_types-1].majortype = T_AUDI;
			ptr = line + 6;
		}
		else if ( !strncmp(line, "chemical/", 9) ) {
			mimetypes[num_types-1].majortype = T_CHEM;
			ptr = line + 9;
		}
		else if ( !strncmp(line, "image/", 6) ) {
			mimetypes[num_types-1].majortype = T_IMAG;
			ptr = line + 6;
		}
		else if ( !strncmp(line, "model/", 6) ) {
			mimetypes[num_types-1].majortype = T_MODE;
			ptr = line + 6;
		}
		else if ( !strncmp(line, "text/", 5) ) {
			mimetypes[num_types-1].majortype = T_TEXT;
			ptr = line + 5;
		}
		else if ( !strncmp(line, "video/", 6) ) {
			mimetypes[num_types-1].majortype = T_VIDE;
			ptr = line + 6;
		}
		else if ( !strncmp(line, "x-conference/", 13) ) {
			mimetypes[num_types-1].majortype = T_XCON; 
			ptr = line + 13;
		}

		for(counter=0;*(ptr+counter)!='\t'&&*(ptr+counter)!=' ';counter++);
		mimetypes[num_types-1].minortype = malloc(counter+2);
		strncpy(mimetypes[num_types-1].minortype, ptr, counter);
		mimetypes[num_types-1].minortype[counter]='\0';


		ptr=ptr+counter;

		for(counter=0;ptr[counter]=='\t'||ptr[counter]==' ';counter++);
		//counter++;


		ptr=ptr+counter;

		for(spaces=1,counter=0;counter<strlen(ptr);counter++){
			if (ptr[counter]==' ' || ptr[counter]=='\t')
				spaces++;
		}
		mimetypes[num_types-1].num_extensions = spaces;

		mimetypes[num_types-1].extensions = malloc( sizeof(char *) * spaces);

		for(counter=0;counter<spaces;counter++) {
			strtok(ptr, " \t");
			mimetypes[num_types-1].extensions[counter]=malloc(strlen(ptr)+1);
			strncpy(mimetypes[num_types-1].extensions[counter], ptr, strlen(ptr));
			mimetypes[num_types-1].extensions[counter][strlen(ptr)] = '\0';
			ptr=ptr+strlen(ptr)+1;
		}
	}

	free(line);
	fclose(mimefile);
        return mimetypes;
}

void content_type(char *extension, s_type *majortype, char *minortype) {
}
