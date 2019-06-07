#include "main.h"
#include "game/game.h"
#include "net/netgame.h"
#include "game/common.h"
#include "gui/gui.h"
#include "servers.h"
#include "vendor/imgui/imgui_internal.h"
#include "keyboard.h"
#include "chatwindow.h"
#include "net/vehiclepool.h"
#include <stdlib.h>
#include <string.h>
#include "settings.h"
#include "sets.h"
#include "modsa.h"
#include "dialog.h"
#include "customserver.h"

extern CGUI *pGUI;
extern CGame *pGame;
extern CNetGame *pNetGame;
extern CKeyBoard *pKeyBoard;
extern CChatWindow *pChatWindow;
extern CSettings *pSettings;
extern CSetsWindow *pSetsWindow;
extern CModSAWindow *pModSAWindow;
extern CCustomServerWindow *pCustomServer;

CServersWindow::CServersWindow()
{
	m_bIsActive = false;
    m_bMenuStep = 0;
}

CServersWindow::~CServersWindow()
{

}

void CServersWindow::Show(bool bShow)
{
	if(pGame) 
        pGame->FindPlayerPed()->TogglePlayerControllableWithoutLock(!bShow);

	m_bIsActive = bShow;
}

void CServersWindow::Clear()
{
	m_bIsActive = false;
    m_bMenuStep = 0;
}

void CServersWindow::ShowAllServers()
{
   if(ImGui::Button("Custom Server", ImVec2(350, 50)))
   {
        pCustomServer->Show(true);
        Show(false);
   }

    if(ImGui::Button("Axwell World", ImVec2(350, 50)))
    {
        unsigned short port = 7778;

        pNetGame = new CNetGame( 
            "93.170.76.34",
            port, 
            pSetsWindow->username,
            pSetsWindow->password);
        Show(false);
        pModSAWindow->protect = 0;
    }

    if(ImGui::Button("Mordor RP", ImVec2(350, 50)))
    {
        unsigned short port = 7777;

        pNetGame = new CNetGame( 
            "37.143.12.132",
            port, 
            pSetsWindow->username,
            pSetsWindow->password);
        Show(false);

        pModSAWindow->protect = 1;
    }

    if(ImGui::Button("Flin RP", ImVec2(350, 50)))
    {
        unsigned short port = 7771;

        pNetGame = new CNetGame( 
            "flin-rp.su",
            port, 
            pSetsWindow->username,
            pSetsWindow->password);
        Show(false);
        pModSAWindow->protect = 0;
    }

    if(ImGui::Button("SanTrope RP", ImVec2(350, 50)))
    {
        unsigned short port = 7777;

        pNetGame = new CNetGame( 
            "37.230.139.49",
            port, 
            pSetsWindow->username,
            pSetsWindow->password);
        Show(false);
        pModSAWindow->protect = 1;
        CDialogWindow *pDialogWindow;
    }

    if(ImGui::Button("Chance RP", ImVec2(350, 50)))
    {
        unsigned short port = 5555;

        pNetGame = new CNetGame( 
            "176.32.36.4",
            port, 
            pSetsWindow->username,
            pSetsWindow->password);
        Show(false);
        pModSAWindow->protect = 0;
    }

    if(ImGui::Button("UP RP", ImVec2(350, 50)))
    {
        unsigned short port = 7777;

        pNetGame = new CNetGame( 
            "54.36.223.60",
            port, 
            pSetsWindow->username,
            pSetsWindow->password);
        Show(false);
        pModSAWindow->protect = 0;
    }

    if(ImGui::Button("Aries RP", ImVec2(350, 50)))
    {
        unsigned short port = 7777;

        pNetGame = new CNetGame( 
            "176.32.37.21",
            port, 
            pSetsWindow->username,
            pSetsWindow->password);
        Show(false);
        pModSAWindow->protect = 0;
    }
}

void CServersWindow::Render()
{
	if(!m_bIsActive || m_bMenuStep == 0) return;

	ImGuiIO &io = ImGui::GetIO();

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));

    ImGui::Begin("> Select the server", nullptr, 
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);

    switch(m_bMenuStep){
        case 1:
            ShowAllServers();
        break;
    }
    
    ImGui::SetWindowSize(ImVec2(-8, -8));
    ImVec2 size = ImGui::GetWindowSize();
    ImGui::SetWindowPos( ImVec2( ((io.DisplaySize.x - size.x)/2), ((io.DisplaySize.y - size.y)/2) ) );
    ImGui::End();
    ImGui::PopStyleVar();
}