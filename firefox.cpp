#include "firefox.h"

Firefox::Firefox(const char *pPath)
{
	if(strlen(pPath) >= 2048)
		strcpy(profilePath, "./");
	else
		strncpy(profilePath, pPath, 2047);

	buildCredentialList();
	buildHistoryList();
	buildAutoCompleteList();
	printReport();
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

}

void Firefox::printReport()
{
	printf("\n\n\n***Credentials***\n");
	uint32_t maxHostWidth = 0, maxUserWidth = 0, maxPassWidth = 0;

	for(uint32_t i = 0; i < credentialList.size(); i++)
	{
		maxHostWidth = ffMax(maxHostWidth, strlen(credentialList[i]->hostname));
		maxUserWidth = ffMax(maxUserWidth, strlen(credentialList[i]->userName));
		maxPassWidth = ffMax(maxPassWidth, strlen(credentialList[i]->passWord));
	}

	maxHostWidth += 2;
	maxUserWidth += 2;
	maxPassWidth += 2;
	uint32_t tableWidth = maxHostWidth + maxUserWidth + maxPassWidth + 4;

	#define repeatChar(chr, count) for(uint32_t i = 0; i < count; i++) putchar(chr);
	printf(" ");
	repeatChar('-', (tableWidth - 2));
	printf(" \n");

	uint32_t padCount = (maxHostWidth - strlen("URL")) / 2;
	printf("|%*sURL", padCount, " ");
	padCount = ((maxHostWidth - strlen("URL")) % 2 != 0) ? padCount + 1 : padCount;
	printf("%*s|", padCount, " ");

	padCount = (maxUserWidth - strlen("User")) / 2;
	printf("%*sUser", padCount, " ");
	padCount = ((maxUserWidth - strlen("User")) % 2 != 0) ? padCount + 1 : padCount;
	printf("%*s|", padCount, " ");

	padCount = (maxPassWidth - strlen("Pass")) / 2;
	printf("%*sPass", padCount, " ");
	padCount = ((maxPassWidth - strlen("Pass")) % 2 != 0) ? padCount + 1 : padCount;
	printf("%*s|", padCount, " ");

	printf("\n|");
	repeatChar('-', (tableWidth - 2));
	printf("|\n");

	for(uint32_t i = 0; i < credentialList.size(); i++)
	{
		CredentialEntry *ce = credentialList[i];

		printf("| %-*s |", (maxHostWidth - 2), ce->hostname);
		printf(" %-*s |", (maxUserWidth - 2), ce->userName);
		printf(" %-*s |\n", (maxPassWidth - 2), ce->passWord);
	}
	printf(" ");
	repeatChar('-', tableWidth - 2);
	printf(" \n\n");

	printf("\n***History (First 20 entries, max)***\n");
	uint32_t historyCount = ffMin(20, historyList.size());
	uint32_t titleMaxWidth = 0, addrMaxWidth = 0, timeMaxWidth = 0;

	for(uint32_t i = 0; i < historyCount; i++)
	{
		titleMaxWidth = ffMax(titleMaxWidth, strlen(historyList[i]->title));
		addrMaxWidth = ffMax(addrMaxWidth, strlen(historyList[i]->address));

		char tmpTime[512] = {0};
		struct tm *timeData = localtime((time_t*) &(historyList[i]->timestamp));
		strftime(tmpTime, 512, "%R %x", timeData);

		timeMaxWidth = ffMax(timeMaxWidth, strlen(tmpTime));
	}

	titleMaxWidth = ffMin(60, titleMaxWidth);
	addrMaxWidth = ffMin(60, addrMaxWidth);

	titleMaxWidth += 2;
	addrMaxWidth += 2;
	timeMaxWidth += 2;

	tableWidth = titleMaxWidth + addrMaxWidth + timeMaxWidth + 4;
	printf(" ");
	repeatChar('-', tableWidth - 2);
	printf(" \n");

	padCount = (titleMaxWidth - strlen("Title")) / 2;
	printf("|%*sTitle", padCount, " ");
	padCount = ((titleMaxWidth - strlen("Title")) % 2 != 0) ? padCount + 1 : padCount;
	printf("%*s", padCount, " ");

	padCount = (addrMaxWidth - strlen("Address")) / 2;
	printf("|%*sAddress", padCount, " ");
	padCount = ((addrMaxWidth - strlen("Address")) % 2 != 0) ? padCount + 1 : padCount;
	printf("%*s", padCount, " ");

	padCount = (timeMaxWidth - strlen("Time")) / 2;
	printf("|%*sTime", padCount, " ");
	padCount = ((timeMaxWidth - strlen("Time")) % 2 != 0) ? padCount + 1 : padCount;
	printf("%*s|\n", padCount, " ");

	printf("|");
	repeatChar('-', tableWidth - 2);
	printf("|\n");

	for(uint32_t i = 0; i < historyCount; i++)
	{
		printf("| %-*.*s ", (titleMaxWidth - 2), (titleMaxWidth - 2), historyList[i]->title);
		printf("| %-*.*s ", (addrMaxWidth - 2), (addrMaxWidth - 2), historyList[i]->address);

		char tmpTime[512];
		struct tm *timeData = localtime((time_t*) &(historyList[i]->timestamp));
		strftime(tmpTime, 512, "%R %x", timeData);

		printf("| %-*s |\n", (timeMaxWidth - 2), tmpTime);
	}

	printf(" ");
	repeatChar('-', tableWidth - 2);
	printf(" \n\n");

	printf("\n***Form AutoComplete List (First 20 entries, max)***\n");
	uint32_t formlistCount = ffMin(20, autocompleteList.size());
	uint32_t fieldNameMax = 0, fieldValueMax = 0;

	for(uint32_t i = 0; i < formlistCount; i++)
	{
		fieldNameMax = ffMax(fieldNameMax, strlen(autocompleteList[i]->fieldName));
		fieldValueMax = ffMax(fieldValueMax, strlen(autocompleteList[i]->fieldValue));
	}

	fieldNameMax = ffMax(fieldNameMax, strlen(" Field Name "));
	fieldValueMax = ffMax(fieldValueMax, strlen(" Value "));

	fieldNameMax = ffMin(60, fieldNameMax) + 2;
	fieldValueMax = ffMin(60, fieldValueMax) + 2;

	tableWidth = fieldNameMax + fieldValueMax + 3;
	printf(" ");
	repeatChar('-', tableWidth - 2);
	printf(" \n");

	padCount = (fieldNameMax - strlen("Field Name")) / 2;
	printf("|%*sField Name", padCount, " ");
	padCount = ((fieldNameMax - strlen("Field Name")) % 2 != 0) ? padCount + 1 : padCount;
	printf("%*s", padCount, " ");

	padCount = (fieldValueMax - strlen("Value")) / 2;
	printf("|%*sValue", padCount, " ");
	padCount = ((fieldValueMax - strlen("Value")) % 2 != 0) ? padCount + 1 : padCount;
	printf("%*s|\n", padCount, " ");

	printf("|");
	repeatChar('-', tableWidth - 2);
	printf("|\n");

	for(uint32_t i = 0; i < formlistCount; i++)
	{
		printf("| %-*.*s ", (fieldNameMax - 2), (fieldNameMax - 2), autocompleteList[i]->fieldName);
		printf("| %-*.*s |\n", (fieldValueMax - 2), (fieldValueMax - 2), autocompleteList[i]->fieldValue);
	}

	printf(" ");
	repeatChar('-', tableWidth - 2);
	printf(" \n\n");
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

			printf("Failed to decrypte credentials!\n");
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