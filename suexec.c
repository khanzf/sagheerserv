#include <sys/types.h>
#include <pwd.h>

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char **argv) {

	if (argc < 3) {
		printf("Usage: suexec username command [envs...]\n");
	}
	else {
		char *username;
		char *script;
		char *args[2];
		char **envs;
		struct passwd *ent;

		username = argv[1];
		script = argv[2];

		args[0] = script;
		args[1] = NULL;

		envs = argv+3;

		ent = getpwnam(username);

		setgid(ent->pw_gid);
		setuid(ent->pw_uid);

		execve(script, args, envs);
	}

	return -1;
}
