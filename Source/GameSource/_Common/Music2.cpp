
// author : gmpbigsun
// date : 2009_11_02
// music player for ogg and mp3 ( use miles lib )

#include "StdAfx.h"

#define _MUSIC_LIB_MILES	0		//use Miles lib
#define _MUSiC_LIB_FMOD		1		//use FMod Lib

#ifdef  __USE_SOUND_LIB_FMOD
#define __MUSIC_LIB  ( _MUSiC_LIB_FMOD ) 
#else
#define __MUSIC_LIB  ( _MUSIC_LIB_MILES ) 
#endif

//-------------------------------------------------------------------------------------------
// Miles
//-------------------------------------------------------------------------------------------
#if ( __MUSIC_LIB == _MUSIC_LIB_MILES )

#include "Music.h"
#include <miles/mss.h>

#pragma comment( lib, "mss32.lib" )

CMusicMng g_MusicMng;

static BOOL g_bEnableMusic = TRUE;
//static BOOL g_bLockMusic = FALSE;
static BOOL g_bFadeOutMusic = FALSE;
static TCHAR g_filename[128];
HSTREAM  g_Stream = NULL;
HDIGDRIVER  g_Dig = NULL;
TCHAR g_szFileName[64];

static float g_fMusicVolume = 1.0f;
static float g_fCurrVolume = 1.0f;

//BEGIN=================================================================================================
//아래 4개의 CALLBACK함수는 packing된 음원을 읽기위한 함수임.
#if 0 
CResFile g_kResfile_music; 

U32 AILCALLBACK Miles_File_Open( const char* pFileName, U32* pFileHandle )
{
	g_kResfile_music.Close( );

	if( TRUE == g_kResfile_music.Open(pFileName, "rb" ) ) 
	{
		if( g_kResfile_music.fp )
			pFileHandle = (U32*)&g_kResfile_music;
	}

	return TRUE;
}

void AILCALLBACK Miles_File_Close( U32 FileHandle )
{
	CResFile* pHandle = NULL;
	pHandle = ( CResFile* ) &FileHandle;

	if( NULL == pHandle )
		return;

	pHandle->Close( );
}

S32 AILCALLBACK Miles_File_Seek( U32 FileHandle, S32 offset, U32 type )
{
	CResFile* pHandle = NULL;
	pHandle = ( CResFile* )&FileHandle;

	if( !pHandle )
		return -1;

	U32 nNewPos = (U32)offset;
	switch( type )
	{
	case AIL_FILE_SEEK_BEGIN:
		nNewPos = offset;
		break;

	case AIL_FILE_SEEK_CURRENT:
		nNewPos += pHandle->GetCurrPos( );
		break;

	case AIL_FILE_SEEK_END:
		break;

	default:
		nNewPos += pHandle->GetCurrPos( );
		break;

	}

	pHandle->Seek( nNewPos, 0 );
	return pHandle->GetCurrPos();
}

U32 AILCALLBACK Miles_File_Read( U32 FileHandle, void* buffer, U32 bytes )
{
	CResFile* pHandle = NULL;
	pHandle = ( CResFile* ) &FileHandle;

	if( NULL == pHandle )
		return -1;

	uint32 nBeRead = bytes;

	if( pHandle->GetCurrPos() + (DWORD)bytes > (DWORD)pHandle->GetLength() )
	{
		nBeRead = pHandle->GetLength() - pHandle->GetCurrPos( );
	}

	size_t readok = pHandle->Read( buffer, nBeRead );
	if( readok == 0 )
		return -1;

	return nBeRead;
}
#endif
//END=================================================================================================

BOOL PlayMusic_( LPCTSTR lpszFileName, int nLoopCount )
{
	static BOOL sFORCELOCK = FALSE;

	if( sFORCELOCK )
		return FALSE;

	if( !g_Dig )
		return FALSE;

	if( g_bEnableMusic == FALSE )		// 사운드 카드가 없거나 껐을때
		return FALSE;

	if( _tcscmp( lpszFileName, g_filename ) == 0 && IsPlayingMusic() )
		return FALSE;

	// 아직 음악이 연주중이라면, 볼륨을 서서히 낮춰줄 세팅을 한다.
// 	if( IsPlayingMusic() )
// 	{
// 		_tcscpy( g_szFileName, lpszFileName );
// 		g_bFadeOutMusic = TRUE;
// 		g_fCurrVolume = g_fMusicVolume;
// 
// 		return TRUE;
// 	}

	// 제거 !
	if( g_Stream )
		AIL_close_stream( g_Stream );

	FLStrcpy( g_filename, _countof( g_filename ), lpszFileName );
	
	// 새로운 스트림으로!
	g_Stream = AIL_open_stream( g_Dig, g_filename, 0 );

#if 0 //suntodo : working
	if( !g_Stream )
	{

		//packing된 경우는...
		AIL_set_file_callbacks( Miles_File_Open, Miles_File_Close, Miles_File_Seek, Miles_File_Read );

		g_Stream = AIL_open_stream( g_Dig, g_filename, 0 );
	}
#endif 
		
	if( !g_Stream )
	{
		//FLERROR_LOG( PROGRAM_NAME, _T( "oh my god... AIL_open_stream error, filename: %s" ), g_filename );
		sFORCELOCK = TRUE;
		return FALSE;
	}
	
	if( nLoopCount < 0 )
		nLoopCount = 0;

	AIL_set_stream_loop_count( g_Stream, nLoopCount );
	AIL_start_stream( g_Stream );
	AIL_service_stream( g_Stream, 0 );
	
	// 볼륨은 원래대로
	SetVolume( g_fMusicVolume );

	return TRUE;
}


void ProcessFadeMusic()
{
	/*if( g_bFadeOutMusic )
	{
		g_fCurrVolume -= 0.01f;
		if( g_Dig )
			AIL_set_digital_master_volume_level( g_Dig, g_fCurrVolume );
		if( g_fMusicVolume < 0.0f )
		{
		    StopMusic();
			g_bFadeOutMusic = FALSE;
			g_bLockMusic = FALSE;
			if( PlayMusic_( g_szFileName, 0 ) )
				g_bLockMusic = TRUE;

		}
	}*/
}

BOOL PlayMusic( LPCTSTR lpszFileName, int nLoopCount )
{
	return PlayMusic_( lpszFileName, nLoopCount );
}

BOOL PlayMusic( DWORD dwIdMusic, int nLoopCount )
{
	if( dwIdMusic == NULL_ID || dwIdMusic == 0 ) //BGM_NONE )
		return FALSE;
	LPTSTR lpFileName = g_MusicMng.GetFileName( dwIdMusic );
	if( lpFileName[ 0 ] )
	{	
		if( PlayMusic( lpFileName, nLoopCount ) )
			return TRUE;
	}
	return FALSE;
}

BOOL PlayBGM( DWORD dwIdMusic )
{
	if( dwIdMusic == NULL_ID || dwIdMusic == 0 ) //BGM_NONE )
		return FALSE;
	LPTSTR lpFileName = g_MusicMng.GetFileName( dwIdMusic );
	if( lpFileName[ 0 ] )
		return PlayMusic( lpFileName, 1 );
	
	return FALSE;
}

void LockMusic()
{
	//g_bLockMusic = TRUE;
}

void StopMusic()
{
	if( !g_Stream )
		return;

	AIL_pause_stream( g_Stream, 1 );		//1 pause, 0 palyback
}

void SetVolume( FLOAT fVolume )
{
	g_fMusicVolume = fVolume;	

	AIL_set_digital_master_volume_level( g_Dig, g_fMusicVolume );
}

FLOAT GetVolume()
{
	return g_fMusicVolume;
}

BOOL IsPlayingMusic()
{
///	HSAMPLE hs = AIL_stream_sample_handle( g_Stream );
	if( !g_Stream )
		return FALSE;

	U32 status = AIL_stream_status( g_Stream );
	return ( SMP_PLAYING == status );
}

BOOL IsStopMusic()
{
//	HSAMPLE hs = AIL_stream_sample_handle( g_Stream );
	if( !g_Stream )
		return TRUE;

	U32 status = AIL_stream_status( g_Stream );
	return ( SMP_PLAYING != status );

}

void InitCustomSound( BOOL bEnable )
{
	AIL_startup();

	g_bEnableMusic = bEnable;

	g_Dig = AIL_open_digital_driver( 44100, 16, MSS_MC_USE_SYSTEM_CONFIG, 0 );
	if( !g_Dig )
	{
#ifdef __BUILD_FOR_MSGBOX
#else
		MessageBox(0,AIL_last_error(),"Music initialize error",MB_OK);		
#endif 
		FLERROR_LOG( PROGRAM_NAME, _T( "AIL_open_digital_driver error, Music init error" ) );
	}

}

void UnInitCustomSound()
{
	AIL_close_stream( g_Stream );
	AIL_close_digital_driver( g_Dig );
	AIL_shutdown();
}

#elif ( __MUSIC_LIB == _MUSiC_LIB_FMOD )

//------------------------------------------------------------------------------------------------------------------------------
// FMOD here 
// gmpbigsun : FMOD , 기존의 Sound Interface를 그대로 유지 
//------------------------------------------------------------------------------------------------------------------------------

#include <fmod/fmod.h>
#include <fmod/fmoddyn.h>
#include <fmod/fmod_errors.h>

#pragma comment ( lib, "fmodvc.lib" )

CMusicMng g_MusicMng;

static BOOL g_bLockMusic = FALSE;
static BOOL g_bEnableMusic = TRUE;
static BOOL g_bFadeOutMusic = FALSE;
static float g_fMusicVolume = 1.0f;

struct FModData
{
	FModData( ) { _stream = NULL; _iChannel = -1; }
	FSOUND_STREAM* _stream;
	std::string _filename;
	int _iChannel;
};

FModData g_FModData;		// bgm을 2곡이상 동시에 플레이시킬일이 없으므로...


void ProcessFadeMusic()
{
	// 이기능이 필요한가?
/*	static CTime kOldTime;
	static bool bFirst = true;
	if( g_bFadeOutMusic )
	{
		if( bFirst )
		{
			kOldTime = CTime::GetCurrentTime( );
			bFirst = false;
		}

		CTime kCurTime = CTime::GetCurrentTime( );
		CTime kDelta = kCurTime - kOldTime;

		FLOAT fVol = GetVolume( );
		fVol -= ( kDelta * kDelta.GetSecond( ) );

		if( fVol <= 0.0f )
		{
			//ok finished the processing
			g_bFadeOutMusic = FALSE
			bFirst = true;
		}else
	} */
}

BOOL PlayMusic_( LPCTSTR lpszFileName, int nLoopCount )
{
	if( g_bEnableMusic == FALSE )		// 사운드 카드가 없거나 껐을때
		return FALSE;

	// 이미 로드된 Stream이 있다면..
	if( g_FModData._stream )
	{
		if( g_FModData._filename == string( lpszFileName ) && IsPlayingMusic( ) )
			return FALSE;
	}

	// 제거 !
	FSOUND_Stream_Close( g_FModData._stream );

	// 새로운 스트림으로!
	g_FModData._stream = FSOUND_Stream_Open( lpszFileName,  FSOUND_NORMAL | FSOUND_MPEGACCURATE, 0, 0);
	if( !g_FModData._stream )
	{
		FLASSERT( g_FModData._stream && "cannot find music file!!" );
		return FALSE;
	}

	g_FModData._filename = string( lpszFileName );

	// Loop!
	FLASSERT( nLoopCount > -1 );
	if( nLoopCount > 0 )
		FSOUND_Stream_SetLoopCount( g_FModData._stream, nLoopCount );

	g_FModData._iChannel = FSOUND_Stream_Play( FSOUND_FREE, g_FModData._stream );

	// 볼륨은 원래대로
	SetVolume( g_fMusicVolume );
	
	return FALSE;
}

BOOL PlayMusic( LPCTSTR lpszFileName, int nLoopCount )
{
	if( g_bLockMusic )
		return FALSE;
	return PlayMusic_( lpszFileName, nLoopCount );
}

BOOL PlayMusic( DWORD dwIdMusic, int nLoopCount )
{
	if( dwIdMusic == NULL_ID || dwIdMusic == 0 ) //BGM_NONE )
		return FALSE;
	LPTSTR lpFileName = g_MusicMng.GetFileName( dwIdMusic );
	if( lpFileName[ 0 ] )
	{	
		g_bLockMusic = FALSE;
		if( PlayMusic( lpFileName, nLoopCount ) )
		{
			g_bLockMusic = TRUE;
			return TRUE;
		}
	}
	return FALSE;
}

BOOL PlayBGM( DWORD dwIdMusic )
{
	if( dwIdMusic == NULL_ID || dwIdMusic == 0 ) //BGM_NONE )
		return FALSE;
	LPTSTR lpFileName = g_MusicMng.GetFileName( dwIdMusic );
	if( lpFileName[ 0 ] )
		return PlayMusic( lpFileName, 1 );
	
	return FALSE;
}

void LockMusic()
{
	g_bLockMusic = TRUE;
}

void StopMusic()
{
	FSOUND_StopSound( g_FModData._iChannel );
}

void SetVolume( FLOAT fVolume )
{
	g_fMusicVolume = fVolume;	//Miles는 0.0 ~ 1.0f 를 쓰는관계로 이값을 255로 곱해줘야 실제 FMOD volum 수치가 된다.
	int iFModVolum = int( g_fMusicVolume * 255.0f );
	FSOUND_SetVolume( g_FModData._iChannel, iFModVolum );
}

FLOAT GetVolume()
{
	return g_fMusicVolume;
	//return (FLOAT)FSOUND_GetVolume( g_FModData._iChannel );
}

BOOL IsPlayingMusic()
{
	return FSOUND_IsPlaying( g_FModData._iChannel );
}

BOOL IsStopMusic()
{
	if( g_FModData._iChannel == -1 )
		return TRUE;

	return ( !FSOUND_IsPlaying( g_FModData._iChannel ) );
}

void InitCustomSound( BOOL bEnable )
{
	g_bEnableMusic = bEnable;
	
	if (FSOUND_GetVersion() < FMOD_VERSION)
	{
		FLASSERT( 0 );
		UnInitCustomSound( );
		return;
	}

	if (!FSOUND_Init(44100, 32, FSOUND_NONBLOCKING ))
    {
		FLERROR_LOG( PROGRAM_NAME, _T( "FSOUND_Init failed" ) );
		UnInitCustomSound( );
		return;
    }

	FSOUND_Stream_SetBufferSize(1000);
}

void UnInitCustomSound()
{
	FSOUND_Stream_Close( g_FModData._stream );

	FSOUND_Close();
}

#endif 

BOOL CMusicMng::LoadScript( LPCTSTR lpszFileName )
{
	CScript script;
	if( script.Load( lpszFileName ) == FALSE )
		return FALSE;
	CString string;
	MUSIC music;
	int id = script.GetNumber();
	do
	{
		script.GetToken(); // fileName
		FLStrcpy( music.szMusicFileName, _countof( music.szMusicFileName ), script.token );
		m_aMusic.SetAt( id, &music );
		id = script.GetNumber();
	} while( script.tok != FINISHED );
	m_aMusic.Optimize();

	return TRUE;
}

LPTSTR CMusicMng::GetFileName( DWORD dwId )
{
	MUSIC* pMusic = m_aMusic.GetAt( dwId );
	if( pMusic )
		return (LPTSTR)pMusic->szMusicFileName;
	return "";
}