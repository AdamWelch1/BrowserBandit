#include <cstdio>
#include <cstdint>

#include <nss/nss.h>
#include <nss/pk11pub.h>
#include <nss/pk11sdr.h>

#include "b64.h"

int main(int argc, char **argv)
{
	char profilePath[] = "/media/aj/OS/Users/Adam.Welch/AppData/Roaming/Mozilla/Firefox/Profiles/g5xqkm0i.default";
	char masterPassword[] = "password";

	SECStatus nssInit = NSS_Init(profilePath);

	if(nssInit != SECSuccess)
	{
		printf("Failed to initialize NSS Library! Exiting...\n");
		return -1;
	}

	PK11SlotInfo *keySlot = PK11_GetInternalKeySlot();
	SECStatus pwCheck = PK11_CheckUserPassword(keySlot, ""); // Check the default blank password first

	if(pwCheck != SECSuccess)
	{
		printf("Failed to decrypt key database! Exiting...\n");
		return -1;
	}

	SECStatus authStatus = PK11_Authenticate(keySlot, true, 0);

	if(authStatus != SECSuccess)
	{
		printf("Failed to authenticate! Exiting...\n");
		return -1;
	}

	SECItem userName;
	SECItem passWord;

	char b64User[] = "MDIEEPgAAAAAAAAAAAAAAAAAAAEwFAYIKoZIhvcNAwcECK6ayQrmzP/MBAh6M1KM/hwrZQ==";
	char b64Pass[] = "MEIEEPgAAAAAAAAAAAAAAAAAAAEwFAYIKoZIhvcNAwcECN+/ftH/GSaWBBgFt/lOF6c4YwawgiGWuAD8MHspEzzv3vk=";

	uint8_t encUser[200];
	uint8_t encPass[200];
	uint32_t encUserSize = Base64::decodedSize(b64User);
	uint32_t encPassSize = Base64::decodedSize(b64Pass);

	Base64::decode(b64User, encUser);
	Base64::decode(b64Pass, encPass);

	userName.data = encUser;
	userName.len = encUserSize;
	passWord.data = encPass;
	passWord.len = encPassSize;

	SECItem decUser;
	SECItem decPass;

	SECStatus decStatus = PK11SDR_Decrypt(&userName, &decUser, 0);

	if(decStatus == SECSuccess)
		printf("Username: %s\n", decUser.data);

	return 0;
}