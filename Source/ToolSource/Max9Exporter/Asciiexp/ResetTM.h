#ifndef __RESETTM_H__
#define __RESETTM_H__

#include "mods.h"
#include "utilapi.h"
#include "istdplug.h"
#include "modstack.h"
#include "simpmod.h"

class ResetXForm : public UtilityObj {
public:
	IUtil *iu;
	Interface *ip;
	HWND hPanel;
	
	ResetXForm();
	void BeginEditParams(Interface *ip,IUtil *iu);
	void EndEditParams(Interface *ip,IUtil *iu);
	void SelectionSetChanged(Interface *ip,IUtil *iu);
	void DeleteThis() {}		
	void ResetSel();
	BOOL ResetNodeEnum( INode* node );
		
};

extern ResetXForm theResetXForm;



#endif // __RESETTM_H__