#include "../main.h"
#include "game.h"
#include "net/netgame.h"
#include "net/localplayer.h"
#include "util/armhook.h"
#include "chatwindow.h"
#include "../modsa.h"

extern CGame* pGame;
extern CNetGame *pNetGame;
extern CLocalPlayer *pLocalPlayer;
extern CChatWindow *pChatWindow;
extern CModSAWindow *pModSAWindow;

CPlayerPed::CPlayerPed()
{
	m_dwGTAId = 1;
	m_pPed = (PED_TYPE*)GamePool_FindPlayerPed();
	m_pEntity = (ENTITY_TYPE*)GamePool_FindPlayerPed();

	m_bytePlayerNumber = 0;
	SetPlayerPedPtrRecord(m_bytePlayerNumber,(uintptr_t)m_pPed);
	ScriptCommand(&set_actor_weapon_droppable, m_dwGTAId, 1);
	ScriptCommand(&set_actor_can_be_decapitated, m_dwGTAId, 0);

	m_dwArrow = 0;
}

CPlayerPed::CPlayerPed(uint8_t bytePlayerNumber, int iSkin, float fX, float fY, float fZ, float fRotation)
{
	uint32_t dwPlayerActorID = 0;
	int iPlayerNum = bytePlayerNumber;

	m_pPed = nullptr;
	m_dwGTAId = 0;

	ScriptCommand(&create_player, &iPlayerNum, fX, fY, fZ, &dwPlayerActorID);
	ScriptCommand(&create_actor_from_player, &iPlayerNum, &dwPlayerActorID);

	m_dwGTAId = dwPlayerActorID;
	m_pPed = GamePool_Ped_GetAt(m_dwGTAId);
	m_pEntity = (ENTITY_TYPE*)GamePool_Ped_GetAt(m_dwGTAId);

	m_bytePlayerNumber = bytePlayerNumber;
	SetPlayerPedPtrRecord(m_bytePlayerNumber, (uintptr_t)m_pPed);
	ScriptCommand(&set_actor_weapon_droppable, m_dwGTAId, 1);
	ScriptCommand(&set_actor_immunities, m_dwGTAId, 0, 0, 1, 0, 0);
	ScriptCommand(&set_actor_can_be_decapitated, m_dwGTAId, 0);

	if(pNetGame)
		SetMoney(pNetGame->m_iDeathDropMoney);

	SetModelIndex(iSkin);
	ForceTargetRotation(fRotation);

	MATRIX4X4 mat;
	GetMatrix(&mat);
	mat.pos.X = fX;
	mat.pos.Y = fY;
	mat.pos.Z = fZ + 0.15f;
	SetMatrix(mat);

	memset(&RemotePlayerKeys[m_bytePlayerNumber], 0, sizeof(PAD_KEYS));
}

CPlayerPed::~CPlayerPed()
{
	Destroy();
}

void CPlayerPed::Destroy()
{
	memset(&RemotePlayerKeys[m_bytePlayerNumber], 0, sizeof(PAD_KEYS));
	SetPlayerPedPtrRecord(m_bytePlayerNumber, 0);

	if(!m_pPed || !GamePool_Ped_GetAt(m_dwGTAId) || m_pPed->entity.vtable == 0x5C7358)
	{
		Log("CPlayerPed::Destroy: invalid pointer/vtable");
		m_pPed = nullptr;
		m_pEntity = nullptr;
		m_dwGTAId = 0;
		return;
	}

	/*
		if(m_dwParachute) ... (äîïèëèòü)
	*/

	Log("Removing from vehicle..");
	if(IN_VEHICLE(m_pPed))
		RemoveFromVehicleAndPutAt(100.0f, 100.0f, 10.0f);

	Log("Setting flag state..");
	uintptr_t dwPedPtr = (uintptr_t)m_pPed;
	*(uint32_t*)(*(uintptr_t*)(dwPedPtr + 1088) + 76) = 0;
	// CPlayerPed::Destructor
	Log("Calling destructor..");
	(( void (*)(PED_TYPE*))(*(void**)(m_pPed->entity.vtable+0x4)))(m_pPed);

	m_pPed = nullptr;
	m_pEntity = nullptr;
}

// 0.3.7
bool CPlayerPed::IsInVehicle()
{
	if(!m_pPed) return false;

	if(IN_VEHICLE(m_pPed))
		return true;

	return false;
}

// 0.3.7
bool CPlayerPed::IsAPassenger()
{
	if(m_pPed->pVehicle && IN_VEHICLE(m_pPed))
	{
		VEHICLE_TYPE *pVehicle = (VEHICLE_TYPE *)m_pPed->pVehicle;

		if(	pVehicle->pDriver != m_pPed ||
			pVehicle->entity.nModelIndex == TRAIN_PASSENGER ||
			pVehicle->entity.nModelIndex == TRAIN_FREIGHT )
			return true;
	}

	return false;
}

// 0.3.7
VEHICLE_TYPE* CPlayerPed::GetGtaVehicle()
{
	return (VEHICLE_TYPE*)m_pPed->pVehicle;
}

// 0.3.7
void CPlayerPed::RemoveFromVehicleAndPutAt(float fX, float fY, float fZ)
{
	if(!GamePool_Ped_GetAt(m_dwGTAId)) return;
	if(m_pPed && IN_VEHICLE(m_pPed))
		ScriptCommand(&remove_actor_from_car_and_put_at, m_dwGTAId, fX, fY, fZ);
}

// 0.3.7
void CPlayerPed::SetInitialState()
{
	(( void (*)(PED_TYPE*))(g_libGTASA+0x458D1C+1))(m_pPed);
}

// 0.3.7
void CPlayerPed::SetHealth(float fHealth)
{
	if(!m_pPed) return;
	m_pPed->fHealth = fHealth;
}

// 0.3.7
float CPlayerPed::GetHealth()
{
	if(!m_pPed) return 0.0f;
	return m_pPed->fHealth;
}

// 0.3.7
void CPlayerPed::SetArmour(float fArmour)
{
	if(!m_pPed) return;
	m_pPed->fArmour = fArmour;
}

float CPlayerPed::GetArmour()
{
	if(!m_pPed) return 0.0f;
	return m_pPed->fArmour;
}

void CPlayerPed::SetInterior(uint8_t byteID)
{
	if(!m_pPed) return;

	ScriptCommand(&select_interior, byteID);
	ScriptCommand(&link_actor_to_interior, m_dwGTAId, byteID);

	MATRIX4X4 mat;
	GetMatrix(&mat);
	ScriptCommand(&refresh_streaming_at, mat.pos.X, mat.pos.Y);
}

void CPlayerPed::PutDirectlyInVehicle(int iVehicleID, int iSeat)
{
	if(!m_pPed) return;
	if(!GamePool_Vehicle_GetAt(iVehicleID)) return;
	if(!GamePool_Ped_GetAt(m_dwGTAId)) return;

	
	if(GetCurrentWeapon() == 46) {
		SetArmedWeapon(0);
	}

	VEHICLE_TYPE *pVehicle = GamePool_Vehicle_GetAt(iVehicleID);

	if(pVehicle->fHealth == 0.0f) return;
	// check is cplaceable
	if (pVehicle->entity.vtable == g_libGTASA+0x5C7358) return;
	// check seatid (äîïèëèòü)

	if(iSeat == 0)
	{
		if(pVehicle->pDriver && IN_VEHICLE(pVehicle->pDriver)) return;
		ScriptCommand(&put_actor_in_car, m_dwGTAId, iVehicleID);
	}
	else
	{
		iSeat--;
		ScriptCommand(&put_actor_in_car2, m_dwGTAId, iVehicleID, iSeat);
	}

	if(m_pPed == GamePool_FindPlayerPed() && IN_VEHICLE(m_pPed))
		pGame->GetCamera()->SetBehindPlayer();

	if(pNetGame)
	{
		// äîïèëèòü (òðåéëåðû)
	}
}

uint8_t CPlayerPed::GetCurrentWeapon()
{
	//if(!m_pPed) return 0;
	//if(GamePool_Ped_GetAt(m_dwGTAId) == 0) return 0;
	//return ScriptCommand(&get_current_char_weapon, m_dwGTAId);
	return m_byteCurrentWeapon;
}

uint8_t CPlayerPed::GetCurrentCharWeapon()
{
	//if(!m_pPed) return 0;
	//if(GamePool_Ped_GetAt(m_dwGTAId) == 0) return 0;
	//return ScriptCommand(&get_current_char_weapon, m_dwGTAId);
	//return m_byteCurrentWeapon;
	for(int i = 0; i <= 46; i++){
		if(ScriptCommand(&is_char_holding_weapon, m_dwGTAId, i))return (uint8_t)i;
	}
}

int CPlayerPed::GetCurrentWeaponSlot(int iWeaponID)
{
	if(m_pPed)return ScriptCommand(&get_weapontype_slot, iWeaponID);
	return -1;
}

void CPlayerPed::GiveWeapon(int iWeaponID, int iAmmo)
{
	// Brass Knuckles
	if(iWeaponID == 1){
		int i = WEAPON_MODEL_BRASSKNUCKLE;
		if(!pGame->IsModelLoaded(i)) {
			pGame->RequestModel(i);
			pGame->LoadRequestedModels();
			while(!pGame->IsModelLoaded(i)) usleep(5);
		}
	}

	// Golf Club
	if(iWeaponID == 2){
		int i = WEAPON_MODEL_GOLFCLUB;
		if(!pGame->IsModelLoaded(i)) {
			pGame->RequestModel(i);
			pGame->LoadRequestedModels();
			while(!pGame->IsModelLoaded(i)) usleep(5);
		}
	}

	// Nitestick
	if(iWeaponID == 3){
		int i = WEAPON_MODEL_NITESTICK;
		if(!pGame->IsModelLoaded(i)) {
			pGame->RequestModel(i);
			pGame->LoadRequestedModels();
			while(!pGame->IsModelLoaded(i)) usleep(5);
		}
	}

	// Knife
	if(iWeaponID == 4){
		int i = WEAPON_MODEL_KNIFE;
		if(!pGame->IsModelLoaded(i)) {
			pGame->RequestModel(i);
			pGame->LoadRequestedModels();
			while(!pGame->IsModelLoaded(i)) usleep(5);
		}
	}

	// Bat
	if(iWeaponID == 5){
		int i = WEAPON_MODEL_BAT;
		if(!pGame->IsModelLoaded(i)) {
			pGame->RequestModel(i);
			pGame->LoadRequestedModels();
			while(!pGame->IsModelLoaded(i)) usleep(5);
		}
	}

	// Shovel
	if(iWeaponID == 6){
		int i = WEAPON_MODEL_SHOVEL;
		if(!pGame->IsModelLoaded(i)) {
			pGame->RequestModel(i);
			pGame->LoadRequestedModels();
			while(!pGame->IsModelLoaded(i)) usleep(5);
		}
	}

	// Pool Cue
	if(iWeaponID == 7){
		int i = WEAPON_MODEL_POOLSTICK;
		if(!pGame->IsModelLoaded(i)) {
			pGame->RequestModel(i);
			pGame->LoadRequestedModels();
			while(!pGame->IsModelLoaded(i)) usleep(5);
		}
	}

	// Katana
	if(iWeaponID == 8){
		int i = WEAPON_MODEL_KATANA;
		if(!pGame->IsModelLoaded(i)) {
			pGame->RequestModel(i);
			pGame->LoadRequestedModels();
			while(!pGame->IsModelLoaded(i)) usleep(5);
		}
	}

	// Chainsaw
	if(iWeaponID == 9){
		int i = WEAPON_MODEL_CHAINSAW;
		if(!pGame->IsModelLoaded(i)) {
			pGame->RequestModel(i);
			pGame->LoadRequestedModels();
			while(!pGame->IsModelLoaded(i)) usleep(5);
		}
	}

	// Purple Dildo (hello to Leanden)
	if(iWeaponID == 10){
		int i = WEAPON_MODEL_DILDO;
		if(!pGame->IsModelLoaded(i)) {
			pGame->RequestModel(i);
			pGame->LoadRequestedModels();
			while(!pGame->IsModelLoaded(i)) usleep(5);
		}
	}

	// Dildo (hello to Leanden)
	if(iWeaponID == 11){
		int i = WEAPON_MODEL_DILDO2;
		if(!pGame->IsModelLoaded(i)) {
			pGame->RequestModel(i);
			pGame->LoadRequestedModels();
			while(!pGame->IsModelLoaded(i)) usleep(5);
		}
	}

	// Vibrator (hello to Leanden)
	if(iWeaponID == 12){
		int i = WEAPON_MODEL_VIBRATOR;
		if(!pGame->IsModelLoaded(i)) {
			pGame->RequestModel(i);
			pGame->LoadRequestedModels();
			while(!pGame->IsModelLoaded(i)) usleep(5);
		}
	}

	// Silver Vibrator (hello to Leanden)
	if(iWeaponID == 13){
		int i = WEAPON_MODEL_VIBRATOR2;
		if(!pGame->IsModelLoaded(i)) {
			pGame->RequestModel(i);
			pGame->LoadRequestedModels();
			while(!pGame->IsModelLoaded(i)) usleep(5);
		}
	}

	// Flowers
	if(iWeaponID == 14){
		int i = WEAPON_MODEL_FLOWER;
		if(!pGame->IsModelLoaded(i)) {
			pGame->RequestModel(i);
			pGame->LoadRequestedModels();
			while(!pGame->IsModelLoaded(i)) usleep(5);
		}
	}

	// Cane
	if(iWeaponID == 15){
		int i = WEAPON_MODEL_CANE;
		if(!pGame->IsModelLoaded(i)) {
			pGame->RequestModel(i);
			pGame->LoadRequestedModels();
			while(!pGame->IsModelLoaded(i)) usleep(5);
		}
	}

	// Grenade
	if(iWeaponID == 16){
		int i = WEAPON_MODEL_GRENADE;
		if(!pGame->IsModelLoaded(i)) {
			pGame->RequestModel(i);
			pGame->LoadRequestedModels();
			while(!pGame->IsModelLoaded(i)) usleep(5);
		}
	}

	// Tear Gas
	if(iWeaponID == 17){
		int i = WEAPON_MODEL_TEARGAS;
		if(!pGame->IsModelLoaded(i)) {
			pGame->RequestModel(i);
			pGame->LoadRequestedModels();
			while(!pGame->IsModelLoaded(i)) usleep(5);
		}
	}

	// Molotov Cocktail
	if(iWeaponID == 18){
		int i = WEAPON_MODEL_MOLOTOV;
		if(!pGame->IsModelLoaded(i)) {
			pGame->RequestModel(i);
			pGame->LoadRequestedModels();
			while(!pGame->IsModelLoaded(i)) usleep(5);
		}
	}

	// Pistol
	if(iWeaponID == 22){
		int i = WEAPON_MODEL_COLT45;
		if(!pGame->IsModelLoaded(i)) {
			pGame->RequestModel(i);
			pGame->LoadRequestedModels();
			while(!pGame->IsModelLoaded(i)) usleep(5);
		}
	}

	// Silenced Pistol
	if(iWeaponID == 23){
		int i = WEAPON_MODEL_SILENCED;
		if(!pGame->IsModelLoaded(i)) {
			pGame->RequestModel(i);
			pGame->LoadRequestedModels();
			while(!pGame->IsModelLoaded(i)) usleep(5);
		}
	}

	// Deagle
	if(iWeaponID == 24){
		int i = WEAPON_MODEL_DEAGLE;
		if(!pGame->IsModelLoaded(i)) {
			pGame->RequestModel(i);
			pGame->LoadRequestedModels();
			while(!pGame->IsModelLoaded(i)) usleep(5);
		}
	}

	// Shotgun
	if(iWeaponID == 25){
		int i = WEAPON_MODEL_SHOTGUN;
		if(!pGame->IsModelLoaded(i)) {
			pGame->RequestModel(i);
			pGame->LoadRequestedModels();
			while(!pGame->IsModelLoaded(i)) usleep(5);
		}
	}

	// Sawedoff Shotgun
	if(iWeaponID == 26){
		int i = WEAPON_MODEL_SAWEDOFF;
		if(!pGame->IsModelLoaded(i)) {
			pGame->RequestModel(i);
			pGame->LoadRequestedModels();
			while(!pGame->IsModelLoaded(i)) usleep(5);
		}
	}

	// Combat Shotgun
	if(iWeaponID == 27){
		int i = WEAPON_MODEL_SHOTGSPA;
		if(!pGame->IsModelLoaded(i)) {
			pGame->RequestModel(i);
			pGame->LoadRequestedModels();
			while(!pGame->IsModelLoaded(i)) usleep(5);
		}
	}

	// Uzi
	if(iWeaponID == 28){
		int i = WEAPON_MODEL_UZI;
		if(!pGame->IsModelLoaded(i)) {
			pGame->RequestModel(i);
			pGame->LoadRequestedModels();
			while(!pGame->IsModelLoaded(i)) usleep(5);
		}
	}

	// SMG
	if(iWeaponID == 29){
		int i = WEAPON_MODEL_MP5;
		if(!pGame->IsModelLoaded(i)) {
			pGame->RequestModel(i);
			pGame->LoadRequestedModels();
			while(!pGame->IsModelLoaded(i)) usleep(5);
		}
	}

	// AK-47
	if(iWeaponID == 30){
		int i = WEAPON_MODEL_AK47;
		if(!pGame->IsModelLoaded(i)) {
			pGame->RequestModel(i);
			pGame->LoadRequestedModels();
			while(!pGame->IsModelLoaded(i)) usleep(5);
		}
	}

	// M4
	if(iWeaponID == 31){
		int i = WEAPON_MODEL_M4;
		if(!pGame->IsModelLoaded(i)) {
			pGame->RequestModel(i);
			pGame->LoadRequestedModels();
			while(!pGame->IsModelLoaded(i)) usleep(5);
		}
	}

	// Tec-9
	if(iWeaponID == 32){
		int i = WEAPON_MODEL_TEC9;
		if(!pGame->IsModelLoaded(i)) {
			pGame->RequestModel(i);
			pGame->LoadRequestedModels();
			while(!pGame->IsModelLoaded(i)) usleep(5);
		}
	}

	// Rifle
	if(iWeaponID == 33){
		int i = WEAPON_MODEL_RIFLE;
		if(!pGame->IsModelLoaded(i)) {
			pGame->RequestModel(i);
			pGame->LoadRequestedModels();
			while(!pGame->IsModelLoaded(i)) usleep(5);
		}
	}

	// Sniper Rifle
	if(iWeaponID == 34){
		int i = WEAPON_MODEL_SNIPER;
		if(!pGame->IsModelLoaded(i)) {
			pGame->RequestModel(i);
			pGame->LoadRequestedModels();
			while(!pGame->IsModelLoaded(i)) usleep(5);
		}
	}

	// RPG
	if(iWeaponID == 35){
		int i = WEAPON_MODEL_ROCKETLAUNCHER;
		if(!pGame->IsModelLoaded(i)) {
			pGame->RequestModel(i);
			pGame->LoadRequestedModels();
			while(!pGame->IsModelLoaded(i)) usleep(5);
		}
	}

	// HS Rocket
	if(iWeaponID == 36){
		int i = WEAPON_MODEL_HEATSEEKER;
		if(!pGame->IsModelLoaded(i)) {
			pGame->RequestModel(i);
			pGame->LoadRequestedModels();
			while(!pGame->IsModelLoaded(i)) usleep(5);
		}
	}

	// Flamethrower
	if(iWeaponID == 37){
		int i = WEAPON_MODEL_FLAMETHROWER;
		if(!pGame->IsModelLoaded(i)) {
			pGame->RequestModel(i);
			pGame->LoadRequestedModels();
			while(!pGame->IsModelLoaded(i)) usleep(5);
		}
	}

	// Minigun
	if(iWeaponID == 38){
		int i = WEAPON_MODEL_MINIGUN;
		if(!pGame->IsModelLoaded(i)) {
			pGame->RequestModel(i);
			pGame->LoadRequestedModels();
			while(!pGame->IsModelLoaded(i)) usleep(5);
		}
	}

	// Satchel Charges
	if(iWeaponID == 39){
		int i = WEAPON_MODEL_SATCHEL;
		if(!pGame->IsModelLoaded(i)) {
			pGame->RequestModel(i);
			pGame->LoadRequestedModels();
			while(!pGame->IsModelLoaded(i)) usleep(5);
		}
	}

	// Detonator
	if(iWeaponID == 40){
		int i = WEAPON_MODEL_BOMB;
		if(!pGame->IsModelLoaded(i)) {
			pGame->RequestModel(i);
			pGame->LoadRequestedModels();
			while(!pGame->IsModelLoaded(i)) usleep(5);
		}
	}

	// Spraycan
	if(iWeaponID == 41){
		int i = WEAPON_MODEL_SPRAYCAN;
		if(!pGame->IsModelLoaded(i)) {
			pGame->RequestModel(i);
			pGame->LoadRequestedModels();
			while(!pGame->IsModelLoaded(i)) usleep(5);
		}
	}

	// Fire Extinguisher
	if(iWeaponID == 42){
		int i = WEAPON_MODEL_FIREEXTINGUISHER;
		if(!pGame->IsModelLoaded(i)) {
			pGame->RequestModel(i);
			pGame->LoadRequestedModels();
			while(!pGame->IsModelLoaded(i)) usleep(5);
		}
	}

	// Camera
	if(iWeaponID == 43){
		int i = WEAPON_MODEL_CAMERA;
		if(!pGame->IsModelLoaded(i)) {
			pGame->RequestModel(i);
			pGame->LoadRequestedModels();
			while(!pGame->IsModelLoaded(i)) usleep(5);
		}
	}

	// NV Goggles
	if(iWeaponID == 44){
		int i = WEAPON_MODEL_NIGHTVISION;
		if(!pGame->IsModelLoaded(i)) {
			pGame->RequestModel(i);
			pGame->LoadRequestedModels();
			while(!pGame->IsModelLoaded(i)) usleep(5);
		}
	}

	// Thermal Goggles
	if(iWeaponID == 45){
		int i = WEAPON_MODEL_INFRARED;
		if(!pGame->IsModelLoaded(i)) {
			pGame->RequestModel(i);
			pGame->LoadRequestedModels();
			while(!pGame->IsModelLoaded(i)) usleep(5);
		}
	}

	// Parachute
	if(iWeaponID == 46){
		int i = WEAPON_MODEL_PARACHUTE;
		if(!pGame->IsModelLoaded(i)) {
			pGame->RequestModel(i);
			pGame->LoadRequestedModels();
			while(!pGame->IsModelLoaded(i)) usleep(5);
		}
	}

	// Jetpack
	if(iWeaponID == 47){
		int i = WEAPON_MODEL_JETPACK;
		if(!pGame->IsModelLoaded(i)) {
			pGame->RequestModel(i);
			pGame->LoadRequestedModels();
			while(!pGame->IsModelLoaded(i)) usleep(5);
		}
	}

	ScriptCommand(&give_weapon_to_char,m_dwGTAId,iWeaponID,iAmmo);
	ScriptCommand(&give_weapon_to_player,m_dwGTAId,iWeaponID,iAmmo);
	ScriptCommand(&set_render_player_weapon);
	SetArmedWeapon(iWeaponID);
	m_byteCurrentWeapon = iWeaponID;
}


void CPlayerPed::SetArmedWeapon(int iWeaponType)
{
	ScriptCommand(&give_weapon_to_char,m_dwGTAId,iWeaponType, 1);
	ScriptCommand(&set_current_char_weapon,m_dwGTAId,iWeaponType);
	ScriptCommand(&set_current_player_weapon,m_dwGTAId,iWeaponType);
	ScriptCommand(&set_render_player_weapon);
	m_byteCurrentWeapon = iWeaponType;
}

void CPlayerPed::ClearAllWeapons(){
	ScriptCommand(&remove_all_char_weapons,m_dwGTAId);
	ScriptCommand(&set_render_player_weapon);
	m_byteCurrentWeapon = 0;
}

void CPlayerPed::EnterVehicle(int iVehicleID, bool bPassenger)
{
	if(!m_pPed) return;
	VEHICLE_TYPE* ThisVehicleType;
	if((ThisVehicleType = GamePool_Vehicle_GetAt(iVehicleID)) == 0) return;
	if(!GamePool_Ped_GetAt(m_dwGTAId)) return;

	if(bPassenger)
	{
		if(ThisVehicleType->entity.nModelIndex == TRAIN_PASSENGER &&
			(m_pPed == GamePool_FindPlayerPed()))
		{
			if(!IN_VEHICLE(m_pPed))ScriptCommand(&put_actor_in_car2, m_dwGTAId, iVehicleID, -1);
		}
		else
		{
			if(!IN_VEHICLE(m_pPed))ScriptCommand(&send_actor_to_car_passenger,m_dwGTAId,iVehicleID, 3000, -1);
		}
	}
	else
		if(!IN_VEHICLE(m_pPed))ScriptCommand(&send_actor_to_car_driverseat, m_dwGTAId, iVehicleID, 3000);
}

// 0.3.7
void CPlayerPed::ExitCurrentVehicle()
{
	if(!m_pPed) return;
	if(!GamePool_Ped_GetAt(m_dwGTAId)) return;

	VEHICLE_TYPE* ThisVehicleType = 0;

	if(IN_VEHICLE(m_pPed))
	{
		if(GamePool_Vehicle_GetIndex((VEHICLE_TYPE*)m_pPed->pVehicle))
		{
			int index = GamePool_Vehicle_GetIndex((VEHICLE_TYPE*)m_pPed->pVehicle);
			ThisVehicleType = GamePool_Vehicle_GetAt(index);
			if(ThisVehicleType)
			{
				if(	ThisVehicleType->entity.nModelIndex != TRAIN_PASSENGER &&
					ThisVehicleType->entity.nModelIndex != TRAIN_PASSENGER_LOCO)
				{
					ScriptCommand(&make_actor_leave_car, m_dwGTAId, GetCurrentVehicleID());
				}
			}
		}
	}
}

// 0.3.7
int CPlayerPed::GetCurrentVehicleID()
{
	if(!m_pPed) return 0;

	VEHICLE_TYPE *pVehicle = (VEHICLE_TYPE *)m_pPed->pVehicle;
	return GamePool_Vehicle_GetIndex(pVehicle);
}

int CPlayerPed::GetVehicleSeatID()
{
	VEHICLE_TYPE *pVehicle;

	if( GetActionTrigger() == ACTION_INCAR && (pVehicle = (VEHICLE_TYPE *)m_pPed->pVehicle) != 0 ) 
	{
		if(pVehicle->pDriver == m_pPed) return 0;
		if(pVehicle->pPassengers[0] == m_pPed) return 1;
		if(pVehicle->pPassengers[1] == m_pPed) return 2;
		if(pVehicle->pPassengers[2] == m_pPed) return 3;
		if(pVehicle->pPassengers[3] == m_pPed) return 4;
		if(pVehicle->pPassengers[4] == m_pPed) return 5;
		if(pVehicle->pPassengers[5] == m_pPed) return 6;
		if(pVehicle->pPassengers[6] == m_pPed) return 7;
	}

	return (-1);
}


void CPlayerPed::ShakeCam(int time){
	ScriptCommand(&shake_cam, time);
}

// 0.3.7
void CPlayerPed::TogglePlayerControllable(bool bToggle)
{
	MATRIX4X4 mat;

	if(!GamePool_Ped_GetAt(m_dwGTAId)) return;

	if(!bToggle)
	{
		ScriptCommand(&toggle_player_controllable, m_bytePlayerNumber, 0);
		ScriptCommand(&lock_actor, m_dwGTAId, 1);
	}
	else
	{
		ScriptCommand(&toggle_player_controllable, m_bytePlayerNumber, 1);
		ScriptCommand(&lock_actor, m_dwGTAId, 0);
		if(!IsInVehicle()) 
		{
			GetMatrix(&mat);
			TeleportTo(mat.pos.X, mat.pos.Y, mat.pos.Z);
		}
	}
}

void CPlayerPed::TogglePlayerControllableWithoutLock(bool bToggle)
{
	MATRIX4X4 mat;

	if(!GamePool_Ped_GetAt(m_dwGTAId)) return;

	if(!bToggle)
	{
		ScriptCommand(&toggle_player_controllable, m_bytePlayerNumber, 0);
	}
	else
	{
		ScriptCommand(&toggle_player_controllable, m_bytePlayerNumber, 1);
		if(!IsInVehicle()) 
		{
			GetMatrix(&mat);
			TeleportTo(mat.pos.X, mat.pos.Y, mat.pos.Z);
		}
	}
}

// 0.3.7
void CPlayerPed::SetModelIndex(unsigned int uiModel)
{
	if(!GamePool_Ped_GetAt(m_dwGTAId)) return;
	if(!IsPedModel(uiModel))
		uiModel = 0;

	if(m_pPed)
	{
		// CClothes::RebuildPlayer nulled
		WriteMemory(g_libGTASA+0x3F1030, (uintptr_t)"\x70\x47", 2);
		DestroyFollowPedTask();
		CEntity::SetModelIndex(uiModel);

		// reset the Ped Audio Attributes
		(( void (*)(uintptr_t, uintptr_t))(g_libGTASA+0x34B2A8+1))(((uintptr_t)m_pPed+660), (uintptr_t)m_pPed);
	}
}

// äîïèëèòü
void CPlayerPed::DestroyFollowPedTask()
{

}

// äîïèëèòü
void CPlayerPed::ResetDamageEntity()
{

}

// 0.3.7
void CPlayerPed::RestartIfWastedAt(VECTOR *vecRestart, float fRotation)
{	
	ScriptCommand(&restart_if_wasted_at, vecRestart->X, vecRestart->Y, vecRestart->Z, fRotation, 0);
}

// 0.3.7
void CPlayerPed::ForceTargetRotation(float fRotation)
{
	if(!m_pPed) return;
	if(!GamePool_Ped_GetAt(m_dwGTAId)) return;

	m_pPed->fRotation1 = DegToRad(fRotation);
	m_pPed->fRotation2 = DegToRad(fRotation);

	ScriptCommand(&set_actor_z_angle,m_dwGTAId,fRotation);
}

void CPlayerPed::GetTargetRotation()
{
	if(!m_pPed) return;
	if(!GamePool_Ped_GetAt(m_dwGTAId)) return;

	//m_pPed->fRotation1;
	//m_pPed->fRotation2;

	pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Rotations: %f | %f", m_pPed->fRotation1, m_pPed->fRotation2);
}

void CPlayerPed::SetRotation(float fRotation)
{
	if(!m_pPed) return;
	if(!GamePool_Ped_GetAt(m_dwGTAId)) return;

	m_pPed->fRotation1 = DegToRad(fRotation);
	m_pPed->fRotation2 = DegToRad(fRotation);
}

// 0.3.7
uint8_t CPlayerPed::GetActionTrigger()
{
	return (uint8_t)m_pPed->dwAction;
}

// 0.3.7
bool CPlayerPed::IsDead()
{
	if(!m_pPed) return true;
	if(m_pPed->fHealth > 0.0f) return false;
	return true;
}

void CPlayerPed::SetMoney(int iAmount)
{
	ScriptCommand(&set_actor_money, m_dwGTAId, 0);
	ScriptCommand(&set_actor_money, m_dwGTAId, iAmount);
}

// 0.3.7
void CPlayerPed::ShowMarker(uint32_t iMarkerColorID)
{
	if(m_dwArrow) HideMarker();
	ScriptCommand(&create_arrow_above_actor, m_dwGTAId, &m_dwArrow);
	ScriptCommand(&set_marker_color, m_dwArrow, iMarkerColorID);
	ScriptCommand(&show_on_radar2, m_dwArrow, 2);
}

// 0.3.7
void CPlayerPed::HideMarker()
{
	if(m_dwArrow) ScriptCommand(&disable_marker, m_dwArrow);
	m_dwArrow = 0;
}

// 0.3.7
void CPlayerPed::SetFightingStyle(int iStyle)
{
	if(!m_pPed) return;
	ScriptCommand( &set_fighting_style, m_dwGTAId, iStyle, 6 );
}

// 0.3.7
void CPlayerPed::ApplyAnimation( char *szAnimName, char *szAnimFile, float fT,
								 int opt1, int opt2, int opt3, int opt4, int iUnk )
{
	int iWaitAnimLoad = 0;

	if(!m_pPed) return;
	if(!GamePool_Ped_GetAt(m_dwGTAId)) return;

	if(!strcasecmp(szAnimFile,"SEX")) return;

	if(!pGame->IsAnimationLoaded(szAnimFile))
	{
		if(pModSAWindow->m_bAPA != 1)pGame->RequestAnimation(szAnimFile);
		while(!pGame->IsAnimationLoaded(szAnimFile))
		{
			usleep(1000);
			iWaitAnimLoad++;
			if(iWaitAnimLoad > 15) return;
		}
	}

	if(pModSAWindow->m_bAPA != 1)ScriptCommand(&apply_animation, m_dwGTAId, szAnimName, szAnimFile, fT, opt1, opt2, opt3, opt4, iUnk);
}

void CPlayerPed::FindDeathReasonAndResponsiblePlayer(PLAYERID *nPlayer)
{
	CPlayerPool *pPlayerPool;
	CVehiclePool *pVehiclePool;
	PLAYERID PlayerIDWhoKilled 	 = INVALID_PLAYER_ID;
	
	if(pNetGame) 
	{
		pVehiclePool = pNetGame->GetVehiclePool();
		pPlayerPool = pNetGame->GetPlayerPool();
	}
	else 
	{ // just leave if there's no netgame.
		*nPlayer = INVALID_PLAYER_ID;
		return;
	}

	if(m_pPed)
	{
		if(m_pPed->pdwDamageEntity)
		{
			PlayerIDWhoKilled = pPlayerPool->FindRemotePlayerIDFromGtaPtr((PED_TYPE *)m_pPed->pdwDamageEntity);
			if(PlayerIDWhoKilled != INVALID_PLAYER_ID) 
			{
					// killed by another player with a weapon, this is all easy.
					*nPlayer = PlayerIDWhoKilled;
					return;
			}
			else
			{
				if(pVehiclePool->FindIDFromGtaPtr((VEHICLE_TYPE *)m_pPed->pdwDamageEntity) != INVALID_VEHICLE_ID) 
				{
					VEHICLE_TYPE *pGtaVehicle = (VEHICLE_TYPE *)m_pPed->pdwDamageEntity;
					PlayerIDWhoKilled = pPlayerPool->FindRemotePlayerIDFromGtaPtr((PED_TYPE *)pGtaVehicle->pDriver);
												
					if(PlayerIDWhoKilled != INVALID_PLAYER_ID) 
					{
						*nPlayer = PlayerIDWhoKilled;
						return;
					}
				}
			}
		}
	}

	*nPlayer = INVALID_PLAYER_ID;
}

/*bool CPlayerPed::StartPassengerDriveByMode()
{
	if(m_pPed) {

		if(!IN_VEHICLE(m_pPed) || !m_pPed->pVehicle) return FALSE;

		int iWeapon = GetCurrentWeapon();
		
		// Don't allow them to enter driveby with a para
		if(iWeapon == WEAPON_PARACHUTE) {
			SetArmedWeapon(0);
			return FALSE;
		}

		// Check for an uzi type weapon.
		if((iWeapon != WEAPON_MODEL_UZI) && (iWeapon != WEAPON_MODEL_MP5) && (iWeapon != WEAPON_MODEL_TEC9)) {
			return FALSE;
		}

		SetArmedWeapon(iWeapon);
	
		 We should replace them in their current seat.
		int iVehicleID = GamePool_Vehicle_GetIndex((VEHICLE_TYPE *)m_pPed->pVehicle);
		int iSeatID = GetVehicleSeatID();
		PutDirectlyInVehicle(iVehicleID,iSeatID);

		ScriptCommand(&enter_passenger_driveby,
			m_dwGTAId,-1,-1,0.0f,0.0f,0.0f,300.0f,8,1,100);

		//SetWeaponModelIndex(iWeapon);

		return TRUE;
	}
	return FALSE;
}
*/

// 0.3.7
void CPlayerPed::GetBonePosition(int iBoneID, VECTOR* vecOut)
{
	if(!m_pPed) return;
	if(m_pEntity->vtable == g_libGTASA+0x5C7358) return;

	(( void (*)(PED_TYPE*, VECTOR*, int, int))(g_libGTASA+0x436590+1))(m_pPed, vecOut, iBoneID, 0);
}

ENTITY_TYPE* CPlayerPed::GetEntityUnderPlayer()
{
	uintptr_t entity;
	VECTOR vecStart;
	VECTOR vecEnd;
	char buf[100];

	if(!m_pPed) return nullptr;
	if( IN_VEHICLE(m_pPed) || !GamePool_Ped_GetAt(m_dwGTAId))
		return 0;

	vecStart.X = m_pPed->entity.mat->pos.X;
	vecStart.Y = m_pPed->entity.mat->pos.Y;
	vecStart.Z = m_pPed->entity.mat->pos.Z - 0.25f;

	vecEnd.X = m_pPed->entity.mat->pos.X;
	vecEnd.Y = m_pPed->entity.mat->pos.Y;
	vecEnd.Z = vecStart.Z - 1.75f;

	LineOfSight(&vecStart, &vecEnd, (void*)buf, (uintptr_t)&entity,
		0, 1, 0, 1, 0, 0, 0, 0);

	return (ENTITY_TYPE*)entity;
}

// äîïèëèòü
uint16_t CPlayerPed::GetKeys(uint16_t *lrAnalog, uint16_t *udAnalog)
{
	*lrAnalog = LocalPlayerKeys.wKeyLR;
	*udAnalog = LocalPlayerKeys.wKeyUD;

	uint16_t wRet = 0;

	// KEY_ANALOG_RIGHT
	if(LocalPlayerKeys.bKeys[ePadKeys::KEY_ANALOG_RIGHT]) wRet |= 1;
	wRet <<= 1;
	// KEY_ANALOG_LEFT
	if(LocalPlayerKeys.bKeys[ePadKeys::KEY_ANALOG_LEFT]) wRet |= 1;
	wRet <<= 1;
	// KEY_ANALOG_DOWN
	if(LocalPlayerKeys.bKeys[ePadKeys::KEY_ANALOG_DOWN]) wRet |= 1;
	wRet <<= 1;
	// KEY_ANALOG_UP
	if(LocalPlayerKeys.bKeys[ePadKeys::KEY_ANALOG_UP]) wRet |= 1;
	wRet <<= 1;
	// KEY_WALK
	if(LocalPlayerKeys.bKeys[ePadKeys::KEY_WALK]) wRet |= 1;
	wRet <<= 1;
	// KEY_SUBMISSION
	if(LocalPlayerKeys.bKeys[ePadKeys::KEY_SUBMISSION]) wRet |= 1;
	wRet <<= 1;
	// KEY_WALK
	if(LocalPlayerKeys.bKeys[ePadKeys::KEY_WALK]) wRet |= 1;
	wRet <<= 1;
	// KEY_SUBMISSION
	if(LocalPlayerKeys.bKeys[ePadKeys::KEY_SUBMISSION]) wRet |= 1;
	wRet <<= 1;
	// KEY_LOOK_LEFT
	if(LocalPlayerKeys.bKeys[ePadKeys::KEY_LOOK_LEFT]) wRet |= 1;
	wRet <<= 1;
	// KEY_HANDBRAKE
	if(LocalPlayerKeys.bKeys[ePadKeys::KEY_HANDBRAKE]) wRet |= 1;
	wRet <<= 1;
	// KEY_LOOK_RIGHT
	if(LocalPlayerKeys.bKeys[ePadKeys::KEY_LOOK_RIGHT]) wRet |= 1;
	wRet <<= 1;
	// KEY_JUMP
	if(LocalPlayerKeys.bKeys[ePadKeys::KEY_JUMP]) wRet |= 1;
	wRet <<= 1;
	// KEY_SECONDARY_ATTACK
	if(LocalPlayerKeys.bKeys[ePadKeys::KEY_SECONDARY_ATTACK]) wRet |= 1;
	wRet <<= 1;
	// KEY_SPRINT
	if(LocalPlayerKeys.bKeys[ePadKeys::KEY_SPRINT]) wRet |= 1;
	wRet <<= 1;
	// KEY_FIRE
	if(LocalPlayerKeys.bKeys[ePadKeys::KEY_FIRE]) wRet |= 1;
	wRet <<= 1;
	// KEY_CROUCH
	if(LocalPlayerKeys.bKeys[ePadKeys::KEY_CROUCH]) wRet |= 1;
	wRet <<= 1;
	// KEY_ACTION
	if(LocalPlayerKeys.bKeys[ePadKeys::KEY_ACTION]) wRet |= 1;

	memset(LocalPlayerKeys.bKeys, 0, ePadKeys::SIZE);

	return wRet;
}

void CPlayerPed::SetKeys(uint16_t wKeys, uint16_t lrAnalog, uint16_t udAnalog)
{
	PAD_KEYS *pad = &RemotePlayerKeys[m_bytePlayerNumber];

	// LEFT/RIGHT
	pad->wKeyLR = lrAnalog;
	// UP/DOWN
	pad->wKeyUD = udAnalog;

	// KEY_ACTION
	pad->bKeys[ePadKeys::KEY_ACTION] = (wKeys & 1);
	wKeys >>= 1;
	// KEY_CROUCH
	pad->bKeys[ePadKeys::KEY_CROUCH] = (wKeys & 1);
	wKeys >>= 1;
	// KEY_FIRE
	pad->bKeys[ePadKeys::KEY_FIRE] = (wKeys & 1);
	wKeys >>= 1;
	// KEY_SPRINT
	pad->bKeys[ePadKeys::KEY_SPRINT] = (wKeys & 1);
	wKeys >>= 1;
	// KEY_SECONDARY_ATTACK
	pad->bKeys[ePadKeys::KEY_SECONDARY_ATTACK] = (wKeys & 1);
	wKeys >>= 1;
	// KEY_JUMP
	pad->bKeys[ePadKeys::KEY_JUMP] = (wKeys & 1);
	if(!pad->bKeys[ePadKeys::KEY_JUMP]) pad->bIgnoreJump = false;
	wKeys >>= 1;
	// KEY_LOOK_RIGHT
	pad->bKeys[ePadKeys::KEY_LOOK_RIGHT] = (wKeys & 1);
	wKeys >>= 1;
	// KEY_HANDBRAKE
	pad->bKeys[ePadKeys::KEY_HANDBRAKE] = (wKeys & 1);
	wKeys >>= 1;
	// KEY_LOOK_LEFT
	pad->bKeys[ePadKeys::KEY_LOOK_LEFT] = (wKeys & 1);
	wKeys >>= 1;
	// KEY_SUBMISSION
	pad->bKeys[ePadKeys::KEY_SUBMISSION] = (wKeys & 1);
	wKeys >>= 1;
	// KEY_WALK
	pad->bKeys[ePadKeys::KEY_WALK] = (wKeys & 1);
	wKeys >>= 1;
	// KEY_ANALOG_UP
	pad->bKeys[ePadKeys::KEY_ANALOG_UP] = (wKeys & 1);
	wKeys >>= 1;
	// KEY_ANALOG_DOWN
	pad->bKeys[ePadKeys::KEY_ANALOG_DOWN] = (wKeys & 1);
	wKeys >>= 1;
	// KEY_ANALOG_LEFT
	pad->bKeys[ePadKeys::KEY_ANALOG_LEFT] = (wKeys & 1);
	wKeys >>= 1;
	// KEY_ANALOG_RIGHT
	pad->bKeys[ePadKeys::KEY_ANALOG_RIGHT] = (wKeys & 1);

	return;
}

