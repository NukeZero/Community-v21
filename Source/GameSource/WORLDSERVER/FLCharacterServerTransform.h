#ifndef __FLCHARACTERSERVERTRANSFORM_H__
#define __FLCHARACTERSERVERTRANSFORM_H__


const DWORD		MAX_CAN_HAVE_PENYA	= 2100000000;

class FLCharacterServerTransform
{
	public:
		FLCharacterServerTransform();
		~FLCharacterServerTransform();

		static FLCharacterServerTransform*	GetInstance();


		void	OnDoUseCharacterServerTransform( FLWSUser* pUser, DWORD dwItemObjID );

		void	OnDoTransformCharacterServer( FLWSUser* pUser, const FLPacketCharacterServerTransformReq* pMsg );

	
	private:
		bool	CanUseCharacterServerTransform( FLWSUser* pUser, DWORD dwItemObjID );
		
		bool	CanTransformCharacterServer( FLWSUser* pUser, const FLPacketCharacterServerTransformReq* pMsg );
		
		void	DoTransformCharacterServer( FLWSUser* pUser, const FLPacketCharacterServerTransformReq* pMsg );

};

#define		g_pCharacterServerTransform			FLCharacterServerTransform::GetInstance()

#endif // __FLCHARACTERSERVERTRANSFORM_H__
