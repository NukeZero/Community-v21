#pragma  once 

class CFLWndInventorySlotProc
{
public:
	CFLWndInventorySlotProc();
	~CFLWndInventorySlotProc();

	CItemContainer<FLItemElem>* m_pItemContainer;
	void SetSlotItem();
	CItemContainer<FLItemElem>* m_InvenSlot[MAX_INVENTORY];

private:

};