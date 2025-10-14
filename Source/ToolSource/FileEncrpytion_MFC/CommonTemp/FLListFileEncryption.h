#ifndef __FLLISTFILEENCRYPTION_H__
#define __FLLISTFILEENCRYPTION_H__

#include <crypto/FLDes.h>

class FLListFileEncryption
{
	public:
		enum	ENCRYPTION_KEY_TYPE
		{
			CONFIGFILE_ENCRYPTION_KEY	= 0
			, LISTFILE_ENCRYPTION_KEY
		};

		enum	ENCRYPTION_ERROR
		{
			FILE_ENCRYPTION_NOT_INITIALIZE			= 0
			, FILE_ENCRYPTION_SET_SOURCE_SUCCESS
			, FILE_ENCRYPTION_RESET_SOURCE_SUCCESS
			, ENCRYPTION_SUCCESS
			, ENCRYPTION_SOURCE_FILE_NAME_ERROR
			, ENCRYPTION_SOURCE_FILE_OPEN_ERROR
			, ENCRYPTION_RESULT_FILE_OPEN_ERROR
			, DECRYPTION_SUCCESS
			, DECRYPTION_SOURCE_FILE_NAME_ERROR
			, DECRYPTION_SOURCE_FILE_OPEN_ERROR
			, DECRYPTION_RESULT_FILE_OPEN_ERROR
		};

	public:
		FLListFileEncryption();
		~FLListFileEncryption();

	public:
		void	SetSourceFile( const TCHAR* pszFileName );
		bool	IsSetSourceFile();

		DWORD	DecryptionFile();

		void	Decrypt( const TCHAR* szIn, TCHAR* szOut );

		TCHAR*	GetSourceFileName();
		TCHAR*	GetDecryptionFileName();

	private:

		DWORD	DecryptionFile( const TCHAR* pszFileName );


		FL_des3_context m_tCtx3;

		TCHAR	m_szSourceFileName[MAX_PATH];
		TCHAR	m_szDecFileName[MAX_PATH];

};

#endif // __FLLISTFILEENCRYPTION_H__