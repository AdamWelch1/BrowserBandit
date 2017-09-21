#include <cstdio>
#include <cstdint>
#include <cstring>

#include "b64.h"
#include "firefox.h"

int main(int argc, char **argv)
{
	char profilePath[] = "/media/aj/OS/Users/Adam.Welch/AppData/Roaming/Mozilla/Firefox/Profiles/g5xqkm0i.default";
	char loginsJson[] = "/media/aj/OS/Users/Adam.Welch/AppData/Roaming/Mozilla/Firefox/Profiles/g5xqkm0i.default/logins.json";
	char masterPassword[] = "password";

	Firefox ff(profilePath);

	return 0;
}

