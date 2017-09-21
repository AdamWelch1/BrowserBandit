#include "firefox.h"

Firefox::Firefox(const char *pPath)
{
	if(strlen(pPath) >= 2048)
		strcpy(profilePath, "./");
	else
		strncpy(profilePath, pPath, 2047);

	buildHistoryList();
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
		timestamp = strtoul(rowValues[2], 0, 10);

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