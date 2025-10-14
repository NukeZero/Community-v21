#pragma once

class FLDefinedsToString
{
public:
	FLDefinedsToString();
	virtual ~FLDefinedsToString();

	static FLDefinedsToString & GetInstance();

	bool					GetInt( int & o_int, const char * pszStr );

private:
	void					load();

private:
	map< string, int >		m_mapDefineds;
};