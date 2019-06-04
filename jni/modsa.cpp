/*
    * m0d_SA Mobile v0.0.0.1 patch 0.1
    * Author: QDS Team
    * https://www.youtube.com/Quildeesaw
    * https://vk.com/quildeesaw
    * https://github.com/qdsteam/m0d_SA_Mobile
*/

#include "main.h"
#include "game/game.h"
#include "net/netgame.h"
#include "game/common.h"
#include "gui/gui.h"
#include "modsa.h"
#include "vendor/imgui/imgui_internal.h"
#include "keyboard.h"
#include "chatwindow.h"
#include "net/vehiclepool.h"
#include <stdlib.h>
#include <string.h>

extern CGUI *pGUI;
extern CGame *pGame;
extern CNetGame *pNetGame;
extern CKeyBoard *pKeyBoard;
extern CChatWindow *pChatWindow;

CModSAWindow::CModSAWindow()
{
	m_bIsActive = false;
    m_bMenuStep = 0;
    if(m_bGodMode != 1)m_bGodMode = 0;
    if(m_bWallHack != 1)m_bWallHack = 0;
    if(m_bCols != 1)m_bCols = 0;
    if(m_bVP != 1)m_bVP = 0;
    if(m_bVP2 != 1)m_bVP2 = 0;
    if(m_bVV != 1)m_bVV = 0;
    if(m_bCO != 1)m_bCO = 0;
    if(m_bPH != 1)m_bPH = 0;
    if(lockinp != 1)lockinp = 0;
    if(m_bRPFV != 1)m_bRPFV = 0;
    if(m_bSD != 1)m_bSD = 0;
    if(m_bAPA != 1)m_bAPA = 0;
    if(m_bRPW != 1)m_bRPW = 0;
    if(m_bKrutilka != 1)m_bKrutilka = 0;
    if(m_bNF != 1)m_bNF = 0;
    if(marker_X != 1)marker_X = 0;
    if(marker_Y != 1)marker_Y = 0;
    if(marker_Z != 1)marker_Z = 0;
    if(fDist == NULL)fDist = 1.5;
    if(protect == NULL)protect = 0;
}

CModSAWindow::~CModSAWindow()
{

}

void CModSAWindow::Show(bool bShow)
{
	if(pGame) 
        pGame->FindPlayerPed()->TogglePlayerControllableWithoutLock(!bShow);

	m_bIsActive = bShow;
}

void CModSAWindow::Clear()
{
	m_bIsActive = false;
    m_bMenuStep = 0;
    m_bGodMode = 0;
    m_bWallHack = 0;
    m_bCols = 0;
    m_bVP = 0;
    m_bVP2 = 0;
    m_bVV = 0;
    m_bCO = 0;
    m_bPH = 0;
    lockinp = 0;
    m_bRPFV = 0;
    m_bSD = 0;
    m_bAPA = 0;
    m_bRPW = 0;
    m_bKrutilka = 0;
    m_bNF = 0;
    marker_X = 0;
    marker_Y = 0;
    marker_Z = 0;
    protect = 0;
    fDist = 1.5;
}


void CModSAWindow::StoreMarkerXYZ(float x, float y, float z)
{
    marker_X = x; // X
    marker_Y = y; // Y
    marker_Z = z; // Z

    ////pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Marker on X: %f Y: %f Z: %f", marker_X, marker_Y, marker_Z);
}

void CModSAWindow::SetFlySets(float distance)
{
    fDist = distance;
}

float CModSAWindow::GetFlySets()
{
    return (float)fDist;
}

void CModSAWindow::ShowFlyMenu(bool bShow)
{
    if(!bShow)return;

    ImGui::Text("Speed (default: 1.5)");

    if(ImGui::Button("Add", ImVec2(80, 50)))
    {
        SetFlySets((GetFlySets() + 0.1));
        ////pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Set distance to %f", GetFlySets());
        char str[4096];
        sprintf(str, "Speed: %f~n~~n~~n~~n~~n~~n~           ", GetFlySets());
        pGame->DisplayGameText(str, 5000, 3);
    }

    ImGui::SameLine(0, pGUI->GetFontSize());

    if(ImGui::Button("Del", ImVec2(80, 50)))
    {
        SetFlySets((GetFlySets() - 0.1));
        ////pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Set distance to %f", GetFlySets());
        char str[4096];
        sprintf(str, "Speed: %f~n~~n~~n~~n~~n~~n~           ", GetFlySets());
        pGame->DisplayGameText(str, 5000, 3);
    }

    // ---------------------------------------------- //
    ImGui::ItemSize( ImVec2(0, pGUI->GetFontSize()/2 + 5) );
    ImGui::Text("Functions");

    MATRIX4X4 mat;
    pGame->FindPlayerPed()->GetMatrix(&mat);

    pGame->SetGravity(0.000005);

    VECTOR vecMoveSpeed;
    pGame->FindPlayerPed()->ApplyAnimation("SWIM_TREAD", "SWIM", 4.1, 1, 0, 0, 1, 1);

    if(ImGui::Button("FRONT", ImVec2(100, 50)))
    {
        pGame->FindPlayerPed()->ForceTargetRotation(352.46);
        if(!pGame->FindPlayerPed()->IsInVehicle()){
            vecMoveSpeed.X = 0.0f;
            vecMoveSpeed.Y = GetFlySets();
            vecMoveSpeed.Z = 0.0f;
            pGame->FindPlayerPed()->SetMoveSpeedVector(vecMoveSpeed);
        }else{
            ScriptCommand(&set_car_z_angle, pGame->FindPlayerPed()->GetCurrentVehicleID(), 352.46);
            ScriptCommand(&set_car_coordinates, pGame->FindPlayerPed()->GetCurrentVehicleID(), mat.pos.X, mat.pos.Y + GetFlySets(), mat.pos.Z);
        }
        pGame->GetCamera()->SetBehindPlayer();  
    }

    ImGui::SameLine(0, pGUI->GetFontSize());

    if(ImGui::Button("BACK", ImVec2(100, 50)))
    {
        pGame->FindPlayerPed()->ForceTargetRotation(172.46);
        if(!pGame->FindPlayerPed()->IsInVehicle()){
            vecMoveSpeed.X = 0.0f;
            vecMoveSpeed.Y -= GetFlySets();
            vecMoveSpeed.Z = 0.0f;
            pGame->FindPlayerPed()->SetMoveSpeedVector(vecMoveSpeed);
        }else{
            ScriptCommand(&set_car_z_angle, pGame->FindPlayerPed()->GetCurrentVehicleID(), 172.46);
            ScriptCommand(&set_car_coordinates, pGame->FindPlayerPed()->GetCurrentVehicleID(), mat.pos.X, mat.pos.Y - GetFlySets(), mat.pos.Z);
        }
        pGame->GetCamera()->SetBehindPlayer();  
    }

    // ---------------------------------------------- //
    ImGui::ItemSize( ImVec2(0, pGUI->GetFontSize()/2 + 2.5) );

    if(ImGui::Button("UP", ImVec2(100, 50)))
    {
        if(!pGame->FindPlayerPed()->IsInVehicle()){
            pGame->FindPlayerPed()->TeleportTo(mat.pos.X, mat.pos.Y, mat.pos.Z + GetFlySets() * 2.0);
        }else{
            ScriptCommand(&set_car_coordinates, pGame->FindPlayerPed()->GetCurrentVehicleID(), mat.pos.X, mat.pos.Y, mat.pos.Z + GetFlySets());
        }
        pGame->GetCamera()->SetBehindPlayer();  
    }

    ImGui::SameLine(0, pGUI->GetFontSize());

    if(ImGui::Button("DOWN", ImVec2(100, 50)))
    {
        if(!pGame->FindPlayerPed()->IsInVehicle()){
            pGame->FindPlayerPed()->TeleportTo(mat.pos.X, mat.pos.Y, mat.pos.Z - GetFlySets() * 2.0);
        }else{
            ScriptCommand(&set_car_coordinates, pGame->FindPlayerPed()->GetCurrentVehicleID(), mat.pos.X, mat.pos.Y, mat.pos.Z - GetFlySets());
        }
        pGame->GetCamera()->SetBehindPlayer();  
    }

    // ---------------------------------------------- //
    ImGui::ItemSize( ImVec2(0, pGUI->GetFontSize()/2 + 2.5) );

    if(ImGui::Button("LEFT", ImVec2(100, 50)))
    {
        pGame->FindPlayerPed()->ForceTargetRotation(86.51);
        if(!pGame->FindPlayerPed()->IsInVehicle()){
            vecMoveSpeed.X -= GetFlySets();
            vecMoveSpeed.Y = 0.0f;
            vecMoveSpeed.Z = 0.0f;
            pGame->FindPlayerPed()->SetMoveSpeedVector(vecMoveSpeed);
        }else{
            ScriptCommand(&set_car_z_angle, pGame->FindPlayerPed()->GetCurrentVehicleID(), 85.51);
            ScriptCommand(&set_car_coordinates, pGame->FindPlayerPed()->GetCurrentVehicleID(), mat.pos.X - GetFlySets(), mat.pos.Y, mat.pos.Z);
        }
        pGame->GetCamera()->SetBehindPlayer();  
    }

    ImGui::SameLine(0, pGUI->GetFontSize());

    if(ImGui::Button("RIGHT", ImVec2(100, 50)))
    {
        pGame->FindPlayerPed()->ForceTargetRotation(266.48);
        if(!pGame->FindPlayerPed()->IsInVehicle()){
            vecMoveSpeed.X = GetFlySets();
            vecMoveSpeed.Y = 0.0f;
            vecMoveSpeed.Z = 0.0f;
            pGame->FindPlayerPed()->SetMoveSpeedVector(vecMoveSpeed);
        }else{
            ScriptCommand(&set_car_z_angle, pGame->FindPlayerPed()->GetCurrentVehicleID(), 266.48);
            ScriptCommand(&set_car_coordinates, pGame->FindPlayerPed()->GetCurrentVehicleID(), mat.pos.X + GetFlySets(), mat.pos.Y, mat.pos.Z);
        }
        pGame->GetCamera()->SetBehindPlayer();  
    }

    // ---------------------------------------------- //
    ImGui::ItemSize( ImVec2(0, pGUI->GetFontSize()/2 + 2.5) );

    if(ImGui::Button("L-B", ImVec2(100, 50)))
    {
        pGame->FindPlayerPed()->ForceTargetRotation(147.49);
        if(!pGame->FindPlayerPed()->IsInVehicle()){
            vecMoveSpeed.X -= GetFlySets();
            vecMoveSpeed.Y -= GetFlySets();
            vecMoveSpeed.Z = 0.0f;
            pGame->FindPlayerPed()->SetMoveSpeedVector(vecMoveSpeed);
        }else{
            ScriptCommand(&set_car_z_angle, pGame->FindPlayerPed()->GetCurrentVehicleID(), 147.49);
            ScriptCommand(&set_car_coordinates, pGame->FindPlayerPed()->GetCurrentVehicleID(), mat.pos.X - GetFlySets(), mat.pos.Y - GetFlySets(), mat.pos.Z);
        }
        pGame->GetCamera()->SetBehindPlayer();  
    }

    ImGui::SameLine(0, pGUI->GetFontSize());

    if(ImGui::Button("R_F", ImVec2(100, 50)))
    {
        pGame->FindPlayerPed()->ForceTargetRotation(319.07);
        if(!pGame->FindPlayerPed()->IsInVehicle()){
            vecMoveSpeed.X = GetFlySets();
            vecMoveSpeed.Y = GetFlySets();
            vecMoveSpeed.Z = 0.0f;
            pGame->FindPlayerPed()->SetMoveSpeedVector(vecMoveSpeed);
        }else{
            ScriptCommand(&set_car_z_angle, pGame->FindPlayerPed()->GetCurrentVehicleID(), 319.07);
            ScriptCommand(&set_car_coordinates, pGame->FindPlayerPed()->GetCurrentVehicleID(), mat.pos.X + GetFlySets(), mat.pos.Y  + GetFlySets(), mat.pos.Z);
        }
        pGame->GetCamera()->SetBehindPlayer();  
    }

    ImGui::ItemSize( ImVec2(0, pGUI->GetFontSize()/2 + 2.5) );

    if(ImGui::Button("R-B", ImVec2(100, 50)))
    {
        pGame->FindPlayerPed()->ForceTargetRotation(226.99);
        if(!pGame->FindPlayerPed()->IsInVehicle()){
            vecMoveSpeed.X = GetFlySets();
            vecMoveSpeed.Y -= GetFlySets();
            vecMoveSpeed.Z = 0.0f;
            pGame->FindPlayerPed()->SetMoveSpeedVector(vecMoveSpeed);
        }else{
            ScriptCommand(&set_car_z_angle, pGame->FindPlayerPed()->GetCurrentVehicleID(), 226.99);
            ScriptCommand(&set_car_coordinates, pGame->FindPlayerPed()->GetCurrentVehicleID(), mat.pos.X + GetFlySets(), mat.pos.Y - GetFlySets(), mat.pos.Z);
        }
        pGame->GetCamera()->SetBehindPlayer();  
    }

    ImGui::SameLine(0, pGUI->GetFontSize());

    if(ImGui::Button("L-F", ImVec2(100, 50)))
    {
        pGame->FindPlayerPed()->ForceTargetRotation(45.90);
        if(!pGame->FindPlayerPed()->IsInVehicle()){
            vecMoveSpeed.X -= GetFlySets();
            vecMoveSpeed.Y = GetFlySets();
            vecMoveSpeed.Z = 0.0f;
            pGame->FindPlayerPed()->SetMoveSpeedVector(vecMoveSpeed);
        }else{
            ScriptCommand(&set_car_z_angle, pGame->FindPlayerPed()->GetCurrentVehicleID(), 45.90);
            ScriptCommand(&set_car_coordinates, pGame->FindPlayerPed()->GetCurrentVehicleID(), mat.pos.X - GetFlySets(), mat.pos.Y, mat.pos.Z + GetFlySets());
        }
        pGame->GetCamera()->SetBehindPlayer();  
    }

    // ---------------------------------------------- //


    ImGui::ItemSize( ImVec2(0, pGUI->GetFontSize()/2 + 2.5) );

    if(ImGui::Button("Lock", ImVec2(150, 50)))
    {
        ScriptCommand(&lock_actor, pGame->FindPlayerPed()->m_dwGTAId, 1);
    }

    if(ImGui::Button("Unlock", ImVec2(150, 50)))
    {
        ScriptCommand(&lock_actor, pGame->FindPlayerPed()->m_dwGTAId, 0);
    }

    if(ImGui::Button("Close", ImVec2(150, 50)))
    {
        ShowFlyMenu(false);
        Show(false);
        pGame->FindPlayerPed()->TogglePlayerControllable(false);
        pGame->FindPlayerPed()->TogglePlayerControllable(true);
        pGame->SetGravity(0.008);
        ScriptCommand(&lock_actor, pGame->FindPlayerPed()->m_dwGTAId, 0);
        m_bKrutilka = 0;
    }
}

void CModSAWindow::ToggleCheat(int cheatid)
{
    switch(cheatid){
        case 0:
        if(m_bFlyHack == NULL){
            m_bFlyHack = 1;
            //pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Fly Hack Activated!");
            ShowFlyMenu(true);
        }else{
            m_bFlyHack = 0;
            //pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Fly Hack Deactivated!");
            ShowFlyMenu(false);
        }
        break;
        case 1:
        if(m_bGodMode == NULL){
            m_bGodMode = 1;
            m_bPH = 1;
            pGame->FindPlayerPed()->SetHealth(5000);
            ScriptCommand(&set_actor_immunities, pGame->FindPlayerPed()->m_dwGTAId, 1, 1, 1, 1, 1);
            //pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}God Mode Activated!");
        }else{
            m_bGodMode = 0;
            m_bPH = 0;
            pGame->FindPlayerPed()->SetHealth(100);
            ScriptCommand(&set_actor_immunities, pGame->FindPlayerPed()->m_dwGTAId, 0, 0, 0, 0, 0);
            
            VEHICLEID VehicleID;
            if(pNetGame->GetVehiclePool()->GetSlotState(VehicleID))
                pNetGame->GetVehiclePool()->GetAt(VehicleID)->SetInvulnerable(false);
            //pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}God Mode Deactivated!");
        }
        break;
        case 2:
        if(m_bWallHack == NULL){
            m_bWallHack = 1;
            pNetGame->m_fNameTagDrawDistance = 1000;
            //pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Wall Hack Activated!");
        }else{
            m_bWallHack = 0;
            pNetGame->m_fNameTagDrawDistance = 70;
            //pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Wall Hack Deactivated!");
        }
        break;
        case 3:
        if(m_bCols == NULL){
            m_bCols = 1;
            ScriptCommand(&set_char_collision, pGame->FindPlayerPed()->m_dwGTAId, 0);
            //pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Freeze Activated!");
        }else{
            m_bCols = 0;
            ScriptCommand(&set_char_collision, pGame->FindPlayerPed()->m_dwGTAId, 1);
            //pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Freeze Deactivated!");
        } 
        break;
        case 4:
        if(m_bKrutilka == NULL){
            m_bKrutilka = 1;
            //pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Behind Activated!");
        }else{
            m_bKrutilka = 0;
            //pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Behind Deactivated!");
        } 
        break;

        case 5:
        if(m_bNF == NULL){
            m_bNF = 1;
            //pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}No Fall Activated!");
        }else{
            m_bNF = 0;
            //pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}No Fall Deactivated!");
        } 
        break;
    }
}

void CModSAWindow::ToggleRPC(int rpcid){
    switch(rpcid){
        case 0:
        if(m_bVP == NULL){
            m_bVP = 1;
            //pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Ignor RPC enabled!");
        }else{
            m_bVP = 0;
            //pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Ignor RPC disabled!");
        } 
        break;

        case 1:
        if(m_bVP2 == NULL){
            m_bVP2 = 1;
            //pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Ignor RPC enabled!");
        }else{
            m_bVP2 = 0;
            //pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Ignor RPC disabled!");
        } 
        break;

        case 2:
        if(m_bVV == NULL){
            m_bVV = 1;
            //pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Ignor RPC enabled!");
        }else{
            m_bVV = 0;
            //pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Ignor RPC disabled!");
        } 
        break;

        case 3:
        if(m_bCO == NULL){
            m_bCO = 1;
            //pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Ignor RPC enabled!");
        }else{
            m_bCO = 0;
            //pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Ignor RPC disabled!");
        } 
        break;

        case 4:
        if(m_bPH == NULL){
            m_bPH = 1;
            //pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Ignor RPC enabled!");
        }else{
            m_bPH = 0;
            //pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Ignor RPC disabled!");
        } 
        break;
        case 5:
        if(m_bRPFV == NULL){
            m_bRPFV = 1;
            //pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Ignor RPC enabled!");
        }else{
            m_bRPFV = 0;
            //pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Ignor RPC disabled!");
        }
        break;
        case 6:
        if(m_bSD == NULL){
            m_bSD = 1;
            //pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Ignor RPC enabled!");
        }else{
            m_bSD = 0;
            //pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Ignor RPC disabled!");
        }
        break;
        case 7:
        if(m_bAPA == NULL){
            m_bAPA = 1;
            //pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Ignor RPC enabled!");
        }else{
            m_bAPA = 0;
            //pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Ignor RPC disabled!");
        }
        break;
        case 8:
        if(m_bRPW == NULL){
            m_bRPW = 1;
            //pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Ignor RPC enabled!");
        }else{
            m_bRPW = 0;
            //pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Ignor RPC disabled!");
        }
        break;
    }
}

void CModSAWindow::ToggleTeleport(int posid){
    switch(posid){
        case 0: // City Hall
            if(!pGame->FindPlayerPed()->IsInVehicle()){
                pGame->FindPlayerPed()->TeleportTo(1478.51, -1766.22, 18.80);
            }else{
                ScriptCommand(&set_car_coordinates, pGame->FindPlayerPed()->GetCurrentVehicleID(), 1478.51, -1766.22, 18.80);
            }

            //pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Teleported!");
        break;
        case 1: // Fabrica
            if(!pGame->FindPlayerPed()->IsInVehicle()){
                pGame->FindPlayerPed()->TeleportTo(-87.04, -302.43, 3.15);
            }else{
                ScriptCommand(&set_car_coordinates, pGame->FindPlayerPed()->GetCurrentVehicleID(), -87.04, -302.43, 3.15);
            }

            //pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Teleported!");
        break;
        case 2: // Ferma
            if(!pGame->FindPlayerPed()->IsInVehicle()){
                pGame->FindPlayerPed()->TeleportTo(-369.37, -1423.99, 25.73);
            }else{
                ScriptCommand(&set_car_coordinates, pGame->FindPlayerPed()->GetCurrentVehicleID(), -369.37, -1423.99, 25.73);
            }

            //pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Teleported!");
        break;
        case 3: // Vinewood Mountain
            if(!pGame->FindPlayerPed()->IsInVehicle()){
                pGame->FindPlayerPed()->TeleportTo(1422.15, -877.47, 49.67);
            }else{
                ScriptCommand(&set_car_coordinates, pGame->FindPlayerPed()->GetCurrentVehicleID(), 1422.15, -877.47, 49.67);
            }

            //pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Teleported!");
        break;
        case 4: // Mazebank roof
            if(!pGame->FindPlayerPed()->IsInVehicle()){
                pGame->FindPlayerPed()->TeleportTo(1544.82, -1352.42, 329.48);
            }else{
                ScriptCommand(&set_car_coordinates, pGame->FindPlayerPed()->GetCurrentVehicleID(), 1544.82, -1352.42, 329.48);
            }

            //pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Teleported!");
        break;
        case 5: // Railway LS
            if(!pGame->FindPlayerPed()->IsInVehicle()){
                pGame->FindPlayerPed()->TeleportTo(1764.81, -1911.92, 13.57);
            }else{
                ScriptCommand(&set_car_coordinates, pGame->FindPlayerPed()->GetCurrentVehicleID(), 1764.81, -1911.92, 13.57);
            }

            //pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Teleported!");
        break;
        case 6: // Railway LV
            if(!pGame->FindPlayerPed()->IsInVehicle()){
                pGame->FindPlayerPed()->TeleportTo(2860.13, 1290.82, 11.39);
            }else{
                ScriptCommand(&set_car_coordinates, pGame->FindPlayerPed()->GetCurrentVehicleID(), 2860.13, 1290.82, 11.39);
            }

            //pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Teleported!");
        break;
        case 7: // Railway SF
            if(!pGame->FindPlayerPed()->IsInVehicle()){
                pGame->FindPlayerPed()->TeleportTo(-1985.86, 137.66, 27.69);
            }else{
                ScriptCommand(&set_car_coordinates, pGame->FindPlayerPed()->GetCurrentVehicleID(), -1985.86, 137.66, 27.69);
            }

            //pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Teleported!");
        break;
        case 8: // Bus Station
            if(!pGame->FindPlayerPed()->IsInVehicle()){
                pGame->FindPlayerPed()->TeleportTo(1224.93, -1815.15, 16.59);
            }else{
                ScriptCommand(&set_car_coordinates, pGame->FindPlayerPed()->GetCurrentVehicleID(), 1224.93, -1815.15, 16.59);
            }

            //pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Teleported!");
        break;
        case 9: // Taxi Station
            if(!pGame->FindPlayerPed()->IsInVehicle()){
                pGame->FindPlayerPed()->TeleportTo(1108.06, -1799.94, 16.59);
            }else{
                ScriptCommand(&set_car_coordinates, pGame->FindPlayerPed()->GetCurrentVehicleID(), 1108.06, -1799.94, 16.59);
            }

            //pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Teleported!");
        break;
        case 10: // Ganton
            if(!pGame->FindPlayerPed()->IsInVehicle()){
                pGame->FindPlayerPed()->TeleportTo(2536.30, -1663.86, 15.15);
            }else{
                ScriptCommand(&set_car_coordinates, pGame->FindPlayerPed()->GetCurrentVehicleID(), 2536.30, -1663.86, 15.15);
            }

            //pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Teleported!");
        break;
        case 11: // Emmet Lane
            if(!pGame->FindPlayerPed()->IsInVehicle()){
                pGame->FindPlayerPed()->TeleportTo(2446.70, -1978.78, 13.55);
            }else{
                ScriptCommand(&set_car_coordinates, pGame->FindPlayerPed()->GetCurrentVehicleID(), 2446.70, -1978.78, 13.55);
            }

            //pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Teleported!");
        break;
        case 12: // Willowfield
            if(!pGame->FindPlayerPed()->IsInVehicle()){
                pGame->FindPlayerPed()->TeleportTo(2652.78, -2033.69, 13.15);
            }else{
                ScriptCommand(&set_car_coordinates, pGame->FindPlayerPed()->GetCurrentVehicleID(), 2652.78, -2033.69, 13.15);
            }

            //pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Teleported!");
        break;
        case 13: // Glen Park
            if(!pGame->FindPlayerPed()->IsInVehicle()){
                pGame->FindPlayerPed()->TeleportTo(1984.78, -1151.83, 21.07);
            }else{
                ScriptCommand(&set_car_coordinates, pGame->FindPlayerPed()->GetCurrentVehicleID(), 1984.78, -1151.83, 21.07);
            }

            //pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Teleported!");
        break;
        case 14: // LSPD
            if(!pGame->FindPlayerPed()->IsInVehicle()){
                pGame->FindPlayerPed()->TeleportTo(1550.44, -1669.59, 13.56);
            }else{
                ScriptCommand(&set_car_coordinates, pGame->FindPlayerPed()->GetCurrentVehicleID(), 1550.44, -1669.59, 13.56);
            }

            //pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Teleported!");
        break;
        case 15: // LVPD
            if(!pGame->FindPlayerPed()->IsInVehicle()){
                pGame->FindPlayerPed()->TeleportTo(2308.01, 2448.11, 10.82);
            }else{
                ScriptCommand(&set_car_coordinates, pGame->FindPlayerPed()->GetCurrentVehicleID(), 2308.01, 2448.11, 10.82);
            }

            //pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Teleported!");
        break;
        case 16: // SFPD
            if(!pGame->FindPlayerPed()->IsInVehicle()){
                pGame->FindPlayerPed()->TeleportTo(-1598.05, 714.07, 13.10);
            }else{
                ScriptCommand(&set_car_coordinates, pGame->FindPlayerPed()->GetCurrentVehicleID(), -1598.05, 714.07, 13.10);
            }

            //pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Teleported!");
        break;
        case 17: // Driving School
            if(!pGame->FindPlayerPed()->IsInVehicle()){
                pGame->FindPlayerPed()->TeleportTo(-2025.56, -92.01, 35.32);
            }else{
                ScriptCommand(&set_car_coordinates, pGame->FindPlayerPed()->GetCurrentVehicleID(), -2025.56, -92.01, 35.32);
            }

            //pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Teleported!");
        break;
        case 18: // LS Car Shop
            if(!pGame->FindPlayerPed()->IsInVehicle()){
                pGame->FindPlayerPed()->TeleportTo(554.46, -1269.06, 17.24);
            }else{
                ScriptCommand(&set_car_coordinates, pGame->FindPlayerPed()->GetCurrentVehicleID(), 554.46, -1269.06, 17.24);
            }

            //pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Teleported!");
        break;
        case 19: // LV Car Shop
            if(!pGame->FindPlayerPed()->IsInVehicle()){
                pGame->FindPlayerPed()->TeleportTo(2198.83, 1391.02, 10.82);
            }else{
                ScriptCommand(&set_car_coordinates, pGame->FindPlayerPed()->GetCurrentVehicleID(), 2198.83, 1391.02, 10.82);
            }

            //pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Teleported!");
        break;
        case 20: // SF Car Shop
            if(!pGame->FindPlayerPed()->IsInVehicle()){
                pGame->FindPlayerPed()->TeleportTo(-1974.41, 288.67, 35.17);
            }else{
                ScriptCommand(&set_car_coordinates, pGame->FindPlayerPed()->GetCurrentVehicleID(), -1974.41, 288.67, 35.17);
            }

            //pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Teleported!");
        break;
        case 21: // LS Motorcycle Shop
            if(!pGame->FindPlayerPed()->IsInVehicle()){
                pGame->FindPlayerPed()->TeleportTo(2133.05, -1135.37, 25.69);
            }else{
                ScriptCommand(&set_car_coordinates, pGame->FindPlayerPed()->GetCurrentVehicleID(), 2133.05, -1135.37, 25.69);
            }

            //pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Teleported!");
        break;
        case 22: // Vagos #1 (on the beach)
            if(!pGame->FindPlayerPed()->IsInVehicle()){
                pGame->FindPlayerPed()->TeleportTo(2771.16, -1624.63, 10.93);
            }else{
                ScriptCommand(&set_car_coordinates, pGame->FindPlayerPed()->GetCurrentVehicleID(), 2771.16, -1624.63, 10.93);
            }

            //pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Teleported!");
        break;
        case 23: // Vagos #2 (on the mountain)
            if(!pGame->FindPlayerPed()->IsInVehicle()){
                pGame->FindPlayerPed()->TeleportTo(2756.51, -1181.45, 69.40);
            }else{
                ScriptCommand(&set_car_coordinates, pGame->FindPlayerPed()->GetCurrentVehicleID(), 2756.51, -1181.45, 69.40);
            }

            //pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Teleported!");
        break;
        case 24: // Rifa
            if(!pGame->FindPlayerPed()->IsInVehicle()){
                pGame->FindPlayerPed()->TeleportTo(2184.85, -1810.15, 13.38);
            }else{
                ScriptCommand(&set_car_coordinates, pGame->FindPlayerPed()->GetCurrentVehicleID(), 2184.85, -1810.15, 13.38);
            }

            //pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Teleported!");
        break;
        case 25: // Aztecs
            if(!pGame->FindPlayerPed()->IsInVehicle()){
                pGame->FindPlayerPed()->TeleportTo(1665.32, -2109.61, 13.55);
            }else{
                ScriptCommand(&set_car_coordinates, pGame->FindPlayerPed()->GetCurrentVehicleID(), 1665.32, -2109.61, 13.55);
            }

            //pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Teleported!");
        break;
        case 26: // Hospital LS
            if(!pGame->FindPlayerPed()->IsInVehicle()){
                pGame->FindPlayerPed()->TeleportTo(1183.75, -1323.58, 13.58);
            }else{
                ScriptCommand(&set_car_coordinates, pGame->FindPlayerPed()->GetCurrentVehicleID(), 1183.75, -1323.58, 13.58);
            }

            //pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Teleported!");
        break;
        case 27: // Hospital LV
            if(!pGame->FindPlayerPed()->IsInVehicle()){
                pGame->FindPlayerPed()->TeleportTo(1607.87, 1831.97, 10.82);
            }else{
                ScriptCommand(&set_car_coordinates, pGame->FindPlayerPed()->GetCurrentVehicleID(), 1607.87, 1831.97, 10.82);
            }

            //pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Teleported!");
        break;
        case 28: // Hospital SF
            if(!pGame->FindPlayerPed()->IsInVehicle()){
                pGame->FindPlayerPed()->TeleportTo(-2653.50, 631.28, 14.45);
            }else{
                ScriptCommand(&set_car_coordinates, pGame->FindPlayerPed()->GetCurrentVehicleID(), -2653.50, 631.28, 14.45);
            }

            //pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Teleported!");
        break;
        case 29: // FBI
            if(!pGame->FindPlayerPed()->IsInVehicle()){
                pGame->FindPlayerPed()->TeleportTo(-2450.51, 503.69, 30.09);
            }else{
                ScriptCommand(&set_car_coordinates, pGame->FindPlayerPed()->GetCurrentVehicleID(), -2450.51, 503.69, 30.09);
            }

            //pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Teleported!");
        break;
        case 30: // Mine
            if(!pGame->FindPlayerPed()->IsInVehicle()){
                pGame->FindPlayerPed()->TeleportTo(-1858.78, -1570.18, 21.75);
            }else{
                ScriptCommand(&set_car_coordinates, pGame->FindPlayerPed()->GetCurrentVehicleID(), -1858.78, -1570.18, 21.75);
            }

            //pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Teleported!");
        break;
        case 31: // Chiliad Mountain
            if(!pGame->FindPlayerPed()->IsInVehicle()){
                pGame->FindPlayerPed()->TeleportTo(-2234.46, -1735.08, 480.83);
            }else{
                ScriptCommand(&set_car_coordinates, pGame->FindPlayerPed()->GetCurrentVehicleID(), -2234.46, -1735.08, 480.83);
            }

            //pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Teleported!");
        break;
        case 32: // Airport
            if(!pGame->FindPlayerPed()->IsInVehicle()){
                pGame->FindPlayerPed()->TeleportTo(1682.15, -2264.46, 13.51);
            }else{
                ScriptCommand(&set_car_coordinates, pGame->FindPlayerPed()->GetCurrentVehicleID(), 1682.15, -2264.46, 13.51);
            }

            //pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Teleported!");
        break;
        case 33: // Stock
            if(!pGame->FindPlayerPed()->IsInVehicle()){
                pGame->FindPlayerPed()->TeleportTo(2115.66, -2274.92, 20.68);
            }else{
                ScriptCommand(&set_car_coordinates, pGame->FindPlayerPed()->GetCurrentVehicleID(), 2115.66, -2274.92, 20.68);
            }

            //pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Teleported!");
        break;
        case 34: // Railway LS #2
            if(!pGame->FindPlayerPed()->IsInVehicle()){
                pGame->FindPlayerPed()->TeleportTo(1979.31, -1968.83, 16.69);
            }else{
                ScriptCommand(&set_car_coordinates, pGame->FindPlayerPed()->GetCurrentVehicleID(), 1979.31, -1968.83, 16.69);
            }

            //pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Teleported!");
        break;
        case 35: // 24/7 Shop
            if(!pGame->FindPlayerPed()->IsInVehicle()){
                pGame->FindPlayerPed()->TeleportTo(1828.29, -1844.15, 13.58);
            }else{
                ScriptCommand(&set_car_coordinates, pGame->FindPlayerPed()->GetCurrentVehicleID(), 1828.29, -1844.15, 13.58);
            }

            //pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Teleported!");
        break;
        case 36: // Lighthouse
            if(!pGame->FindPlayerPed()->IsInVehicle()){
                pGame->FindPlayerPed()->TeleportTo(155.16, -1939.62, 3.77);
            }else{
                ScriptCommand(&set_car_coordinates, pGame->FindPlayerPed()->GetCurrentVehicleID(), 155.16, -1939.62, 3.77);
            }

            //pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Teleported!");
        break;
        case 37: // Ferris Wheel
            if(!pGame->FindPlayerPed()->IsInVehicle()){
                pGame->FindPlayerPed()->TeleportTo(377.73, -2027.40, 7.83);
            }else{
                ScriptCommand(&set_car_coordinates, pGame->FindPlayerPed()->GetCurrentVehicleID(), 377.73, -2027.40, 7.83);
            }

            //pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Teleported!");
        break;
        case 38: // Gas Station
            if(!pGame->FindPlayerPed()->IsInVehicle()){
                pGame->FindPlayerPed()->TeleportTo(1938.90, -1775.52, 13.38);
            }else{
                ScriptCommand(&set_car_coordinates, pGame->FindPlayerPed()->GetCurrentVehicleID(), 1938.90, -1775.52, 13.38);
            }

            //pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Teleported!");
        break;
        case 39: // Pizzeria
            if(!pGame->FindPlayerPed()->IsInVehicle()){
                pGame->FindPlayerPed()->TeleportTo(2094.95, -1807.60, 13.55);
            }else{
                ScriptCommand(&set_car_coordinates, pGame->FindPlayerPed()->GetCurrentVehicleID(), 2094.95, -1807.60, 13.55);
            }

            //pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Teleported!");
        break;
        case 40: // Pay N Spray
            if(!pGame->FindPlayerPed()->IsInVehicle()){
                pGame->FindPlayerPed()->TeleportTo(2078.11, -1831.03, 13.38);
            }else{
                ScriptCommand(&set_car_coordinates, pGame->FindPlayerPed()->GetCurrentVehicleID(), 2078.11, -1831.03, 13.38);
            }

            //pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Teleported!");
        break;
        case 41: // Bank LS
            if(!pGame->FindPlayerPed()->IsInVehicle()){
                pGame->FindPlayerPed()->TeleportTo(1413.92, -1701.24, 13.54);
            }else{
                ScriptCommand(&set_car_coordinates, pGame->FindPlayerPed()->GetCurrentVehicleID(), 1413.92, -1701.24, 13.54);
            }

            //pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Teleported!");
        break;
        case 42: // Ammunation
            if(!pGame->FindPlayerPed()->IsInVehicle()){
                pGame->FindPlayerPed()->TeleportTo(1364.24, -1279.50, 13.55);
            }else{
                ScriptCommand(&set_car_coordinates, pGame->FindPlayerPed()->GetCurrentVehicleID(), 1364.24, -1279.50, 13.55);
            }

            //pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Teleported!");
        break;
        case 43: // Skate Park
            if(!pGame->FindPlayerPed()->IsInVehicle()){
                pGame->FindPlayerPed()->TeleportTo(1938.28, -1401.67, 13.57);
            }else{
                ScriptCommand(&set_car_coordinates, pGame->FindPlayerPed()->GetCurrentVehicleID(), 1938.28, -1401.67, 13.57);
            }

            //pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Teleported!");
        break;
        case 44: // Hotel #1
            if(!pGame->FindPlayerPed()->IsInVehicle()){
                pGame->FindPlayerPed()->TeleportTo(332.14, -1519.99, 35.87);
            }else{
                ScriptCommand(&set_car_coordinates, pGame->FindPlayerPed()->GetCurrentVehicleID(), 332.14, -1519.99, 35.87);
            }

            //pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Teleported!");
        break;
        case 45: // Hotel #2
            if(!pGame->FindPlayerPed()->IsInVehicle()){
                pGame->FindPlayerPed()->TeleportTo(2226.11, -1159.69, 25.77);
            }else{
                ScriptCommand(&set_car_coordinates, pGame->FindPlayerPed()->GetCurrentVehicleID(), 2226.11, -1159.69, 25.77);
            }

            //pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Teleported!");
        break;
        case 46: // Docks
            if(!pGame->FindPlayerPed()->IsInVehicle()){
                pGame->FindPlayerPed()->TeleportTo(2787.66, -2400.03, 13.63);
            }else{
                ScriptCommand(&set_car_coordinates, pGame->FindPlayerPed()->GetCurrentVehicleID(), 2787.66, -2400.03, 13.63);
            }

            //pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Teleported!");
        break;
        case 47: // Dump
            if(!pGame->FindPlayerPed()->IsInVehicle()){
                pGame->FindPlayerPed()->TeleportTo(2197.49, -1976.91, 13.56);
            }else{
                ScriptCommand(&set_car_coordinates, pGame->FindPlayerPed()->GetCurrentVehicleID(), 2197.49, -1976.91, 13.56);
            }

            //pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Teleported!");
        break;
        case 48: // Zone 51 (enter)
            if(!pGame->FindPlayerPed()->IsInVehicle()){
                pGame->FindPlayerPed()->TeleportTo(213.71, 1879.46, 13.14);
            }else{
                ScriptCommand(&set_car_coordinates, pGame->FindPlayerPed()->GetCurrentVehicleID(), 213.71, 1879.46, 13.14);
            }

            //pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Teleported!");
        break;
        case 49: // Zone 51 (angars)
            if(!pGame->FindPlayerPed()->IsInVehicle()){
                pGame->FindPlayerPed()->TeleportTo(292.31, 1656.36, 17.64);
            }else{
                ScriptCommand(&set_car_coordinates, pGame->FindPlayerPed()->GetCurrentVehicleID(), 292.31, 1656.36, 17.64);
            }

            //pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Teleported!");
        break;
        case 50: // ECDD
            if(!pGame->FindPlayerPed()->IsInVehicle()){
                pGame->FindPlayerPed()->TeleportTo(-385.08, 2237.69, 42.09);
            }else{
                ScriptCommand(&set_car_coordinates, pGame->FindPlayerPed()->GetCurrentVehicleID(), -385.08, 2237.69, 42.09);
            }

            //pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Teleported!");
        break;
        case 51: // Mafia #1 LV
            if(!pGame->FindPlayerPed()->IsInVehicle()){
                pGame->FindPlayerPed()->TeleportTo(1430.80, 753.25, 10.82);
            }else{
                ScriptCommand(&set_car_coordinates, pGame->FindPlayerPed()->GetCurrentVehicleID(), 1430.80, 753.25, 10.82);
            }

            //pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Teleported!");
        break;
        case 52: // Mafia #2 LV
            if(!pGame->FindPlayerPed()->IsInVehicle()){
                pGame->FindPlayerPed()->TeleportTo(2608.07, 1834.11, 10.82);
            }else{
                ScriptCommand(&set_car_coordinates, pGame->FindPlayerPed()->GetCurrentVehicleID(), 2608.07, 1834.11, 10.82);
            }

            //pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Teleported!");
        break;
        case 53: // Russian Mafia
            if(!pGame->FindPlayerPed()->IsInVehicle()){
                pGame->FindPlayerPed()->TeleportTo(947.80, 1733.72, 8.85);
            }else{
                ScriptCommand(&set_car_coordinates, pGame->FindPlayerPed()->GetCurrentVehicleID(), 947.80, 1733.72, 8.85);
            }

            //pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Teleported!");
        break;
        case 54: // Mafia #3 SF
            if(!pGame->FindPlayerPed()->IsInVehicle()){
                pGame->FindPlayerPed()->TeleportTo(-2726.54, -314.45, 7.19);
            }else{
                ScriptCommand(&set_car_coordinates, pGame->FindPlayerPed()->GetCurrentVehicleID(), -2726.54, -314.45, 7.19);
            }

            //pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Teleported!");
        break;
        case 55: // Alhambra
            if(!pGame->FindPlayerPed()->IsInVehicle()){
                pGame->FindPlayerPed()->TeleportTo(1831.02, -1682.09, 13.55);
            }else{
                ScriptCommand(&set_car_coordinates, pGame->FindPlayerPed()->GetCurrentVehicleID(), 1831.02, -1682.09, 13.55);
            }

            //pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Teleported!");
        break;
        case 69:
            if(!pGame->FindPlayerPed()->IsInVehicle()){
                pGame->FindPlayerPed()->TeleportTo(marker_X, marker_Y, marker_Z);
            }else{
                ScriptCommand(&set_car_coordinates, pGame->FindPlayerPed()->GetCurrentVehicleID(), marker_X, marker_Y, marker_Z);
            }

            //pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Teleported!");
        break;
    }
}

void CModSAWindow::Render()
{
	if(!m_bIsActive || m_bMenuStep == 0) return;
    if(protect == 1)return;

	ImGuiIO &io = ImGui::GetIO();

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));

    ImGui::Begin("m0d_sa s0beit v0.0.0.1 by QDS Team", nullptr, 
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);
    switch(m_bMenuStep){
        case 2:
        if(m_bWallHack == 1){
            if(ImGui::Button("(O) Wall Hack", ImVec2(200, 50)))
            {
                ToggleCheat(2);
                Show(false);
            }
        }else{
            if(ImGui::Button("Wall Hack", ImVec2(200, 50)))
            {
                ToggleCheat(2);
                Show(false);
            }
        } 

        if(m_bNF == 1){
            if(ImGui::Button("(O) No Fall", ImVec2(200, 50)))
            {
                ToggleCheat(5);
                Show(false);
            }
        }else{
            if(ImGui::Button("No Fall", ImVec2(200, 50)))
            {
                ToggleCheat(5);
                Show(false);
            }
        }

        if(m_bGodMode == 1){
            if(ImGui::Button("(O) God Mode", ImVec2(200, 50)))
            {
                ToggleCheat(1);
                Show(false);
            }
        }else{
            if(ImGui::Button("God Mode", ImVec2(200, 50)))
            {
                ToggleCheat(1);
                Show(false);
            }
        }

        // TODO : if(ImGui::Button("Air Break", ImVec2(280, 50)))
        // TODO : {
        // TODO :     Show(false);
        // TODO : }

        if(m_bCols == 1){
            if(ImGui::Button("(O) Freeze", ImVec2(200, 50)))
            {
                ToggleCheat(3);
                Show(false);
            }
        }else{
            if(ImGui::Button("Freeze", ImVec2(200, 50)))
            {
                ToggleCheat(3);
                Show(false);
            }
        }

        if(ImGui::Button("Fly Hack", ImVec2(200, 50)))
        {
            m_bKrutilka = 1;
            m_bMenuStep = 11;
        }

        if(m_bKrutilka == 1){
            if(ImGui::Button("(O) Behind", ImVec2(200, 50)))
            {
                ToggleCheat(4);
                Show(false);
            }
        }else{
            if(ImGui::Button("Behind", ImVec2(200, 50)))
            {
                ToggleCheat(4);
                Show(false);
            }
        }

        if(ImGui::Button("Gravity", ImVec2(200, 50)))
        {
            m_bMenuStep = 4;
        }

        if(ImGui::Button("Close", ImVec2(200, 50)))
        {
            Show(false);
        }
        break;
        case 1:
        if(ImGui::Button("Cheats", ImVec2(280, 50)))
        {
            m_bMenuStep = 2;
        }

        // TODO : if(ImGui::Button("Danger Cheats", ImVec2(280, 50)))
        // TODO : {
        // TODO :     m_bMenuStep = 6;
        // TODO : }

        if(ImGui::Button("Weapons", ImVec2(280, 50)))
        {
            m_bMenuStep = 5;
        }

        // TODO : if(ImGui::Button("Vehicles", ImVec2(280, 50)))
        // TODO : {
        // TODO :     Show(false);
        // TODO : }

        if(ImGui::Button("Teleports", ImVec2(280, 50)))
        {
            m_bMenuStep = 7;
        }

        if(ImGui::Button("Settings", ImVec2(280, 50)))
        {
            m_bMenuStep = 3;
        }

        // TODO : if(ImGui::Button("GTA Patches", ImVec2(280, 50)))
        // TODO : {
        // TODO :     Show(false);
        // TODO : }

        if(ImGui::Button("SA-MP Patches", ImVec2(280, 50)))
        {
            m_bMenuStep = 8;
        }

        if(ImGui::Button("Close", ImVec2(280, 50)))
        {
            Show(false);
        }
        break;
        case 3:
        if(ImGui::Button("Clear chat", ImVec2(280, 50)))
        {
            for(int i = 0; i <= 50; i++)pChatWindow->AddInfoMessage(" ");
            Show(false);
        }

        // TODO : if(ImGui::Button("Disable nicknames", ImVec2(280, 50)))
        // TODO : {
        // TODO :     Show(false);
        // TODO : }

        if(lockinp == 1){
            if(ImGui::Button("(O) Lock chat input", ImVec2(280, 50)))
            {
                if(lockinp == 0){
                    lockinp = 1;
                    pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Chat input lock is enabled!");
                }else if(lockinp == 1){
                    lockinp = 0;
                    pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Chat input lock is disabled!");
                }
                Show(false);
            }
        }else{
            if(ImGui::Button("Lock chat input", ImVec2(280, 50)))
            {
                if(lockinp == 0){
                    lockinp = 1;
                    pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Chat input lock is enabled!");
                }else if(lockinp == 1){
                    lockinp = 0;
                    pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Chat input lock is disabled!");
                }
                Show(false);
            }
        }

        if(ImGui::Button("Close", ImVec2(280, 50)))
        {
            Show(false);
        }
        break;

        case 4:
        if(ImGui::Button("- Gravity", ImVec2(280, 50)))
        {
           pGame->SetGravity(pGame->GetGravity() - 0.001);
            //pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Set gravity to %f", pGame->GetGravity());
            char str[4096];
            sprintf(str, "~n~~n~~n~~n~~n~~n~Gravity: %f", pGame->GetGravity());
            pGame->DisplayGameText(str, 5000, 3);
        }

        if(ImGui::Button("+ Gravity", ImVec2(280, 50)))
        {
            pGame->SetGravity(pGame->GetGravity() + 0.001);

            //pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Set gravity to %f", pGame->GetGravity());
            char str[4096];
            sprintf(str, "~n~~n~~n~~n~~n~~n~Gravity: %f", pGame->GetGravity());
            pGame->DisplayGameText(str, 5000, 3);
            
        }

        if(ImGui::Button("Restore Gravity", ImVec2(280, 50)))
        {
            pGame->SetGravity(0.008);
            //pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Set gravity to %f", pGame->GetGravity());
            char str[4096];
            sprintf(str, "~n~~n~~n~~n~~n~~n~Gravity: %f", pGame->GetGravity());
            pGame->DisplayGameText(str, 5000, 3);
        }

        if(ImGui::Button("Close", ImVec2(280, 50)))
        {
            Show(false);
        }
        break;

        case 5:
        if(ImGui::Button("Brass Knuckles", ImVec2(280, 50)))
        {
            pGame->FindPlayerPed()->GiveWeapon(1, 9999);
            //pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Gived weapon id 1");
            Show(false);
        }

        if(ImGui::Button("Golf Club", ImVec2(280, 50)))
        {
            pGame->FindPlayerPed()->GiveWeapon(2, 9999);
            //pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Gived weapon id 2");
            Show(false);
        }

        if(ImGui::Button("Baton", ImVec2(280, 50)))
        {
            pGame->FindPlayerPed()->GiveWeapon(3, 9999);
            //pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Gived weapon id 3");
            Show(false);
        }

        if(ImGui::Button("Knife", ImVec2(280, 50)))
        {
            pGame->FindPlayerPed()->GiveWeapon(4, 9999);
            //pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Gived weapon id 4");
            Show(false);
        }

        if(ImGui::Button("Baseball Bat", ImVec2(280, 50)))
        {
            pGame->FindPlayerPed()->GiveWeapon(5, 9999);
            //pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Gived weapon id 5");
            Show(false);
        }

        if(ImGui::Button("Shovel", ImVec2(280, 50)))
        {
            pGame->FindPlayerPed()->GiveWeapon(6, 9999);
            //pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Gived weapon id 6");
            Show(false);
        }

        if(ImGui::Button("Pool Cue", ImVec2(280, 50)))
        {
            pGame->FindPlayerPed()->GiveWeapon(7, 9999);
            //pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Gived weapon id 7");
            Show(false);
        }

        if(ImGui::Button("Katana", ImVec2(280, 50)))
        {
            pGame->FindPlayerPed()->GiveWeapon(8, 9999);
            //pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Gived weapon id 8");
            Show(false);
        }

        if(ImGui::Button("Chainsaw", ImVec2(280, 50)))
        {
            pGame->FindPlayerPed()->GiveWeapon(9, 9999);
            //pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Gived weapon id 9");
            Show(false);
        }

        if(ImGui::Button("Purple Dildo", ImVec2(280, 50)))
        {
            pGame->FindPlayerPed()->GiveWeapon(10, 9999);
            //pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Gived weapon id 10");
            Show(false);
        }

        if(ImGui::Button("Dildo", ImVec2(280, 50)))
        {
            pGame->FindPlayerPed()->GiveWeapon(11, 9999);
            //pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Gived weapon id 11");
            Show(false);
        }

        if(ImGui::Button("Vibrator", ImVec2(280, 50)))
        {
            pGame->FindPlayerPed()->GiveWeapon(12, 9999);
            //pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Gived weapon id 12");
            Show(false);
        }

        if(ImGui::Button("Silver Vibrator", ImVec2(280, 50)))
        {
            pGame->FindPlayerPed()->GiveWeapon(13, 9999);
            //pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Gived weapon id 13");
            Show(false);
        }

        if(ImGui::Button("Flowers", ImVec2(280, 50)))
        {
            pGame->FindPlayerPed()->GiveWeapon(14, 9999);
            //pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Gived weapon id 14");
            Show(false);
        }

        if(ImGui::Button("Cane", ImVec2(280, 50)))
        {
            pGame->FindPlayerPed()->GiveWeapon(15, 9999);
            //pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Gived weapon id 15");
            Show(false);
        }

        if(ImGui::Button("Grenade", ImVec2(280, 50)))
        {
            pGame->FindPlayerPed()->GiveWeapon(16, 9999);
            //pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Gived weapon id 16");
            Show(false);
        }

        if(ImGui::Button("Tear Gas", ImVec2(280, 50)))
        {
            pGame->FindPlayerPed()->GiveWeapon(17, 9999);
            //pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Gived weapon id 17");
            Show(false);
        }

        if(ImGui::Button("Molotov Cocktail", ImVec2(280, 50)))
        {
            pGame->FindPlayerPed()->GiveWeapon(18, 9999);
            //pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Gived weapon id 18");
            Show(false);
        }
        
        if(ImGui::Button("9mm", ImVec2(280, 50)))
        {
            pGame->FindPlayerPed()->GiveWeapon(22, 9999);
            //pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Gived weapon id 22");
            Show(false);
        }

        if(ImGui::Button("Silenced 9mm", ImVec2(280, 50)))
        {
            pGame->FindPlayerPed()->GiveWeapon(23, 9999);
            //pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Gived weapon id 23");
            Show(false);
        }

        if(ImGui::Button("Desert Eagle", ImVec2(280, 50)))
        {
            pGame->FindPlayerPed()->GiveWeapon(24, 9999);
            //pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Gived weapon id 24");
            Show(false);
        }

        if(ImGui::Button("Shotgun", ImVec2(280, 50)))
        {
            pGame->FindPlayerPed()->GiveWeapon(25, 9999);
            //pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Gived weapon id 25");
            Show(false);
        }

        if(ImGui::Button("Sawnoff Shotgun", ImVec2(280, 50)))
        {
            pGame->FindPlayerPed()->GiveWeapon(26, 9999);
            //pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Gived weapon id 26");
            Show(false);
        }

        if(ImGui::Button("Combat Shotgun", ImVec2(280, 50)))
        {
            pGame->FindPlayerPed()->GiveWeapon(27, 9999);
            //pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Gived weapon id 27");
            Show(false);
        }

        if(ImGui::Button("UZI", ImVec2(280, 50)))
        {
            pGame->FindPlayerPed()->GiveWeapon(28, 9999);
            //pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Gived weapon id 28");
            Show(false);
        }

        if(ImGui::Button("MP5", ImVec2(280, 50)))
        {
            pGame->FindPlayerPed()->GiveWeapon(29, 9999);
            //pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Gived weapon id 29");
            Show(false);
        }

        if(ImGui::Button("AK-47", ImVec2(280, 50)))
        {
            pGame->FindPlayerPed()->GiveWeapon(30, 9999);
            //pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Gived weapon id 30");
            Show(false);
        }

        if(ImGui::Button("M4", ImVec2(280, 50)))
        {
            pGame->FindPlayerPed()->GiveWeapon(31, 9999);
            //pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Gived weapon id 31");
            Show(false);
        }

        if(ImGui::Button("TEC-9", ImVec2(280, 50)))
        {
            pGame->FindPlayerPed()->GiveWeapon(32, 9999);
            //pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Gived weapon id 32");
            Show(false);
        }

        if(ImGui::Button("Country Rifle", ImVec2(280, 50)))
        {
            pGame->FindPlayerPed()->GiveWeapon(33, 9999);
            //pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Gived weapon id 33");
            Show(false);
        }
        
        if(ImGui::Button("Sniper Rifle", ImVec2(280, 50)))
        {
            pGame->FindPlayerPed()->GiveWeapon(34, 9999);
            //pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Gived weapon id 34");
            Show(false);
        }

        if(ImGui::Button("RPG", ImVec2(280, 50)))
        {
            pGame->FindPlayerPed()->GiveWeapon(35, 9999);
            //pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Gived weapon id 35");
            Show(false);
        }

        if(ImGui::Button("HS Rocket", ImVec2(280, 50)))
        {
            pGame->FindPlayerPed()->GiveWeapon(36, 9999);
            //pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Gived weapon id 36");
            Show(false);
        }

        if(ImGui::Button("Flamethrower", ImVec2(280, 50)))
        {
            pGame->FindPlayerPed()->GiveWeapon(37, 9999);
            //pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Gived weapon id 37");
            Show(false);
        }

        if(ImGui::Button("Minigun", ImVec2(280, 50)))
        {
            pGame->FindPlayerPed()->GiveWeapon(38, 9999);
            //pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Gived weapon id 38");
            Show(false);
        }

        if(ImGui::Button("C-4", ImVec2(280, 50)))
        {
            pGame->FindPlayerPed()->GiveWeapon(39, 9999);
            //pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Gived weapon id 39");
            Show(false);
        }

        if(ImGui::Button("Detonator", ImVec2(280, 50)))
        {
            pGame->FindPlayerPed()->GiveWeapon(40, 9999);
            //pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Gived weapon id 40");
            Show(false);
        }

        if(ImGui::Button("Spraycan", ImVec2(280, 50)))
        {
            pGame->FindPlayerPed()->GiveWeapon(41, 9999);
            //pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Gived weapon id 41");
            Show(false);
        }

        if(ImGui::Button("Fire Extinguisher", ImVec2(280, 50)))
        {
            pGame->FindPlayerPed()->GiveWeapon(42, 9999);
            //pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Gived weapon id 42");
            Show(false);
        }

        if(ImGui::Button("Camera", ImVec2(280, 50)))
        {
            pGame->FindPlayerPed()->GiveWeapon(43, 9999);
            //pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Gived weapon id 43");
            Show(false);
        }

        if(ImGui::Button("Night Vis Goggles", ImVec2(280, 50)))
        {
            pGame->FindPlayerPed()->GiveWeapon(44, 9999);
            //pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Gived weapon id 44");
            Show(false);
        }

        if(ImGui::Button("Thermal Goggles", ImVec2(280, 50)))
        {
            pGame->FindPlayerPed()->GiveWeapon(45, 9999);
            //pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Gived weapon id 45");
            Show(false);
        }

        if(ImGui::Button("Parachute", ImVec2(280, 50)))
        {
            pGame->FindPlayerPed()->GiveWeapon(46, 9999);
            //pChatWindow->AddInfoMessage("{F6D200}[{F60000}INFO{F6D200}] {FFFFFF}Gived weapon id 46");
            Show(false);
        }

        if(ImGui::Button("Close", ImVec2(280, 50)))
        {
            Show(false);
        }
        break;

        case 6:
        // TODO : if(ImGui::Button("Damager", ImVec2(280, 50)))
        // TODO : {
        // TODO :     Show(false);
        // TODO : }
// TODO : 
        // TODO : if(ImGui::Button("Crasher", ImVec2(280, 50)))
        // TODO : {
        // TODO :     Show(false);
        // TODO : }
// TODO : 
        // TODO : if(ImGui::Button("Rvanka (onfoot)", ImVec2(280, 50)))
        // TODO : {
        // TODO :     Show(false);
        // TODO : }
// TODO : 
        // TODO : if(ImGui::Button("Rvanka (onvehicle)", ImVec2(280, 50)))
        // TODO : {
        // TODO :     Show(false);
        // TODO : }
// TODO : 
        // TODO : if(ImGui::Button("Car Shot", ImVec2(280, 50)))
        // TODO : {
        // TODO :     Show(false);
        // TODO : }
// TODO : 
        // TODO : if(ImGui::Button("Car Anarchy", ImVec2(280, 50)))
        // TODO : {
        // TODO :     Show(false);
        // TODO : }
// TODO : 
        // TODO : if(ImGui::Button("Car Fault", ImVec2(280, 50)))
        // TODO : {
        // TODO :     Show(false);
        // TODO : }
// TODO : 
        // TODO : if(ImGui::Button("Underground Explosion", ImVec2(280, 50)))
        // TODO : {
        // TODO :     Show(false);
        // TODO : }

        if(ImGui::Button("Close", ImVec2(280, 50)))
        {
            Show(false);
        }

        break;
        case 7:
        if(ImGui::Button("Маркер", ImVec2(280, 50)))
        {
            ToggleTeleport(69);
            Show(false);
        }

        if(ImGui::Button("Мэрия ЛС", ImVec2(280, 50)))
        {
            ToggleTeleport(0);
            Show(false);
        }
        
        if(ImGui::Button("Завод", ImVec2(280, 50)))
        {
            ToggleTeleport(1);
            Show(false);
        }

        if(ImGui::Button("Ферма", ImVec2(280, 50)))
        {
            ToggleTeleport(2);
            Show(false);
        }

        if(ImGui::Button("Гора Vinewood", ImVec2(280, 50)))
        {
            ToggleTeleport(3);
            Show(false);
        }

        if(ImGui::Button("Небоскрёб", ImVec2(280, 50)))
        {
            ToggleTeleport(4);
            Show(false);
        }

        if(ImGui::Button("ЖД ЛС", ImVec2(280, 50)))
        {
            ToggleTeleport(5);
            Show(false);
        }

        if(ImGui::Button("ЖД ЛВ", ImVec2(280, 50)))
        {
            ToggleTeleport(6);
            Show(false);
        }

        if(ImGui::Button("ЖД СФ", ImVec2(280, 50)))
        {
            ToggleTeleport(7);
            Show(false);
        }

        if(ImGui::Button("Автобусная стоянка", ImVec2(280, 50)))
        {
            ToggleTeleport(8);
            Show(false);
        }

        if(ImGui::Button("Таксопарк", ImVec2(280, 50)))
        {
            ToggleTeleport(9);
            Show(false);
        }

        if(ImGui::Button("Улица Грув", ImVec2(280, 50)))
        {
            ToggleTeleport(10);
            Show(false);
        }

        if(ImGui::Button("Переулок Эммета", ImVec2(280, 50)))
        {
            ToggleTeleport(11);
            Show(false);
        }

        if(ImGui::Button("Willowfield", ImVec2(280, 50)))
        {
            ToggleTeleport(12);
            Show(false);
        }

        if(ImGui::Button("Глен Парк", ImVec2(280, 50)))
        {
            ToggleTeleport(13);
            Show(false);
        }

        if(ImGui::Button("LSPD", ImVec2(280, 50)))
        {
            ToggleTeleport(14);
            Show(false);
        }

        if(ImGui::Button("LVPD", ImVec2(280, 50)))
        {
            ToggleTeleport(15);
            Show(false);
        }

        if(ImGui::Button("SFPD", ImVec2(280, 50)))
        {
            ToggleTeleport(16);
            Show(false);
        }

        if(ImGui::Button("Автошкола", ImVec2(280, 50)))
        {
            ToggleTeleport(17);
            Show(false);
        }

        if(ImGui::Button("Автосалон ЛС", ImVec2(280, 50)))
        {
            ToggleTeleport(18);
            Show(false);
        }

        if(ImGui::Button("Автосалон ЛВ", ImVec2(280, 50)))
        {
            ToggleTeleport(19);
            Show(false);
        }

        if(ImGui::Button("Автосалон СФ", ImVec2(280, 50)))
        {
            ToggleTeleport(20);
            Show(false);
        }

        if(ImGui::Button("Мотосалон ЛС", ImVec2(280, 50)))
        {
            ToggleTeleport(21);
            Show(false);
        }

        if(ImGui::Button("Vagos #1", ImVec2(280, 50)))
        {
            ToggleTeleport(22);
            Show(false);
        }

        if(ImGui::Button("Vagos #2", ImVec2(280, 50)))
        {
            ToggleTeleport(23);
            Show(false);
        }

        if(ImGui::Button("Rifa", ImVec2(280, 50)))
        {
            ToggleTeleport(24);
            Show(false);
        }

        if(ImGui::Button("Aztecas", ImVec2(280, 50)))
        {
            ToggleTeleport(25);
            Show(false);
        }

        if(ImGui::Button("Больница ЛС", ImVec2(280, 50)))
        {
            ToggleTeleport(26);
            Show(false);
        }

        if(ImGui::Button("Больница ЛВ", ImVec2(280, 50)))
        {
            ToggleTeleport(27);
            Show(false);
        }

        if(ImGui::Button("Больница СФ", ImVec2(280, 50)))
        {
            ToggleTeleport(28);
            Show(false);
        }

        if(ImGui::Button("База ФБР", ImVec2(280, 50)))
        {
            ToggleTeleport(29);
            Show(false);
        }

        if(ImGui::Button("Шахта", ImVec2(280, 50)))
        {
            ToggleTeleport(30);
            Show(false);
        }

        if(ImGui::Button("Гора Чилиад", ImVec2(280, 50)))
        {
            ToggleTeleport(31);
            Show(false);
        }

        if(ImGui::Button("Аэропорт", ImVec2(280, 50)))
        {
            ToggleTeleport(32);
            Show(false);
        }

        if(ImGui::Button("Склад", ImVec2(280, 50)))
        {
            ToggleTeleport(33);
            Show(false);
        }

        if(ImGui::Button("ЖД ЛС #2", ImVec2(280, 50)))
        {
            ToggleTeleport(34);
            Show(false);
        }

        if(ImGui::Button("24/7 Магазин", ImVec2(280, 50)))
        {
            ToggleTeleport(35);
            Show(false);
        }

        if(ImGui::Button("Маяк", ImVec2(280, 50)))
        {
            ToggleTeleport(36);
            Show(false);
        }

        if(ImGui::Button("Чёртово колесо", ImVec2(280, 50)))
        {
            ToggleTeleport(37);
            Show(false);
        }

        if(ImGui::Button("АЗС", ImVec2(280, 50)))
        {
            ToggleTeleport(38);
            Show(false);
        }

        if(ImGui::Button("Пиццерия", ImVec2(280, 50)))
        {
            ToggleTeleport(39);
            Show(false);
        }

        if(ImGui::Button("Перекрасочная", ImVec2(280, 50)))
        {
            ToggleTeleport(40);
            Show(false);
        }

        if(ImGui::Button("Банк ЛС", ImVec2(280, 50)))
        {
            ToggleTeleport(41);
            Show(false);
        }

        if(ImGui::Button("Аммунация ЛС", ImVec2(280, 50)))
        {
            ToggleTeleport(42);
            Show(false);
        }

        if(ImGui::Button("Скейт Парк", ImVec2(280, 50)))
        {
            ToggleTeleport(43);
            Show(false);
        }

        if(ImGui::Button("Отель #1", ImVec2(280, 50)))
        {
            ToggleTeleport(44);
            Show(false);
        }

        if(ImGui::Button("Отель #2", ImVec2(280, 50)))
        {
            ToggleTeleport(45);
            Show(false);
        }

        if(ImGui::Button("Доки", ImVec2(280, 50)))
        {
            ToggleTeleport(46);
            Show(false);
        }

        if(ImGui::Button("Свалка", ImVec2(280, 50)))
        {
            ToggleTeleport(47);
            Show(false);
        }

        if(ImGui::Button("Зона 51 (В)", ImVec2(280, 50)))
        {
            ToggleTeleport(48);
            Show(false);
        }

        if(ImGui::Button("Зона 51 (А)", ImVec2(280, 50)))
        {
            ToggleTeleport(49);
            Show(false);
        }

        if(ImGui::Button("ECDD", ImVec2(280, 50)))
        {
            ToggleTeleport(50);
            Show(false);
        }

        if(ImGui::Button("Мафия ЛВ #1", ImVec2(280, 50)))
        {
            ToggleTeleport(51);
            Show(false);
        }

        if(ImGui::Button("Мафия ЛВ #2", ImVec2(280, 50)))
        {
            ToggleTeleport(52);
            Show(false);
        }

        if(ImGui::Button("Русская Мафия", ImVec2(280, 50)))
        {
            ToggleTeleport(53);
            Show(false);
        }

        if(ImGui::Button("Мафия СФ", ImVec2(280, 50)))
        {
            ToggleTeleport(54);
            Show(false);
        }

        if(ImGui::Button("Алхамбра", ImVec2(280, 50)))
        {
            ToggleTeleport(55);
            Show(false);
        }

        if(ImGui::Button("Close", ImVec2(280, 50)))
        {
            Show(false);
        }
        break;

        case 8:
        if(ImGui::Button("Ignor Incoming RPC", ImVec2(280, 50)))
        {
            m_bMenuStep = 9;
        }

       //if(ImGui::Button("Ignor Outcoming RPC", ImVec2(280, 50)))
       //{
       //    m_bMenuStep = 10;
       //}

       //if(ImGui::Button("Ignor Incoming Packet", ImVec2(280, 50)))
       //{
       //    Show(false);
       //}

       //if(ImGui::Button("Ignor Outcoming Packet", ImVec2(280, 50)))
       //{
       //    Show(false);
       //}

        if(ImGui::Button("Close", ImVec2(280, 50)))
        {
            Show(false);
        }
        break;

        case 9:
        //if(ImGui::Button("SetPlayerName", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}
//
        //if(ImGui::Button("SetPlayerPos", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}
//
        //if(ImGui::Button("SetPlayerPosFindZ", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}

        if(m_bPH == 1){
            if(ImGui::Button("(O) SetPlayerHealth", ImVec2(280, 50)))
            {
                ToggleRPC(4);
                Show(false);
            }
        }else{
            if(ImGui::Button("SetPlayerHealth", ImVec2(280, 50)))
            {
                ToggleRPC(4);
                Show(false);
            }
        }

        //if(ImGui::Button("TogglePlayerControllable", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}
//
        //if(ImGui::Button("PlaySound", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}
//
        //if(ImGui::Button("SetPlayerWorldBounds", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}
//
        //if(ImGui::Button("GivePlayerMoney", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}
//
        //if(ImGui::Button("SetPlayerFacingAngle", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}
//
        //if(ImGui::Button("ResetPlayerMoney", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}

        if(m_bRPW == 1){
            if(ImGui::Button("(O) ResetPlayerWeapons", ImVec2(280, 50)))
            {
                ToggleRPC(8);
                Show(false);
            }
        }else{
            if(ImGui::Button("ResetPlayerWeapons", ImVec2(280, 50)))
            {
                ToggleRPC(8);
                Show(false);
            }
        }

        //if(ImGui::Button("GivePlayerWeapon", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}

        if(m_bVP2 == 1){
            if(ImGui::Button("(O) SetVehicleParamsEx", ImVec2(280, 50)))
            {
                ToggleRPC(1);
                Show(false);
            }
        }else{
            if(ImGui::Button("SetVehicleParamsEx", ImVec2(280, 50)))
            {
                ToggleRPC(1);
                Show(false);
            }
        }

        //if(ImGui::Button("CancelEdit", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}
//
        //if(ImGui::Button("SetPlayerTime", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}
//
        //if(ImGui::Button("ToggleClock", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}
//
        //if(ImGui::Button("WorldPlayerAdd", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}
//
        //if(ImGui::Button("SetPlayerShopName", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}
//
        //if(ImGui::Button("SetPlayerSkillLevel", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}
//
        //if(ImGui::Button("SetPlayerDrunkLevel", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}
//
        //if(ImGui::Button("Create3DTextLabel", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}
//
        //if(ImGui::Button("DisableCheckpoint", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}
//
        //if(ImGui::Button("SetRaceCheckpoint", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}
//
        //if(ImGui::Button("DisableRaceCheckpoint", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}
//
        //if(ImGui::Button("GameModeRestart", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}
//
        //if(ImGui::Button("PlayAudioStream", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}
//
        //if(ImGui::Button("StopAudioStream", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}
//
        //if(ImGui::Button("RemoveBuildingForPlayer", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}

        if(m_bCO == 1){
            if(ImGui::Button("(O) CreateObject", ImVec2(280, 50)))
            {
                ToggleRPC(3);
                Show(false);
            }
        }else{
            if(ImGui::Button("CreateObject", ImVec2(280, 50)))
            {
                ToggleRPC(3);
                Show(false);
            }
        }

        //if(ImGui::Button("SetObjectPos", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}
//
        //if(ImGui::Button("SetObjectRot", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}
//
        //if(ImGui::Button("DestroyObject", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}
//
        //if(ImGui::Button("DeathMessage", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}
//
        //if(ImGui::Button("SetPlayerMapIcon", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}
//
        //if(ImGui::Button("RemoveVehicleComponent", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}
//
        //if(ImGui::Button("Update3DTextLabel", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}
//
        //if(ImGui::Button("ChatBubble", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}
//
        //if(ImGui::Button("UpdateSystemTime", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}


        if(m_bSD == 1){
            if(ImGui::Button("(O) ShowDialog", ImVec2(280, 50)))
            {
                ToggleRPC(6);
                Show(false);
            }
        }else{
            if(ImGui::Button("ShowDialog", ImVec2(280, 50)))
            {
                ToggleRPC(6);
                Show(false);
            }
        }

        //if(ImGui::Button("DestroyPickup", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}
//
        //if(ImGui::Button("WeaponPickupDestroy", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}
//
        //if(ImGui::Button("LinkVehicleToInterior", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}
//
        //if(ImGui::Button("SetPlayerArmor", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}
//
        //if(ImGui::Button("SetPlayerArmedWeapon", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}
//
        //if(ImGui::Button("SetSpawnInfo", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}
//
        //if(ImGui::Button("SetPlayerTeam", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}
//
        //if(ImGui::Button("PutPlayerInVehicle", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}

        if(m_bRPFV == 1){
            if(ImGui::Button("(O) RemovePlayerFromVehicle", ImVec2(280, 50)))
            {
                ToggleRPC(5);
                Show(false);
            }
        }else{
            if(ImGui::Button("RemovePlayerFromVehicle", ImVec2(280, 50)))
            {
                ToggleRPC(5);
                Show(false);
            }
        }

        //if(ImGui::Button("SetPlayerColor", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}
//
        //if(ImGui::Button("DisplayGameText", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}
//
        //if(ImGui::Button("ForceClassSelection", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}
//
        //if(ImGui::Button("AttachObjectToPlayer", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}
//
        //if(ImGui::Button("InitMenu", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}
//
        //if(ImGui::Button("ShowMenu", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}
//
        //if(ImGui::Button("HideMenu", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}
//
        //if(ImGui::Button("CreateExplosion", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}
//
        //if(ImGui::Button("ShowPlayerNameTagForPlayer", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}
//
        //if(ImGui::Button("AttachCameraToObject", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}
//
        //if(ImGui::Button("InterpolateCamera", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}
//
        //if(ImGui::Button("SetObjectMaterial", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}
//
        //if(ImGui::Button("GangZoneStopFlash", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}

        
        if(m_bAPA == 1){
           if(ImGui::Button("(O) ApplyAnimations", ImVec2(280, 50)))
            {
                ToggleRPC(7);
                Show(false);
            }
        }else{
            if(ImGui::Button("ApplyAnimations", ImVec2(280, 50)))
            {
                ToggleRPC(7);
                Show(false);
            }
        }

        //if(ImGui::Button("ClearAnimations", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}
//
        //if(ImGui::Button("SetPlayerSpecialAction", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}
//
        //if(ImGui::Button("SetPlayerFightingStyle", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}
//
        //if(ImGui::Button("SetPlayerVelocity", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}

        if(m_bVV == 1){
            if(ImGui::Button("(O) SetVehicleVelocity", ImVec2(280, 50)))
            {
                ToggleRPC(2);
                Show(false);
            }
        }else{
            if(ImGui::Button("SetVehicleVelocity", ImVec2(280, 50)))
            {
                ToggleRPC(2);
                Show(false);
            }
        }

        //if(ImGui::Button("SetPlayerDrunkVisuals", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}
//
        //if(ImGui::Button("ClientMessage", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}
//
        //if(ImGui::Button("SetWorldTime", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}
//
        //if(ImGui::Button("CreatePickup", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}
//
        //if(ImGui::Button("SetVehicleTireStatus", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}
//
        //if(ImGui::Button("MoveObject", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}
//
        //if(ImGui::Button("EnableStuntBonusForPlayer", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}
//
        //if(ImGui::Button("TextDrawSetString", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}
//
        //if(ImGui::Button("SetCheckpoint", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}
//
        //if(ImGui::Button("GangZoneCreate", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}
//
        //if(ImGui::Button("PlayCrimeReport", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}
//
        //if(ImGui::Button("SetPlayerAttachedObject", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}
//
        //if(ImGui::Button("GangZoneDestroy", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}
//
        //if(ImGui::Button("GangZoneFlash", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}
//
        //if(ImGui::Button("StopObject", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}
//
        //if(ImGui::Button("SetNumberPlate", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}
//
        //if(ImGui::Button("TogglePlayerSpectating", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}
//
        //if(ImGui::Button("PlayerSpectatePlayer", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}
//
        //if(ImGui::Button("PlayerSpectateVehicle", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}
//
        //if(ImGui::Button("SetPlayerWantedLevel", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}
//
        //if(ImGui::Button("ShowTextDraw", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}
//
        //if(ImGui::Button("TextDrawHideForPlayer", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}
//
        //if(ImGui::Button("ServerJoin", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}
//
        //if(ImGui::Button("ServerQuit", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}
//
        //if(ImGui::Button("InitGame", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}
        //
        //if(ImGui::Button("RemovePlayerMapIcon", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}
//
        //if(ImGui::Button("SetPlayerAmmo", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}
//
        //if(ImGui::Button("SetPlayerGravity", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}
//
        //if(ImGui::Button("SetVehicleHealth", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}
//
        //if(ImGui::Button("AttachTrailerToVehicle", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}
//
        //if(ImGui::Button("DetachTrailerFromVehicle", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}
//
        //if(ImGui::Button("SetPlayerDrunkHandling", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}
//
        //if(ImGui::Button("DestroyPickups", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}
//
        //if(ImGui::Button("SetWeather", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}
//
        //if(ImGui::Button("SetPlayerSkin", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}
//
        //if(ImGui::Button("SetPlayerInterior", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}
//
        //if(ImGui::Button("SetPlayerCameraPos", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}
//
        //if(ImGui::Button("SetPlayerCameraLookAt", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}
//
        //if(ImGui::Button("SetVehiclePos", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}
//
        //if(ImGui::Button("SetVehicleZAngle", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}

        if(m_bVP == 1){
            if(ImGui::Button("(O) SetVehicleParams", ImVec2(280, 50)))
            {
                ToggleRPC(0);
                Show(false);
            }
        }else{
            if(ImGui::Button("SetVehicleParams", ImVec2(280, 50)))
            {
                ToggleRPC(0);
                Show(false);
            }
        }

        //if(ImGui::Button("SetCameraBehindPlayer", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}
//
        //if(ImGui::Button("WorldPlayerRemove", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}
//
        //if(ImGui::Button("WorldVehicleAdd", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}
//
        //if(ImGui::Button("WorldVehicleRemove", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}
//
        //if(ImGui::Button("WorldPlayerDeath", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}
//
        //if(ImGui::Button("EnterVehicle", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}
//
        //if(ImGui::Button("EnterEditObject", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}
//
        //if(ImGui::Button("SCMEvent", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}
//
        //if(ImGui::Button("Chat", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}
//
        //if(ImGui::Button("SrvNetStats", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}
//
        //if(ImGui::Button("ClientCheck", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}
//
        //if(ImGui::Button("DamageVehicle", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}
//
        //if(ImGui::Button("EditAttachedObject", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}
//
        //if(ImGui::Button("EditObject", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}
//
        //if(ImGui::Button("RequestClass", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}
//
        //if(ImGui::Button("RequestSpawn", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}
//
        //if(ImGui::Button("ExitVehicle", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}
//
        //if(ImGui::Button("UpdateScoresPingsIPs", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}

        if(ImGui::Button("Close", ImVec2(280, 50)))
        {
            Show(false);
        }
        break;

        case 10:
        //if(ImGui::Button("EnterVehicle", ImVec2(280, 50)))
        //{
        //    m_bMenuStep = 9;
        //}
//
        //if(ImGui::Button("EnterEditObject", ImVec2(280, 50)))
        //{
        //    m_bMenuStep = 10;
        //}
//
        //if(ImGui::Button("SCMEvent", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}
//
        //if(ImGui::Button("Chat", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}
//
        //if(ImGui::Button("SrvNetStats", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}
//
        //if(ImGui::Button("DamageVehicle", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}
//
        //if(ImGui::Button("EditAttachedObject", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}
//
        //if(ImGui::Button("EditObject", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}
//
        //if(ImGui::Button("RequestClass", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}
//
        //if(ImGui::Button("RequestSpawn", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}
//
        //if(ImGui::Button("ExitVehicle", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}
//
        //if(ImGui::Button("UpdateScoresPingsIPs", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}
//
        //if(ImGui::Button("ClickPlayer", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}
//
        //if(ImGui::Button("ClientJoin", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}
//
        //if(ImGui::Button("ScriptCash", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}
//
        //if(ImGui::Button("ServerCommand", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}
//
        //if(ImGui::Button("Spawn", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}
//
        //if(ImGui::Button("Death", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}
//
        //if(ImGui::Button("NPCJoin", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}
//
        //if(ImGui::Button("DialogResponse", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}
//
        //if(ImGui::Button("ClickTextDraw", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}
//
        //if(ImGui::Button("GiveTakeDamage", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}
//
        //if(ImGui::Button("SetInteriorId", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}
//
        //if(ImGui::Button("MapMarker", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}
//
        //if(ImGui::Button("PickedUpPickup", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}
//
        //if(ImGui::Button("MenuSelect", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}
//
        //if(ImGui::Button("VehicleDestroyed", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}
//
        //if(ImGui::Button("MenuQuit", ImVec2(280, 50)))
        //{
        //    Show(false);
        //}
//
        if(ImGui::Button("Close", ImVec2(280, 50)))
        {
            Show(false);
        }
        break;

        case 11:
            ShowFlyMenu(true);
        break;
    }
    
    ImGui::SetWindowSize(ImVec2(-8, -8));
    ImVec2 size = ImGui::GetWindowSize();
    ImGui::SetWindowPos( ImVec2( ((io.DisplaySize.x - size.x + 435)/2), ((io.DisplaySize.y - size.y)/2) ) );
    ImGui::End();
    ImGui::PopStyleVar();
}