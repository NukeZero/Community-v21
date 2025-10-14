#pragma once

class CMover;

class AutoSynchronizer
{
public:
	AutoSynchronizer( CMover* pMover );
	~AutoSynchronizer();
	
	void	Initialize();
	void	Serialize( CAr & ar );
	
	D3DXVECTOR3 GetSyncPos()	{	return m_vSyncPos;	}
	float		GetSyncAngle()	{	return m_fSyncAngle;	}
	float		GetSyncAngleX()	{	return m_fSyncAngleX;	}

	void	CollisionState( BOOL bCollision );

private:
	void	Synchronize();
	void	SendSynchronize();
#ifdef __WORLDSERVER
	enum	Check_Result	{	valid, replace, invalid	};
	Check_Result	CheckValidSynchronize();
#endif // __WORLDSERVER

#ifdef __CLIENT
public:
	void	UpdateGhost();
private:
	void	UpdateGhostPos();
	void	UpdateGhostAngle();
#endif // __CLIENT
	
private:
	const D3DXVECTOR3 null_vec3;

	CMover* m_pMover;
	D3DXVECTOR3 m_vSyncPos;
	float	m_fSyncAngle, m_fSyncAngleX;
	
	// 아래 멤버는 일회성
	D3DXVECTOR3 vSyncPos, vDelta;
	float	fAngle, fAngleX;
	bool	bEmptyDest;
	DWORD	dwMotion, dwMotionOption;
	int		nMotionEx, nLoop;
	DWORD	dwState, dwStateFlag;
	DWORD	m_collisionSynchronizeTime;
};