#include <cstring>
#include <iostream>
#include <string>

extern "C"
{
	#include "des.h"
}


int main(const int argc, char* argv[])
{	
	std::string pwd;
	if (argc >= 2) 
		pwd = argv[1];
	else
	{
		std::cout << "Insert the password you would like to encrypt: " << std::endl;
		std::getline(std::cin, pwd);
	}


	// des3 keys.
	static unsigned char keys[3][8] =
	{
		{ 0x32, 0x89, 0x45, 0x9A, 0x89, 0xAB, 0xCD, 0x11 },
		{ 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF, 0x01 },
		{ 0x1F, 0x67, 0x89, 0x54, 0xCD, 0xEF, 0x00, 0x23 }
	};

	des3_context ctx3;
	des3_set_3keys(&ctx3, keys[0], keys[1], keys[2]);

	int i;
	unsigned char input[64] = { 0, };       // original password
	unsigned char output[64];

	strcpy(reinterpret_cast<char*>(input), pwd.c_str());

	for (i = 0; i < 3; i++)
	{
		unsigned char buf[8] = { 0, };
		memcpy(buf, input + i * 8, 8);
		des3_encrypt(&ctx3, buf, output + i * 8);
	}

	char szBuffer[256] = { 0, };
	for (i = 0; i < 24; i++)
		sprintf(szBuffer + i * 2, "%02X", output[i]);

	std::cout << szBuffer << std::endl;
	std::cout << "press any key to close." << std::endl;
	std::cin.get();
	return 0;
}

