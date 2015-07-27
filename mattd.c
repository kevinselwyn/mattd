#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <arpa/inet.h>

char *name = "Matt Daemon",
	 *red = "\033[0;31m",
	 *green = "\033[0;32m",
	 *normal = "\033[0;0m";

static pid_t pidof(char *exec) {
	size_t length = 0, offset = 0;
	char *cmd_str = NULL, *pid_str = NULL;
	FILE *cmd = NULL;
	pid_t pid = 0;

	length = 41 + strlen(exec);

	if (strncmp(exec, "./", 2) == 0) {
		offset = 2;
		length -= 2;
	}

	cmd_str = malloc(sizeof(char) * (length + 1));
	sprintf(cmd_str, "ps axc | awk \"{if (\\$5==\\\"%s\\\") print \\$1}\"", exec + offset);
	cmd = popen(cmd_str, "r");

	length = 6;
	pid_str = malloc(sizeof(char) * (length + 1));

	fread(pid_str, 1, length, cmd);

	sscanf(pid_str, "%d", &pid);

	return pid;
}

static int usage(char *exec) {
	int rc = 0;

	printf("Usage: %s [start|stop|restart|status]\n", exec);

	return rc;
}

static int status(char *exec) {
	int rc = 0;

	rc = pidof(exec) == getpid() ? 1 : 0;

	return rc;
}

static int start(char *exec) {
	int rc = 0;
	pid_t pid = 0, sid = 0;

	if (status(exec) == 0) {
		printf("%s is already running.\n", name);

		rc = 1;
		goto cleanup;
	}

	printf("Starting %s... %s[ OK ]%s\n", name, green, normal);

	pid = fork();

	if (pid < 0) {
		rc = 1;
		goto cleanup;
	}

	if (pid > 0) {
		rc = 0;
		goto cleanup;
	}

	umask(0);

	sid = setsid();

	if (sid < 0) {
		rc = 1;
		goto cleanup;
	}

	if ((chdir("/")) < 0) {
		rc = 1;
		goto cleanup;
	}

	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);

	while (1) {
		sleep(60);
	}

cleanup:
	return rc;
}

static int stop(char *exec) {
	int rc = 0;
	char *cmd = NULL;
	pid_t pid = 0;

	if (status(exec) == 1) {
		printf("%s is not running.\n", name);

		rc = 1;
		goto cleanup;
	}

	if (getuid() != 0) {
		printf("You can't stop %s. Permission denied. %s[ FAILED ]%s\n", name, red, normal);

		rc = 1;
	} else {
		printf("Stopping %s... %s[ OK ]%s\n", name, green, normal);

		pid = pidof(exec);

		cmd = malloc(sizeof(char) * 12);
		sprintf(cmd, "kill %d", pid);
		system(cmd);
	}

cleanup:
	if (cmd) {
		free(cmd);
	}

	return rc;
}

static int restart(char *exec) {
	int rc = 0;

	if (status(exec) == 1) {
		printf("%s is not running.\n", name);

		rc = 1;
		goto cleanup;
	}

	printf("%s is restarting.\n", name);

	rc = stop(exec);

	if (rc == 0) {
		start(exec);
	}

cleanup:
	return rc;
}

int main(int argc, char *argv[]) {
	int rc = 0;
	char *exec = NULL, *action = NULL;

	exec = argv[0];

	if (argc > 1) {
		action = argv[1];
	} else {
		rc = usage(exec);
		goto cleanup;
	}

	if (strcmp(action, "start") == 0) {
		rc = start(exec);
	} else if (strcmp(action, "stop") == 0) {
		rc = stop(exec);
	} else if (strcmp(action, "restart") == 0 || strcmp(action, "reload") == 0) {
		rc = restart(exec);
	} else if (strcmp(action, "status") == 0) {
		rc = status(exec);

		printf("%s is %srunning.\n", name, rc == 1 ? "not " : "");
	}

cleanup:
	return rc;
}