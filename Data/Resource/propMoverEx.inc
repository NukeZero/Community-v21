/*						
06.04.07 -xuzhu-						
일반적인 아이템 습득의 기본 정책 변경.						
1.일반적인 무기/방어구의 기본장비들은 모든 몬스터에게서 드랍되게 변경되었다.						
2.떨어지는 무기/방어구의 레벨은 몬스터레벨보다 2~5레벨 낮은 아이템이 드랍되도록 하였다.						
3.유니크급의 무기는 희박한 확률로 월드드랍으로도 구할 수 있지만 특정 몬스터에게서 구할 확률이 더 높게 한다.						
.DropKind의 nMinUniq, nMaxUniq의 파라메터는 더이상 사용되지 않게 바꾸었다.						
.서버에서 스크립트를 읽을때 무조건 pPropMover->dwLevel - 2 ~ -5범위로 설정되게 바꾸었다.						
.이는 일반적으로 몬스터들이 장비를 드랍할때 몹레벨보다 2~5레벨 낮은 템이 드랍되게 하여						
.낮은템으로 사냥은 가능하게 발란스를 설정하고, 돈이 있다면 더 좋은 아이템을 사서 사냥할 수 있도록 했다.						
*/						
//DropKind(아이템 3차 분류, 레어등급최소값, 레어등급최대값)						
//DropKind은 게임에 존재하는 아이템 분류의 레어등급 구간에 해당되는 아이템을 랜덤하게 드롭 시킬때 사용하는 함수이다.						
//EX) DropKind(IK3_SWD, 1, 6);						
/*						
아이템 발생 판정 O -> 발생 최대 개수에 의한 아이템 발생, 25% 돈						
		X -> 아이템 없이 100% 돈				
MVI_MONSTER						
{						
	생성확률이 낮은 것을 제일 상위로 두어 생성확률이 적은 아이템은 무조건 발생개수에					
	우선적으로 포함되도록 한다.					
	QuestItem(어떤 퀘스트인가, 퀘스트 상태, 퀘스트용으로 지급되는 아이템, 확률, 지급갯수 );					
	m_dwAttackMoveDelay = 0; // 이동시 딜레이(밀리세컨드)					
	m_dwRunawayDelay = 1000; // 도망갈 때 딜레이(밀리세컨드)					
	SetRunAway( HP(percent) );//, NPC Index(0이면 핼퍼 없음), 갯수(0은 몽땅) );					
	현재 hp, 핼퍼 몬스터 index, 도움 요청할 몬스터 수(0 주변에 검색되는 놈 모두)					
	만약 도움 요청할 몬스터가 필요 없다면 index를 0으로					
	SetCallHelper( HP(Percent), NPC(Index), 갯수(0이면 몽땅), bParty(TRUE,FALSE, 공격 대상자를 해당 플레이어의 파티 맴버로 설정) );					
	m_nAttackFirstRange = 10; // 선공 몹이 선공하기 위한 스캔 범위. 미터 단위,					
				// 1. 100이상 사용할 수 없음. 선공 몹의 시야 뿐만이 아닌		
				// 2. 동족 인식을 하는 몬스터의 헬퍼 인식 범위에도 포함됨.		
				// 3. 기본은 10으로 하며 10이상 설정할땐 프로그램팀과 상의.		
				// 4. 10이상 범위는 AI가 AII_AGGRO_NORMAL일 경우에만 적용 가능. (다른 AI 일 경우 프로그램팀과 상의)		
}						
AI						
{						
Scan jon (jobNum) range (num) quest (QuestID) item (ItemID) chao (100,101) s						
Attack (%) cunning (Hi,sam,low)                B						
Recovery (%) (%) (%) (u/m/a)                   B	// 발동 체력 %, 회복 체력 %, 소비마나 (아군/자신/아군과자신)					
Summon (%) (num) (MoverID) B  // 20초마다 한번씩 (%)확률로 소환시도. MoverID를 num마리 생성.						
Rangeattack (X)                                B	//입력한 수치는 원거리 사정거리					
Keeprangeattack (Range)                        B// 입력한 수치 거리 만큼 적이 근접해있을 때 도망감						
Evade (%)                                      B // 입력한 수치 값에 몬스터의 HP가 도달했을 때 도망간다.						
Helper   (freq) (sr) (num) (mtype)             B						
Berserk  (hp) (%)                              B						
Randomtarget  제일먼저 쓰이는 경우 사용        B						
teleport (freq) (X,Y or 1~5) (mana) (batt)     M						
Loot     (d) (X,Y) (code) (?)                  M						
}						
숫자		확률		1개가 떨어질때 몬스터수		
9375		0.00%	320000			
18750		0.00%	160000			
37500		0.00%	80000			
75000		0.00%		40000		
150000		0.01%		20000		
300000		0.01%		10000		
1000000		0.03%		3000		
1875000		0.06%		1600		
3000000		0.10%		1000		
3750000		0.13%		800		
5000000		0.17%		600		
7500000		0.25%		400		
15000000	0.50%		200			
30000000	1%		100			
300000000	10%		10			
3000000000	100%		1			
*/						
MI_AIBATT1						
{						
	m_dwAttackMoveDelay = 2000;					
	m_dwRunawayDelay = 3000;					
	m_nAttackFirstRange = 8;					
AI						
	{					
		#Scan				
{						
			scan			
}						
		#battle				
		{				
			Attack cunning low			
//			evade 5			
}						
		#move				
		{				
			Loot d 5			
		}				
	}					
}						
MI_AIBATT2						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
{						
			scan			
}						
		#battle				
		{				
			Attack cunning low			
}						
		#move				
		{				
			Loot d 5			
		}				
	}					
}						
MI_AIBATT3						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
{						
			scan range 8			
}						
		#battle				
		{				
			Attack cunning low			
}						
		#move				
		{				
		}				
	}					
}						
MI_AIBATT4						
{						
	m_dwAttackMoveDelay = 0;					
	m_dwRunawayDelay = 1000;					
	SetRunAway( 10, MI_AIBATT2, 5 );					
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
{						
			scan			
}						
		#battle				
		{				
			Attack cunning low			
			Recovery 10 50 0 m			
			evade 20			
}						
		#move				
		{				
			Loot d 5			
		}				
	}					
}						
MI_MUSHPANG1						
{						
	m_dwAttackMoveDelay = 2000;					
	m_dwRunawayDelay = 3000;					
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
{						
			scan range 6			
}						
		#battle				
		{				
			Attack			
}						
		#move				
		{				
			Loot d 5			
		}				
	}					
}						
MI_MUSHPANG2						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
{						
			scan			
}						
		#battle				
		{				
			Attack cunning low			
//                        evade 10						
}						
		#move				
		{				
		}				
	}					
}						
MI_MUSHPANG3						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
{						
			scan			
}						
		#battle				
		{				
			Attack cunning low			
}						
		#move				
		{				
			Loot d 3			
		}				
	}					
}						
MI_MUSHPANG4						
{						
	m_dwAttackMoveDelay = 0;					
	m_dwRunawayDelay = 1000;					
	SetRunAway( 10, MI_MUSHPANG2, 5 );					
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
{						
			scan			
}						
		#battle				
		{				
			Attack cunning low			
evade 20						
			Recovery 20 50 0 m			
}						
		#move				
		{				
			Loot d 4			
		}				
	}					
}						
MI_BURUDENG1						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
{						
			scan			
}						
		#battle				
		{				
			Attack cunning low			
}						
		#move				
		{				
		}				
	}					
}						
MI_BURUDENG2						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
{						
			scan			
}						
		#battle				
		{				
			Attack			
}						
		#move				
		{				
			Loot d 3			
		}				
	}					
}						
MI_BURUDENG3						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
{						
			scan			
}						
		#battle				
		{				
			Attack cunning low			
}						
		#move				
		{				
		}				
	}					
}						
MI_BURUDENG4						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
{						
			scan			
}						
		#battle				
		{				
			Attack cunning low			
evade 30						
}						
		#move				
		{				
			loot d 5			
		}				
	}					
}						
MI_PUKEPUKE1						
{						
	m_dwAttackMoveDelay = 0;					
	m_dwRunawayDelay = 750;					
	SetRunAway( 10, 0, 1 );					
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
{						
			scan			
}						
		#battle				
		{				
			Attack			
}						
		#move				
		{				
		}				
	}					
}						
MI_PUKEPUKE2						
{						
	m_dwAttackMoveDelay = 0;					
	m_dwRunawayDelay = 750;					
	SetRunAway( 10, MI_BURUDENG3, 1 );					
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
{						
			scan			
}						
		#battle				
		{				
			Attack cunning low			
}						
		#move				
		{				
			Loot d 3			
		}				
	}					
}						
MI_PUKEPUKE3						
{						
	m_dwAttackMoveDelay = 0;					
	m_dwRunawayDelay = 750;					
	SetRunAway( 10, 0, 1 );					
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
{						
			scan			
}						
		#battle				
		{				
			Attack			
}						
		#move				
		{				
			loot d 5			
		}				
	}					
}						
MI_PUKEPUKE4						
{						
	m_dwAttackMoveDelay = 0;					
	m_dwRunawayDelay = 1000;					
	SetRunAway( 10, MI_PUKEPUKE1, 5 );					
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
{						
			scan			
}						
		#battle				
		{				
			Attack			
}						
		#move				
		{				
		}				
	}					
}						
MI_PEAKYTURTLE1						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
{						
			scan			
}						
		#battle				
		{				
			Attack 20 cunning hi			
}						
		#move				
		{				
			Loot d 3			
		}				
	}					
}						
MI_PEAKYTURTLE2						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
{						
			scan			
}						
		#battle				
		{				
			Attack 10 cunning low			
}						
		#move				
		{				
		}				
	}					
}						
MI_PEAKYTURTLE3						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
{						
			scan			
}						
		#battle				
		{				
			Attack cunning low			
}						
		#move				
		{				
		}				
	}					
}						
MI_PEAKYTURTLE4						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
{						
			scan			
}						
		#battle				
		{				
			Attack cunning low			
berserk 30 1.2						
}						
		#move				
		{				
			loot d 5			
		}				
	}					
}						
MI_DEMIAN1						
{						
	m_dwAttackMoveDelay = 0;					
	m_dwRunawayDelay = 750;					
	SetRunAway( 10, 0, 1 );					
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
{						
			scan			
}						
		#battle				
		{				
			Attack cunning hi			
Evade 15						
}						
		#move				
		{				
			Loot d 3			
		}				
	}					
}						
MI_DEMIAN2						
{						
	m_dwAttackMoveDelay = 0;					
	m_dwRunawayDelay = 750;					
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
{						
			scan			
}						
		#battle				
		{				
			Attack cunning hi			
Evade 15						
}						
		#move				
		{				
		}				
	}					
}						
MI_DEMIAN3						
{						
	m_dwAttackMoveDelay = 0;					
	m_dwRunawayDelay = 750;					
	SetRunAway( 10, 0, 1 );					
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
{						
			scan			
}						
		#battle				
		{				
			Attack cunning hi			
Evade 15						
}						
		#move				
		{				
			Loot d 3			
		}				
	}					
}						
MI_DEMIAN4						
{						
	m_dwAttackMoveDelay = 0;					
	m_dwRunawayDelay = 1000;					
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
{						
			scan			
}						
		#battle				
		{				
			Attack cunning low			
}						
		#move				
		{				
			loot d 5			
		}				
	}					
}						
MI_DORIDOMA1						
{						
	m_nAttackFirstRange = 8;					
AI						
	{					
		#Scan				
{						
			scan			
}						
		#battle				
		{				
			Attack			
//                        evade 20						
}						
		#move				
		{				
		}				
	}					
}						
MI_DORIDOMA2						
{						
	m_nAttackFirstRange = 8;					
AI						
	{					
		#Scan				
{						
			scan			
}						
		#battle				
		{				
			Attack			
}						
		#move				
		{				
			loot d 3			
		}				
	}					
}						
MI_DORIDOMA3						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
{						
			scan			
}						
		#battle				
		{				
			Attack			
}						
		#move				
		{				
		}				
	}					
}						
MI_DORIDOMA4						
{						
	m_dwAttackMoveDelay = 0;					
	m_dwRunawayDelay = 1000;					
	SetRunAway( 10, MI_DORIDOMA1, 2 );					
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
{						
			scan			
}						
		#battle				
		{				
			Attack			
Berserk 20 1.2						
}						
		#move				
		{				
			loot d 3			
		}				
	}					
}						
MI_LAWOLF1						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
{						
			scan			
}						
		#battle				
		{				
			Attack			
//                        Evade 30						
}						
		#move				
		{				
		}				
	}					
}						
MI_LAWOLF2						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
{						
			scan			
}						
		#battle				
		{				
			Attack			
}						
		#move				
		{				
			loot d 5			
		}				
	}					
}						
MI_LAWOLF3						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
{						
			scan			
}						
		#battle				
		{				
			Attack sam			
}						
		#move				
		{				
		}				
	}					
}						
MI_LAWOLF4						
{						
	m_dwAttackMoveDelay = 0;					
	m_dwRunawayDelay = 1000;					
	SetRunAway( 20, MI_LAWOLF1, 3 );					
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
{						
			scan			
}						
		#battle				
		{				
			Attack			
Berserk 10 1.2						
}						
		#move				
		{				
			loot d 3			
		}				
	}					
}						
MI_FEFERN1						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
{						
			scan			
}						
		#battle				
		{				
			Attack			
//                        evade 10						
}						
		#move				
		{				
		}				
	}					
}						
MI_FEFERN2						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
{						
			scan			
}						
		#battle				
		{				
			Attack 20 cunning sam			
}						
		#move				
		{				
			loot d 5			
		}				
	}					
}						
MI_FEFERN3						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
{						
			scan			
}						
		#battle				
		{				
			Attack cunning sam			
//                        berserk 20 1.25						
}						
		#move				
		{				
			loot d 3			
		}				
	}					
}						
MI_FEFERN4						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
{						
			scan			
}						
		#battle				
		{				
			Attack			
}						
		#move				
		{				
			loot d 3			
		}				
	}					
}						
MI_NYANGNYANG1						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
{						
			scan			
}						
		#battle				
		{				
			Attack cunning hi			
evade 10						
}						
		#move				
		{				
		}				
	}					
}						
MI_NYANGNYANG2						
{						
	m_dwAttackMoveDelay = 0;					
	m_dwRunawayDelay = 1000;					
	SetRunAway( 10, 0, 1 );					
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
{						
			scan			
}						
		#battle				
		{				
			Attack 30 cunning sam			
}						
		#move				
		{				
			loot d 3			
		}				
	}					
}						
MI_NYANGNYANG3						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
{						
			scan			
}						
		#battle				
		{				
			Attack cunning sam			
}						
		#move				
		{				
		}				
	}					
}						
MI_NYANGNYANG4						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
{						
			scan			
}						
		#battle				
		{				
			Attack			
Berserk 20 1.4						
}						
		#move				
		{				
			loot d 3			
		}				
	}					
}						
MI_BANG1						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
{						
			scan			
}						
		#battle				
		{				
			Attack 30 cunning sam			
}						
		#move				
		{				
			loot d 4			
		}				
	}					
}						
MI_BANG2						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
{						
			scan			
}						
		#battle				
		{				
			Attack 10			
//                        berserk 20 1.2						
}						
		#move				
		{				
		}				
	}					
}						
MI_BANG3						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
{						
			scan			
}						
		#battle				
		{				
			Attack cunning sam			
Evade 10						
}						
		#move				
		{				
		}				
	}					
}						
MI_BANG4						
{						
	m_dwAttackMoveDelay = 0;					
	m_dwRunawayDelay = 1000;					
	SetRunAway( 10, MI_BANG1, 2 );					
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
{						
			scan			
}						
		#battle				
		{				
			Attack			
}						
		#move				
		{				
			loot d 3			
		}				
	}					
}						
MI_WAGSAAC1						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
{						
			scan			
}						
		#battle				
		{				
			Attack 20 cunning sam			
keeprangeattack 6						
}						
		#move				
		{				
		}				
	}					
}						
MI_WAGSAAC2						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
{						
			scan			
}						
		#battle				
		{				
			Attack cunning sam			
}						
		#move				
		{				
		}				
	}					
}						
MI_WAGSAAC3						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
{						
			scan			
}						
		#battle				
		{				
			Attack cunning low			
keeprangeattack 6						
}						
		#move				
		{				
			loot d 4			
		}				
	}					
}						
MI_WAGSAAC4						
{						
	m_dwAttackMoveDelay = 0;					
	m_dwRunawayDelay = 1000;					
	SetRunAway( 10, 0, 1 );					
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
{						
			scan			
}						
		#battle				
		{				
			Attack			
rangeattack 10						
}						
		#move				
		{				
			loot d 5			
		}				
	}					
}						
MI_MIA1						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
{						
			scan			
}						
		#battle				
		{				
			Attack			
Berserk 50 1.2						
}						
		#move				
		{				
		}				
	}					
}						
MI_MIA2						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
{						
			scan			
}						
		#battle				
		{				
			Attack 30 cunning sam			
}						
		#move				
		{				
			loot d 3			
		}				
	}					
}						
MI_MIA3						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
{						
			scan			
}						
		#battle				
		{				
			Attack cunning sam			
Berserk 5 1.2						
}						
		#move				
		{				
		}				
	}					
}						
MI_MIA4						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
{						
			scan			
}						
		#battle				
		{				
			Attack			
berserk 30 1.2						
			Evade 10			
}						
		#move				
		{				
			loot d 5			
		}				
	}					
}						
MI_MIA5						
{	
	SetLevelDropPanalty_Off					
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
{						
			scan			
}						
		#battle				
		{				
			Attack			
berserk 30 1.2						
			Evade 10			
}						
		#move				
		{				
			loot d 5			
		}				
	}					
}						
MI_MZOMBIE						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
{						
			scan			
}						
		#battle				
		{				
			Attack			
berserk 30 1.2						
			Evade 10			
}						
		#move				
		{				
			loot d 5			
		}				
	}					
}						
MI_FZOMBIE						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
{						
			scan			
}						
		#battle				
		{				
			Attack			
berserk 30 1.2						
			Evade 10			
}						
		#move				
		{				
			loot d 5			
		}				
	}					
}						
MI_NMZOMBIE						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
{						
			scan			
}						
		#battle				
		{				
			Attack			
berserk 30 1.2						
			Evade 10			
}						
		#move				
		{				
			loot d 5			
		}				
	}					
}						
MI_NFZOMBIE						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
{						
			scan			
}						
		#battle				
		{				
			Attack			
berserk 30 1.2						
			Evade 10			
}						
		#move				
		{				
			loot d 5			
		}				
	}					
}						
MI_MRPUMPKIN1						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
{						
			scan			
}						
		#battle				
		{				
			Attack			
}						
		#move				
		{				
			loot d 3			
		}				
	}					
}						
MI_MRPUMPKIN2						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
{						
			scan			
}						
		#battle				
		{				
			Keeprangeattack 8			
}						
		#move				
		{				
		}				
	}					
}						
MI_MRPUMPKIN3						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
{						
			scan			
}						
		#battle				
		{				
			Attack cunning low			
}						
		#move				
		{				
		}				
	}					
}						
MI_MRPUMPKIN4						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
{						
			scan			
}						
		#battle				
		{				
			Attack			
}						
		#move				
		{				
			loot d 2			
		}				
	}					
}						
MI_REDMANTIS1						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
{						
			scan			
}						
		#battle				
		{				
			Attack 20			
Berserk 30 1.4						
}						
		#move				
		{				
		}				
	}					
}						
MI_REDMANTIS2						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
{						
			scan			
}						
		#battle				
		{				
			Attack cunning sam			
}						
		#move				
		{				
			loot d 3			
		}				
	}					
}						
MI_REDMANTIS3						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
{						
			scan			
}						
		#battle				
		{				
			Attack 50 cunning sam			
}						
		#move				
		{				
			loot d 4			
		}				
	}					
}						
MI_REDMANTIS4						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
{						
			scan			
}						
		#battle				
		{				
			Attack			
Berserk 10 1.2						
}						
		#move				
		{				
			loot d 5			
		}				
	}					
}						
MI_JACKTHEHAMMER1						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
{						
			scan			
}						
		#battle				
		{				
			Attack 30			
evade 10						
}						
		#move				
		{				
			loot d 3			
		}				
	}					
}						
MI_JACKTHEHAMMER2						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
{						
			scan			
}						
		#battle				
		{				
			Keeprangeattack 8			
evade 20						
}						
		#move				
		{				
			loot d 4			
		}				
	}					
}						
MI_JACKTHEHAMMER3						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
{						
			scan			
}						
		#battle				
		{				
			Attack cunning low			
}						
		#move				
		{				
		}				
	}					
}						
MI_JACKTHEHAMMER4						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
{						
			scan			
}						
		#battle				
		{				
			Attack			
}						
		#move				
		{				
			loot d 3			
		}				
	}					
}						
MI_GIGGLEBOX1						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
{						
			scan			
}						
		#battle				
		{				
			Attack 30 cunning sam			
}						
		#move				
		{				
			loot d 3			
		}				
	}					
}						
MI_GIGGLEBOX2						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
{						
			scan			
}						
		#battle				
		{				
			Attack 50			
}						
		#move				
		{				
			loot d 3			
		}				
	}					
}						
MI_GIGGLEBOX3						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
{						
			scan			
}						
		#battle				
		{				
			Attack cunning low			
}						
		#move				
		{				
		}				
	}					
}						
MI_GIGGLEBOX4						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
{						
			scan			
}						
		#battle				
		{				
			Attack			
}						
		#move				
		{				
		}				
	}					
}						
MI_ROCKMUSCLE1						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
{						
			scan			
}						
		#battle				
		{				
			Attack 50			
}						
		#move				
		{				
		}				
	}					
}						
MI_ROCKMUSCLE2						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
{						
			scan			
}						
		#battle				
		{				
			Attack 30 cunning hi			
}						
		#move				
		{				
			loot d 3			
		}				
	}					
}						
MI_ROCKMUSCLE3						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
{						
			scan			
}						
		#battle				
		{				
			Attack cunning low			
berserk 20 1.4						
}						
		#move				
		{				
			loot d 4			
		}				
	}					
}						
MI_ROCKMUSCLE4						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
{						
			scan			
}						
		#battle				
		{				
			Attack			
}						
		#move				
		{				
			loot d 3			
	}					
	}					
}						
MI_BIGMUSCLE						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
{						
			scan			
}						
		#battle				
		{				
			Attack			
			Berserk 30 2			
recovery 5 20 0 m						
}						
		#move				
		{				
		}				
	}					
}						
MI_HOBO1						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
{						
			scan			
}						
		#battle				
		{				
			Attack 20 cunning sam			
rangeattack 10						
			Evade 20			
}						
		#move				
		{				
			loot d 5			
		}				
	}					
}						
MI_HOBO2						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
{						
			scan			
}						
		#battle				
		{				
			Keeprangeattack 8			
evade 10						
}						
		#move				
		{				
			loot d 3			
		}				
	}					
}						
MI_HOBO3						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
{						
			scan			
}						
		#battle				
		{				
			Attack			
Berserk 30 1.4						
}						
		#move				
		{				
			loot d 3			
		}				
	}					
}						
MI_HOBO4						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
{						
			scan			
}						
		#battle				
		{				
			Attack			
}						
		#move				
		{				
			loot d 4			
		}				
	}					
}						
MI_DUMBBULL1						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
{						
			scan			
}						
		#battle				
		{				
			Attack 10 cunning sam			
berserk 20 1.2						
}						
		#move				
		{				
			loot d 5			
		}				
	}					
}						
MI_DUMBBULL2						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
{						
			scan			
}						
		#battle				
		{				
			Attack cunning sam			
}						
		#move				
		{				
		}				
	}					
}						
MI_DUMBBULL3						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
{						
			scan			
}						
		#battle				
		{				
			Attack cunning low			
}						
		#move				
		{				
		}				
	}					
}						
MI_DUMBBULL4						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
{						
			scan			
}						
		#battle				
		{				
			Attack			
}						
		#move				
		{				
			loot d 3			
		}				
	}					
}						
MI_TOTEMIA1						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
{						
			scan			
}						
		#battle				
		{				
			Attack 20 cunning sam			
}						
		#move				
		{				
			loot d 2			
		}				
	}					
}						
MI_TOTEMIA2						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
{						
			scan			
}						
		#battle				
		{				
			Attack 20 cunning hi			
Berserk 20 1.4						
}						
		#move				
		{				
		}				
	}					
}						
MI_TOTEMIA3						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
{						
			scan			
}						
		#battle				
		{				
			Attack			
}						
		#move				
		{				
		}				
	}					
}						
MI_TOTEMIA4						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
{						
			scan			
}						
		#battle				
		{				
			Attack			
}						
		#move				
		{				
			loot d 3			
		}				
	}					
}						
MI_CARDPUPPET1						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
{						
			scan			
}						
		#battle				
		{				
			Attack cunning sam			
}						
		#move				
		{				
		}				
	}					
}						
MI_CARDPUPPET2						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
{						
			scan			
}						
		#battle				
		{				
			Keeprangeattack 8			
Evade 20						
}						
		#move				
		{				
			loot d 2			
		}				
	}					
}						
MI_CARDPUPPET3						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
{						
			scan			
}						
		#battle				
		{				
			Attack cunning low			
Evade 10						
}						
		#move				
		{				
		}				
	}					
}						
MI_CARDPUPPET4						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
{						
			scan			
}						
		#battle				
		{				
			Attack			
}						
		#move				
		{				
		}				
	}					
}						
MI_CARDPUPPET5						
{			
	SetLevelDropPanalty_Off			
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
{						
			scan			
}						
		#battle				
		{				
			Attack			
}						
		#move				
		{				
		}				
	}					
}						
MI_TOMBSTONEBEARER1						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
{						
			scan			
}						
		#battle				
		{				
			Attack cunning sam			
}						
		#move				
		{				
			loot d 4			
		}				
	}					
}						
MI_TOMBSTONEBEARER2						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
{						
			scan			
}						
		#battle				
		{				
			Attack			
berserk 20 1.5						
}						
		#move				
		{				
		}				
	}					
}						
MI_TOMBSTONEBEARER3						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
{						
			scan			
}						
		#battle				
		{				
			Attack cunning low			
}						
		#move				
		{				
		}				
	}					
}						
MI_TOMBSTONEBEARER4						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
{						
			scan			
}						
		#battle				
		{				
			Attack			
berserk 20 1.2						
}						
		#move				
		{				
			loot d 3			
		}				
	}					
}						
MI_BASQUE1						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
{						
			scan			
}						
		#battle				
		{				
			Attack 50			
keeprangeattack 6						
}						
		#move				
		{				
			loot d 4			
		}				
	}					
}						
MI_BASQUE2						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
{						
			scan			
}						
		#battle				
		{				
			Berserk 20 1.3			
keeprangeattack 7						
}						
		#move				
		{				
		}				
	}					
}						
MI_BASQUE3						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
{						
			scan			
}						
		#battle				
		{				
			Attack 50			
Rangeattack 6						
			evade 30			
}						
		#move				
		{				
		}				
	}					
}						
MI_BASQUE4						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
{						
			scan			
}						
		#battle				
		{				
			Attack			
rangeattack 10						
}						
		#move				
		{				
			loot d 3			
		}				
	}					
}						
MI_PRANKSTER1						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
{						
			scan			
}						
		#battle				
		{				
			Attack cunning sam			
}						
		#move				
		{				
		}				
	}					
}						
MI_PRANKSTER2						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
{						
			scan			
}						
		#battle				
		{				
			Attack cunning low			
Berserk 20 1.3						
}						
		#move				
		{				
			loot d 2			
		}				
	}					
}						
MI_PRANKSTER3						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
{						
			scan			
}						
		#battle				
		{				
			Attack cunning low			
}						
		#move				
		{				
		}				
	}					
}						
MI_PRANKSTER4						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
{						
			scan			
}						
		#battle				
		{				
			Attack			
}						
		#move				
		{				
			loot d 3			
		}				
	}					
}						
MI_WHEELEM1						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
{						
			scan			
}						
		#battle				
		{				
			Attack			
evade 10						
}						
		#move				
		{				
		}				
	}					
}						
MI_WHEELEM2						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 50 			
                        berserk 30 1.2            						
                    }						
		#move				
		    {				
			loot d 4			
		    }				
   	}               					
}						
MI_WHEELEM3						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack cunning low			
                                   						
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_WHEELEM4						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 			
                          						
                        Berserk 20 1.3            						
                    }						
		#move				
		    {				
			loot d 5			
		    }				
   	}               					
}						
MI_LEYENA1						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack  			
                        berserk 30 1.2						
			Evade 10            			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_LEYENA2						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Keeprangeattack 8 			
                                    						
                    }						
		#move				
		    {				
			loot d 3			
		    }				
   	}               					
}						
MI_LEYENA3						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 			
                        						
                                    						
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_LEYENA4						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 			
                        Berserk 30 1.5						
			summon 10 1 MI_LEYENA3			
			recovery 10 40 0 m  			
                          						
                                    						
                    }						
		#move				
		    { 				
			loot d 3			
		    }				
   	}               					
}						
MI_STEAMWALKER1						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack cunning sam 			
                        evade 30            						
                    }						
		#move				
		    {				
			loot d 4			
		    }				
   	}               					
}						
MI_STEAMWALKER2						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 50 			
                        Berserk 30 1.2            						
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_STEAMWALKER3						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
                                    						
                    }						
		#move				
		    {				
			loot d 3			
		    }				
   	}               					
}						
MI_STEAMWALKER4						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 			
                        Berserk 30 1.5						
			summon 10 1 MI_STEAMWALKER3			
			recovery 10 40 0 m 			
                          						
                                    						
                    }						
		#move				
		    {				
			loot d 5			
		    }				
   	}               					
}						
MI_STEELKNIGHT1						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack  			
                                    						
                    }						
		#move				
		    {				
			loot d 5			
		    }				
   	}               					
}						
MI_STEELKNIGHT2						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 60 			
                        Keeprangeattack 10            						
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_STEELKNIGHT3						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack cunning low			
                                    						
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_STEELKNIGHT4						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 			
                        Berserk 30 1.5						
			summon 10 1 MI_STEELKNIGHT3			
			recovery 10 40 0 m 			
                          						
                                    						
                    }						
		#move				
		    {				
			loot d 3			
		    }				
   	}               					
}						
MI_NUTTYWHEEL1						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 80 			
                                    						
                    }						
		#move				
		    {				
			loot d 5			
		    }				
   	}               					
}						
MI_NUTTYWHEEL2						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Keeprangeattack 6 			
                                    						
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_NUTTYWHEEL3						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack cunning low			
                        						
                                    						
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_NUTTYWHEEL4						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 			
                        Berserk 30 1.5						
			summon 10 1 MI_NUTTYWHEEL3			
			recovery 10 40 0 m            			
                    }						
		#move				
		    {				
			loot d 3			
		    }				
   	}               					
}						
MI_DRILLER1    // 여기서부터 59레벨임.						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack cunning sam 			
                                    						
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_DRILLER2						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
			keeprangeattack 7  			
                                    						
                    }						
		#move				
		    {				
			loot d 3			
		    }				
   	}               					
}						
MI_DRILLER3						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 			
                        						
                                    						
                    }						
		#move				
		    {				
			loot d 3			
		    }				
   	}               					
}						
MI_DRILLER4						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 			
                        Berserk 30 1.5						
			summon 10 1 MI_DRILLER3			
			recovery 10 50 0 m 			
                        						
                                    						
                    }						
		#move				
		    {				
			loot d 4			
		    }				
   	}               					
}						
MI_VOLT1						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack  			
                        						
			evade 20            			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_VOLT2						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 70 cunning low			
			berserk 20 1.2 			
                                    						
                    }						
		#move				
		    {				
			loot d 3			
		    }				
   	}               					
}						
MI_VOLT3						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 			
                                    						
                    }						
		#move				
		    {				
			loot d 3			
		    }				
   	}               					
}						
MI_VOLT4						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 			
                        Berserk 30 1.5						
			summon 10 1 MI_VOLT3			
			recovery 10 50 0 m  			
                                    						
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_ELDERGUARD1						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack  			
                                    						
                    }						
		#move				
		    {				
			loot d 4			
		    }				
   	}               					
}						
MI_ELDERGUARD2						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			attack			
			 			
                                    						
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_ELDERGUARD3						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 			
                        						
                        berserk 20 1.25            						
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_ELDERGUARD4						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 			
                        Berserk 30 1.5						
			summon 10 1 MI_ELDERGUARD3			
			recovery 10 50 0 m 			
                          						
                                    						
                    }						
		#move				
		    {				
			loot d 3			
		    }				
   	}               					
}						
MI_GARBAGEPIDER1						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 50 cunning sam			
			berserk 20 1.15 			
                                    						
                    }						
		#move				
		    {				
			loot d 3			
		    }				
   	}               					
}						
MI_GARBAGEPIDER2						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			attack 			
                        evade 20            						
                    }						
		#move				
		    {				
			loot d 4			
		    }				
   	}               					
}						
MI_GARBAGEPIDER3						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 50			
                                    						
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_GARBAGEPIDER4						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 			
                        Berserk 30 1.5						
			summon 10 1 MI_GARBAGEPIDER3			
			recovery 10 50 0 m          			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_CRANEMACHINERY1						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack cunning sam 			
                                    						
                    }						
		#move				
		    {				
			loot d 10			
		    }				
   	}               					
}						
MI_CRANEMACHINERY2						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			attack cunning low  			
                        evade 20            						
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_CRANEMACHINERY3						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack cunning low			
                        berserk 30 1.25            						
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_CRANEMACHINERY4						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 			
                        Berserk 30 1.5						
			summon 10 1 MI_CRANEMACHINERY3			
			recovery 10 50 0 m 			
                          						
                                    						
                    }						
		#move				
		    {				
			loot d 3			
		    }				
   	}               					
}						
MI_MOTHBEE1						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack  			
                                    						
                    }						
		#move				
		    {				
			loot d 5			
		    }				
   	}               					
}						
MI_MOTHBEE2						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack cunning sam 			
                        berserk 30 1.25            						
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_MOTHBEE3						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 			
                                    						
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_MOTHBEE4						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 			
                          						
                        berserk 30 1.2            						
                    }						
		#move				
		    {				
			loot d 3			
		    }				
   	}               					
}						
MI_FLBYRIGEN1						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 70 			
                                    						
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_FLBYRIGEN2						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack  			
                        berserk 20 1.2            						
                    }						
		#move				
		    {				
			loot d 3			
		    }				
   	}              					
}						
MI_FLBYRIGEN3						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack cunning low			
                        						
                        evade 5            						
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_FLBYRIGEN4						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 			
                          						
                                    						
                    }						
		#move				
		    {				
			loot d 4			
		    }				
   	}               					
}						
MI_ROCKEPELLER1						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 60 			
                        evade 20            						
                    }						
		#move				
		    {				
			loot d 3			
		    }				
   	}               					
}						
MI_ROCKEPELLER2						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack cunning low 			
                        berserk 30 1.35            						
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_ROCKEPELLER3						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack cunning sam			
                                                          						
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_ROCKEPELLER4						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 			
                        Berserk 30 1.5						
			summon 10 1 MI_ROCKEPELLER3			
			recovery 10 40 0 m             			
                    }						
		#move				
		    {				
			loot d 3			
		    }				
   	}               					
}						
MI_MINECATCHER						
{						
}						
MI_ERONSCATCHER						
{
}						
MI_KRASECCATCHER						
{						
}						
MI_GURUCATCHER						
{						
}						
//시일라카						
MI_SYLIACA1						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 70 			
                        Berserk 20 1.3            						
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_SYLIACA2						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Keeprangeattack 8			
                                    						
                    }						
		#move				
		    {				
			loot d 4			
		    }				
   	}               					
}						
MI_SYLIACA3						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack cunning low			
                        						
                        evade 20            						
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_SYLIACA4						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 			
                        Berserk 30 1.5						
			summon 10 1 MI_SYLIACA3			
			recovery 10 40 0 m            			
                    }						
		#move				
		    {				
			loot d 5			
		    }				
   	}               					
}						
//그리몽						
MI_GREEMONG1						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 30 			
                        Rangeattack 6            						
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_GREEMONG2						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack cunning hi 			
                                   						
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_GREEMONG3						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack cunning sam			
                       						
                        Berserk 20 1.3           						
                    }						
		#move				
		    {				
			loot d 4			
		    }				
   	}               					
}						
MI_GREEMONG4						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 			
                        Berserk 30 1.5						
			summon 10 1 MI_GREEMONG3			
			recovery 10 40 0 m             			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
//캐리아봄						
MI_CARRIERBOMB1						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 60 			
                        berserk 20 1.2            						
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_CARRIERBOMB2						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack  			
                        keeprangeattack 5            						
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_CARRIERBOMB3						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack cunning low			
			recovery 10 20 0 u			
                        Rangeattack 6            						
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_CARRIERBOMB4						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 			
                        Berserk 30 1.5						
			summon 10 1 MI_CARRIERBOMB3			
			recovery 10 40 0 m            			
                    }						
		#move				
		    {				
			loot d 3			
		    }				
   	}               					
}						
//라이젬						
MI_RISEM1						
{						
	SetCallHelper( 100, MI_RISEM3, 1, TRUE);					
	SetCallHelper( 100, MI_RISEM2, 2, TRUE);					
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 60			
			berserk 20 1.3 			
                                    						
                    }						
		#move				
		    {				
			loot d 5			
		    }				
   	}               					
}						
MI_RISEM2						
{						
	SetCallHelper( 100, MI_RISEM3, 1, TRUE);					
	SetCallHelper( 100, MI_RISEM1, 2, TRUE);					
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Keeprangeattack 8 			
                                    						
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_RISEM3						
{						
	SetCallHelper( 100, MI_RISEM1, 2, TRUE);					
	SetCallHelper( 100, MI_RISEM2, 2, TRUE);					
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack cunning low			
                        Rangeattack 6            						
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_RISEM4						
{						
	SetCallHelper( 100, MI_RISEM3, 5, TRUE);					
	SetCallHelper( 100, MI_RISEM2, 5, TRUE);					
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 			
                        Berserk 30 1.5						
			summon 10 1 MI_RISEM3			
			recovery 10 40 0 m            			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
//낙터비클						
MI_NUCTUVEHICLE1						
{						
	SetCallHelper( 100, MI_NUCTUVEHICLE3, 1, TRUE);					
	SetCallHelper( 100, MI_NUCTUVEHICLE2, 2, TRUE);					
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack cunning sam 			
                        Rangeattack 6            						
                    }						
		#move				
		    {				
			loot d 5			
		    }				
   	}               					
}						
MI_NUCTUVEHICLE2						
{						
	SetCallHelper( 100, MI_NUCTUVEHICLE1, 2, TRUE);					
	SetCallHelper( 100, MI_NUCTUVEHICLE3, 1, TRUE);					
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack  			
                        berserk 30 1.3            						
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_NUCTUVEHICLE3						
{						
	SetCallHelper( 100, MI_NUCTUVEHICLE1, 2, TRUE);					
	SetCallHelper( 100, MI_NUCTUVEHICLE2, 2, TRUE);					
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack cunning low			
                        Rangeattack 10            						
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_NUCTUVEHICLE4						
{						
	SetCallHelper( 100, MI_NUCTUVEHICLE3, 5, TRUE);					
	SetCallHelper( 100, MI_NUCTUVEHICLE2, 5, TRUE);					
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 			
                        Berserk 30 1.5						
			summon 10 1 MI_NUCTUVEHICLE3			
			recovery 10 50 0 m 			
                         						
                                    						
                    }						
		#move				
		    {				
			loot d 5			
		    }				
   	}               					
}						
//잠비거						
MI_ZOMBIGER1						
{						
	SetCallHelper( 100, MI_ZOMBIGER3, 1, TRUE);					
	SetCallHelper( 100, MI_ZOMBIGER2, 2, TRUE);					
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 50 			
                        Berserk 20 1.2            						
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_ZOMBIGER2						
{						
	SetCallHelper( 100, MI_ZOMBIGER1, 2, TRUE);					
	SetCallHelper( 100, MI_ZOMBIGER3, 1, TRUE);					
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			KeepRangeattack 10			
			evade 30            			
                    }						
		#move				
		    {				
			loot d 5			
		    }				
   	}               					
}						
MI_ZOMBIGER3						
{						
	SetCallHelper( 100, MI_ZOMBIGER1, 2, TRUE);					
	SetCallHelper( 100, MI_ZOMBIGER2, 2, TRUE);					
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack cunning low			
                        						
                        Rangeattack 6            						
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_ZOMBIGER4						
{						
	SetCallHelper( 100, MI_ZOMBIGER3, 5, TRUE);					
	SetCallHelper( 100, MI_ZOMBIGER2, 5, TRUE);					
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 			
                        Berserk 30 1.5						
			summon 10 1 MI_ZOMBIGER3			
			recovery 10 50 0 m         			
                    }						
		#move				
		    {				
			loot d 3			
		    }				
   	}               					
}						
//극단 사냥용 몬스터						
MI_FLYBAT1						
{						
	SetCallHelper( 100, MI_FLYBAT3, 1, TRUE);					
	SetCallHelper( 100, MI_FLYBAT2, 2, TRUE);					
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack cunning low			
                                    						
                    }						
		#move				
		    {				
			Loot d 3			
		    }	 			
   	}               					
}						
MI_FLYBAT2						
{						
	SetCallHelper( 100, MI_FLYBAT1, 2, TRUE);					
	SetCallHelper( 100, MI_FLYBAT3, 1, TRUE);					
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack cunning low			
                                    						
                    }						
		#move				
		    {				
			Loot d 5			
		    }				
   	}               					
}						
MI_FLYBAT3						
{						
	SetCallHelper( 100, MI_FLYBAT1, 2, TRUE);					
	SetCallHelper( 100, MI_FLYBAT2, 2, TRUE);					
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack cunning low			
                                    						
                    }						
		#move				
		    {				
			Loot d 3			
		    }				
   	}               					
}						
MI_FLYBAT4						
{						
	SetCallHelper( 100, MI_FLYBAT3, 5, TRUE);					
	SetCallHelper( 100, MI_FLYBAT2, 5, TRUE);					
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack cunning low			
                                    						
                    }						
		#move				
		    {				
			Loot d 3			
		    }				
   	}               					
}						
MI_BUCROW1						
{						
	SetCallHelper( 100, MI_BUCROW2, 2, TRUE);					
	SetCallHelper( 100, MI_BUCROW3, 1, TRUE);					
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack cunning low			
                                    						
                    }						
		#move				
		    {				
			Loot d 5			
		    }				
   	}               					
}						
MI_BUCROW2						
{						
	SetCallHelper( 100, MI_BUCROW1, 2, TRUE);					
	SetCallHelper( 100, MI_BUCROW3, 1, TRUE);					
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack cunning low			
                                    						
                    }						
		#move				
		    {				
			Loot d 5			
		    }	 			
   	}               					
}						
MI_BUCROW3						
{						
	SetCallHelper( 100, MI_BUCROW2, 2, TRUE);					
	SetCallHelper( 100, MI_BUCROW1, 2, TRUE);					
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack cunning low			
                                    						
                    }						
		#move				
		    {				
			Loot d 3			
		    }				
   	}               					
}						
MI_BUCROW4						
{						
	SetCallHelper( 100, MI_BUCROW2, 5, TRUE);					
	SetCallHelper( 100, MI_BUCROW3, 5, TRUE);					
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 			
                        Berserk 30 1.5						
			summon 10 1 MI_BUCROW3			
			recovery 10 50 0 m			
                                    						
                    }						
		#move				
		    {				
			Loot d 3			
		    }				
   	}               					
}						
MI_SCOPICON1						
{						
	SetCallHelper( 100, MI_SCOPICON2, 2, TRUE);					
	SetCallHelper( 100, MI_SCOPICON3, 1, TRUE);					
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack cunning low			
                                    						
                    }						
		#move				
		    {				
			Loot d 3			
		    }				
   	}               					
}						
MI_SCOPICON2						
{						
	SetCallHelper( 100, MI_SCOPICON1, 2, TRUE);					
	SetCallHelper( 100, MI_SCOPICON3, 1, TRUE);					
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack cunning low			
                                    						
                    }						
		#move				
		    {				
			Loot d 3			
		    }				
   	}               					
}						
MI_SCOPICON3						
{						
	SetCallHelper( 100, MI_SCOPICON2, 2, TRUE);					
	SetCallHelper( 100, MI_SCOPICON1, 2, TRUE);					
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack cunning low			
                                    						
                    }						
		#move				
		    {				
			Loot d 3			
		    }				
   	}               					
}						
MI_SCOPICON4						
{						
	SetCallHelper( 100, MI_SCOPICON2, 5, TRUE);					
	SetCallHelper( 100, MI_SCOPICON3, 5, TRUE);					
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 			
                        Berserk 30 1.5						
			summon 10 1 MI_SCOPICON3			
			recovery 10 30 0 m			
                                    						
                    }						
		#move				
		    {				
			Loot d 3			
		    }				
   	}               					
}						
MI_TRANGFOMA1						
{						
	SetCallHelper( 100, MI_TRANGFOMA2, 2, TRUE);					
	SetCallHelper( 100, MI_TRANGFOMA3, 1, TRUE);					
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack cunning low			
                                    						
                    }						
		#move				
		    {				
			Loot d 5			
		    }				
   	}               					
}						
MI_TRANGFOMA2						
{						
	SetCallHelper( 100, MI_TRANGFOMA1, 2, TRUE);					
	SetCallHelper( 100, MI_TRANGFOMA3, 1, TRUE);					
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack cunning low			
                                    						
                    }						
		#move				
		    {				
			Loot d 5			
		    }				
   	}               					
}						
MI_TRANGFOMA3						
{						
	SetCallHelper( 100, MI_TRANGFOMA2, 2, TRUE);					
	SetCallHelper( 100, MI_TRANGFOMA1, 2, TRUE);					
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack cunning low			
                                    						
                    }						
		#move				
		    {				
			Loot d 5			
		    }				
   	}               					
}						
MI_TRANGFOMA4						
{						
	SetCallHelper( 100, MI_TRANGFOMA2, 5, TRUE);					
	SetCallHelper( 100, MI_TRANGFOMA3, 5, TRUE);					
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 			
                        Berserk 30 1.5						
			summon 10 1 MI_TRANGFOMA3			
			recovery 10 30 0 m			
                                    						
                    }						
		#move				
		    {				
			Loot d 3			
		    }				
   	}               					
}						
MI_CLOCKWORK1						
{
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
			Berserk 30 2			
                        recovery 5 20 0 m            						
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
// 다콘세션3용 몬스터 아이템 리스폰						
MI_GRRR1	// 그르릉					
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_GRRR2						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack  			
                                    						
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_GRRR3						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack cunning low			
                        Rangeattack 6            						
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_GRRR4						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 			
                        Berserk 30 1.5						
			summon 10 1 MI_GRRR3			
			recovery 10 30 0 m            			
                    }						
		#move				
		    {				
			loot d 5			
		    {				
		    }				
   	}               					
}						
MI_KRRR						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 			
                        Berserk 30 1.5						
			summon 10 1 MI_GRRR3			
			recovery 10 30 0 m            			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_MUSHMOOT						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 			
                        Berserk 30 1.5						
			summon 10 1 MI_GRRR3			
			recovery 10 30 0 m            			
                    }						
		#move				
		    {				
		    }				
   	}               					
}      						
MI_SMALL_MUSHPOIE						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_DUMP1	//덤프					
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 60 			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_DUMP2						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack  			
                        keeprangeattack 5            						
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_DUMP3						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack cunning low			
                        Rangeattack 6            						
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_DUMP4						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 			
                        recovery 20 30 0 a						
			Berserk 30 1.8			
			summon 10 1 MI_DUMP3            			
                    }						
		#move				
		    {				
			loot d 3			
		    }				
   	}               					
}						
MI_NAUTREPY1						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_NAUTREPY2						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack  			
                                   						
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_NAUTREPY3						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack cunning low			
                                    						
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_NAUTREPY4						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 			
                        recovery 10 20 0 a						
			Berserk 30 1.5			
			summon 10 1 MI_NAUTREPY3            			
                    }						
		#move				
		    {				
			loot d 4			
		    }				
   	}               					
}						
MI_BOO1						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			 			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_BOO2						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack  			
                                   						
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_BOO3						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack cunning low			
                                    						
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_BOO4						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 			
                        recovery 10 30 0 a						
			Berserk 30 1.5			
			summon 10 1 MI_BOO3            			
                    }						
		#move				
		    {				
			loot d 4			
		    }				
   	}               					
}						
MI_HOPPRE1						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_HOPPRE2						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack  			
                        keeprangeattack 8            						
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_HOPPRE3						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack cunning low			
                                   						
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_HOPPRE4						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 			
                        Berserk 30 1.5						
			summon 10 1 MI_HOPPRE3			
			recovery 10 20 0 m            			
                    }						
		#move				
		    {				
			loot d 3			
		    }				
   	}               					
}						
MI_MUSHPOIE1						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_MUSHPOIE2						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack  			
                                    						
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_MUSHPOIE3						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack cunning low			
                                    						
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_MUSHPOIE4						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 			
                        recovery 10 30 0 a						
			Berserk 30 1.8			
			summon 10 1 MI_MUSHPOIE3            			
                    }						
		#move				
		    {				
			loot d 5			
		    }				
   	}               					
}						
MI_IREN1						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 60 			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_IREN2						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack  			
                                   						
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_IREN3						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack cunning low			
                                   						
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_IREN4						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 			
                        Berserk 30 1.5						
			summon 10 1 MI_IREN3			
			recovery 10 30 0 a            			
                    }						
		#move				
		    {				
			loot d 5			
		    }				
   	}               					
}						
MI_WATANGKA1						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_WATANGKA2						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack  			
                        keeprangeattack 5            						
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_WATANGKA3						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack cunning low			
                        recovery 10 10 0 a            						
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_WATANGKA4						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 			
                       	Berserk 30 1.5					
			summon 10 1 MI_WATANGKA3            			
                    }						
		#move				
		    {				
			loot d 3			
		    }				
   	}               					
}						
MI_ANTIQUERY1						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			 			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_ANTIQUERY2						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack  			
                                    						
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_ANTIQUERY3						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack cunning low			
                        						
			            			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_ANTIQUERY4						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 			
                        recovery 10 50 0 a 						
			Berserk 30 1.5			
			summon 10 1 MI_ANTIQUERY3            			
                    }						
		#move				
		    {				
			loot d 4 			
		    }				
   	}               					
}						
MI_LUIA1						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 60 			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_LUIA2						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack  			
                                    						
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_LUIA3						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack cunning low			
                                    						
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_LUIA4						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 			
                        Berserk 30 1.5						
			recovery 10 50 0 m			
			summon 10 1 MI_LUIA3            			
                    }						
		#move				
		    {				
			loot d 3			
		    }				
   	}               					
}						
MI_GONGURY1						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_GONGURY2						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack  			
                                   						
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_GONGURY3						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack cunning low			
                        recovery 10 20 0 a            						
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_GONGURY4						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 			
                        Berserk 30 1.6						
			summon 10 1 MI_GONGURY3			
			recovery 20 30 0 a           			
                    }						
		#move				
		    {				
			loot d 3			
		    }				
   	}               					
}						
MI_SHUHAMMA1						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 60 			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_SHUHAMMA2						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack  			
                        keeprangeattack 5            						
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_SHUHAMMA3						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack cunning low			
                        Rangeattack 6            						
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_SHUHAMMA4						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 			
                        Berserk 30 1.6						
			summon 10 1 MI_SHUHAMMA3             			
                    }						
		#move				
		    {				
			loot d 4			
		    }				
   	}               					
}						
MI_KERN1						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			 			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_KERN2						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack  			
                                    						
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_KERN3						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack cunning low			
                        recovery 20 10 0 a            						
                    }						
		#move				
		    {				
			loot d 9			
		    }				
   	}               					
}						
MI_KERN4						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 			
                        Recovery 10 50 0 m						
			Berserk 30 1.5			
			summon 10 1 MI_KERN3            			
                    }						
		#move				
		    {				
			loot d 5			
		    }				
   	}               					
}						
MI_GLAPHAN1						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 60 			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_GLAPHAN2						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack  			
                                    						
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_GLAPHAN3						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack cunning low			
                                    						
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_GLAPHAN4						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 			
                        summon 10 1 MI_GLAPHAN3						
			Berserk 30 1.8			
			recovery 10 40 0 m            			
                    }						
		#move				
		    {				
			loot d 5			
		    }				
   	}               					
}						
//신규 상위레벨 몬스터						
MI_KIMERADON1	//키메라돈					
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 			
			berserk 15 2                                 			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_KIMERADON2						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 			
			berserk 15 2                                 			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_KIMERADON3						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
			berserk 15 2.5                                 			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_KIMERADON4						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
			berserk 15 3                                  			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_KIMERADON5						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack  			
			berserk 15 3.5                               			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_BEARNUCKY1	// 베어너키					
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 			
			berserk 15 2                                			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_BEARNUCKY2						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack    			
			berserk 15 2                             			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_BEARNUCKY3						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack   			
			berserk 15 2.5                              			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_BEARNUCKY4						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
			berserk 15 3                                 			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_BEARNUCKY5						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 			
			berserk 15 3.5                                			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_MUFFRIN1	//머푸린					
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack   			
			berserk 15 2                               			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_MUFFRIN2						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack   			
			berserk 15 2                            			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_MUFFRIN3						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
			berserk 15 2.5                                 			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_MUFFRIN4						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 			
			berserk 15 3                                			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_MUFFRIN6						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 			
			berserk 15 3.5                                 			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_POPCRANK1						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack  			
			berserk 15 2                                			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_POPCRANK2						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack   			
			berserk 15 2                               			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_POPCRANK3						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			 Attack 			
			berserk 15 2.5                                			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_POPCRANK4						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
			berserk 15 3                                  			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_POPCRANK5						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack  			
			berserk 15 3.5                               			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
//던전용 몬스터						
MI_DUFEFERN1						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 20 cunning sam 			
                                   						
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_DUFEFERN2						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack cunning sam			
                        						
                        berserk 20 1.25           						
                    }						
		#move				
		    {				
			loot d 5			
		    }				
   	}               					
}						
MI_DUFEFERN3						
{						
	m_dwAttackMoveDelay = 0;					
	m_dwRunawayDelay = 1000;					
	SetRunAway( 10, 0, 1 );					
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
                          						
                                    						
                    }						
		#move				
		    {				
			loot d 5			
		    }				
   	}            					
}						
MI_DUNYANGNYANG1						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 30 cunning sam			
                                    						
                    }						
		#move				
		    {				
			loot d 3			
		    }				
   	}               					
}						
MI_DUNYANGNYANG2						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack cunning sam			
                        						
                                    						
                    }						
		#move				
		    {				
		    }				
   	}              					
}						
MI_DUNYANGNYANG3						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 			
                          						
                        Berserk 20 1.4            						
                    }						
		#move				
		    {				
			loot d 3			
		    }				
   	}               					
}						
MI_DUBANG1						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 10  			
                        berserk 20 1.2            						
                    }						
		#move				
		    {				
		    }				
   	}            					
}						
MI_DUBANG2						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack cunning sam			
                        						
                        Evade 10            						
                    }						
		#move				
		    {				
		    }				
   	}   					
}						
MI_DUBANG3						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 			
                          						
                                    						
                    }						
		#move				
		    {				
			loot d 4			
		    }				
   	}                            					
}						
MI_WORMVEDUQUE						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack cunning low			
    		    }				
		#move				
		    {				
			loot d 2			
		    }				
   	}               					
}						
MI_SERUSURIEL						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack cunning low			
			recovery 20 70 0 m			
			berserk 10 2                			
                    }						
		#move				
		    {				
			loot d 2			
		    }				
   	}               					
}						
MI_VICEVEDUQUE						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack cunning low			
                    }						
		#move				
		    {				
			loot d 2			
		    }				
   	}               					
}						
//퀘스트 몬스터 설정						
MI_GUARDMON1						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
		                        				
                    }						
		#move				
		    {				
		    }				
   	}   					
}						
MI_SHURAITURE						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
		         berserk 10 1.5              				
                    }						
		#move				
		    {				
		    }				
   	}   					
}						
MI_REN						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
		         summon 30 1 MI_AIBATT3				
			 berserk 10 1.5              			
                    }						
		#move				
		    {				
		    }				
   	}   					
}						
MI_SISIF						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
		         berserk 15 1.5            				
                    }						
		#move				
		    {				
		    }				
   	}   					
}						
MI_RUBO						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
		         summon 30 1 MI_AIBATT3				
			 berserk 10 1.5             			
                    }						
		#move				
		    {				
		    }				
   	}   					
}						
MI_CHANER						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
		         berserk 10 1.5              				
                    }						
		#move				
		    {				
		    }				
   	}   					
}						
MI_BABARI						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
		         berserk 10 1.5              				
                    }						
		#move				
		    {				
		    }				
   	}   					
}						
MI_SEIDO						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
		         berserk 10 1.5              				
                    }						
		#move				
		    {				
		    }				
   	}   					
}						
MI_KIDLER						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
		         summon 30 2 MI_LAWOLF1				
			 berserk 15 2             			
                    }						
		#move				
		    {				
		    }				
   	}   					
}						
MI_SHAKALPION						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
		         berserk 20 2             				
                    }						
		#move				
		    {				
		    }				
   	}   					
}						
MI_HOIREN						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
		         summon 30 3 MI_WAGSAAC2				
			 berserk 10 1.5             			
                    }						
		#move				
		    {				
		    }				
   	}   					
}						
MI_HUNTERX						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
		         summon 30 3 MI_BANG2				
			 berserk 10 1.5             			
                    }						
		#move				
		    {				
		    }				
   	}   					
}						
MI_KYNSY						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
		         summon 30 3 MI_FEFERN3				
			 berserk 15 1.5             			
                    }						
		#move				
		    {				
		    }				
   	}   					
}						
//일반 퀘스트 몬스터						
MI_RBANG1						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
		         berserk 20 2             				
                    }						
		#move				
		    {				
		    }				
   	}   					
}						
MI_RBANG2						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
		         berserk 25 2             				
                    }						
		#move				
		    {				
		    }				
   	}   					
}						
MI_LORDBANG						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan quest 112			
                    }						
		#battle				
		    {				
		         berserk 10 2             				
                    }						
		#move				
		    {				
		    }				
   	}   					
}						
MI_LBHANOYAN						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
		         berserk 10 3            				
                    }						
		#move				
		    {				
		    }				
   	}   					
}						
MI_MOMYORN						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
		                     				
                    }						
		#move				
		    {				
		    }				
   	}					
}						
MI_CLOCKS						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
		         berserk 10 2.5             				
                    }						
		#move				
		    {				
		    }				
   	}   					
}						
//시나리오 퀘스트(2차) 몬스터						
MI_VIOLMAGICION						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan quest 1019			
                    }						
		#battle				
		    {				
		         berserk 15 2             				
                    }						
		#move				
		    {				
		    }				
   	}   					
}						
MI_VIOLMAGICION2						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan quest 1021			
                    }						
		#battle				
		    {				
		         berserk 20 2             				
                    }						
		#move				
		    {				
		    }				
   	}   					
}						
MI_VIOLMAGICION3						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan quest 1024			
                    }						
		#battle				
		    {				
		         berserk 15 3            				
                    }						
		#move				
		    {				
		    }				
   	}   					
}						
MI_EMERALDMANTIS						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan quest 118			
                    }						
		#battle				
		    {				
		         berserk 15 3            				
                    }						
		#move				
		    {				
		    }				
   	}   					
}						
//일반 퀘스트(보스Mob) 몬스터						
MI_ORGANIGOR						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
		         berserk 25 2             				
                    }						
		#move				
		    {				
		    }				
   	}   					
}						
MI_GANGARD						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
		         berserk 20 2             				
                    }						
		#move				
		    {				
		    }				
   	}   					
}						
MI_HADESEOR						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
		         berserk 15 2             				
                    }						
		#move				
		    {				
		    }				
   	}   					
}						
//P.K 관련 경비병 및 몬스터 세팅						
MI_MaEw_GUARDIAN01						
{						
	m_nAttackFirstRange = 10;					
	SetCallHelper( 0, 0, 0, FALSE );					
	AI					
	{					
		#Scan				
                    {						
			scan range 10 chao 100			
                    }						
		#battle				
		    {				
			attack			
			Helper sam 5000 1			
			Rangeattack 10              			
                    }						
		#move				
		    {				
		    }				
   	}   					
}						
MI_MaEw_GUARDIAN02						
{						
	m_nAttackFirstRange = 10;					
	SetCallHelper( 0, 0, 0, FALSE );					
	AI					
	{					
		#Scan				
                    {						
			scan range 10 chao 100			
                    }						
		#battle				
		    {				
			attack			
			Helper sam 5000 1			
			Rangeattack 10              			
                    }						
		#move				
		    {				
		    }				
   	}   					
}						
MI_GUARDIAN						
{						
	m_nAttackFirstRange = 10;					
	SetCallHelper( 0, 0, 0, FALSE );					
	AI					
	{					
		#Scan				
                    {						
			scan range 10 chao 100			
                    }						
		#battle				
		    {				
			attack			
			Helper sam 5000 1			
			Rangeattack 10              			
                    }						
		#move				
		    {				
		    }				
   	}   					
}						
MI_CHAOGUARDIAN						
{						
	m_nAttackFirstRange = 10;					
	SetCallHelper( 0, 0, 0, FALSE );					
	AI					
	{					
		#Scan				
                    {						
			scan range 10 chao 101			
                    }						
		#battle				
		    {				
		       attack				
		       Helper sam 5000 1 				
		       Rangeattack 10                  				
                    }						
		#move				
		    {				
		    }				
   	}   					
}						
MI_PK_FEFERN						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
		                       				
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_PK_WAGSAAC						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
		                       				
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_PK_MRPUMPKIN						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
		                       				
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_PK_GIGGLEBOX						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
		                       				
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_PK_HOBO						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
		                       				
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_PK_CARDPUPPET						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
		                       				
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_PK_BASQUE						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
		                       				
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_PK_LEYENA						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
		                       				
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_PK_STEELKNIGHT						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
		                       				
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_PK_VOLT						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
		                       				
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_PK_GARBAGEPIDER						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
		                       				
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_PK_GREEMONG						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
		                       				
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_PK_HOPPRE						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
		                       				
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_PK_IREN						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
		                       				
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_PK_WATANGKA						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
		                       				
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_PK_LUIA						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
		                       				
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_PK_SHUHAMMA						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
		                       				
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_PK_GLAPHAN						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
		                       				
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_HAMMERKICK						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
		                       				
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_ANTTURTLE						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
		                       				
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
//데카네스 던전관련 몬스터						
MI_DU_DKKEAKOON1						
{						
	
	m_nAttackFirstRange = 10;					
	AI					
	{ 					
		#Scan				
                    {						
                    }						
		#battle				
		    {				
			evade 5               			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_DU_DKKEAKOON2						
{						
	
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan range 5 quest 68			
                    }						
		#battle				
		    {				
			                 			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_DU_DKKEAKOON3						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
                    }						
		#battle				
		    {				
			evade 1                			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_DU_DKKEAKOON4						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan range 5 quest 68			
                    }						
		#battle				
		    {    				
			berserk 10 1.5           			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_DU_DKKEAKOON5						
{						
	
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
                    }						
		#battle				
		    {				
			evade 1                  			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_DU_DKKEAKOON6						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan range 5 quest 70			
                    }						
		#battle				
		    {				
		                       				
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_DU_DKKEAKOON7						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan range 5 quest 70			
                    }						
		#battle				
		    {				
			evade 1             			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_DU_DKKEAKOON8						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan range 5 quest 70			
                    }						
		#battle				
		    {    				
			berserk 10 1.5			
			recovery 20 50 0 m               			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_DU_DKKEAKOON9						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    { 				
			berserk 10 1.5			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_DU_DKKEAKOON10						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			berserk 10 1.5			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_DU_DKKEAKOON11						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {  				
			berserk 10 1.5           			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_DU_DKKEAKOON12						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {  				
			 berserk 10 1.8			
		         recovery 20 70 0 a             				
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_DU_DKROACHFL1						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
		                       				
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_DU_DKROACHFL2						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
		                       				
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_DU_DKROACHFL3						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
		        berserk 10 1.5              				
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_DU_DKROACHFL4						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
		        berserk 10 1.5           				
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_DU_DKROACHFL5						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
		       				
		       evade 5   		              		
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_DU_DKROACHFL6						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
		       evade 5               				
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_DU_DKTRILLIPY1						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
		        berserk 15 1.2     				
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_DU_DKTRILLIPY2						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {     				
			berserk 20 1.5			
			recovery 20 50 0 m                			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_DU_DKTRILLIPY3						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
		     	berserk 15 1.2             			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_DU_DKTRILLIPY4						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {  				
			berserk 20 1.5			
			recovery 20 50 0 m                 			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_DU_DKTRILLIPY5						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
		        berserk 15 1.2             				
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_DU_DKTRILLIPY6						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {  				
			berserk 20 1.5			
			recovery 20 50 0 m               			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_DU_DKKIMERADON1						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
		                       				
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_DU_DKKIMERADON2						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
		        berserk 30 1.8				
			recovery 20 50 0 m               			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_DU_METEONYKER						
{						

	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
		        berserk 20 2.0				
        						
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_DU_METEONYKER2						
{						
	DropItem(II_GEN_JEW_RIN_INTRING,3000000000, 18, 1);
	DropItem(II_GEN_JEW_RIN_DEXRING,3000000000, 18, 1);
	DropItem(II_GEN_JEW_RIN_STRRING,3000000000, 18, 1);					
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#battle				
		{				
			berserk 2 1.5			
		}				
	}					
}						
// 이벤트용 몬스터 세팅						
MI_DEMIAN5						
{						
	m_nAttackFirstRange = 8;					
}						
MI_KEAKOON5						
{						
	m_nAttackFirstRange = 8;					
}						
MI_MUFFRIN5						
{						
	m_nAttackFirstRange = 8;					
}						
// 미국 이벤트 몬스터						
MI_LOADCLOCKWORK						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
			Berserk 30 2			
                        recovery 5 20 0 m            						
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_BRIGADIER						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
			Berserk 30 1.5			
                        recovery 5 20 0 m            						
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_LIEUTENANT 						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
			Berserk 30 1.5			
                        recovery 5 20 0 m            						
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_SPIKETAIL 						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
			Berserk 30 1.5			
                        recovery 5 20 0 m            						
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_GLYPHAXZ 						
{						

	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
			Berserk 20 1.7			
                        summon 50 2 MI_DU_DKKIMERADON1       						
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
//이블리스의 사원 몬스터						
MI_IBLCRASHER						
{						

	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
			Berserk 20 1.5			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_IBLPOISONER						
{						

	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
			Berserk 20 1.5			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_IBLWRECKER						
{						

	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
			Berserk 20 1.5			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_IBLDOZER						
{						

	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
			Berserk 20 1.5			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_IBLPUPPET						
{						

	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
			Berserk 20 1.5			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_IBLTAKER						
{						

	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
			Berserk 20 1.5			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_IBLREDOTEM						
{						

	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
			Berserk 20 1.5			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_IBLBLACKOTEM						
{						

	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
			Berserk 20 1.5			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_IBLBOXTER						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
			Berserk 30 2.0			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_IBLDANDISHER						
{						

	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
			Berserk 20 1.5			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_IBLGUARDER						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
			Berserk 20 1.0			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_IBLQUAKER						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
			Berserk 20 1.0			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_IBLMUCILAGER						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
			Berserk 20 1.0			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_IBLLINESS						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 			
                        Berserk 30 1.5						
			summon 30 3 MI_IBLDANDISHER			
			recovery 10 30 0 m            			
                    }						
		#move				
		    {				
		    }				
   	}               					
}                 						
MI_CYCLOPSX						
{						

	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
			Berserk 20 2.0			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_NPC_YETI01						
{						

	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
                    }						
		#move				
		    {				
	            }					
   	}               					
}						
MI_NPC_YETI02						
{						

	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
                    }						
		#move				
		    {				
	            }					
   	}               					
}						
MI_NPC_AUGOO01						
{						

	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
                    }						
		#move				
		    {				
	            }					
   	}               					
}						
MI_NPC_AUGOO02						
{						

	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
                    }						
		#move				
		    {				
	            }					
   	}               					
}						
MI_NPC_SADKING01						
{						

	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
                    }						
		#move				
		    {				
	            }					
   	}               					
}						
MI_NPC_SADKING02						
{						

	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
                    }						
		#move				
		    {				
	            }					
   	}               					
}						
MI_NPC_MAMMOTH01						
{						

	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
                    }						
		#move				
		    {				
	            }					
   	}               					
}						
MI_NPC_MAMMOTH02						
{						

	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
                    }						
		#move				
		    {				
	            }					
   	}               					
}						
MI_EVENT01						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
			Berserk 20 2.0			
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
MI_EVENT02						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
			Berserk 20 2.0			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_EVENT03						
{		
	SetLevelDropPanalty_Off				
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
			Berserk 20 2.0			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_EVENT04						
{		
	SetLevelDropPanalty_Off				
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
			Berserk 20 2.0			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
//12차 버전 추가 몬스터(임시버전)						
MI_CAITSITH01						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_CAITSITH02						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_CAITSITH03						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_CAITSITH04						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_CAITSITH04_1						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_HARPY01						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_HARPY02						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_HARPY03						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_HARPY04						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_HARPY04_1						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_POLEVIK01						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_POLEVIK02						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_POLEVIK03						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_POLEVIK04						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_POLEVIK04_1						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_ABRAXAS01						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_ABRAXAS02						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_ABRAXAS03						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_ABRAXAS04						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_ABRAXAS04_1						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_HAG01						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_HAG02						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_HAG03						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_HAG04						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_HAG04_1						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_THOTH01						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_THOTH02						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_THOTH03						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_THOTH04						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_THOTH04_1						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_KHNEMU01						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_KHNEMU02						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_KHNEMU03						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_KHNEMU04						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_KHNEMU04_1						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_DANTALIAN01						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_DANTALIAN02						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_DANTALIAN03						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_DANTALIAN04						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_DANTALIAN04_1						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_GANESA01						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_GANESA02						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_GANESA03						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_GANESA04						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_GANESA04_1						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_ASURA01						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_ASURA02						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_ASURA03						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_ASURA04						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_ASURA04_1						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_IMP01						
{						

	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
                        Rangeattack 6  						
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_IMP02						
{						

	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
                        Rangeattack 6  						
			Summon	40	2	MI_IMP01
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_IMP03						
{						

	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
                        Rangeattack 6  						
			Summon	30	2	MI_IMP02
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_LUCIFER01						
{						

	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
                        Rangeattack 10						
                        Berserk 10 1.5 						
			Summon	20	2	MI_IMP03
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_RANGDA01						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 			
                        Berserk 30 1.5						
			recovery 10 50 0 m  			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_RANGDA02						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 			
                        Berserk 30 1.5						
			recovery 10 50 0 m  			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_RANGDA05						
{		
	SetLevelDropPanalty_Off				
	
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 			
                        Berserk 30 1.5						
			recovery 10 50 0 m  			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_RANGDA03						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 			
                        Berserk 30 1.5						
			recovery 10 50 0 m  			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_RANGDA04						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 			
                        Berserk 30 1.5						
			recovery 10 50 0 m  			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_KINGSTER01						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_KINGSTER02						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_KINGSTER03						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_KRAKEN01						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_KRAKEN02						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_KRAKEN03						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_CREPER01						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_CREPER02						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_CREPER03						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_NAGA01						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_NAGA02						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_NAGA03						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_ATROX01						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_ATROX02						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_ATROX03						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_OKEAN01						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_OKEAN02						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_OKEAN03						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_TAIGA01						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_TAIGA02						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_TAIGA03						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_DORIAN01						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_DORIAN02						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_DORIAN03						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_MEREL01						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_MEREL02						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_MEREL03						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_Crohell01						
{						

	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
			Berserk 20 2.0			
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
MI_Crohell02						
{						

	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
			Berserk 20 2.0			
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
MI_Crohell03						
{						

	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
			Berserk 20 2.0			
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
MI_Crohell04						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
			Berserk 20 3.0			
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
MI_Frinker01						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
			Berserk 20 2.0			
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
MI_Frinker02						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
			Berserk 20 2.0			
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
MI_Frinker03						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
			Berserk 20 2.0			
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
MI_Frinker04						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
			Berserk 20 3.0			
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
MI_Toadrin01						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
			Berserk 20 2.0			
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
MI_Toadrin02						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
			Berserk 20 2.0			
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
MI_Toadrin03						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
			Berserk 20 2.0			
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
MI_Toadrin04						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
			Berserk 20 3.0			
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
MI_Hatsalra01						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
			Berserk 20 2.0			
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
MI_Hatsalra02						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
			Berserk 20 2.0			
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
MI_Hatsalra03						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
			Berserk 20 2.0			
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
MI_Hatsalra04						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
			Berserk 20 3.0			
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
MI_Berken01						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
			Berserk 20 2.0			
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
MI_Berken02						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
			Berserk 20 2.0			
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
MI_Berken03						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
			Berserk 20 2.5			
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
MI_Berken04						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
			Berserk 20 3.0			
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
MI_PRICKANT01						
{						

	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
			Berserk 20 2.5			
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
MI_PRICKANT02						
{						

	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
			Berserk 20 2.5			
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
MI_PRICKANT03						
{						

	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
			Berserk 20 2.5			
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
MI_PRICKANT04						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
			Berserk 20 2.5			
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
MI_CRIPESCENTIPEDE01						
{						

	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
			Berserk 20 2.5			
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
MI_CRIPESCENTIPEDE02						
{						

	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
			Berserk 20 2.5			
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
MI_CRIPESCENTIPEDE03						
{						

	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
			Berserk 20 2.5			
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
MI_CRIPESCENTIPEDE04						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
			Berserk 20 2.5			
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
MI_MAULMOUSE01						
{						

	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
			Berserk 20 2.5			
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
MI_MAULMOUSE02						
{						

	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
			Berserk 20 2.5			
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
MI_MAULMOUSE03						
{						

	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
			Berserk 20 2.5			
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
MI_MAULMOUSE04						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
			Berserk 20 2.5			
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
MI_LYCANOS01						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
			Berserk 20 2.5			
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
MI_VEMPAIN01						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
                        Rangeattack 10						
			Berserk 20 3.0			
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
MI_PRICKANT01_1						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
			Berserk 20 2.5			
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
MI_PRICKANT02_1						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
			Berserk 20 2.5			
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
MI_PRICKANT03_1						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
			Berserk 20 2.5			
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
MI_PRICKANT04_1						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
			Berserk 20 2.5			
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
MI_CRIPESCENTIPEDE01_1						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
			Berserk 20 2.5			
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
MI_CRIPESCENTIPEDE02_1						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
			Berserk 20 2.5			
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
MI_CRIPESCENTIPEDE03_1						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
			Berserk 20 2.5			
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
MI_CRIPESCENTIPEDE04_1						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
			Berserk 20 2.5			
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
MI_MAULMOUSE01_1						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
			Berserk 20 2.5			
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
MI_MAULMOUSE02_1						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
			Berserk 20 2.5			
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
MI_MAULMOUSE03_1						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
			Berserk 20 2.5			
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
MI_MAULMOUSE04_1						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
			Berserk 20 2.5			
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
MI_LYCANOS01_1						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
			Berserk 20 2.5			
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
MI_VEMPAIN01_1						
{						
      				
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
                        Rangeattack 10						
			Berserk 20 3.0			
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
MI_MOCOMOCHI1						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
                        Rangeattack 10						
			Berserk 20 3.0			
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
//15차 몬스터						
MI_SKELWOLF						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Berserk 20 2.0			
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
MI_SKELSWORD						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
			Berserk 20 2.0			
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
MI_SKELSPEAR						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
			Berserk 20 2.0			
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
MI_SKELMAGE						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
			keeprangeattack 6			
			Berserk 20 2.0			
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
MI_SKELASSASSIN						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
			Berserk 20 2.0			
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
MI_SKELFIGHTER						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
			Berserk 20 2.0			
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
MI_SKELGENERAL						
{						

	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 20 cunning sam			
                        Rangeattack 10						
			Berserk 20 2.0			
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
MI_SKELGRIFFIN						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 10 cunning sam			
			Berserk 20 2.0			
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
MI_SKELLEADER						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 10 cunning sam			
			Berserk 20 2.0			
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
MI_SKELSPAIN						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 10 cunning sam			
			Berserk 20 2.0			
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
MI_SKELSHAMEN						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 10 cunning sam			
			Berserk 20 2.0			
			Keeprangeattack 6			
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
MI_SKELRIDER						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 10 cunning sam			
			Berserk 20 2.0			
			rangeattack 6			
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
MI_SKELDEVIL						
{						
	 
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 20 cunning sam			
                        Rangeattack 10						
			Berserk 20 2.0			
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
MI_DREADSTONE01						
{						
	AI					
	{					
		#Scan				
                    {						
                    }						
		#battle				
		    {				
			Summon	20	2	MI_SKELGRIFFIN
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_DREADSTONE02						
{						
	AI					
	{					
		#Scan				
                    {						
                    }						
		#battle				
		    {				
			Summon	20	2	MI_SKELLEADER
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_DREADSTONE03						
{						
	AI					
	{					
		#Scan				
                    {						
                    }						
		#battle				
		    {				
			Summon	20	2	MI_SKELSPAIN
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_DREADSTONE04						
{						
	AI					
	{					
		#Scan				
                    {						
                    }						
		#battle				
		    {				
			Summon	20	2	MI_SKELSHAMEN
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_DREADSTONE05						
{						
	AI					
	{					
		#Scan				
                    {						
                    }						
		#battle				
		    {				
			Summon	20	2	MI_SKELRIDER
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_DREADSTONE06						
{						
	AI					
	{					
		#Scan				
                    {						
                    }						
		#battle				
		    {				
			Summon	20	2	MI_SKELRIDER
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_RUSTIACRASHGATE01						
{						
	AI					
	{					
		#Scan				
                    {						
		    }				
		#battle				
		    {				
			Attack 10 cunning sam			
			Berserk 20 2.0			
			rangeattack 6			
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
MI_RUSTIACRASHGATE02						
{						
	AI					
	{					
		#Scan				
                    {						
		    }				
		#battle				
		    {				
			Attack 10 cunning sam			
			Berserk 20 2.0			
			rangeattack 6			
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
MI_RUSTIACRASHGATE03						
{						
	AI					
	{					
		#Scan				
                    {						
		    }				
		#battle				
		    {				
			Attack 10 cunning sam			
			Berserk 20 2.0			
			rangeattack 6			
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
MI_RUSTIACRASHGATE04						
{						
	AI					
	{					
		#Scan				
                    {						
		    }				
		#battle				
		    {				
			Attack 10 cunning sam			
			Berserk 20 2.0			
			rangeattack 6			
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
MI_BIGCOBRA01						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan 			
                    }						
		#battle				
		    {				
			Attack cunning low			
                               						
                    }						
		#move				
		    {				
			Loot d 5			
		    }				
   	}               					
}						
MI_BIGVARIETYCOBRA01						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan 			
                    }						
		#battle				
		    {				
			Attack cunning low			
                               						
                    }						
		#move				
		    {				
			Loot d 5			
		    }				
   	}               					
}						
MI_BIGRACCOON01						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan 			
                    }						
		#battle				
		    {				
			Attack cunning low			
                               						
                    }						
		#move				
		    {				
			Loot d 5			
		    }				
   	}               					
}						
MI_BIGSOLDIERRACCOON01						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan 			
                    }						
		#battle				
		    {				
			Attack cunning low			
                               						
                    }						
		#move				
		    {				
			Loot d 5			
		    }				
   	}               					
}						
MI_SOLDIERTANGZ01						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan 			
                    }						
		#battle				
		    {				
			Attack cunning low			
                               						
                    }						
		#move				
		    {				
			Loot d 5			
		    }				
   	}               					
}						
MI_SHAMANWUTANGKA01						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan 			
                    }						
		#battle				
		    {				
			Attack cunning low			
                               						
                    }						
		#move				
		    {				
			Loot d 5			
		    }				
   	}               					
}						
MI_BESIBIGFOOT01						
{						
	DropItem(II_GEN_JEW_NEC_HPNECKLACE,3000000000, 12, 1);
	DropItem(II_GEN_JEW_NEC_MPNECKLACE,3000000000, 12, 1);
	DropItem(II_GEN_JEW_NEC_FPNECKLACE,3000000000, 12, 1);
	DropItem(II_GEN_JEW_EAR_ATTEARRING,3000000000, 12, 1);
	DropItem(II_GEN_JEW_EAR_DEFEARRING,3000000000, 12, 1); 	 
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan 			
                    }						
		#battle				
		    {				
			Attack cunning low			
                               						
                    }						
		#move				
		    {				
			Loot d 5			
		    }				
   	}               					
}						
MI_BIGCOBRA02						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan 			
                    }						
		#battle				
		    {				
			Attack cunning low			
                               						
                    }						
		#move				
		    {				
			Loot d 5			
		    }				
   	}               					
}						
MI_BIGVARIETYCOBRA02						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan 			
                    }						
		#battle				
		    {				
			Attack cunning low			
                               						
                    }						
		#move				
		    {				
			Loot d 5			
		    }				
   	}               					
}						
MI_BIGRACCOON02						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan 			
                    }						
		#battle				
		    {				
			Attack cunning low			
                               						
                    }						
		#move				
		    {				
			Loot d 5			
		    }				
   	}               					
}						
MI_BIGSOLDIERRACCOON02						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan 			
                    }						
		#battle				
		    {				
			Attack cunning low			
                               						
                    }						
		#move				
		    {				
			Loot d 5			
		    }				
   	}               					
}						
MI_SOLDIERTANGZ02						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan 			
                    }						
		#battle				
		    {				
			Attack cunning low			
                               						
                    }						
		#move				
		    {				
			Loot d 5			
		    }				
   	}               					
}						
MI_SHAMANWUTANGKA02						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan 			
                    }						
		#battle				
		    {				
			Attack cunning low			
                               						
                    }						
		#move				
		    {				
			Loot d 5			
		    }				
   	}               					
}						
MI_BESIBIGFOOT02						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan 			
                    }						
		#battle				
		    {				
			Attack cunning low			
                               						
                    }						
		#move				
		    {				
			Loot d 5			
		    }				
   	}               					
}						
MI_RABBITGUARDER01						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan 			
                    }						
		#battle				
		    {				
			Attack cunning low			
                               						
                    }						
		#move				
		    {				
			Loot d 5			
		    }				
   	}               					
}						
MI_TUTTLESWORDER01						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan 			
                    }						
		#battle				
		    {				
			Attack cunning low			
                               						
                    }						
		#move				
		    {				
			Loot d 5			
		    }				
   	}               					
}						
MI_TUTTLEFIGHTER01						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan 			
                    }						
		#battle				
		    {				
			Attack cunning low			
                               						
                    }						
		#move				
		    {				
			Loot d 5			
		    }				
   	}               					
}						
MI_TUTTLEASSASSIN01						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan 			
                    }						
		#battle				
		    {				
			Attack cunning low			
                               						
                    }						
		#move				
		    {				
			Loot d 5			
		    }				
   	}               					
}						
MI_TUTTLESPEAR01						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan 			
                    }						
		#battle				
		    {				
			Attack cunning low			
                               						
                    }						
		#move				
		    {				
			Loot d 5			
		    }				
   	}               					
}						
MI_TUTTLEAXE01						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan 			
                    }						
		#battle				
		    {				
			Attack cunning low			
                               						
                    }						
		#move				
		    {				
			Loot d 5			
		    }				
   	}               					
}						
MI_TUTTLEKING01						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan 			
                    }						
		#battle				
		    {				
			Attack cunning low			
                               						
                    }						
		#move				
		    {				
			Loot d 5			
		    }				
   	}               					
}						
//16차 카일룬 초원						
MI_PUPPETWOLF						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan 			
                    }						
		#battle				
		    {				
			Attack			
			Berserk 20 2.0			
                               						
                    }						
		#move				
		    {				
			Loot d 5			
		    }				
   	}               					
}						
MI_RYSENTRY						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan 			
                    }						
		#battle				
		    {				
			Attack			
			Berserk 20 2.0			
                               						
                    }						
		#move				
		    {				
			Loot d 5			
		    }				
   	}               					
}						
MI_RYGUARD						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan 			
                    }						
		#battle				
		    {				
			Attack			
			Berserk 20 2.0			
                               						
                    }						
		#move				
		    {				
			Loot d 5			
		    }				
   	}               					
}						
MI_RYARCHER						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 10 cunning sam			
			Berserk 20 2.0			
			Keeprangeattack 8    			
			           			
                    }						
		#move				
		    {				
			loot d 3			
		    }				
   	}               					
}						
MI_RYMAGI						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 10 cunning sam			
			Berserk 20 2.0			
			Keeprangeattack 8                                 			         
                    }						
		#move				
		    {				
			loot d 3			
		    }				
   	}               					
}						
MI_RYWARRIOR						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan 			
                    }						
		#battle				
		    {				
			Attack			
			Berserk 20 2.0			
                               						
                    }						
		#move				
		    {				
			Loot d 5			
		    }				
   	}               					
}						
MI_RYBARGA						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan 			
                    }						
		#battle				
		    {				
			Attack 20 cunning sam			
                        Rangeattack 10						
			Berserk 20 2.0			
                               						
                    }						
		#move				
		    {				
			Loot d 5			
		    }				
   	}               					
}						
MI_CLOCKWORKBUTLER01						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
                        Rangeattack 10						
			Berserk 20 3.0			
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
MI_SPAINDRAGON01						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
                        Rangeattack 10						
			Berserk 20 3.0			
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
MI_DUMBBULL5						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
                        Rangeattack 10						
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
//16차 베히모스						
MI_STATUE						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan 			
                    }						
		#battle				
		    {				
			Attack			
			Berserk 20 2.0			
                               						
                    }						
		#move				
		    {				
			Loot d 5			
		    }				
   	}               					
}						
MI_SPIRITTULA						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan 			
                    }						
		#battle				
		    {				
			Attack			
			Berserk 20 2.0			
                               						
                    }						
		#move				
		    {				
			Loot d 5			
		    }				
   	}               					
}						
MI_SPIRITUMTULA						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan 			
                    }						
		#battle				
		    {				
			Attack 10 cunning sam			
			Berserk 20 2.0			
			Keeprangeattack 8 			
                               						
                    }						
		#move				
		    {				
			Loot d 5			
		    }				
   	}               					
}						
MI_SPIRITOBNIS						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan 			
                    }						
		#battle				
		    {				
			Attack			
			Berserk 20 2.0			
                               						
                    }						
		#move				
		    {				
			Loot d 5			
		    }				
   	}               					
}						
MI_SPIRITBAGNIS						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan 			
                    }						
		#battle				
		    {				
			Attack 10 cunning sam			
			Berserk 20 2.0			
			Keeprangeattack 8 			
                               						
                    }						
		#move				
		    {				
			Loot d 5			
		    }				
   	}               					
}						
MI_BEGUARDIAN						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan 			
                    }						
		#battle				
		    {				
			Attack			
			Berserk 20 2.0			
                               						
                    }						
		#move				
		    {				
			Loot d 5			
		    }				
   	}               					
}						
MI_BEHEMOTH						
{						
	m_nAttackFirstRange = 40;					
	AI					
	{					
		#Scan				
                    {						
			scan 			
                    }						
		#battle				
		    {				
			Attack 20 cunning sam			
                        Rangeattack 15						
			Berserk 20 2.0			
                               						
                    }						
		#move				
		    {				
			Loot d 5			
		    }				
   	}               					
}						
MI_PINATA						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan 			
                    }						
		#battle				
		    {				
			Attack			
			Berserk 20 2.0			
                               						
                    }						
		#move				
		    {				
			Loot d 5			
		    }				
   	}               					
}						
MI_BEHESTATUE01						
{						
	AI					
	{					
		#Scan				
                    {						
                    }						
		#battle				
		    {				
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_TGLUIA01						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 			
                        						
                        berserk 30 1.2						
			Evade 10            			
                    }						
		#move				
		    { 				
			loot d 5			
		    }				
   	}               					
}						
//17차 몬스터						
MI_RUGALHEAT01						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan 			
                    }						
		#battle				
		    {				
			Attack			
			Berserk 20 2.0			
                               						
                    }						
		#move				
		    {				
			Loot d 5			
		    }				
   	}               					
}						
MI_RUGALWIN						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan 			
                    }						
		#battle				
		    {				
			Attack			
			Berserk 20 2.0			
                               						
                    }						
		#move				
		    {				
			Loot d 5			
		    }				
   	}               					
}						
MI_RUGALKUM						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan 			
                    }						
		#battle				
		    {				
			Attack			
			Berserk 20 2.0			
                               						
                    }						
		#move				
		    {				
			Loot d 5			
		    }				
   	}               					
}						
MI_RUGALRIM						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan 			
                    }						
		#battle				
		    {				
			Attack 10 cunning sam			
			Berserk 20 2.0			
			Keeprangeattack 8 			
                               						
                    }						
		#move				
		    {				
			Loot d 5			
		    }				
   	}               					
}						
MI_RUGALKUMA						
{						
	AddSummonMonster( MI_RUGALWIND02 );					
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan 			
                    }						
		#battle				
		    {				
			Attack			
			Berserk 20 2.0			
                               						
                    }						
		#move				
		    {				
			Loot d 5			
		    }				
   	}               					
}						
MI_RUGALWIND02						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan 			
                    }						
		#battle				
		    {				
			Attack			
			Berserk 20 2.0			
                               						
                    }						
		#move				
		    {				
			Loot d 5			
		    }				
   	}               					
}						
MI_RUGALRIMA						
{						
	AddSummonMonster( MI_RUGALHEAT02 );					
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan 			
                    }						
		#battle				
		    {				
			Attack 10 cunning sam			
			Berserk 20 2.0			
			Keeprangeattack 8 			
                               						
                    }						
		#move				
		    {				
			Loot d 5			
		    }				
   	}               					
}						
MI_RUGALHEAT02						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan 			
                    }						
		#battle				
		    {				
			Attack			
			Berserk 20 2.0			
                               						
                    }						
		#move				
		    {				
			Loot d 5			
		    }				
   	}               					
}						
MI_BASILISK						
{						
    
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
			Berserk 20 2.0			
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
//17차 인던						
MI_KALGASKUMA01  //지하의 망령 쿠마						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan 			
                    }						
		#battle				
		    {				
			Attack			
			Berserk 20 2.0			
                               						
                    }						
		#move				
		    {				
			Loot d 5			
		    }				
   	}               					
}						
MI_KALGASRIMA01		//지하의 망령 리마				
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan 			
                    }						
		#battle				
		    {				
			Attack 10 cunning sam			
			Berserk 20 2.0			
			Keeprangeattack 8 			
                               						
                    }						
		#move				
		    {				
			Loot d 5			
		    }				
   	}               					
}						
MI_KALGASBABY01		//베이비 카르고				
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan 			
                    }						
		#battle				
		    {				
			Attack			
			Berserk 20 2.0			
                               						
                    }						
		#move				
		    {				
			Loot d 5			
		    }				
   	}               					
}						
MI_KALGASFLY01		//플라이 카르고				
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan 			
                    }						
		#battle				
		    {				
			Attack 10 cunning sam			
			Berserk 20 2.0			
			Keeprangeattack 8 			
                               						
                    }						
		#move				
		    {				
			Loot d 5			
		    }				
   	}               					
}						
MI_KALGASLESSER01		//레서 칼가스				
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan 			
                    }						
		#battle				
		    {				
			Attack 10 cunning sam			
			Berserk 20 2.0			
			Keeprangeattack 8 			
                               						
                    }						
		#move				
		    {				
			Loot d 5			
		    }				
   	}               					
}						
MI_KALGASAKIN01		//에킨				
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan 			
                    }						
		#battle				
		    {				
			Attack 10 cunning sam			
			Berserk 20 2.0			
			summon 30 1 MI_KALGASKUMA01			
			Keeprangeattack 8 			
                               						
                    }						
		#move				
		    {				
			Loot d 5			
		    }				
   	}               					
}						
MI_KALGASBALT01		//발트				
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan 			
                    }						
		#battle				
		    {				
			Attack			
			summon 10 1 MI_KALGASRIMA01			
			Berserk 20 2.0			
                               						
                    }						
		#move				
		    {				
			Loot d 5			
		    }				
   	}               					
}						
MI_KALGASBOSS		//칼가스				
{						
	
	m_nAttackFirstRange = 60;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 20 cunning sam			
                        Rangeattack 60						
                        KeepRangeAttack 60						
			Berserk 20 2.0			
			summon 10 3 MI_KALGASLESSER01			
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
MI_KALGASSTELE01			//에킨 석상			
{						
	AI					
	{					
		#Scan				
                    {						
                    }						
		#battle				
		    {				
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_KALGASSTELE02			//발트 석상			
{						
	AI					
	{					
		#Scan				
                    {						
                    }						
		#battle				
		    {				
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_KALGASEGG01			// 칼가스 알			
{						
	AI					
	{					
		#Scan				
                    {						
                    }						
		#battle				
		    {				
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_COLOBANG // 거대뱅						
{						
SetLevelDropPanalty_Off 						
	m_dwAttackMoveDelay = 0;					
	m_dwRunawayDelay = 1000;					
	SetRunAway( 10, MI_BANG1, 2 );					
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 			
                          						
                                    						
                    }						
		#move				
		    {				
			loot d 3			
		    }				
   	}               					
}						
MI_COLOWAGJAK // 거대 와그작						
{						
SetLevelDropPanalty_Off 						
	m_dwAttackMoveDelay = 0;					
	m_dwRunawayDelay = 1000;					
	SetRunAway( 10, 0, 1 );					
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 			
                        						
                        rangeattack 10            						
                    }						
		#move				
		    {				
			loot d 5			
		    }				
   	}               					
}						
MI_COLOREDMANTIS // 거대 레드맨티스						
{						
SetLevelDropPanalty_Off 						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 			
                                                 						
                        Berserk 10 1.2            						
                    }						
		#move				
		    {				
			loot d 5			
		    }				
   	}               					
}						
MI_COLOJACKTHEHAMMER // 거대 잭더해머						
{						
SetLevelDropPanalty_Off 						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 			
                          						
                                    						
                    }						
		#move				
		    {				
			loot d 3			
		    }				
   	}               					
}						
MI_COLOBERKRO // 거대 버크로						
{						
SetLevelDropPanalty_Off 						
	SetCallHelper( 100, MI_BUCROW2, 5, TRUE);					
	SetCallHelper( 100, MI_BUCROW3, 5, TRUE);					
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 			
                        Berserk 30 1.5						
			summon 10 1 MI_BUCROW3			
			recovery 10 50 0 m			
                                    						
                    }						
		#move				
		    {				
			Loot d 3			
		    }				
   	}               					
}						
MI_COLOHOWBOW // 거대 호우보우						
{						
SetLevelDropPanalty_Off 						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 			
                          						
                                    						
                    }						
		#move				
		    {				
			loot d 4			
		    }				
   	}               					
}						
MI_COLOROKEPELER // 거대 로케펠러						
{						
SetLevelDropPanalty_Off 						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 			
                        Berserk 30 1.5						
			summon 10 1 MI_ROCKEPELLER3			
			recovery 10 40 0 m             			
                    }						
		#move				
		    {				
			loot d 3			
		    }				
   	}               					
}						
MI_COLOSTIMEWORK // 거대 스팀워커						
{						
SetLevelDropPanalty_Off 						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 			
                        Berserk 30 1.5						
			summon 10 1 MI_STEAMWALKER3			
			recovery 10 40 0 m 			
                          						
                                    						
                    }						
		#move				
		    {				
			loot d 5			
		    }				
   	}               					
}						
MI_COLOGRREUNG // 거대 그르릉						
{						
SetLevelDropPanalty_Off 						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 			
                        Berserk 30 1.5						
			summon 10 1 MI_GRRR3			
			recovery 10 30 0 m            			
                    }						
		#move				
		    {				
			loot d 5			
		    {				
		    }				
   	}               					
}						
MI_COLOGRIMONG // 거대 그리몽						
{						
SetLevelDropPanalty_Off 						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 			
                        Berserk 30 1.5						
			summon 10 1 MI_GREEMONG3			
			recovery 10 40 0 m             			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_COLOLUIA // 거대 루이아						
{						
SetLevelDropPanalty_Off 						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 			
                        Berserk 30 1.5						
			recovery 10 50 0 m			
			summon 10 1 MI_LUIA3            			
                    }						
		#move				
		    {				
			loot d 3			
		    }				
   	}               					
}						
MI_COLOGONGRI // 거대 공그리						
{						
SetLevelDropPanalty_Off 						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 			
                        Berserk 30 1.6						
			summon 10 1 MI_GONGURY3			
			recovery 20 30 0 a           			
                    }						
		#move				
		    {				
			loot d 3			
		    }				
   	}               					
}						
MI_COLOKEREUN // 거대 케른						
{						
SetLevelDropPanalty_Off 						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 			
                        Recovery 10 50 0 m						
			Berserk 30 1.5			
			summon 10 1 MI_KERN3            			
                    }						
		#move				
		    {				
			loot d 5			
		    }				
   	}               					
}						
MI_COLOMATEONIKER // 메테오니커						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
		        berserk 20 2.0				
        						
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_COLOCYCLOPSX // 싸이클롭스 X						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
			Berserk 20 2.0			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_COLODIEOPNIS // 죽음의 정령 옵니스						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan 			
                    }						
		#battle				
		    {				
			Attack			
			Berserk 20 2.0			
                               						
                    }						
		#move				
		    {				
			Loot d 5			
		    }				
   	}               					
}						
MI_COLORACCOUN // 거대 라쿤						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan 			
                    }						
		#battle				
		    {				
			Attack cunning low			
                               						
                    }						
		#move				
		    {				
			Loot d 5			
		    }				
   	}               					
}						
MI_COLOMATEONIKER01 // 불의 메테오니커						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#battle				
		{				
			berserk 2 1.5			
		}				
	}					
}						
MI_COLOCLOCKWORK // 클락워크						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
			Berserk 30 2			
                        recovery 5 20 0 m            						
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_COLOBIGFOOT // 악령의 빅풋						
{						
SetLevelDropPanalty_Off 						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan 			
                    }						
		#battle				
		    {				
			Attack cunning low			
                               						
                    }						
		#move				
		    {				
			Loot d 5			
		    }				
   	}               					
}						
MI_COLOLUCIFER // 루시퍼						
{						
SetLevelDropPanalty_Off 						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
                        Rangeattack 10						
                        Berserk 10 1.5 						
			Summon	20	2	MI_IMP03
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_COLOLYCANOS // 죽음의 라이카노스						
{						
SetLevelDropPanalty_Off 						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
			Berserk 20 2.5			
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
MI_COLOSOULVAMPAIN // 영혼의 뱀파인						
{						
SetLevelDropPanalty_Off 						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
                        Rangeattack 10						
			Berserk 20 3.0			
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
MI_COLOSKELGENERAL // 스켈튼 제너럴						
{						
SetLevelDropPanalty_Off 						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 20 cunning sam			
                        Rangeattack 10						
			Berserk 20 2.0			
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
MI_COLOSKELDEVIL // 스켈튼 데빌						
{						
SetLevelDropPanalty_Off 						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 20 cunning sam			
                        Rangeattack 10						
			Berserk 20 2.0			
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
MI_COLOTUTTLEKING // 터틀킹						
{						
SetLevelDropPanalty_Off 						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan 			
                    }						
		#battle				
		    {				
			Attack cunning low			
                               						
                    }						
		#move				
		    {				
			Loot d 5			
		    }				
   	}               					
}						
MI_COLORYCANBARGA // 라이칸 바르가						
{						
SetLevelDropPanalty_Off 						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan 			
                    }						
		#battle				
		    {				
			Attack 20 cunning sam			
                        Rangeattack 10						
			Berserk 20 2.0			
                               						
                    }						
		#move				
		    {				
			Loot d 5			
		    }				
   	}               					
}						
MI_COLOANGRYBEHEMOTH // 분노한 베히모스						
{						
SetLevelDropPanalty_Off 						
	m_nAttackFirstRange = 40;					
	AI					
	{					
		#Scan				
                    {						
			scan 			
                    }						
		#battle				
		    {				
			Attack 20 cunning sam			
                        Rangeattack 15						
			Berserk 20 2.0			
                               						
                    }						
		#move				
		    {				
			Loot d 5			
		    }				
   	}               					
}						
MI_COLOBASILISK // 바실리스크						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 20 cunning sam			
                        Rangeattack 10						
			Berserk 20 2.0			
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
MI_COLOKALGAS		//칼가스				
{						
	m_nAttackFirstRange = 60;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 20 cunning sam			
                        Rangeattack 60						
                        KeepRangeAttack 60						
			Berserk 20 2.0			
			summon 10 3 MI_KALGASLESSER01			
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
MI_COLOBANG_1 // 거대뱅						
{						
	SetLevelDropPanalty_Off 					
	m_dwAttackMoveDelay = 0;					
	m_dwRunawayDelay = 1000;					
	SetRunAway( 10, MI_BANG1, 2 );					
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 			
                          						
                                    						
                    }						
		#move				
		    {				
			loot d 3			
		    }				
   	}               					
}						
MI_COLOWAGJAK_1 // 거대 와그작						
{						
	SetLevelDropPanalty_Off 					
	m_dwAttackMoveDelay = 0;					
	m_dwRunawayDelay = 1000;					
	SetRunAway( 10, 0, 1 );					
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 			
                        						
                        rangeattack 10            						
                    }						
		#move				
		    {				
			loot d 5			
		    }				
   	}               					
}						
MI_COLOREDMANTIS_1 // 거대 레드맨티스						
{						
	SetLevelDropPanalty_Off 					
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 			
                                                 						
                        Berserk 10 1.2            						
                    }						
		#move				
		    {				
			loot d 5			
		    }				
   	}               					
}						
MI_COLOJACKTHEHAMMER_1 // 거대 잭더해머						
{						
	SetLevelDropPanalty_Off 					
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 			
                          						
                                    						
                    }						
		#move				
		    {				
			loot d 3			
		    }				
   	}               					
}						
MI_COLOBERKRO_1 // 거대 버크로						
{						
	SetLevelDropPanalty_Off 					
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 			
                        Berserk 30 1.5						
			summon 10 1 MI_BUCROW3			
			recovery 10 50 0 m			
                                    						
                    }						
		#move				
		    {				
			Loot d 3			
		    }				
   	}               					
}						
MI_COLOHOWBOW_1 // 거대 호우보우						
{						
	SetLevelDropPanalty_Off 					
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 			
                          						
                                    						
                    }						
		#move				
		    {				
			loot d 4			
		    }				
   	}               					
}						
MI_COLOROKEPELER_1 // 거대 로케펠러						
{						
SetLevelDropPanalty_Off 						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 			
                        Berserk 30 1.5						
			summon 10 1 MI_ROCKEPELLER3			
			recovery 10 40 0 m             			
                    }						
		#move				
		    {				
			loot d 3			
		    }				
   	}               					
}						
MI_COLOSTIMEWORK_1 // 거대 스팀워커						
{						
	SetLevelDropPanalty_Off 					
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 			
                        Berserk 30 1.5						
			summon 10 1 MI_STEAMWALKER3			
			recovery 10 40 0 m 			
                          						
                                    						
                    }						
		#move				
		    {				
			loot d 5			
		    }				
   	}               					
}						
MI_COLOGRREUNG_1 // 거대 그르릉						
{						
SetLevelDropPanalty_Off 						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 			
                        Berserk 30 1.5						
			summon 10 1 MI_GRRR3			
			recovery 10 30 0 m            			
                    }						
		#move				
		    {				
			loot d 5			
		    {				
		    }				
   	}               					
}						
MI_COLOGRIMONG_1 // 거대 그리몽						
{						
	SetLevelDropPanalty_Off 					
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 			
                        Berserk 30 1.5						
			summon 10 1 MI_GREEMONG3			
			recovery 10 40 0 m             			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_COLOLUIA_1 // 거대 루이아						
{						
SetLevelDropPanalty_Off 						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 			
                        Berserk 30 1.5						
			recovery 10 50 0 m			
			summon 10 1 MI_LUIA3            			
                    }						
		#move				
		    {				
			loot d 3			
		    }				
   	}               					
}						
MI_COLOGONGRI_1 // 거대 공그리						
{						
SetLevelDropPanalty_Off 						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 			
                        Berserk 30 1.6						
			summon 10 1 MI_GONGURY3			
			recovery 20 30 0 a           			
                    }						
		#move				
		    {				
			loot d 3			
		    }				
   	}               					
}						
MI_COLOKEREUN_1 // 거대 케른						
{						
SetLevelDropPanalty_Off 						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 			
                        Recovery 10 50 0 m						
			Berserk 30 1.5			
			summon 10 1 MI_KERN3            			
                    }						
		#move				
		    {				
			loot d 5			
		    }				
   	}               					
}						
MI_COLOMATEONIKER_1 // 메테오니커						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
		        berserk 20 2.0				
        						
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_COLOCYCLOPSX_1 // 싸이클롭스 X						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
			Berserk 20 2.0			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_COLODIEOPNIS_1 // 죽음의 정령 옵니스						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan 			
                    }						
		#battle				
		    {				
			Attack			
			Berserk 20 2.0			
                               						
                    }						
		#move				
		    {				
			Loot d 5			
		    }				
   	}               					
}						
MI_COLORACCOUN_1 // 거대 라쿤						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan 			
                    }						
		#battle				
		    {				
			Attack cunning low			
                               						
                    }						
		#move				
		    {				
			Loot d 5			
		    }				
   	}               					
}						
MI_COLOMATEONIKER01_1 // 불의 메테오니커						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#battle				
		{				
			berserk 2 1.5			
		}				
	}					
}						
MI_COLOCLOCKWORK_1 // 클락워크						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
			Berserk 30 2			
                        recovery 5 20 0 m            						
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_COLOBIGFOOT_1 // 악령의 빅풋						
{						
SetLevelDropPanalty_Off 						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan 			
                    }						
		#battle				
		    {				
			Attack cunning low			
                               						
                    }						
		#move				
		    {				
			Loot d 5			
		    }				
   	}               					
}						
MI_COLOLUCIFER_1 // 루시퍼						
{						
SetLevelDropPanalty_Off 						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
                        Rangeattack 10						
                        Berserk 10 1.5 						
			Summon	20	2	MI_IMP03
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_COLOLYCANOS_1 // 죽음의 라이카노스						
{						
SetLevelDropPanalty_Off 						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
			Berserk 20 2.5			
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
MI_COLOSOULVAMPAIN_1 // 영혼의 뱀파인						
{						
SetLevelDropPanalty_Off 						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
                        Rangeattack 10						
			Berserk 20 3.0			
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
MI_COLOSKELGENERAL_1 // 스켈튼 제너럴						
{						
SetLevelDropPanalty_Off 						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 20 cunning sam			
                        Rangeattack 10						
			Berserk 20 2.0			
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
MI_COLOSKELDEVIL_1 // 스켈튼 데빌						
{						
SetLevelDropPanalty_Off 						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 20 cunning sam			
                        Rangeattack 10						
			Berserk 20 2.0			
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
MI_COLOTUTTLEKING_1 // 터틀킹						
{						
SetLevelDropPanalty_Off 						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan 			
                    }						
		#battle				
		    {				
			Attack cunning low			
                               						
                    }						
		#move				
		    {				
			Loot d 5			
		    }				
   	}               					
}						
MI_COLORYCANBARGA_1 // 라이칸 바르가						
{						
SetLevelDropPanalty_Off 						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan 			
                    }						
		#battle				
		    {				
			Attack 20 cunning sam			
                        Rangeattack 10						
			Berserk 20 2.0			
                               						
                    }						
		#move				
		    {				
			Loot d 5			
		    }				
   	}               					
}						
MI_COLOANGRYBEHEMOTH_1 // 분노한 베히모스						
{						
SetLevelDropPanalty_Off 						
	m_nAttackFirstRange = 40;					
	AI					
	{					
		#Scan				
                    {						
			scan 			
                    }						
		#battle				
		    {				
			Attack 20 cunning sam			
                        Rangeattack 15						
			Berserk 20 2.0			
                               						
                    }						
		#move				
		    {				
			Loot d 5			
		    }				
   	}               					
}						
MI_COLOBASILISK_1 // 바실리스크						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 20 cunning sam			
                        Rangeattack 10						
			Berserk 20 2.0			
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
MI_COLOKALGAS_1 // 약간 부족한 콜로세움 정예 날카로운 송곳니의 칼가스						
{						
	m_nAttackFirstRange = 60;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 20 cunning sam			
                        Rangeattack 60						
						KeepRangeAttack 60
			Berserk 20 2.0			
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
MI_EVEROCKEPELLER						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 60 			
                               						
                    }						
		#move				
		    {				
			loot d 3			
		    }				
   	}               					
}						
MI_EVEMOTHBEE						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack  			
                                    						
                    }						
		#move				
		    {				
			loot d 5			
		    }				
   	}               					
}						
MI_BURR						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
                        Rangeattack 10						
			Berserk 20 3.0			
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
MI_SAPHYRYAN						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
                        Rangeattack 10						
			Berserk 20 3.0			
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
MI_GRAYEARL						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
                        Rangeattack 10						
			Berserk 20 3.0			
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
MI_GPOTATO01						
{						
   						
SetLevelDropPanalty_Off 						
    
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
                        Rangeattack 10						
			Berserk 20 3.0			
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
MI_GPOTATO02						
{						
   	SetLevelDropPanalty_Off 					
    
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
                        Rangeattack 10						
			Berserk 20 3.0			
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
MI_EVEKALGASBABY						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
                        Rangeattack 10						
			Berserk 20 3.0			
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
MI_LOVETHIEF						
{				
SetLevelDropPanalty_Off

	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 			
                        						
                        berserk 30 1.2						
			Evade 10            			
                    }						
		#move				
		    { 				
			loot d 5			
		    }				
   	}               					
}						
MI_SHIPHARPINEES		//하피니스				
{						
	DropItem(II_GEN_JEW_NEC_HPNECKLACE,3000000000, 18, 1);
	DropItem(II_GEN_JEW_NEC_MPNECKLACE,3000000000, 18, 1);
	DropItem(II_GEN_JEW_NEC_FPNECKLACE,3000000000, 18, 1);
	DropItem(II_GEN_JEW_EAR_ATTEARRING,3000000000, 18, 1);
	DropItem(II_GEN_JEW_EAR_DEFEARRING,3000000000, 18, 1);	
	m_nAttackFirstRange = 60;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 20 cunning sam			
			Rangeattack 20	           						
                        						
			Berserk 20 2.0			
			summon 10 3 MI_SHIPREDHARPY			
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
MI_SHIPMOUGUS						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
                        Rangeattack 10						
			Berserk 20 3.0			
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
MI_SHIPWINGMOUGUS						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
                        Rangeattack 10						
			Berserk 20 3.0			
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
MI_SHIPMESPI						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
                        Rangeattack 10						
			Berserk 20 3.0			
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
MI_SHIPWINDMESPI						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
                        Rangeattack 10						
			Berserk 20 3.0			
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
MI_SHIPREDHARPY						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
                        Rangeattack 10						
			Berserk 20 3.0			
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
MI_SHIPBLUEHARPY						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
                        Rangeattack 10						
			Berserk 20 3.0			
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
MI_SHIPHIPPOGRIPH						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
                        Rangeattack 35						
			Berserk 20 3.0			
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
MI_DREAMFLAME01						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
                        Rangeattack 10						
			Berserk 20 3.0			
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
MI_DREAMRAPRA01						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
                        Rangeattack 10						
			Berserk 20 3.0			
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
MI_DREAMOLDRUT01						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
                        Rangeattack 10						
			Berserk 20 3.0			
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
MI_DREAMMINIMUSHU01						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
                        Rangeattack 10						
			Berserk 20 3.0			
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
MI_DREAMLADYBLUM01						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
                        Rangeattack 10						
			Berserk 20 3.0			
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
MI_DREAMNIGHTMIST01						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
                        Rangeattack 10						
			Berserk 20 3.0			
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
MI_DREAMQEEN01		//퀸 엔타니스				
{						
	DropItem(II_GEN_JEW_NEC_HPNECKLACE,3000000000, 12, 1);
	DropItem(II_GEN_JEW_NEC_MPNECKLACE,3000000000, 12, 1);
	DropItem(II_GEN_JEW_NEC_FPNECKLACE,3000000000, 12, 1);
	DropItem(II_GEN_JEW_EAR_ATTEARRING,3000000000, 12, 1);
	DropItem(II_GEN_JEW_EAR_DEFEARRING,3000000000, 12, 1); 				
	m_nAttackFirstRange = 60;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 20 cunning sam			
                        KeepRangeattack 50						
			Berserk 20 2.0			
			summon 10 3 MI_DREAMNIGHTMIST01			
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
MI_HERNSHARK01						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
                        Rangeattack 10						
			Berserk 20 3.0			
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
MI_HERNMERMAN01						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
                        Rangeattack 10						
			Berserk 20 3.0			
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
MI_HERNMERMAID01						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
                        Rangeattack 10						
			Berserk 20 3.0			
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
MI_HERNTURTLE01						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
                        Rangeattack 10						
			Berserk 20 3.0			
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
MI_HERNMERMAN02						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
                        Rangeattack 10						
			Berserk 20 3.0			
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
MI_HERNMERMAID02						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
                        Rangeattack 10						
			Berserk 20 3.0			
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
MI_HERNSIREN01						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
                        Rangeattack 10						
			Berserk 20 3.0			
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
MI_HERNKRAKEN01		//크라켄				
{						
	DropItem(II_GEN_JEW_NEC_HPNECKLACE,3000000000, 18, 1);
	DropItem(II_GEN_JEW_NEC_MPNECKLACE,3000000000, 18, 1);
	DropItem(II_GEN_JEW_NEC_FPNECKLACE,3000000000, 18, 1);
	DropItem(II_GEN_JEW_EAR_ATTEARRING,3000000000, 18, 1);
	DropItem(II_GEN_JEW_EAR_DEFEARRING,3000000000, 18, 1);		
	m_nAttackFirstRange = 60;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 20 cunning sam			
                        						
                        KeepRangeattack 50						
			Berserk 20 2.0			
			summon 10 3 MI_HERNMERMAID02			
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
MI_SHIPHARPINEES_1		//하피니스				
{						
	m_nAttackFirstRange = 60;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 20 cunning sam			
            Rangeattack 20	       						
                        						
			Berserk 20 2.0			
			summon 10 3 MI_SHIPREDHARPY_1			
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
MI_SHIPMOUGUS_1						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
                        Rangeattack 10						
			Berserk 20 3.0			
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
MI_SHIPWINGMOUGUS_1						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
                        Rangeattack 10						
			Berserk 20 3.0			
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
MI_SHIPMESPI_1						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
                        Rangeattack 10						
			Berserk 20 3.0			
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
MI_SHIPWINDMESPI_1						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
                        Rangeattack 10						
			Berserk 20 3.0			
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
MI_SHIPREDHARPY_1						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
                        Rangeattack 10						
			Berserk 20 3.0			
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
MI_SHIPBLUEHARPY_1						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
                        Rangeattack 10						
			Berserk 20 3.0			
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
MI_SHIPHIPPOGRIPH_1						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
                        Rangeattack 35
			Berserk 20 3.0			
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
MI_DREAMFLAME01_1						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
                        Rangeattack 10						
			Berserk 20 3.0			
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
MI_DREAMRAPRA01_1						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
                        Rangeattack 10						
			Berserk 20 3.0			
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
MI_DREAMOLDRUT01_1						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
                        Rangeattack 10						
			Berserk 20 3.0			
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
MI_DREAMMINIMUSHU01_1						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
                        Rangeattack 10						
			Berserk 20 3.0			
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
MI_DREAMLADYBLUM01_1						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
                        Rangeattack 10						
			Berserk 20 3.0			
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
MI_DREAMNIGHTMIST01_1						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
                        Rangeattack 10						
			Berserk 20 3.0			
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
MI_DREAMQEEN01_1		//퀸 엔타니스				
{						
	m_nAttackFirstRange = 60;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 20 cunning sam			
                      						
                        KeepRangeattack 50						
			Berserk 20 2.0			
			summon 10 3 MI_DREAMNIGHTMIST01_1			
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
MI_HERNSHARK01_1						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
                        Rangeattack 10						
			Berserk 20 3.0			
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
MI_HERNMERMAN01_1						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
                        Rangeattack 10						
			Berserk 20 3.0			
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
MI_HERNMERMAID01_1						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
                        Rangeattack 10						
			Berserk 20 3.0			
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
MI_HERNTURTLE01_1						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
                        Rangeattack 10						
			Berserk 20 3.0			
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
MI_HERNMERMAN02_1						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
                        Rangeattack 10						
			Berserk 20 3.0			
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
MI_HERNMERMAID02_1						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
                        Rangeattack 10						
			Berserk 20 3.0			
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
MI_HERNSIREN01_1						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
                        Rangeattack 10						
			Berserk 20 3.0			
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
MI_HERNKRAKEN01_1		//크라켄				
{						
	m_nAttackFirstRange = 60;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 20 cunning sam			
                       						
                        KeepRangeattack 50						
			Berserk 20 2.0			
			summon 10 3 MI_HERNMERMAID02_1			
                    }						
		#move				
		    {				
		    }				
   	}                					
}						
MI_EVENT_FWCMONSTER		//아레나 사신 몬스터				
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
			Berserk 20 2.0			
                    }						
		#move				
		    {				
		    }				
   	}                					
}			

MI_CHAMELEON_GREEN
{
	Maxitem = 2;	
	
	DropItem(II_SYS_SYS_SCR_TWOMGGREEN,3000000000, 0, 1);

	m_nAttackFirstRange = 10;

	AI
	{
		#Scan
                    {
			scan
                    }
		#battle
		    {
			Attack
                        Rangeattack 10
			Berserk 20 3.0
                    }
		#move
		    {
			
		    }	
   	}                
}

MI_CHAMELEON_BLUE
{
	Maxitem = 2;	
	
	DropItem(II_SYS_SYS_SCR_TWOMGBLUE,3000000000, 0, 1);

	m_nAttackFirstRange = 10;

	AI
	{
		#Scan
                    {
			scan
                    }
		#battle
		    {
			Attack
                        Rangeattack 10
			Berserk 20 3.0
                    }
		#move
		    {
			
		    }	
   	}                
}

MI_CHAMELEON_YELLOW
{
	Maxitem = 2;	
	
	DropItem(II_SYS_SYS_SCR_TWOMGYELLOW,3000000000, 0, 1);

	m_nAttackFirstRange = 10;

	AI
	{
		#Scan
                    {
			scan
                    }
		#battle
		    {
			Attack
                        Rangeattack 10
			Berserk 20 3.0
                    }
		#move
		    {
			
		    }	
   	}                
}

MI_CHAMELEON_RED
{
	Maxitem = 2;	
	
	DropItem(II_SYS_SYS_SCR_TWOMGRED,3000000000, 0, 1);

	m_nAttackFirstRange = 10;

	AI
	{
		#Scan
                    {
			scan
                    }
		#battle
		    {
			Attack
                        Rangeattack 10
			Berserk 20 3.0
                    }
		#move
		    {
			
		    }	
   	}                
}

MI_CRISTMASGIGLEBOX						
{		
	SetLevelDropPanalty_Off									
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
{						
			scan			
}						
		#battle				
		{				
			Attack			
}						
		#move				
		{				
		}				
	}					
}			


MI_TURTLEGUARDER						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan 			
                    }						
		#battle				
		    {				
			Attack cunning low			
                               						
                    }						
		#move				
		    {				
			Loot d 5			
		    }				
   	}               					
}					
	
MI_RABBITFIGHTER						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan 			
                    }						
		#battle				
		    {				
			Attack cunning low			
                               						
                    }						
		#move				
		    {				
			Loot d 5			
		    }				
   	}               					
}					
	
MI_RABBITSWORDER						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan 			
                    }						
		#battle				
		    {				
			Attack cunning low			
                               						
                    }						
		#move				
		    {				
			Loot d 5			
		    }				
   	}               					
}						

MI_RABBITSPEAR			
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan 			
                    }						
		#battle				
		    {				
			Attack cunning low			
                               						
                    }						
		#move				
		    {				
			Loot d 5			
		    }				
   	}               					
}				
		
MI_RABBITASSASSIN						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan 			
                    }						
		#battle				
		    {				
			Attack cunning low			
                               						
                    }						
		#move				
		    {				
			Loot d 5			
		    }				
   	}               					
}		
				
MI_RABBITAXE						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan 			
                    }						
		#battle				
		    {				
			Attack cunning low			
                               						
                    }						
		#move				
		    {				
			Loot d 5			
		    }				
   	}               					
}			
			
MI_IBLESSPUPPET						
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan 			
                    }						
		#battle				
		    {				
			Attack cunning low	
			 Rangeattack 20		
                               						
                    }						
		#move				
		    {				
			Loot d 5			
		    }				
   	}               					
}

MI_GRAYEARL01						
{		
	SetLevelDropPanalty_Off									
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
{						
			scan			
}						
		#battle				
		{				
			Attack			
}						
		#move				
		{				
		}				
	}					
}



				
//이 이하는 사용하지 않음. 추가 작업은 바로 위에서 부터..						
MI_PUKEPUKE5						
{						
	m_nAttackFirstRange = 8;					
}						
MI_BANG5						
{						
	m_nAttackFirstRange = 8;					
}						
MI_ROCKMUSCLE5						
{						
	m_nAttackFirstRange = 8;					
}						
MI_MRPUMPKIN5						
{						
	m_nAttackFirstRange = 8;					
}						
MI_MAFL_PATROL						
{						
	m_nAttackFirstRange = 10;					
	SetCallHelper( 0, 0, 0, FALSE );					
	AI					
	{					
		#Scan				
                    {						
			scan range 10 chao 100			
                    }						
		#battle				
		    {				
			Attack			
                        Rangeattack 10						
			Berserk 20 3.0			
                    }						
		#move				
		    {				
		    }				
   	}   					
}
						
MI_DU_METEONYKER5						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#battle				
		{
			Attack
			summon 99 5 MI_CYCLOPSX2					
			berserk 2 1.5			
		}				
	}					
}

MI_CYCLOPSX2						
{						

	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
			Berserk 20 2.0			
                    }						
		#move				
		    {				
		    }				
   	}               					
}

MI_GLAPHAN5					
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack
			rangeattack 10 			
                        summon 50 1 MI_GLAPHAN6					
			Berserk 30 1.8			
			recovery 10 40 0 m            			
                    }						
		#move				
		    {				
			loot d 5			
		    }				
   	}               					
}

MI_GLAPHAN6						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack  			
                                    						
                    }						
		#move				
		    {				
		    }				
   	}               					
}

MI_MUSHMOOT2					
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 			
                        Berserk 30 1.5						
			summon 10 1 MI_SMALL_MUSHPOIE2			
			recovery 10 30 0 m            			
                    }						
		#move				
		    {				
		    }				
   	}               					
}

MI_SMALL_MUSHPOIE2
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
                    }						
		#move				
		    {				
		    }				
   	}               					
}

MI_KRRR2						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 			
                        Berserk 30 1.5						
			summon 10 1 MI_GRRR3			
			recovery 10 30 0 m            			
                    }						
		#move				
		    {				
		    }				
   	}               					
}

MI_WAGSAAC5						
{						
	m_dwAttackMoveDelay = 0;					
	m_dwRunawayDelay = 1000;					
	SetRunAway( 10, 0, 1 );					
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
		{						
			scan			
		}						
		#battle				
		{				
			Attack			
			rangeattack 10						
		}						
		#move				
		{				
			loot d 5			
		}				
	}					
}

	
MI_AIBATT6						
{						
	m_dwAttackMoveDelay = 0;					
	m_dwRunawayDelay = 1000;					
	SetRunAway( 10, 0, 5 );					
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
{						
			scan			
}						
		#battle				
		{				
			Attack cunning low			
			Recovery 10 50 0 m			
			evade 20			
}						
		#move				
		{				
			Loot d 5			
		}				
	}					
}


MI_ANT1						
{	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
			Berserk 20 2.0			
                    }						
		#move				
		    {				
		    }				
   	} 							
}		

MI_ANT2						
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
                    }						
		#move				
		    {				
		    }				
   	}  					
}		

MI_ANT3						
{						
	m_dwAttackMoveDelay = 0;					
	m_dwRunawayDelay = 1000;					
	SetRunAway( 10, 0, 1 );					
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
		{						
			scan			
		}						
		#battle				
		{				
			Attack			
			rangeattack 10						
		}						
		#move				
		{				
			loot d 5			
		}				
	}					
}	

MI_BEETLE1					
{						
	SetLevelDropPanalty_Off
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
			Berserk 20 2.0			
                    }						
		#move				
		    {				
		    }				
   	}  					
}	

MI_BEETLE2					
{										
	SetLevelDropPanalty_Off
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
		{						
			scan			
		}						
		#battle				
		{				
			Attack	
			Berserk 30 1.5
			rangeattack 10		
			summon 99 2 MI_BEETLE1			
		}						
		#move				
		{						
		}				
	}						
}	

MI_DSPIDER1				
{						
	SetLevelDropPanalty_Off
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack
			rangeattack 10 						
			Berserk 30 1.8			
			recovery 10 40 0 m            			
                    }						
		#move				
		    {				
			loot d 5			
		    }				
   	}   					
}	

MI_DSPIDER2			
{						
	SetLevelDropPanalty_Off
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
{						
			scan			
}						
		#battle				
		{				
			Attack			
}						
		#move				
		{				
			Loot d 3			
		}				
	}					
}	

MI_DSPIDER3			
{						
	SetLevelDropPanalty_Off
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
{						
			scan			
}						
		#battle				
		{				
			Attack			
}						
		#move				
		{				
			Loot d 3			
		}				
	}					
}	

MI_GIANTTREEK1		
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
                        Rangeattack 10						
                        Berserk 10 1.5 						
                    }						
		#move				
		    {				
		    }				
   	}  					
}	

MI_GIANTTREEQ1		
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack
			rangeattack 10 							
			Berserk 30 1.8			
			recovery 10 40 0 m            			
                    }						
		#move				
		    {				
			loot d 5			
		    }				
   	}					
}

MI_GRLARVA1		
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
			Berserk 20 2.0			
                    }						
		#move				
		    {				
		    }				
   	}  					
}	

MI_GRLARVA2		
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
{						
			scan			
}						
		#battle				
		{				
			Attack			
}						
		#move				
		{				
			Loot d 3			
		}				
	}					
}	

MI_GRLARVA3		
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
{						
			scan			
}						
		#battle				
		{				
			Attack			
}						
		#move				
		{				
			Loot d 3			
		}				
	}					
}	

MI_WOODSPIRIT1	
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
			Berserk 20 2.0			
                    }						
		#move				
		    {				
		    }				
   	}   					
}	

MI_WOODSPIRIT2	
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
			Berserk 20 2.0			
                    }						
		#move				
		    {				
		    }				
   	}   					
}	

MI_WOODSPIRIT3	
{						
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
{						
			scan			
}						
		#battle				
		{				
			Attack			
}						
		#move				
		{				
			Loot d 3			
		}				
	}					
}	

MI_WOODSPIRIT4
{						
	m_dwAttackMoveDelay = 0;					
	m_dwRunawayDelay = 1000;					
	SetRunAway( 10, 0, 1 );					
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
		{						
			scan			
		}						
		#battle				
		{				
			Attack			
			rangeattack 10						
		}						
		#move				
		{				
			loot d 5			
		}				
	}					
}	

MI_WOODSPIRITBOSS1
{						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 			
                        Berserk 30 1.5								
			recovery 10 30 0 m            			
                    }						
		#move				
		    {				
		    }				
   	}   			 					
}	

MI_MOCOMOCHI_BORA					
{
	SetLevelDropPanalty_Off						
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
                        Rangeattack 10						
			Berserk 20 3.0			
                    }						
		#move				
		    {				
		    }				
   	}                					
}

MI_MOCOMOCHI_BORA_JP					
{
	SetLevelDropPanalty_Off
	SetAddDropCount_Off
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
                        Rangeattack 10						
			Berserk 20 3.0			
                    }						
		#move				
		    {				
		    }				
   	}                					
}

MI_RANGDAJP						
{		
	SetLevelDropPanalty_Off
	SetAddDropCount_Off
	
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 			
                        Berserk 30 1.5						
			recovery 10 50 0 m  			
                    }						
		#move				
		    {				
		    }				
   	}               					
}

MI_MZOMBIE_JP			
{			
	SetLevelDropPanalty_Off		
	SetAddDropCount_Off		
			
	m_nAttackFirstRange = 8;		
	AI		
	{		
		#Scan	
		{	
			scan
		}	
		#battle	
		{	
			Attack
			berserk 30 1.2
			Evade 10
		}	
		#move	
		{	
			loot d 5
		}	
	}		
}			
MI_FZOMBIE_JP			
{			
	SetLevelDropPanalty_Off		
	SetAddDropCount_Off		
			
	m_nAttackFirstRange = 8;		
	AI		
	{		
		#Scan	
		{	
			scan
		}	
		#battle	
		{	
			Attack
			berserk 30 1.2
			Evade 10
		}	
		#move	
		{	
			loot d 5
		}	
	}		
}			
MI_NMZOMBIE_JP			
{			
	SetLevelDropPanalty_Off		
	SetAddDropCount_Off		
			
	m_nAttackFirstRange = 8;		
	AI		
	{		
		#Scan	
		{	
			scan
		}	
		#battle	
		{	
			Attack
			berserk 30 1.2
			Evade 10
		}	
		#move	
		{	
			loot d 5
		}	
	}		
}			
MI_NFZOMBIE_JP			
{			
	SetLevelDropPanalty_Off		
	SetAddDropCount_Off		
			
	m_nAttackFirstRange = 8;		
	AI		
	{		
		#Scan	
		{	
			scan
		}	
		#battle	
		{	
			Attack
			berserk 30 1.2
			Evade 10
		}	
		#move	
		{	
			loot d 5
		}	
	}		
}					

MI_MOCOMOCHI_TW					
{
	SetLevelDropPanalty_Off
	SetAddDropCount_Off
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
                        Rangeattack 10						
			Berserk 20 3.0			
                    }						
		#move				
		    {				
		    }				
   	}                					
}

MI_MOCOMOCHI_1ST					
{
	SetLevelDropPanalty_Off
	SetAddDropCount_Off
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
                        Rangeattack 10						
			Berserk 20 3.0			
                    }						
		#move				
		    {				
		    }				
   	}                					
}

MI_MOCOMOCHI_THANKS					
{
	SetLevelDropPanalty_Off
	SetAddDropCount_Off
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
                        Rangeattack 10						
			Berserk 20 3.0			
                    }						
		#move				
		    {				
		    }				
   	}                					
}

MI_MZOMBIE_TW			
{			
	SetLevelDropPanalty_Off		
	SetAddDropCount_Off		
			
	m_nAttackFirstRange = 8;		
	AI		
	{		
		#Scan	
		{	
			scan
		}	
		#battle	
		{	
			Attack
			berserk 30 1.2
			Evade 10
		}	
		#move	
		{	
			loot d 5
		}	
	}		
}			
MI_FZOMBIE_TW			
{			
	SetLevelDropPanalty_Off		
	SetAddDropCount_Off		
			
	m_nAttackFirstRange = 8;		
	AI		
	{		
		#Scan	
		{	
			scan
		}	
		#battle	
		{	
			Attack
			berserk 30 1.2
			Evade 10
		}	
		#move	
		{	
			loot d 5
		}	
	}		
}

MI_BURR_EXP						
{						
	SetLevelDropPanalty_Off		
	SetAddDropCount_Off	
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
                        Rangeattack 10						
			Berserk 20 3.0			
                    }						
		#move				
		    {				
		    }				
   	}                					
}

MI_BURR_MUSIC						
{						
	SetLevelDropPanalty_Off		
	SetAddDropCount_Off	
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
                        Rangeattack 10						
			Berserk 20 3.0			
                    }						
		#move				
		    {				
		    }				
   	}                					
}

MI_CLOCKWORKBUTLER_MUSIC					
{				
	SetLevelDropPanalty_Off		
	SetAddDropCount_Off			
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
                        Rangeattack 10						
			Berserk 20 3.0			
                    }						
		#move				
		    {				
		    }				
   	}                					
}

MI_MOCOMOCHI_MUSIC				
{
	SetLevelDropPanalty_Off
	SetAddDropCount_Off
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
                        Rangeattack 10						
			Berserk 20 3.0			
                    }						
		#move				
		    {				
		    }				
   	}                					
}

MI_PINATA_THANKS						
{						
	SetLevelDropPanalty_Off
	SetAddDropCount_Off
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan 			
                    }						
		#battle				
		    {				
			Attack			
			Berserk 20 2.0			
                               						
                    }						
		#move				
		    {				
			Loot d 5			
		    }				
   	}               					
}	

MI_CHAMELEON_GREEN_CHIRST
{
	SetLevelDropPanalty_Off
	SetAddDropCount_Off
	m_nAttackFirstRange = 10;

	AI
	{
		#Scan
                    {
			scan
                    }
		#battle
		    {
			Attack
                        Rangeattack 10
			Berserk 20 3.0
                    }
		#move
		    {
			
		    }	
   	}                
}

MI_CHAMELEON_RED_CHIRST
{
	SetLevelDropPanalty_Off
	SetAddDropCount_Off
	m_nAttackFirstRange = 10;

	AI
	{
		#Scan
                    {
			scan
                    }
		#battle
		    {
			Attack
                        Rangeattack 10
			Berserk 20 3.0
                    }
		#move
		    {
			
		    }	
   	}                
}

MI_SAPHYRYAN_TW						
{						
	SetLevelDropPanalty_Off
	SetAddDropCount_Off
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
                        Rangeattack 10						
			Berserk 20 3.0			
                    }						
		#move				
		    {				
		    }				
   	}                					
}

MI_TAIGA01_2022						
{						
	SetLevelDropPanalty_Off
	SetAddDropCount_Off
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 			
                    }						
		#move				
		    {				
		    }				
   	}               					
}						
MI_TAIGA02_2022						
{						
	SetLevelDropPanalty_Off
	SetAddDropCount_Off
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 			
                    }						
		#move				
		    {				
		    }				
   	}               					
}

// 22차 몬스터
MI_REDMANTIS1_FC						
{						
	SetLevelDropPanalty_Off
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
{						
			scan			
}						
		#battle				
		{				
			Attack 20			
Berserk 30 1.4						
}						
		#move				
		{				
		}				
	}					
}

MI_REDMANTIS4_FC						
{						
	SetLevelDropPanalty_Off
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
{						
			scan			
}						
		#battle				
		{				
			Attack			
Berserk 10 1.2						
}						
		#move				
		{				
			loot d 5			
		}				
	}					
}

MI_PEAKYTURTLE3_FC						
{						
	
	SetLevelDropPanalty_Off
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
{						
			scan			
}						
		#battle				
		{				
			Attack cunning low			
}						
		#move				
		{				
		}				
	}					
}	

MI_HADESEOR_FC						
{						
	SetLevelDropPanalty_Off
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
		         berserk 15 2             				
                    }						
		#move				
		    {				
		    }				
   	}   					
}	

MI_ANT2_FC						
{						
	SetLevelDropPanalty_Off
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
                    }						
		#move				
		    {				
		    }				
   	}  					
}		

MI_ANT3_FC						
{						
	SetLevelDropPanalty_Off
	m_dwAttackMoveDelay = 0;					
	m_dwRunawayDelay = 1000;					
	SetRunAway( 10, 0, 1 );					
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
		{						
			scan			
		}						
		#battle				
		{				
			Attack			
			rangeattack 10						
		}						
		#move				
		{				
			loot d 5			
		}				
	}					
}	

MI_DU_DKTRILLIPY1_FC						
{						
	SetLevelDropPanalty_Off
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
		        berserk 15 1.2     				
                    }						
		#move				
		    {				
		    }				
   	}               					
}

MI_DU_DKTRILLIPY5_FC						
{						
	SetLevelDropPanalty_Off
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
		        berserk 15 1.2             				
                    }						
		#move				
		    {				
		    }				
   	}               					
}

MI_GRLARVA1_FC		
{						
	SetLevelDropPanalty_Off
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
			Berserk 20 2.0			
                    }						
		#move				
		    {				
		    }				
   	}  					
}	

MI_SHAKALPION_FC						
{						
	SetLevelDropPanalty_Off
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
		         berserk 20 2             				
                    }						
		#move				
		    {				
		    }				
   	}   					
}	

MI_Berken02_FC						
{						
	SetLevelDropPanalty_Off
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
			Berserk 20 2.0			
                    }						
		#move				
		    {				
		    }				
   	}                					
}	

MI_Toadrin01_FC						
{						
	SetLevelDropPanalty_Off
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
			Berserk 20 2.0			
                    }						
		#move				
		    {				
		    }				
   	}                					
}

MI_BASQUE2_FC						
{						
	SetLevelDropPanalty_Off
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
{						
			scan			
}						
		#battle				
		{				
			Berserk 20 1.3			
keeprangeattack 7						
}						
		#move				
		{				
		}				
	}					
}

MI_SPAINDRAGON01_TW						
{						
	m_nAttackFirstRange = 10;
	SetLevelDropPanalty_Off
	SetAddDropCount_Off					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
                        Rangeattack 10						
			Berserk 20 3.0			
                    }						
		#move				
		    {				
		    }				
   	}                					
}

MI_SPAINDRAGON02_TW						
{						
	m_nAttackFirstRange = 10;
	SetLevelDropPanalty_Off
	SetAddDropCount_Off					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
                        Rangeattack 10						
			Berserk 20 3.0			
                    }						
		#move				
		    {				
		    }				
   	}                					
}



MI_BEHESTATUE01_PH_01						
{						
	AI					
	{					
		#Scan				
                    {						
                    }						
		#battle				
		    {	
			recovery 10 10 0 m  						
                    }						
		#move				
		    {				
		    }				
   	}               					
}						

MI_BEHESTATUE01_PH_02						
{						
	AI					
	{					
		#Scan				
                    {						
                    }						
		#battle				
		    {	
			recovery 10 10 0 m  						
                    }						
		#move				
		    {				
		    }				
   	}               					
}						

MI_BEHESTATUE01_PH_03						
{						
	AI					
	{					
		#Scan				
                    {						
                    }						
		#battle				
		    {	
			recovery 10 10 0 m  						
                    }						
		#move				
		    {				
		    }				
   	}               					
}						

MI_KALGASSTELE02_PH_01			
{						
	AI					
	{					
		#Scan				
                    {						
                    }						
		#battle				
		    {	
			recovery 10 10 0 m  			
                    }						
		#move				
		    {				
		    }				
   	}               					
}	

MI_KALGASSTELE02_PH_02			
{						
	AI					
	{					
		#Scan				
                    {						
                    }						
		#battle				
		    {	
			recovery 10 10 0 m  			
                    }						
		#move				
		    {				
		    }				
   	}               					
}	

MI_KALGASSTELE02_PH_03			
{						
	AI					
	{					
		#Scan				
                    {						
                    }						
		#battle				
		    {	
			recovery 10 10 0 m  			
                    }						
		#move				
		    {				
		    }				
   	}               					
}	

MI_PET_RABBIT02_2022
{
	SetLevelDropPanalty_Off
	SetAddDropCount_Off
	m_nAttackFirstRange = 10;

	AI
	{
		#Scan
                    {
			scan
                    }
		#battle
		    {
			Attack
                        Rangeattack 10
			Berserk 20 3.0
                    }
		#move
		    {
			
		    }	
   	}                
}

MI_LUIA2_EASTER						
{						
	m_nAttackFirstRange = 10;	
	SetLevelDropPanalty_Off
	SetAddDropCount_Off				
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack  			
                                    						
                    }						
		#move				
		    {				
		    }				
   	}               					
}

MI_PETSAKURASPIRIT_EU						
{						
	SetLevelDropPanalty_Off
	SetAddDropCount_Off
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 			
                    }						
		#move				
		    {				
		    }				
   	}               					
}	

MI_DEMIAN_PH						
{						
	SetLevelDropPanalty_Off
	SetAddDropCount_Off
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
{						
			scan			
}						
		#battle				
		    {				
			Attack 			
                    }						
		#move				
		    {				
		    }				
   	}               					
}	

MI_MEREL_PH						
{						
	SetLevelDropPanalty_Off
	SetAddDropCount_Off
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 			
                    }						
		#move				
		    {				
		    }				
   	}               					
}	

MI_ROCKEPELLER_PH						
{						
	SetLevelDropPanalty_Off
	SetAddDropCount_Off
	m_nAttackFirstRange = 10;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack 60 			
                        evade 20            						
                    }						
		#move				
		    {				
			loot d 3			
		    }				
   	}               					
}	


MI_CAITSITH02_TW						
{						
	SetLevelDropPanalty_Off
	SetAddDropCount_Off
	m_nAttackFirstRange = 8;					
	AI					
	{					
		#Scan				
                    {						
			scan			
                    }						
		#battle				
		    {				
			Attack			
                    }						
		#move				
		    {				
		    }				
   	}               					
}	