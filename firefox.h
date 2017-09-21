#ifndef FIREFOX_H
#define FIREFOX_H

#include "browser.h"
#include "b64.h"

#include <vector>
#include <cstring>
#include <cstdio>
#include <cstdint>
#include <cstdlib>

#include <nss/nss.h>
#include <nss/pk11pub.h>
#include <nss/pk11sdr.h>

#include <sqlite3.h>

using namespace std;

class Firefox : public Browser {
	
	public:

		Firefox(const char *pPath);
		~Firefox();

		int buildData(uint64_t dataGetFlags);
		void printReport();
		void saveReport(uint32_t reportType);

		static int sqlHistoryCallback(void *ffObject, int numColumns, char **rowValues, char** rowHeaders);
		void addHistoryItem(char *title, char *address, uint32_t timestamp);

		static int sqlAutoCompleteCallback(void *ffObject, int numColumns, char **rowValues, char **rowHeaders);
		void addAutoCompleteItem(char *fieldName, char *fieldValue);

	private:

		char profilePath[2048];

		vector<CredentialEntry*> credentialList;
		vector<HistoryEntry*> historyList;
		vector<AutoCompleteEntry*> autocompleteList;

		bool buildCredentialList();
		bool buildHistoryList();
		bool buildAutoCompleteList();
};

#endif