#ifndef HEADER_DEKANEHPP
#define HEADER_DEKANEHPP
#pragma once
#include "../all/Lexer.hpp"

/*
This file came from https://github.com/NukeZero
 - Credits are given to the following people for their contributions in this project via our github page.
*/

namespace Settings
{
	constexpr char NEUZ_TITLE[] = "FLYFF";
	constexpr char NEUZ_IPSET[] = "127.0.0.1";
	constexpr char NEUZ_CPORT[] = "5400";
	constexpr unsigned short NEUZ_CPORTUS = 5400;
	constexpr char NEUZ_BHASH[] = "";
	constexpr char NEUZ_PHASH[] = "kikugalanet";
	constexpr char NEUZ_MSGVR[] = "20100412";
}

#ifndef __CLIENT
namespace dbf
{
	constexpr char account[] = "ACCOUNT_DBF";
	constexpr char character[] = "CHARACTER_01_DBF";
	constexpr char logging[] = "LOGGING_01_DBF";
	constexpr char ranking[] = "RANKING_DBF";
	constexpr char backend[] = "BACKEND_DBF";
	constexpr char itemdb[] = "ITEM_DBF";
	constexpr char world[] = "World_dbf";
	constexpr char username[] = "sa";
	constexpr char server[] = "DESKTOP-XXXXXXX";
	constexpr char password[] = "PASS";
}
#endif

/*
Uses des3 to encrypt the password for the db connections. The Encrypter project was renamed to des3Encrypter and Altered. Just run and convert password.
#define __PWD_CRYPT_DB				

Hardcodes db strings to avoid setting them through the .ini files, although, that is still possible with this type of system. It does skip ODBC.
*/
#define __DbStuff

#endif