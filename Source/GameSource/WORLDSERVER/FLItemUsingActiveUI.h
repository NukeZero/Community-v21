#ifndef __FLITEMUSINGACTIVEUI_H__
#define __FLITEMUSINGACTIVEUI_H__


class FLItemUsingActiveUI
{
	public:
		FLItemUsingActiveUI();
		~FLItemUsingActiveUI();


		static FLItemUsingActiveUI*		GetInstance();


		void	OnDoUseActiveUIItem( FLWSUser* pUser, DWORD dwItemObjID );

};

#define		g_pItemUsingActiveUI		FLItemUsingActiveUI::GetInstance()

#endif // __FLITEMUSINGACTIVEUI_H__
