#include <cstdio>
#include <cstdint>
#include <cstring>

#include <unistd.h>

#include "b64.h"
#include "firefox.h"

int main(int argc, char **argv)
{
	char profilePath[] = "/media/aj/OS/Users/Adam.Welch/AppData/Roaming/Mozilla/Firefox/Profiles/g5xqkm0i.default";

	char cwdPath[512] = {0};
	readlink("/proc/self/cwd", cwdPath, 512);

	printf("Path: %s\n");

	Firefox ff(profilePath);

	return 0;
}

