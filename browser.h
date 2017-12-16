#ifndef BROWSER_H
#define BROWSER_H

#include <cstdint>
#include <cstring>

#define BR_GET_CREDENTIALS		1
#define BR_GET_HISTORY			2
#define BR_GET_AUTOCOMPLETE		4

#define BR_GET_ALL				(BR_GET_CREDENTIALS | BR_GET_HISTORY | BR_GET_AUTOCOMPLETE)

#define REPORT_HTML		1
#define REPORT_TEXT		2
#define REPORT_CSV		3

struct CredentialEntry {

	char userName[256];
	char passWord[256];
	char hostname[1024];
};

struct HistoryEntry {

	char title[4096];
	char address[4096];
	uint32_t timestamp;
};

struct AutoCompleteEntry {

	char fieldName[256];
	char fieldValue[4096];
};

class Browser {
	
	public:

		virtual int buildData(uint64_t dataGetFlags) = 0;
		virtual void printReport() = 0;
		virtual void saveReport(uint32_t reportType = REPORT_HTML) = 0;

		void makeCenteredString(const char *strIn, char *strOut, uint32_t length)
		{
			uint32_t spacesBefore = 0;
			spacesBefore = (length / 2) - (strlen(strIn) / 2);

			if(strlen(strIn) % 2 != 0)
				spacesBefore -= 1;

			for(uint32_t i = 0; i < spacesBefore; i++)
				strOut[i] = ' ';

			strcpy((strOut + spacesBefore), strIn);
		}
};

#endif