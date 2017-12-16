#include <cstdio>
#include <cstdint>
#include <cstring>

#include <unistd.h>

#include "b64.h"
#include "firefox.h"

/*
	Options
	-p, --path [path] - profile path
	-b, --browser [type] - profile type "firefox" or "chrome"
	-r, --report [type] - report type "plain", "html" or "csv"
	-o, --outfile [file] - save report to [file]
	-v, --verbose - prints out the first 20 lines of user/password, history, and autocomplete lists
	-h, --help - prints help
*/

void printHelp();
bool streqi(const char *str1, const char *str2);
int fatalError(const char *str);

uint32_t argFlags = 0;

#define F_PATH_SET 		(1 << 0)
#define F_BROWSER_FF	(1 << 1) // Firefox
#define F_BROWSER_GC	(1 << 2) // Chrome
#define F_REPORT_PLAIN	(1 << 3)
#define F_REPORT_HTML	(1 << 4)
#define F_REPORT_CSV	(1 << 5)
#define F_OUTFILE_SET	(1 << 6)
#define F_VERBOSE		(1 << 7)

char profilePath[1024] = {0};
char outfilePath[1024] = {0};
uint8_t reportType = 0;
uint8_t profileType = 0;

int main(int argc, char **argv)
{
	if(argc <= 1)
	{
		printHelp();
		return 0;
	}

	for(uint32_t i = 1; i < argc; i++)
	{
		// -p, --path
		if(streqi(argv[i], "-p") || streqi(argv[i], "--path"))
		{
			if((argFlags & F_PATH_SET) != 0)
			{
				fprintf(stderr, "Path is set more than once! (-p, --path)\n");
				return -1;
			}

			if(i == (argc - 1) || argv[i+1][0] == '-')
			{
				fprintf(stderr, "--path given, but no path was provided\n");
				return -1;
			}

			strncpy(profilePath, argv[i+1], 1024);
			argFlags |= F_PATH_SET;

			i++;
			continue;
		}

		// -b, --browser
		if(streqi(argv[i], "-b") || streqi(argv[i], "--browser"))
		{
			if((argFlags & F_BROWSER_GC) != 0 || (argFlags & F_BROWSER_FF) != 0)
			{
				fprintf(stderr, "Browser type is set more than once! (-b, --browser)\n");
				return -1;
			}

			if(i == (argc - 1) || argv[i+1][0] == '-')
			{
				fprintf(stderr, "--browser given, but no browser type was provided\n");
				return -1;
			}

			i++;

			if(streqi(argv[i], "firefox"))
			{
				argFlags |= F_BROWSER_FF;
				profileType = F_BROWSER_FF;
			} else if(streqi(argv[i], "chrome")) {
				argFlags |= F_BROWSER_GC;
				profileType = F_BROWSER_GC;
			} else {
				fprintf(stderr, "Invalid browser type passed to --browser\n");
				return -1;
			}

			continue;
		}

		//-r, --report
		if(streqi(argv[i], "-r") || streqi(argv[i], "--report"))
		{
			if(reportType != 0)
			{
				fprintf(stderr, "Report type is set more than once! (-r, --report)\n");
				return -1;
			}

			if(i == (argc - 1) || argv[i+1][0] == '-')
			{
				fprintf(stderr, "--report given, but no report type was provided\n");
				return -1;
			}

			i++;

			if(streqi(argv[i], "plain"))
			{
				reportType = F_REPORT_PLAIN;
			} else if(streqi(argv[i], "html")) {
				reportType = F_REPORT_HTML;
			} else if(streqi(argv[i], "csv")) {
				reportType = F_REPORT_CSV;
			} else {
				fprintf(stderr, "Invalid report type passed to --report\n");
				return -1;
			}

			continue;
		}

		// -o, --outfile
		if(streqi(argv[i], "-o") || streqi(argv[i], "--outfile"))
		{
			if((argFlags & F_OUTFILE_SET) != 0)
			{
				fprintf(stderr, "Outfile is set more than once! (-o, --outfile)\n");
				return -1;
			}

			if(i == (argc - 1) || argv[i+1][0] == '-')
			{
				fprintf(stderr, "--outfile given, but no outfile path was provided\n");
				return -1;
			}

			strncpy(outfilePath, argv[i+1], 1024);
			argFlags |= F_OUTFILE_SET;
			i++;
			continue;
		}

		// -v, --verbose
		if(streqi(argv[i], "-v") || streqi(argv[i], "--verbose"))
		{
			if((argFlags & F_VERBOSE) != 0)
				fprintf(stderr, "You specified --verbose more than once, but there is only one verbosity level. Just FYI.\n");
			
			argFlags |= F_VERBOSE;
			continue;
		}

		// -h, --help
		if(streqi(argv[i], "-h") || streqi(argv[i], "--help"))
		{
			printHelp();
			return 0;
		}

		// If we reach this point, it means an invalid parameter was passed.
		fprintf(stderr, "Invalid paremeter given: \"%s\"\n", argv[i]);
		fprintf(stderr, "For usage information, try \"%s --help\"\n\n", argv[0]);
		return -1;
	}

	if((argFlags & F_PATH_SET) == 0)
		return fatalError("You must pass a profile path using --path!\n");

	if((argFlags & F_OUTFILE_SET) != 0 && reportType == 0)
		return fatalError("You must specify a report type (--report) with --outfile!\n");

	if(reportType != 0 && (argFlags & F_OUTFILE_SET) == 0)
		return fatalError("You must specify an outfile (--outfile) with --report!\n");

	if(profileType == 0)
		return fatalError("You must provide a profile type with --browser!\n");


	if((argFlags & F_BROWSER_FF) != 0)
	{
		Firefox ff(profilePath);
		ff.buildData(BR_GET_ALL);

		if((argFlags & F_VERBOSE) != 0)
			ff.printReport();
	}

	return 0;
}

int fatalError(const char *str)
{
	fprintf(stderr, str);
	return -1;
}

void printHelp()
{
	printf("Usage: bbandit [options]\n\n");
	printf("BrowserBandit is a utility to glean information from a browser profile. Given a path to a Mozilla Firefox or Google Chrome ");
	printf("browser profile, BrowserBandit will attempt to dump the saved user/pass combos, history list, and autocomplete list to a file.");
	printf(" Dumping the user/pass combos from Google Chrome is currently not possible using this software.\n\n");

	printf("Options:\n");
	printf("\t-p, --path [path]    - Path to the browser profile\n");
	printf("\t-b, --browser [type] - Browser type. Either \"firefox\" or \"chrome\"\n");
	printf("\t-r, --report [type]  - Output report type. Either \"plain\", \"html\", or \"csv\"\n");
	printf("\t-o, --outfile [path] - Output file path\n");
	printf("\t-v, --verbose        - Verbose output. Will print a summary of findings to the console.\n");
	printf("\t-h, --help           - You're lookin' at it.\n\n");

	printf("\"Is that you, John Wayne? Is this me?\"\n");
}

// str-is-equal, ignore case. ASCII only.
bool streqi(const char *str1, const char *str2)
{
	uint32_t s1len, s2len;
	s1len = strlen(str1);
	s2len = strlen(str2);

	if(s1len != s2len)
		return false;

	char *s1 = new char[s1len+1];
	char *s2 = new char[s1len+1];

	strcpy(s1, str1);
	strcpy(s2, str2);

	for(uint32_t i = 0; i < s1len; i++)
	{
		if(s1[i] >= 'A' && s1[i] <= 'Z')
			s1[i] += 32;

		if(s2[i] >= 'A' && s2[i] <= 'Z')
			s2[i] += 32;

		if(s2[i] != s1[i])
		{
			delete[] s1;
			delete[] s2;

			return false;
		}
	}

	delete[] s1;
	delete[] s2;

	return true;
}