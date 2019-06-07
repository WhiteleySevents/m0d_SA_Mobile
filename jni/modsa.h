#pragma once

class CModSAWindow
{
public:
	CModSAWindow();
	~CModSAWindow();

	void Render();
	void Clear();
	void Show(bool bShow);
	void ToggleCheat(int cheatid);
	void ToggleRPC(int rpcid);
	void ToggleTeleport(int posid);
	void StoreMarkerXYZ(float x, float y, float z);
	void ShowFlyMenu(bool bShow);
	void ToggleNoKickTeleport(float x, float y, float z);

	void SetFlySets(float distance);

	//void ShowBotSets(int botid, bool enabled);
	//void StartBot(int botid);
	float GetFlySets();

public:
	bool		m_bIsActive;
	int			m_bMenuStep;
	int 		m_bFlyHack;
	int			m_bGodMode;
	int			m_bWallHack;
	int			m_bCols;

	int			m_bVP;
	int			m_bVP2;
	int			m_bVV;
	int			m_bCO;
	int			m_bPH;
	int			m_bRPFV;
	int			m_bSD;
	int			m_bAPA;
	int			m_bRPW;
	int			m_bNF;
	int			m_bCP;
	int			m_bGCW;

	int			protect;
	int			m_bKrutilka;
	int			lockinp;
	int			lock_weather;
	int 		lock_time;

	int			zbot_reps;
	int			m_bot;

	float i_x;
	float i_y;
	float i_z;

	float		marker_X;
	float		marker_Y;
	float		marker_Z;

	float fDist;
};