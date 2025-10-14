#ifndef __FLFILEENCRYPTION_H__
#define __FLFILEENCRYPTION_H__

#include <crypto/FLDes.h>

class FLFileEncryption
{
	public:
		enum	ENCTYPTION_ERROR
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
		FLFileEncryption();
		~FLFileEncryption();

	public:
		static FLFileEncryption * GetInstance();

		void	SetSourceFile( const TCHAR* pszFileName );
		bool	IsSetSourceFile();

		DWORD	EncryptionFile();
		DWORD	DecryptionFile();

		void	Encrypt( const TCHAR* szIn, TCHAR* szOut );
		void	Decrypt( const TCHAR* szIn, TCHAR* szOut );

		TCHAR*	GetSourceFileName();
		TCHAR*	GetEncryptionFileName();
		TCHAR*	GetDecryptionFileName();

	private:
		void	Init();
		void	InitKey();

		DWORD	EncryptionFile( const TCHAR* pszFileName );
		DWORD	DecryptionFile( const TCHAR* pszFileName );


		FL_des3_context m_tCtx3;

		TCHAR	m_szSourceFileName[MAX_PATH];
		TCHAR	m_szEncFileName[MAX_PATH];
		TCHAR	m_szDecFileName[MAX_PATH];

};

//#define g_xFileEncryption	FLFileEncryption::GetInstance()

#endif // __FLFILEENCRYPTION_H__