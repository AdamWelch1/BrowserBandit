#include "firefox.h"

Firefox::Firefox(const char *pPath)
{
	if(strlen(pPath) >= 2048)
		strcpy(profilePath, "./");
	else
		strncpy(profilePath, pPath, 2047);
}

Firefox::~Firefox()
{
	for(uint32_t i = 0; i < credentialList.size(); i++)
		delete credentialList[i];

	for(uint32_t i = 0; i < historyList.size(); i++)
		delete historyList[i];
}

int Firefox::buildData(uint64_t dataGetFlags = BR_GET_ALL)
{
	if((dataGetFlags & BR_GET_CREDENTIALS) != 0)
		buildCredentialList();

	if((dataGetFlags & BR_GET_HISTORY) != 0)
		buildHistoryList();

	if((dataGetFlags & BR_GET_AUTOCOMPLETE) != 0)
		buildAutoCompleteList();
}

void Firefox::printReport()
{
	#define repeatChar(chr, count) for(uint32_t i = 0; i < count; i++) putchar(chr);

	if(credentialList.size() > 0)
	{
		printf("\n\n\n***Credentials (20 entries, max)***\n");
		uint32_t maxHostWidth = 0, maxUserWidth = 0, maxPassWidth = 0;

		for(uint32_t i = 0; i < credentialList.size(); i++)
		{
			maxHostWidth = ffMax(maxHostWidth, strlen(credentialList[i]->hostname));
			maxUserWidth = ffMax(maxUserWidth, strlen(credentialList[i]->userName));
			maxPassWidth = ffMax(maxPassWidth, strlen(credentialList[i]->passWord));
		}

		maxHostWidth = ffMin(maxHostWidth, 50);
		maxUserWidth = ffMin(maxUserWidth, 50);
		maxPassWidth = ffMin(maxPassWidth, 50);

		uint32_t rowWidth = maxHostWidth + maxUserWidth + maxPassWidth + 10;
		
		char tmpColumn[500] = {0};
		repeatChar('-', rowWidth);
		printf("\n");
		makeCenteredString("URL", tmpColumn, maxHostWidth);
		printf("| %-*s ", maxHostWidth, tmpColumn);
		makeCenteredString("User", tmpColumn, maxUserWidth);
		printf("| %-*s ", maxUserWidth, tmpColumn);
		makeCenteredString("Pass", tmpColumn, maxPassWidth);
		printf("| %-*s |\n", maxPassWidth, tmpColumn);
		repeatChar('-', rowWidth);
		printf("\n");

		for(uint32_t i = 0; i < ffMin(20, credentialList.size()); i++)
		{
			printf("| %-*.50s ", maxHostWidth, credentialList[i]->hostname);
			printf("| %-*.50s ", maxUserWidth, credentialList[i]->userName);
			printf("| %-*.50s |\n", maxPassWidth, credentialList[i]->passWord);
		}

		repeatChar('-', rowWidth);
		printf("\n\n");
	} else {

		printf("No credentials were found.\n\n");
	}

	if(historyList.size() > 0)
	{
		printf("\n\n\n***History (20 entries, max)***\n");
		uint32_t maxTitleWidth = 0, maxAddrWidth = 0, maxTimestampWidth = 0;

		for(uint32_t i = 0; i < credentialList.size(); i++)
		{
			maxTitleWidth = ffMax(maxTitleWidth, strlen(historyList[i]->title));
			maxAddrWidth = ffMax(maxAddrWidth, strlen(historyList[i]->address));
			maxTimestampWidth = ffMax(maxTimestampWidth, strlen(ctime((time_t*) &(historyList[i]->timestamp))));
		}

		maxTitleWidth = ffMin(maxTitleWidth, 50);
		maxAddrWidth = ffMin(maxAddrWidth, 50);
		maxTimestampWidth = ffMin(maxTimestampWidth, 50);

		uint32_t rowWidth = maxTitleWidth + maxAddrWidth + maxTimestampWidth + 10;
		char tmpColumn[500] = {0};

		repeatChar('-', rowWidth);
		printf("\n");
		makeCenteredString("Title", tmpColumn, maxTitleWidth);
		printf("| %-*s ", maxTitleWidth, tmpColumn);
		makeCenteredString("Address", tmpColumn, maxAddrWidth);
		printf("| %-*s", maxAddrWidth, tmpColumn);
		makeCenteredString("Date/Time", tmpColumn, maxTimestampWidth);
		printf("| %-*s |\n", maxTimestampWidth, tmpColumn);
		repeatChar('-', rowWidth);
		printf("\n");

		for(uint32_t i = 0; i < ffMin(20, historyList.size()); i++)
		{
			char *tsstr = ctime((time_t*) &(historyList[i]->timestamp));
			tsstr[strlen(tsstr)-1] = 0;

			char tmpTitle[51] = {0};
			char tmpAddr[51] = {0};
			strncpy(tmpTitle, historyList[i]->title, 51);
			strncpy(tmpAddr, historyList[i]->address, 51);

			while(strlen(tmpTitle) < 50)
				tmpTitle[strlen(tmpTitle)] = ' ';
			tmpTitle[50] = 0;

			while(strlen(tmpAddr) < 50)
				tmpAddr[strlen(tmpAddr)] = ' ';
			tmpAddr[50] = 0;

			printf("| %-50s ", tmpTitle);
			printf("| %-50s ", tmpAddr);
			printf("| %-24s |\n", tsstr);
		}

		repeatChar('-', rowWidth);
		printf("\n\n");

	} else {

		printf("No history entries were found.\n\n");
	}

	if(autocompleteList.size() > 0)
	{
		uint32_t maxFnWidth = 0, maxFvWidth = 0;

		for(uint32_t i = 0; i < autocompleteList.size(); i++)
		{
			maxFnWidth = ffMax(maxFnWidth, strlen(autocompleteList[i]->fieldName));
			maxFvWidth = ffMax(maxFvWidth, strlen(autocompleteList[i]->fieldValue));
		}

		maxFnWidth = ffMin(maxFnWidth, 100);
		maxFvWidth = ffMin(maxFvWidth, 100);

		uint32_t rowWidth = maxFnWidth + maxFvWidth + 7;
		char tmpColumn[500] = {0};

		repeatChar('-', rowWidth);
		printf("\n");
		makeCenteredString("Field Name", tmpColumn, maxFnWidth);
		printf("| %-*s ", maxFnWidth, tmpColumn);
		makeCenteredString("Value", tmpColumn, maxFvWidth);
		printf("| %-*s |\n", maxFvWidth, tmpColumn);

		repeatChar('-', rowWidth);
		printf("\n");

		for(uint32_t i = 0; i < ffMin(20, autocompleteList.size()); i++)
		{
			printf("| %-*.100s ", maxFnWidth, autocompleteList[i]->fieldName);
			printf("| %-*.100s |\n", maxFvWidth, autocompleteList[i]->fieldValue);
		}

		repeatChar('-', rowWidth);
		printf("\n\n");

	} else {

		printf("No AutoComplete entries were found.\n\n");
	}

	#undef repeatChar
}

void Firefox::saveReport(uint32_t reportType = REPORT_HTML)
{

}

bool Firefox::buildCredentialList()
{
	char loginsFile[3072] = {0};
	strcpy(loginsFile, profilePath);
	uint32_t lfLen = strlen(loginsFile);

	if(loginsFile[lfLen-1] != '/')
	{
		loginsFile[lfLen] = '/';
		loginsFile[lfLen+1] = 0;
	}

	strcat(loginsFile, "logins.json");

	FILE *logins = fopen(loginsFile, "rb");

	if(logins == 0)
	{
		printf("Failed to open logins.json.\n");
		return false;
	}

	uint64_t ljSize = 0;
	fseek(logins, 0, SEEK_END);
	ljSize = ftell(logins);
	rewind(logins);

	char *loginData = new char[ljSize+1];
	fread(loginData, ljSize, 1, logins);
	fclose(logins);

	loginData[ljSize] = 0;

	char *token = strtok(loginData, ",");

	vector<char*> hostnames;
	vector<char*> encUsers;
	vector<char*> encPasses;

	while(token != 0)
	{
		if(strncmp((token+1), "hostname\":", strlen("hostname\":")) == 0)
		{
			char *tmp = strchr(token, ':');
			tmp += 2;
			uint32_t tmpLen = strlen(tmp) - 1;

			char *hostname = new char[tmpLen+1];
			strncpy(hostname, tmp, tmpLen);
			hostname[tmpLen] = 0;

			hostnames.push_back(hostname);
		}

		if(strncmp((token+1), "encryptedUsername\":", strlen("encryptedUsername\":")) == 0)
		{
			char *tmp = strchr(token, ':');
			tmp += 2;
			uint32_t tmpLen = strlen(tmp) - 1;

			char *encUser = new char[tmpLen+1];
			strncpy(encUser, tmp, tmpLen);
			encUser[tmpLen] = 0;

			encUsers.push_back(encUser);
		}

		if(strncmp((token+1), "encryptedPassword\":", strlen("encryptedPassword\":")) == 0)
		{
			char *tmp = strchr(token, ':');
			tmp += 2;
			uint32_t tmpLen = strlen(tmp) - 1;

			char *encPass = new char[tmpLen+1];
			strncpy(encPass, tmp, tmpLen);
			encPass[tmpLen] = 0;

			encPasses.push_back(encPass);
		}

		token = strtok(0, ",");
	}

	uint32_t hnSize = hostnames.size();
	uint32_t euSize = encUsers.size();
	uint32_t epSize = encPasses.size();

	if(hnSize != euSize || hnSize != epSize || euSize != epSize)
	{
		for(uint32_t i = 0; i < hnSize; i++)
			delete[] hostnames[i];

		for(uint32_t i = 0; i < euSize; i++)
			delete[] encUsers[i];

		for(uint32_t i = 0; i < epSize; i++)
			delete[] encPasses[i];

		printf("Error: Unequal number of Hostnames, Usernames, and Passwords found in logins.json!\n");
		return false;
	}

	SECStatus nssInit = NSS_Init(profilePath);

	if(nssInit != SECSuccess)
	{
		printf("Failed to initialize NSS Library! Exiting...\n");
		return false;
	}

	PK11SlotInfo *keySlot = PK11_GetInternalKeySlot();
	SECStatus pwCheck = PK11_CheckUserPassword(keySlot, ""); // Check the default blank password first

	if(pwCheck != SECSuccess)
	{
		printf("Failed to decrypt keyfile! Exiting...\n");
		return false;
	}

	SECStatus authStatus = PK11_Authenticate(keySlot, true, 0);

	if(authStatus != SECSuccess)
	{
		printf("Failed to authenticate! Exiting...\n");
		return false;
	}

	for(uint32_t i = 0; i < hnSize; i++)
	{
		SECItem userName;
		SECItem passWord;

		char *b64User = encUsers[i];
		char *b64Pass = encPasses[i];

		char encUser[256] = {0};
		char encPass[256] = {0};

		uint32_t encUserSize = Base64::decodedSize(b64User);
		uint32_t encPassSize = Base64::decodedSize(b64Pass);

		Base64::decode(b64User, (uint8_t*) encUser);
		Base64::decode(b64Pass, (uint8_t*) encPass);

		userName.data = (uint8_t*) encUser;
		userName.len = encUserSize;
		passWord.data = (uint8_t*) encPass;
		passWord.len = encPassSize;

		SECItem decUser;
		SECItem decPass;

		SECStatus duStatus = PK11SDR_Decrypt(&userName, &decUser, 0);
		SECStatus dpStatus = PK11SDR_Decrypt(&passWord, &decPass, 0);

		if(duStatus == SECSuccess && dpStatus == SECSuccess)
		{
			CredentialEntry *ce = new CredentialEntry;

			strncpy(ce->userName, (char*) decUser.data, decUser.len);
			ce->userName[decUser.len] = 0;

			strncpy(ce->passWord, (char*) decPass.data, decPass.len);
			ce->passWord[decPass.len] = 0;

			strcpy(ce->hostname, hostnames[i]);

			credentialList.push_back(ce);
		
		} else {

			printf("Failed to decrypt credentials!\n");
		}

		delete[] encUsers[i];
		delete[] encPasses[i];
		delete[] hostnames[i];
	}

	return true;
}

bool Firefox::buildHistoryList()
{
	char placesFile[3072] = {0};
	strcpy(placesFile, profilePath);
	uint32_t pfLen = strlen(placesFile);

	if(placesFile[pfLen-1] != '/')
	{
		placesFile[pfLen] = '/';
		placesFile[pfLen+1] = 0;
	}

	strcat(placesFile, "places.sqlite");

	sqlite3 *dbHandle = 0;
	int openResult = sqlite3_open(placesFile, &dbHandle);

	if(openResult != SQLITE_OK)
	{
		const char *errMsg = sqlite3_errmsg(dbHandle);
		printf("SQLite3 Error Message: %s\n", errMsg);
		return false;
	}

	sqlite3_exec(dbHandle, "SELECT title,url,last_visit_date FROM moz_places ORDER BY last_visit_date DESC", Firefox::sqlHistoryCallback, (void*) this, 0);

	sqlite3_close(dbHandle);
	return true;
}

int Firefox::sqlHistoryCallback(void *ffObject, int numColumns, char **rowValues, char** rowHeaders)
{
	uint32_t timestamp = 0;

	if(rowValues[2] != 0)
		timestamp = static_cast<uint32_t>(strtod(rowValues[2], 0) / 1000000.0);

	((Firefox*) ffObject)->addHistoryItem(rowValues[0], rowValues[1], timestamp);

	return 0;
}

void Firefox::addHistoryItem(char *title, char *address, uint32_t timestamp)
{
	HistoryEntry *he = new HistoryEntry;

	if(title != 0)
	{
		strncpy(he->title, title, 4095);
		he->title[4095] = 0;
	} else {
		he->title[0] = 0;
	}

	if(address != 0)
	{
		strncpy(he->address, address, 4095);
		he->address[4095] = 0;
	} else {
		he->address[0] = 0;
	}

	he->timestamp = timestamp;

	historyList.push_back(he);
}

bool Firefox::buildAutoCompleteList()
{
	char formsFile[3072] = {0};
	strcpy(formsFile, profilePath);
	uint32_t ffLen = strlen(formsFile);

	if(formsFile[ffLen-1] != '/')
	{
		formsFile[ffLen] = '/';
		formsFile[ffLen+1] = 0;
	}

	strcat(formsFile, "formhistory.sqlite");

	sqlite3 *dbHandle = 0;
	int openResult = sqlite3_open(formsFile, &dbHandle);

	if(openResult != SQLITE_OK)
	{
		const char *errMsg = sqlite3_errmsg(dbHandle);
		printf("SQLite3 Error Message: %s\n", errMsg);
		return false;
	}

	sqlite3_exec(dbHandle, "SELECT fieldname,value FROM moz_formhistory", Firefox::sqlAutoCompleteCallback, (void*) this, 0);

	sqlite3_close(dbHandle);
	return true;
}

int Firefox::sqlAutoCompleteCallback(void *ffObject, int numColumns, char **rowValues, char **rowHeaders)
{
	((Firefox*) ffObject)->addAutoCompleteItem(rowValues[0], rowValues[1]);

	return 0;
}

void Firefox::addAutoCompleteItem(char *fieldName, char *fieldValue)
{
	AutoCompleteEntry *ace = new AutoCompleteEntry;

	if(fieldName != 0)
	{
		strncpy(ace->fieldName, fieldName, 255);
		ace->fieldName[255] = 0;

	} else {

		ace->fieldName[0] = 0;
	}

	if(fieldValue != 0)
	{
		strncpy(ace->fieldValue, fieldValue, 4095);
		ace->fieldValue[4095] = 0;

	} else {

		ace->fieldValue[0] = 0;
	}

	autocompleteList.push_back(ace);
}