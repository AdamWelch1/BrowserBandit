#ifndef BROWSER_H
#define BROWSER_H

#include <cstdint>

#define BR_GET_CREDENTIALS		1
#define BR_GET_HISTORY			2
#define BR_GET_AUTOCOMPLETE		4

#define BR_GET_ALL				(BR_GET_CREDENTIALS | BR_GET_HISTORY | BR_GET_AUTOCOMPLETE)

#define REPORT_HTML		1
#define REPORT_TEXT		2

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

	char formField[256];
	char inputData[4096];
};

class Browser {
	
	public:

		virtual int buildData(uint64_t dataGetFlags = BR_GET_ALL) = 0;
		virtual void printReport() = 0;
		virtual void saveReport(uint32_t reportType = REPORT_HTML) = 0;

};

#endif