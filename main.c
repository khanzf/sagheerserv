#include "httpd.h"
#include "config.h"
#include "modules.h"

int main(int argc, char **argv) {
	int c;

	while( (c = getopt(argc, argv, "sbuc:m:") ) != -1) {
		switch(c) {
			case 'b':
				if (fork()) // exits the parent
					return 0;
					break;
			case 'c':
				if (optarg == NULL) {
					usage(argv[0]);
					return 0;
				}
				//config_default_path = optarg;
				config_default_path = malloc(strlen(optarg));
				strncpy(config_default_path, optarg, strlen(optarg));
				break;
			case 'm':
				if (optarg == NULL) {
					usage(argv[0]);
					return 0;
				}

				moduleFileNames = realloc(moduleFileNames, sizeof(char *) * (numMods+1) );
				moduleFileNames[numMods] = malloc( strlen(optarg) + 1 );
				strncpy(moduleFileNames[numMods], optarg, strlen(optarg)+1);
				numMods++;

				//addModule(optarg);
				break;
			case 's':
				singleProcess = 1;
				break;
			case 'u':
			case '?':
			default:
				printf("Invalid option %c\n", c);
				usage(argv[0]); 
				break;
		}
	}
	realmain(argc, argv);
	return 0;
}
