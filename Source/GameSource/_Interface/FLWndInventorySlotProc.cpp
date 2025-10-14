#include "stdafx.h"


CFLWndInventorySlotProc::CFLWndInventorySlotProc()
{
	for(int nIndex = 0; nIndex < MAX_VENDOR_NEW_INVENTAB ; nIndex++)
	{
		DWORD dwInvenSize = 0;
		switch(nIndex)
		{
		case 0:
			dwInvenSize = MAX_INVENTORY_GENERAL;
			break;
		case 1:
			dwInvenSize = MAX_INVENTORY_GENERAL + MAX_INVENTORY_PET;
			break;
		case 2:
			dwInvenSize = MAX_INVENTORY_GENERAL + MAX_INVENTORY_PET + MAX_INVENTORY_COSTUME;
			break;
		case 3:
			dwInvenSize = MAX_INVENTORY;
			break;
		}
		m_InvenSlot[nIndex] = new CItemContainer< FLItemElem  >;
		m_InvenSlot[nIndex]->SetItemContainer( ITYPE_ITEM, CONTAINER_TYPE_INVENTORY, dwInvenSize, MAX_HUMAN_PARTS );
	}
}
CFLWndInventorySlotProc::~CFLWndInventorySlotProc()
{
}

void CFLWndInventorySlotProc::SetSlotItem()
{
	int nMax = m_pItemContainer->GetMax();

	for(int nIndex = 0; nIndex < nMax; nIndex++)
	{
		FLItemElem* pItemElem = new FLItemElem;

		if( nIndex < MAX_INVENINDEX_GENERAL )
		{
			pItemElem = m_pItemContainer->GetAt(nIndex);
			if(pItemElem)
			{
				m_InvenSlot[0]->Add(pItemElem);
			}
		}
		else if( nIndex < MAX_INVENINDEX_PET )
		{
			pItemElem = m_pItemContainer->GetAt(nIndex);
			if(pItemElem)
			{
				m_InvenSlot[1]->Add(pItemElem);
			}
		}
		else if( nIndex < MAX_INVENINDEX_COSTUME )
		{
			pItemElem = m_pItemContainer->GetAt(nIndex);
			if(pItemElem)
			{
				m_InvenSlot[2]->Add(pItemElem);
			}
		}
		else if( nIndex < MAX_INVENINDEX_QUEST )
		{
			pItemElem = m_pItemContainer->GetAt(nIndex);
			if(pItemElem)
			{
				m_InvenSlot[3]->Add(pItemElem);
			}
		}
	}
}


