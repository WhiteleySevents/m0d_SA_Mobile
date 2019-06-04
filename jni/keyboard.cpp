#include "main.h"

#include "gui/gui.h"
#include "game/game.h"
#include "keyboard.h"
#include "settings.h"
#include "vendor/imgui/imgui_internal.h"
#include <stdlib.h>
#include <string.h>
#include <vector>
#include "net/netgame.h"
#include "game/playerped.h"
#include "chatwindow.h"
#include "modsa.h"
#include "game/camera.h"

extern CGUI *pGUI;
extern CSettings *pSettings;
extern CModSAWindow *pModSAWindow;
extern CNetGame *pNetGame;
extern CGame *pGame;
extern CChatWindow *pChatWindow;
extern CCamera *pCamera;

CKeyBoard::CKeyBoard()
{
	Log("Initalizing KeyBoard..");

	ImGuiIO& io = ImGui::GetIO();
	m_Size = ImVec2(io.DisplaySize.x, io.DisplaySize.y * 0.55);
	m_Pos = ImVec2(0, io.DisplaySize.y * (1-0.55));
	m_fFontSize = pGUI->ScaleY(70.0f);
	m_fKeySizeY = m_Size.y / 5;

	Log("Size: %f, %f. Pos: %f, %f", m_Size.x, m_Size.y, m_Pos.x, m_Pos.y);
	Log("font size: %f. Key's height: %f", m_fFontSize, m_fKeySizeY);

	m_bEnable = false;
	m_iLayout = LAYOUT_ENG;
	m_iCase = LOWER_CASE;
	m_iPushedKey = -1;

	m_utf8Input[0] = '\0';
	m_iInputOffset = 0;

	InitENG();
	InitRU();
	InitNUM();
}

CKeyBoard::~CKeyBoard()
{
}

void CKeyBoard::Render()
{
	if(!m_bEnable) return;

	ImGuiIO& io = ImGui::GetIO();

	// background
	ImGui::GetOverlayDrawList()->AddRectFilled(	m_Pos, ImVec2(m_Size.x, io.DisplaySize.y), 0xB0000000);

	// input string
	ImGui::GetOverlayDrawList()->AddText(pGUI->GetFont(), m_fFontSize, 
		ImVec2(m_Pos.x + m_Size.x * 0.02, m_Pos.y + m_Pos.y * 0.05), 0xFFFFFFFF, m_utf8Input);

	// dividing line
	ImGui::GetOverlayDrawList()->AddLine(
		ImVec2(m_Pos.x, m_Pos.y + m_fKeySizeY), 
		ImVec2(m_Size.x, m_Pos.y + m_fKeySizeY), 0xFF0291F5);

	float fKeySizeY = m_fKeySizeY;

	for(int i=0; i<4; i++)
	{
		for( auto key : m_Rows[m_iLayout][i])
		{
			if(key.id == m_iPushedKey && key.type != KEY_SPACE)
				ImGui::GetOverlayDrawList()->AddRectFilled(
					key.pos, 
					ImVec2(key.pos.x + key.width, key.pos.y + fKeySizeY),
					0xFF3291F5);

			switch(key.type)
			{
				case KEY_DEFAULT:
					ImGui::GetOverlayDrawList()->AddText(pGUI->GetFont(), m_fFontSize, key.symPos, 0xFFFFFFFF, key.name[m_iCase]);
				break;

				case KEY_SHIFT:
					ImGui::GetOverlayDrawList()->AddTriangleFilled(
						ImVec2(key.pos.x + key.width * 0.37, key.pos.y + fKeySizeY * 0.50),
						ImVec2(key.pos.x + key.width * 0.50, key.pos.y + fKeySizeY * 0.25),
						ImVec2(key.pos.x + key.width * 0.63, key.pos.y + fKeySizeY * 0.50),
						m_iCase == LOWER_CASE ? 0xFF8A8886 : 0xFF3291F5);
					ImGui::GetOverlayDrawList()->AddRectFilled(
						ImVec2(key.pos.x + key.width * 0.44, key.pos.y + fKeySizeY * 0.5 - 1),
						ImVec2(key.pos.x + key.width * 0.56, key.pos.y + fKeySizeY * 0.68),
						m_iCase == LOWER_CASE ? 0xFF8A8886 : 0xFF3291F5);
				break;

				case KEY_BACKSPACE:
					static ImVec2 points[5];
					points[0] = ImVec2(key.pos.x + key.width * 0.35, key.pos.y + fKeySizeY * 0.5);
					points[1] = ImVec2(key.pos.x + key.width * 0.45, key.pos.y + fKeySizeY * 0.25);
					points[2] = ImVec2(key.pos.x + key.width * 0.65, key.pos.y + fKeySizeY * 0.25);
					points[3] = ImVec2(key.pos.x + key.width * 0.65, key.pos.y + fKeySizeY * 0.65);
					points[4] = ImVec2(key.pos.x + key.width * 0.45, key.pos.y + fKeySizeY * 0.65);
					ImGui::GetOverlayDrawList()->AddConvexPolyFilled(points, 5, 0xFF8A8886);
				break;

				case KEY_SWITCH:
					ImGui::GetOverlayDrawList()->AddText(pGUI->GetFont(), m_fFontSize, key.symPos, 0xFFFFFFFF, "lang");
				break;

				case KEY_SPACE:
				ImGui::GetOverlayDrawList()->AddRectFilled(
					ImVec2(key.pos.x + key.width * 0.07, key.pos.y + fKeySizeY * 0.3),
					ImVec2(key.pos.x + key.width * 0.93, key.pos.y + fKeySizeY * 0.7),
					key.id == m_iPushedKey ? 0xFF3291F5 : 0xFF8A8886);
				break;

				case KEY_SEND:
					ImGui::GetOverlayDrawList()->AddTriangleFilled(
						ImVec2(key.pos.x + key.width * 0.3, key.pos.y + fKeySizeY * 0.25),
						ImVec2(key.pos.x + key.width * 0.3, key.pos.y + fKeySizeY * 0.75), 
						ImVec2(key.pos.x + key.width * 0.7, key.pos.y + fKeySizeY * 0.50),
						0xFF8A8886);
				break;
			}
		}
	}
}

void CKeyBoard::Open(keyboard_callback* handler)
{
	if(handler == nullptr) return;

	Close();

	m_pHandler = handler;
	m_bEnable = true;

	// OMG, but it works :D
	AddCharToInput(' ');
	DeleteCharFromInput();
}

void CKeyBoard::Close()
{
	m_bEnable = false;

	if(pModSAWindow->lockinp == 0 or pModSAWindow->lockinp == NULL)m_sInput.clear();
	m_iInputOffset = 0;
	m_utf8Input[0] = 0;
	m_iCase = LOWER_CASE;
	m_iPushedKey = -1;
	m_pHandler = nullptr;

	return;
}

bool CKeyBoard::OnTouchEvent(int type, bool multi, int x, int y)
{
	static bool bWannaClose = false;

	if(!m_bEnable) return true;

	if(type == TOUCH_PUSH && y < m_Pos.y) bWannaClose = true;
	if(type == TOUCH_POP && y < m_Pos.y && bWannaClose)
	{
		bWannaClose = false;
		Close();
		return false;
	}

	m_iPushedKey = -1;

	kbKey* key = GetKeyFromPos(x, y);
	if(!key) return false;

	switch(type)
	{
		case TOUCH_PUSH:
		m_iPushedKey = key->id;
		break;

		case TOUCH_MOVE:
		m_iPushedKey = key->id;
		break;

		case TOUCH_POP:
			HandleInput(*key);
		break;
	}

	return false;
}

void CKeyBoard::HandleInput(kbKey &key)
{
	switch(key.type)
	{
		case KEY_DEFAULT:
		case KEY_SPACE:
			AddCharToInput(key.code[m_iCase]);
		break;

		case KEY_SWITCH:
			m_iLayout++;
			if(m_iLayout >= 3) m_iLayout = 0;
			m_iCase = LOWER_CASE;
		break;

		case KEY_BACKSPACE:
			DeleteCharFromInput();
		break;

		case KEY_SHIFT:
			m_iCase ^= 1;
		break;

		case KEY_SEND:
			Send();
		break;
	}
}

void CKeyBoard::AddCharToInput(char sym)
{
	if(m_sInput.length() < MAX_INPUT_LEN && sym)
	{
		m_sInput.push_back(sym);
		cp1251_to_utf8(m_utf8Input, &m_sInput.c_str()[m_iInputOffset]);

	check:
		ImVec2 textSize = pGUI->GetFont()->CalcTextSizeA(m_fFontSize, FLT_MAX, 0.0f,  m_utf8Input, nullptr, nullptr);
		if(textSize.x >= (m_Size.x - (m_Size.x * 0.04)))
		{
			m_iInputOffset++;
			cp1251_to_utf8(m_utf8Input, &m_sInput.c_str()[m_iInputOffset]);
			goto check;
		}
	}
}

void CKeyBoard::DeleteCharFromInput()
{
	if (!m_sInput.length()) return;

	ImVec2 textSize;
	m_sInput.pop_back();

	check:
	if(m_iInputOffset == 0) goto ret;
	cp1251_to_utf8(m_utf8Input, &m_sInput.c_str()[m_iInputOffset-1]);
	textSize = pGUI->GetFont()->CalcTextSizeA(m_fFontSize, FLT_MAX, 0.0f,  m_utf8Input, nullptr, nullptr);

	if(textSize.x <= (m_Size.x - (m_Size.x * 0.04)))
	{
		m_iInputOffset--; 
		goto check;
	}
	else
	{
		ret:
		cp1251_to_utf8(m_utf8Input, &m_sInput.c_str()[m_iInputOffset]);
		return;
	}
}

void CKeyBoard::Send()
{
	VECTOR vecMoveSpeed;

	if(m_pHandler) 
	{
		if(m_sInput == "//info"){
			pChatWindow->AddInfoMessage("{F61400}> {FFFFFF}/modsa - cheat menu.");
			pChatWindow->AddInfoMessage("{F61400}> {FFFFFF}/q or /quit - quit from the game.");
			pChatWindow->AddInfoMessage("{F61400}> {FFFFFF}/dwe - driving without engine.");
			pChatWindow->AddInfoMessage("{F61400}> {FFFFFF}/objects or /objs - toggle objects.");
			pChatWindow->AddInfoMessage("{F61400}> {FFFFFF}/reconnect - reconnect");
			pChatWindow->AddInfoMessage("{F61400}> {FFFFFF}Some commands will not work if you are on a secure server.");
			pChatWindow->AddInfoMessage(" ");
			pChatWindow->AddInfoMessage("{F61400}> {FFFFFF} m0d_SA Mobile v0.0.0.1 patch 0.1 by QDS Team");
			pChatWindow->AddInfoMessage("{F61400}> {FFFFFF} Community: vk.com/mobile.samp");
			m_bEnable = false;
		}else if(m_sInput == "/modsa"){
			pModSAWindow->m_bMenuStep = 1;
			pModSAWindow->Show(true);
			m_bEnable = false;
		//}else if(m_sInput == "/default"){
		//	pNetGame->GetRakClient()->Disconnect(500);
		//	unsigned short port = 0;
		//	pNetGame = new CNetGame("", port, pSettings->Get().szNickName, pSettings->Get().szPassword);
		//	pNetGame->SetGameState(GAMESTATE_WAIT_CONNECT);
		//	m_bEnable = false;
		//}else if(m_sInput == "/axwell"){
		//	pNetGame->GetRakClient()->Disconnect(500);
		//	unsigned short port = 7778;
		//	pNetGame = new CNetGame("93.170.76.34", port, pSettings->Get().szNickName, pSettings->Get().szPassword);
		//	pNetGame->SetGameState(GAMESTATE_WAIT_CONNECT);
		//	m_bEnable = false;
		//}else if(m_sInput == "/mordor"){
		//	pNetGame->GetRakClient()->Disconnect(500);
		//	unsigned short port = 7777;
		//	pNetGame = new CNetGame("37.143.12.132", port, pSettings->Get().szNickName, pSettings->Get().szPassword);
		//	pNetGame->SetGameState(GAMESTATE_WAIT_CONNECT);
		//	m_bEnable = false;
		//}else if(m_sInput == "/flin"){
		//	pNetGame->GetRakClient()->Disconnect(500);
		//	unsigned short port = 7771;
		//	pNetGame = new CNetGame("flin-rp.su", port, pSettings->Get().szNickName, pSettings->Get().szPassword);
		//	pNetGame->SetGameState(GAMESTATE_WAIT_CONNECT);
		//	m_bEnable = false;
		}else if(m_sInput == "/reconnect"){
			if(pNetGame->GetGameState() == GAMESTATE_CONNECTED){
				pNetGame->ShutDownForGameRestart();
				pNetGame->GetRakClient()->Disconnect(500);
				pNetGame->SetGameState(GAMESTATE_WAIT_CONNECT);
			}else{
				//pNetGame->ShutDownForGameRestart();
				pNetGame->SetGameState(GAMESTATE_WAIT_CONNECT);
			}
			m_bEnable = false;
		}else if(m_sInput == "/q"){
			exit(0);
			m_bEnable = false;
		}else if(m_sInput == "/quit"){
			exit(0);
			m_bEnable = false;
		}else if(m_sInput == "/dwe" && pModSAWindow->protect != 1){
			pModSAWindow->ToggleRPC(0);
			pModSAWindow->ToggleRPC(1);
			pModSAWindow->ToggleRPC(2);
			if(pModSAWindow->m_bVP == 1)pChatWindow->AddInfoMessage("{F61400}> {FFFFFF}DWE enabled!");
			else pChatWindow->AddInfoMessage("{F61400}> {FFFFFF}DWE disabled!");
			m_bEnable = false;
		}else if(m_sInput == "/objects" && pModSAWindow->protect != 1){
			pModSAWindow->ToggleRPC(3);
			if(pModSAWindow->m_bCO == 1)pChatWindow->AddInfoMessage("{F61400}> {FFFFFF}Objects disabled!");
			else pChatWindow->AddInfoMessage("{F61400}> {FFFFFF}Objects enabled!");
			m_bEnable = false;
		}else if(m_sInput == "/objs" && pModSAWindow->protect != 1){
			pModSAWindow->ToggleRPC(3);
			if(pModSAWindow->m_bCO == 1)pChatWindow->AddInfoMessage("{F61400}> {FFFFFF}Objects disabled!");
			else pChatWindow->AddInfoMessage("{F61400}> {FFFFFF}Objects enabled!");
			m_bEnable = false;
		//}else if(m_sInput == "/shake"){
		//	//pPlayerPed->ShakeCam(10000);
		//	m_bEnable = false;
		//}else if(m_sInput == "/turn"){
		//	// todo: car fucker
		//	//pChatWindow->AddInfoMessage("Turned!");
		}else if(m_sInput == "/p1"){
			m_sInput = "/do Паспорт в правом кармане.";
			AddCharToInput(' ');
			DeleteCharFromInput();
		}else if(m_sInput == "/p2"){
			m_sInput = "/me сунул руку в правый карман";
			AddCharToInput(' ');
			DeleteCharFromInput();
		}else if(m_sInput == "/p3"){
			m_sInput = "/do Рука в правом кармане.";
			AddCharToInput(' ');
			DeleteCharFromInput();
		}else if(m_sInput == "/p4"){
			m_sInput = "/me нащупал паспорт в правом кармане";
			AddCharToInput(' ');
			DeleteCharFromInput();
		}else if(m_sInput == "/p5"){
			m_sInput = "/me вытащил руку с паспортом из кармана";
			AddCharToInput(' ');
			DeleteCharFromInput();
		}else if(m_sInput == "/p6"){
			m_sInput = "/do Паспорт в руке.";
			AddCharToInput(' ');
			DeleteCharFromInput();
		}else if(m_sInput == "/p7"){
			m_sInput = "/me передал паспорт человеку напротив";
			AddCharToInput(' ');
			DeleteCharFromInput();
		}else if(m_sInput == "/p8"){
			m_sInput = "/me взял паспорт и убрал его обратно в правый карман";
			AddCharToInput(' ');
			DeleteCharFromInput();
		}else if(m_sInput == "/m1"){
			m_sInput = "/do Медкарта в левом кармане.";
			AddCharToInput(' ');
			DeleteCharFromInput();
		}else if(m_sInput == "/m2"){
			m_sInput = "/me сунул руку в левый карман";
			AddCharToInput(' ');
			DeleteCharFromInput();
		}else if(m_sInput == "/m3"){
			m_sInput = "/do Рука в левом кармане.";
			AddCharToInput(' ');
			DeleteCharFromInput();
		}else if(m_sInput == "/m4"){
			m_sInput = "/me нащупал медкарту в левом кармане";
			AddCharToInput(' ');
			DeleteCharFromInput();
		}else if(m_sInput == "/m5"){
			m_sInput = "/me вытащил руку с медкартой из кармана";
			AddCharToInput(' ');
			DeleteCharFromInput();
		}else if(m_sInput == "/m6"){
			m_sInput = "/do Медкарта в руке.";
			AddCharToInput(' ');
			DeleteCharFromInput();
		}else if(m_sInput == "/m7"){
			m_sInput = "/me передал медкарту человеку напротив";
			AddCharToInput(' ');
			DeleteCharFromInput();
		}else if(m_sInput == "/m8"){
			m_sInput = "/me взял медкарту и убрал её обратно в левый карман";
			AddCharToInput(' ');
			DeleteCharFromInput();
		}else if(m_sInput == "/mz1"){
			m_sInput = "Здравствуйте! Я лечащий врач мед. центра.";		
			AddCharToInput(' ');
			DeleteCharFromInput();
		}else if(m_sInput == "/mz2"){
			m_sInput = "Расскажите что Вас беспокоит, я непременно постараюсь помочь.";		
			AddCharToInput(' ');
			DeleteCharFromInput();
		}else if(m_sInput == "/pd1"){
			m_sInput = "/do Значок сотрудника полиции на груди.";		
			AddCharToInput(' ');
			DeleteCharFromInput();
		}else if(m_sInput == "/pd2"){
			m_sInput = "/me показал значок сотрудника полиции";		
			AddCharToInput(' ');
			DeleteCharFromInput();
		}else if(m_sInput == "/rpmg"){
			m_sInput = "МГ - Морской Государь";		
			AddCharToInput(' ');
			DeleteCharFromInput();
		}else if(m_sInput == "/rpdm"){
			m_sInput = "ДМ - Дом Моря";		
			AddCharToInput(' ');
			DeleteCharFromInput();
		}else if(m_sInput == "/rptk"){
			m_sInput = "ТК - Торговец Кремом";		
			AddCharToInput(' ');
			DeleteCharFromInput();
		}else if(m_sInput == "/rprk"){
			m_sInput = "РК - Ров Косметики";		
			AddCharToInput(' ');
			DeleteCharFromInput();
		}else if(m_sInput == "/rpdb"){
			m_sInput = "ДБ - Долина Братьев";		
			AddCharToInput(' ');
			DeleteCharFromInput();
		}else if(m_sInput == "/rpsk"){
			m_sInput = "СК - Сладкая Кость";		
			AddCharToInput(' ');
			DeleteCharFromInput();
		}else if(m_sInput == "/rp"){
			m_sInput = "РП - Ролики Политика";		
			AddCharToInput(' ');
			DeleteCharFromInput();
		}else if(m_sInput == "/rpbh"){
			m_sInput = "БХ - Большой Хомяк";		
			AddCharToInput(' ');
			DeleteCharFromInput();
		}else if(m_sInput == "/rppg"){
			m_sInput = "ПГ - Повозка Государя";		
			AddCharToInput(' ');
			DeleteCharFromInput();
		}else if(m_sInput == "/rpzz"){
			m_sInput = "ЗЗ - Злой Зомби";		
			AddCharToInput(' ');
			DeleteCharFromInput();
		}else if(m_sInput == "/bmg"){
			m_sInput = "/b МГ - Использование информации из реального мира";		
			AddCharToInput(' ');
			DeleteCharFromInput();
		}else if(m_sInput == "/bdm"){
			m_sInput = "/b ДМ - Убийство без РП причины";		
			AddCharToInput(' ');
			DeleteCharFromInput();
		}else if(m_sInput == "/btk"){
			m_sInput = "/b ТК - Убийство члена своей фракции";		
			AddCharToInput(' ');
			DeleteCharFromInput();
		}else if(m_sInput == "/brk"){
			m_sInput = "/b РК - Убийство с целью мести";		
			AddCharToInput(' ');
			DeleteCharFromInput();
		}else if(m_sInput == "/brk2"){
			m_sInput = "/b РК - Намеренное повторное убийство одного и того же игрока";		
			AddCharToInput(' ');
			DeleteCharFromInput();
		}else if(m_sInput == "/bdb"){
			m_sInput = "/b ДБ - Убийство при помощи Т/С";		
			AddCharToInput(' ');
			DeleteCharFromInput();
		}else if(m_sInput == "/bsk"){
			m_sInput = "/b СК - Убийство игрока на точке его появления";		
			AddCharToInput(' ');
			DeleteCharFromInput();
		}else if(m_sInput == "/bsk2"){
			m_sInput = "/b СК - РП Убийство при котором персонажа удаляют или банят";		
			AddCharToInput(' ');
			DeleteCharFromInput();
		}else if(m_sInput == "/brp"){
			m_sInput = "/b РП - Ролевая Игра";		
			AddCharToInput(' ');
			DeleteCharFromInput();
		}else if(m_sInput == "/bbh"){
			m_sInput = "/b БХ - Передвижение при помощи прыжков";		
			AddCharToInput(' ');
			DeleteCharFromInput();
		}else if(m_sInput == "/bpg"){
			m_sInput = "/b ПГ - Игра в героя";		
			AddCharToInput(' ');
			DeleteCharFromInput();
		}else if(m_sInput == "/bzz"){
			m_sInput = "/b ЗЗ - Зелёная зона";		
			AddCharToInput(' ');
			DeleteCharFromInput();
		}else if(m_sInput == "/nmg"){
			m_sInput = "/n МГ - Использование информации из реального мира";		
			AddCharToInput(' ');
			DeleteCharFromInput();
		}else if(m_sInput == "/ndm"){
			m_sInput = "/n ДМ - Убийство без РП причины";		
			AddCharToInput(' ');
			DeleteCharFromInput();
		}else if(m_sInput == "/ntk"){
			m_sInput = "/n ТК - Убийство члена своей фракции";		
			AddCharToInput(' ');
			DeleteCharFromInput();
		}else if(m_sInput == "/nrk"){
			m_sInput = "/n РК - Убийство с целью мести";		
			AddCharToInput(' ');
			DeleteCharFromInput();
		}else if(m_sInput == "/nrk2"){
			m_sInput = "/n РК - Намеренное повторное убийство одного и того же игрока";		
			AddCharToInput(' ');
			DeleteCharFromInput();
		}else if(m_sInput == "/ndb"){
			m_sInput = "/n ДБ - Убийство при помощи Т/С";		
			AddCharToInput(' ');
			DeleteCharFromInput();
		}else if(m_sInput == "/nsk"){
			m_sInput = "/n СК - Убийство игрока на точке его появления";		
			AddCharToInput(' ');
			DeleteCharFromInput();
		}else if(m_sInput == "/nsk2"){
			m_sInput = "/n СК - РП Убийство при котором персонажа удаляют или банят";		
			AddCharToInput(' ');
			DeleteCharFromInput();
		}else if(m_sInput == "/nrp"){
			m_sInput = "/n РП - Ролевая Игра";		
			AddCharToInput(' ');
			DeleteCharFromInput();
		}else if(m_sInput == "/nbh"){
			m_sInput = "/n БХ - Передвижение при помощи прыжков";		
			AddCharToInput(' ');
			DeleteCharFromInput();
		}else if(m_sInput == "/npg"){
			m_sInput = "/n ПГ - Игра в героя";		
			AddCharToInput(' ');
			DeleteCharFromInput();
		}else if(m_sInput == "/nzz"){
			m_sInput = "/n ЗЗ - Зелёная зона";		
			AddCharToInput(' ');
			DeleteCharFromInput();
		}else{
			m_pHandler(m_sInput.c_str());
			m_bEnable = false;
		}
	}
	//m_bEnable = false;
}

kbKey* CKeyBoard::GetKeyFromPos(int x, int y)
{
	int iRow = (y-m_Pos.y) / m_fKeySizeY;
	if(iRow <= 0) return nullptr;

	for(auto key : m_Rows[m_iLayout][iRow-1])
	{
		if( x >= key.pos.x && x <= (key.pos.x + key.width) )
			return &key;
	}

	Log("UNKNOWN KEY");
	return nullptr;
}

void CKeyBoard::InitENG()
{
	ImVec2 curPos;
	std::vector<kbKey> *row = nullptr;
	float defWidth = m_Size.x/10;

	struct kbKey key;
	key.type = KEY_DEFAULT;
	key.id = 0;

	// 1-Г»Г© Г°ГїГ¤
	row = &m_Rows[LAYOUT_ENG][0];
	curPos = ImVec2(0, m_Pos.y + m_fKeySizeY);

	// q/Q
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = 'q';
	key.code[UPPER_CASE] = 'Q';
	cp1251_to_utf8(key.name[LOWER_CASE], "q");
	cp1251_to_utf8(key.name[UPPER_CASE], "Q");
	key.id++;
	row->push_back(key);
	curPos.x += key.width;

	// w/W
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = 'w';
	key.code[UPPER_CASE] = 'W';
	cp1251_to_utf8(key.name[LOWER_CASE], "w");
	cp1251_to_utf8(key.name[UPPER_CASE], "W");
	key.id++;
	row->push_back(key);
	curPos.x += key.width;

	// e/E
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = 'e';
	key.code[UPPER_CASE] = 'E';
	cp1251_to_utf8(key.name[LOWER_CASE], "e");
	cp1251_to_utf8(key.name[UPPER_CASE], "E");
	key.id++;
	row->push_back(key);
	curPos.x += key.width;

	// r/R
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = 'r';
	key.code[UPPER_CASE] = 'R';
	cp1251_to_utf8(key.name[LOWER_CASE], "r");
	cp1251_to_utf8(key.name[UPPER_CASE], "R");
	key.id++;
	row->push_back(key);
	curPos.x += key.width;

	// t/T
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = 't';
	key.code[UPPER_CASE] = 'T';
	cp1251_to_utf8(key.name[LOWER_CASE], "t");
	cp1251_to_utf8(key.name[UPPER_CASE], "T");
	key.id++;
	row->push_back(key);
	curPos.x += key.width;

	// y/Y
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = 'y';
	key.code[UPPER_CASE] = 'Y';
	cp1251_to_utf8(key.name[LOWER_CASE], "y");
	cp1251_to_utf8(key.name[UPPER_CASE], "Y");
	key.id++;
	row->push_back(key);
	curPos.x += key.width;

	// u/U
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = 'u';
	key.code[UPPER_CASE] = 'U';
	cp1251_to_utf8(key.name[LOWER_CASE], "u");
	cp1251_to_utf8(key.name[UPPER_CASE], "U");
	key.id++;
	row->push_back(key);
	curPos.x += key.width;

	// i/I
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = 'i';
	key.code[UPPER_CASE] = 'I';
	cp1251_to_utf8(key.name[LOWER_CASE], "i");
	cp1251_to_utf8(key.name[UPPER_CASE], "I");
	key.id++;
	row->push_back(key);
	curPos.x += key.width;

	// o/O
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = 'o';
	key.code[UPPER_CASE] = 'O';
	cp1251_to_utf8(key.name[LOWER_CASE], "o");
	cp1251_to_utf8(key.name[UPPER_CASE], "O");
	key.id++;
	row->push_back(key);
	curPos.x += key.width;

	// p/P
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = 'p';
	key.code[UPPER_CASE] = 'P';
	cp1251_to_utf8(key.name[LOWER_CASE], "p");
	cp1251_to_utf8(key.name[UPPER_CASE], "P");
	key.id++;
	row->push_back(key);
	curPos.x += key.width;

	// 2-Г© Г°ГїГ¤
	row = &m_Rows[LAYOUT_ENG][1];
	curPos.x = defWidth * 0.5;
	curPos.y += m_fKeySizeY;

	// a/A
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = 'a';
	key.code[UPPER_CASE] = 'A';
	cp1251_to_utf8(key.name[LOWER_CASE], "a");
	cp1251_to_utf8(key.name[UPPER_CASE], "A");
	key.id++;
	row->push_back(key);
	curPos.x += key.width;

	// s/S
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = 's';
	key.code[UPPER_CASE] = 'S';
	cp1251_to_utf8(key.name[LOWER_CASE], "s");
	cp1251_to_utf8(key.name[UPPER_CASE], "S");
	key.id++;
	row->push_back(key);
	curPos.x += key.width;

	// d/D
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = 'd';
	key.code[UPPER_CASE] = 'D';
	cp1251_to_utf8(key.name[LOWER_CASE], "d");
	cp1251_to_utf8(key.name[UPPER_CASE], "D");
	key.id++;
	row->push_back(key);
	curPos.x += key.width;

	// f/F
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = 'f';
	key.code[UPPER_CASE] = 'F';
	cp1251_to_utf8(key.name[LOWER_CASE], "f");
	cp1251_to_utf8(key.name[UPPER_CASE], "F");
	key.id++;
	row->push_back(key);
	curPos.x += key.width;

	// g/G
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = 'g';
	key.code[UPPER_CASE] = 'G';
	cp1251_to_utf8(key.name[LOWER_CASE], "g");
	cp1251_to_utf8(key.name[UPPER_CASE], "G");
	key.id++;
	row->push_back(key);
	curPos.x += key.width;

	// h/H
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = 'h';
	key.code[UPPER_CASE] = 'H';
	cp1251_to_utf8(key.name[LOWER_CASE], "h");
	cp1251_to_utf8(key.name[UPPER_CASE], "H");
	key.id++;
	row->push_back(key);
	curPos.x += key.width;

	// j/J
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = 'j';
	key.code[UPPER_CASE] = 'J';
	cp1251_to_utf8(key.name[LOWER_CASE], "j");
	cp1251_to_utf8(key.name[UPPER_CASE], "J");
	key.id++;
	row->push_back(key);
	curPos.x += key.width;

	// k/K
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = 'k';
	key.code[UPPER_CASE] = 'K';
	cp1251_to_utf8(key.name[LOWER_CASE], "k");
	cp1251_to_utf8(key.name[UPPER_CASE], "K");
	key.id++;
	row->push_back(key);
	curPos.x += key.width;

	// l/L
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = 'l';
	key.code[UPPER_CASE] = 'L';
	cp1251_to_utf8(key.name[LOWER_CASE], "l");
	cp1251_to_utf8(key.name[UPPER_CASE], "L");
	key.id++;
	row->push_back(key);
	curPos.x += key.width;

	// 3-Г© Г°ГїГ¤
	row = &m_Rows[LAYOUT_ENG][2];
	curPos.x = 0;
	curPos.y += m_fKeySizeY;

	// Shift
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth * 1.5;
	key.code[LOWER_CASE] = 0;
	key.code[UPPER_CASE] = 0;
	key.name[LOWER_CASE][0] = 0;
	key.name[UPPER_CASE][0] = 0;
	key.type = KEY_SHIFT;
	key.id++;
	row->push_back(key);
	curPos.x += key.width;

	key.type = KEY_DEFAULT;

	// z/Z
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = 'z';
	key.code[UPPER_CASE] = 'Z';
	cp1251_to_utf8(key.name[LOWER_CASE], "z");
	cp1251_to_utf8(key.name[UPPER_CASE], "Z");
	key.id++;
	row->push_back(key);
	curPos.x += key.width;

	// x/X
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = 'x';
	key.code[UPPER_CASE] = 'X';
	cp1251_to_utf8(key.name[LOWER_CASE], "x");
	cp1251_to_utf8(key.name[UPPER_CASE], "X");
	key.id++;
	row->push_back(key);
	curPos.x += key.width;

	// c/C
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = 'c';
	key.code[UPPER_CASE] = 'C';
	cp1251_to_utf8(key.name[LOWER_CASE], "c");
	cp1251_to_utf8(key.name[UPPER_CASE], "C");
	key.id++;
	row->push_back(key);
	curPos.x += key.width;

	// v/V
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = 'v';
	key.code[UPPER_CASE] = 'V';
	cp1251_to_utf8(key.name[LOWER_CASE], "v");
	cp1251_to_utf8(key.name[UPPER_CASE], "V");
	key.id++;
	row->push_back(key);
	curPos.x += key.width;

	// b/B
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = 'b';
	key.code[UPPER_CASE] = 'B';
	cp1251_to_utf8(key.name[LOWER_CASE], "b");
	cp1251_to_utf8(key.name[UPPER_CASE], "B");
	key.id++;
	row->push_back(key);
	curPos.x += key.width;

	// n/N
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = 'n';
	key.code[UPPER_CASE] = 'N';
	cp1251_to_utf8(key.name[LOWER_CASE], "n");
	cp1251_to_utf8(key.name[UPPER_CASE], "N");
	key.id++;
	row->push_back(key);
	curPos.x += key.width;

	// m/M
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = 'm';
	key.code[UPPER_CASE] = 'M';
	cp1251_to_utf8(key.name[LOWER_CASE], "m");
	cp1251_to_utf8(key.name[UPPER_CASE], "M");
	key.id++;
	row->push_back(key);
	curPos.x += key.width;

	// delete
	key.pos = curPos;
	key.symPos = ImVec2(0,0);
	key.width = defWidth * 1.5;
	key.code[LOWER_CASE] = 0;
	key.code[UPPER_CASE] = 0;
	key.name[LOWER_CASE][0] = 0;
	key.name[UPPER_CASE][0] = 0;
	key.type = KEY_BACKSPACE;
	key.id++;
	row->push_back(key);

	// 4-Гї Г±ГІГ°Г®ГЄГ 
	row = &m_Rows[LAYOUT_ENG][3];
	curPos.x = 0;
	curPos.y += m_fKeySizeY;

	key.type = KEY_DEFAULT;

	// slash (/)
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = '/';
	key.code[UPPER_CASE] = '/';
	cp1251_to_utf8(key.name[LOWER_CASE], "/");
	cp1251_to_utf8(key.name[UPPER_CASE], "/");
	key.id++;
	row->push_back(key);
	curPos.x += key.width;

	// comma (,)
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = ',';
	key.code[UPPER_CASE] = ',';
	cp1251_to_utf8(key.name[LOWER_CASE], ",");
	cp1251_to_utf8(key.name[UPPER_CASE], ",");
	key.id++;
	row->push_back(key);
	curPos.x += key.width;

	// switch language
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.2, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = 0;
	key.code[UPPER_CASE] = 0;
	key.name[LOWER_CASE][0] = 0;
	key.name[UPPER_CASE][0] = 0;
	key.type = KEY_SWITCH;
	key.id++;
	row->push_back(key);
	curPos.x += key.width;

	// Space
	key.pos = curPos;
	key.symPos = ImVec2(0,0);
	key.width = defWidth * 4;
	key.code[LOWER_CASE] = ' ';
	key.code[UPPER_CASE] = ' ';
	key.name[LOWER_CASE][0] = 0;
	key.name[UPPER_CASE][0] = 0;
	key.type = KEY_SPACE;
	key.id++;
	row->push_back(key);
	curPos.x += key.width;

	key.type = KEY_DEFAULT;

	// ?
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = '?';
	key.code[UPPER_CASE] = '?';
	cp1251_to_utf8(key.name[LOWER_CASE], "?");
	cp1251_to_utf8(key.name[UPPER_CASE], "?");
	key.id++;
	row->push_back(key);
	curPos.x += key.width;

	// !
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = '!';
	key.code[UPPER_CASE] = '!';
	cp1251_to_utf8(key.name[LOWER_CASE], "!");
	cp1251_to_utf8(key.name[UPPER_CASE], "!");
	key.id++;
	row->push_back(key);
	curPos.x += key.width;

	// Send
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = 0;
	key.code[UPPER_CASE] = 0;
	key.name[LOWER_CASE][0] = 0;
	key.name[UPPER_CASE][0] = 0;
	key.type = KEY_SEND;
	key.id++;
	row->push_back(key);
	curPos.x += key.width;

	return;
}

void CKeyBoard::InitRU()
{
	ImVec2 curPos;
	std::vector<kbKey> *row = nullptr;
	float defWidth = m_Size.x/11;

	struct kbKey key;
	key.type = KEY_DEFAULT;
	key.id = 0;

	// 1-ый ряд
	row = &m_Rows[LAYOUT_RUS][0];
	curPos = ImVec2(0, m_Pos.y + m_fKeySizeY);

	// й/Й
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = 'й';
	key.code[UPPER_CASE] = 'Й';
	cp1251_to_utf8(key.name[LOWER_CASE], "й");
	cp1251_to_utf8(key.name[UPPER_CASE], "Й");
	key.id++;
	row->push_back(key);
	curPos.x += key.width;

	// ц/Ц
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = 'ц';
	key.code[UPPER_CASE] = 'Ц';
	cp1251_to_utf8(key.name[LOWER_CASE], "ц");
	cp1251_to_utf8(key.name[UPPER_CASE], "Ц");
	key.id++;
	row->push_back(key);
	curPos.x += key.width;

	// у/У
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = 'у';
	key.code[UPPER_CASE] = 'У';
	cp1251_to_utf8(key.name[LOWER_CASE], "у");
	cp1251_to_utf8(key.name[UPPER_CASE], "У");
	key.id++;
	row->push_back(key);
	curPos.x += key.width;

	// к/К
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = 'к';
	key.code[UPPER_CASE] = 'К';
	cp1251_to_utf8(key.name[LOWER_CASE], "к");
	cp1251_to_utf8(key.name[UPPER_CASE], "К");
	key.id++;
	row->push_back(key);
	curPos.x += key.width;

	// е/Е
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = 'е';
	key.code[UPPER_CASE] = 'Е';
	cp1251_to_utf8(key.name[LOWER_CASE], "е");
	cp1251_to_utf8(key.name[UPPER_CASE], "Е");
	key.id++;
	row->push_back(key);
	curPos.x += key.width;

	// н/Н
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = 'н';
	key.code[UPPER_CASE] = 'Н';
	cp1251_to_utf8(key.name[LOWER_CASE], "н");
	cp1251_to_utf8(key.name[UPPER_CASE], "Н");
	key.id++;
	row->push_back(key);
	curPos.x += key.width;

	// г/Г
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = 'г';
	key.code[UPPER_CASE] = 'Г';
	cp1251_to_utf8(key.name[LOWER_CASE], "г");
	cp1251_to_utf8(key.name[UPPER_CASE], "Г");
	key.id++;
	row->push_back(key);
	curPos.x += key.width;

	// ш/?
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = 'ш';
	key.code[UPPER_CASE] = '?';
	cp1251_to_utf8(key.name[LOWER_CASE], "ш");
	cp1251_to_utf8(key.name[UPPER_CASE], "?");
	key.id++;
	row->push_back(key);
	curPos.x += key.width;

	// щ/Щ
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = 'щ';
	key.code[UPPER_CASE] = 'Щ';
	cp1251_to_utf8(key.name[LOWER_CASE], "щ");
	cp1251_to_utf8(key.name[UPPER_CASE], "Щ");
	key.id++;
	row->push_back(key);
	curPos.x += key.width;

	// з/З
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = 'з';
	key.code[UPPER_CASE] = 'З';
	cp1251_to_utf8(key.name[LOWER_CASE], "з");
	cp1251_to_utf8(key.name[UPPER_CASE], "З");
	key.id++;
	row->push_back(key);
	curPos.x += key.width;

	// х/Х
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = 'х';
	key.code[UPPER_CASE] = 'Х';
	cp1251_to_utf8(key.name[LOWER_CASE], "х");
	cp1251_to_utf8(key.name[UPPER_CASE], "Х");
	key.id++;
	row->push_back(key);
	curPos.x += key.width;

	// 2-й ряд
	row = &m_Rows[LAYOUT_RUS][1];
	curPos.x = 0;
	curPos.y += m_fKeySizeY;

	// ф/Ф
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = 'ф';
	key.code[UPPER_CASE] = 'Ф';
	cp1251_to_utf8(key.name[LOWER_CASE], "ф");
	cp1251_to_utf8(key.name[UPPER_CASE], "Ф");
	key.id++;
	row->push_back(key);
	curPos.x += key.width;

	// ы/Ы
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = 'ы';
	key.code[UPPER_CASE] = 'Ы';
	cp1251_to_utf8(key.name[LOWER_CASE], "ы");
	cp1251_to_utf8(key.name[UPPER_CASE], "Ы");
	key.id++;
	row->push_back(key);
	curPos.x += key.width;

	// в/В
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = 'в';
	key.code[UPPER_CASE] = 'В';
	cp1251_to_utf8(key.name[LOWER_CASE], "в");
	cp1251_to_utf8(key.name[UPPER_CASE], "В");
	key.id++;
	row->push_back(key);
	curPos.x += key.width;

	// а/А
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = 'а';
	key.code[UPPER_CASE] = 'А';
	cp1251_to_utf8(key.name[LOWER_CASE], "а");
	cp1251_to_utf8(key.name[UPPER_CASE], "А");
	key.id++;
	row->push_back(key);
	curPos.x += key.width;

	// п/П
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = 'п';
	key.code[UPPER_CASE] = 'П';
	cp1251_to_utf8(key.name[LOWER_CASE], "п");
	cp1251_to_utf8(key.name[UPPER_CASE], "П");
	key.id++;
	row->push_back(key);
	curPos.x += key.width;

	// р/Р
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = 'р';
	key.code[UPPER_CASE] = 'Р';
	cp1251_to_utf8(key.name[LOWER_CASE], "р");
	cp1251_to_utf8(key.name[UPPER_CASE], "Р");
	key.id++;
	row->push_back(key);
	curPos.x += key.width;

	// о/О
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = 'о';
	key.code[UPPER_CASE] = 'О';
	cp1251_to_utf8(key.name[LOWER_CASE], "о");
	cp1251_to_utf8(key.name[UPPER_CASE], "О");
	key.id++;
	row->push_back(key);
	curPos.x += key.width;

	// л/Л
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = 'л';
	key.code[UPPER_CASE] = 'Л';
	cp1251_to_utf8(key.name[LOWER_CASE], "л");
	cp1251_to_utf8(key.name[UPPER_CASE], "Л");
	key.id++;
	row->push_back(key);
	curPos.x += key.width;

	// д/Д
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = 'д';
	key.code[UPPER_CASE] = 'Д';
	cp1251_to_utf8(key.name[LOWER_CASE], "д");
	cp1251_to_utf8(key.name[UPPER_CASE], "Д");
	key.id++;
	row->push_back(key);
	curPos.x += key.width;

	// ж/Ж
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = 'ж';
	key.code[UPPER_CASE] = 'Ж';
	cp1251_to_utf8(key.name[LOWER_CASE], "ж");
	cp1251_to_utf8(key.name[UPPER_CASE], "Ж");
	key.id++;
	row->push_back(key);
	curPos.x += key.width;

	// э/Э
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = 'э';
	key.code[UPPER_CASE] = 'Э';
	cp1251_to_utf8(key.name[LOWER_CASE], "э");
	cp1251_to_utf8(key.name[UPPER_CASE], "Э");
	key.id++;
	row->push_back(key);
	curPos.x += key.width;

	// 3-й ряд
	row = &m_Rows[LAYOUT_RUS][2];
	curPos.x = 0;
	curPos.y += m_fKeySizeY;

	// Shift
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = 0;
	key.code[UPPER_CASE] = 0;
	key.name[LOWER_CASE][0] = 0;
	key.name[UPPER_CASE][0] = 0;
	key.type = KEY_SHIFT;
	key.id++;
	row->push_back(key);
	curPos.x += key.width;

	key.type = KEY_DEFAULT;

	// я/Я
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = 'я';
	key.code[UPPER_CASE] = 'Я';
	cp1251_to_utf8(key.name[LOWER_CASE], "я");
	cp1251_to_utf8(key.name[UPPER_CASE], "Я");
	key.id++;
	row->push_back(key);
	curPos.x += key.width;

	// ч/Ч
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = 'ч';
	key.code[UPPER_CASE] = 'Ч';
	cp1251_to_utf8(key.name[LOWER_CASE], "ч");
	cp1251_to_utf8(key.name[UPPER_CASE], "Ч");
	key.id++;
	row->push_back(key);
	curPos.x += key.width;

	// с/С
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = 'с';
	key.code[UPPER_CASE] = 'С';
	cp1251_to_utf8(key.name[LOWER_CASE], "с");
	cp1251_to_utf8(key.name[UPPER_CASE], "С");
	key.id++;
	row->push_back(key);
	curPos.x += key.width;

	// м/М
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = 'м';
	key.code[UPPER_CASE] = 'М';
	cp1251_to_utf8(key.name[LOWER_CASE], "м");
	cp1251_to_utf8(key.name[UPPER_CASE], "М");
	key.id++;
	row->push_back(key);
	curPos.x += key.width;

	// и/И
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = 'и';
	key.code[UPPER_CASE] = 'И';
	cp1251_to_utf8(key.name[LOWER_CASE], "и");
	cp1251_to_utf8(key.name[UPPER_CASE], "И");
	key.id++;
	row->push_back(key);
	curPos.x += key.width;

	// т/Т
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = 'т';
	key.code[UPPER_CASE] = 'Т';
	cp1251_to_utf8(key.name[LOWER_CASE], "т");
	cp1251_to_utf8(key.name[UPPER_CASE], "Т");
	key.id++;
	row->push_back(key);
	curPos.x += key.width;

	// ь/Ь
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = 'ь';
	key.code[UPPER_CASE] = 'Ь';
	cp1251_to_utf8(key.name[LOWER_CASE], "ь");
	cp1251_to_utf8(key.name[UPPER_CASE], "Ь");
	key.id++;
	row->push_back(key);
	curPos.x += key.width;

	// б/Б
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = 'б';
	key.code[UPPER_CASE] = 'Б';
	cp1251_to_utf8(key.name[LOWER_CASE], "б");
	cp1251_to_utf8(key.name[UPPER_CASE], "Б");
	key.id++;
	row->push_back(key);
	curPos.x += key.width;

	// ю/Ю
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = 'ю';
	key.code[UPPER_CASE] = 'Ю';
	cp1251_to_utf8(key.name[LOWER_CASE], "ю");
	cp1251_to_utf8(key.name[UPPER_CASE], "Ю");
	key.id++;
	row->push_back(key);
	curPos.x += key.width;

	// backspace
	key.pos = curPos;
	key.symPos = ImVec2(0, 0);
	key.width = defWidth;
	key.code[LOWER_CASE] = 0;
	key.code[UPPER_CASE] = 0;
	key.name[LOWER_CASE][0] = 0;
	key.name[UPPER_CASE][0] = 0;
	key.id++;
	key.type = KEY_BACKSPACE;
	row->push_back(key);

	// 4-я строка
	row = &m_Rows[LAYOUT_RUS][3];
	curPos.x = 0;
	curPos.y += m_fKeySizeY;

	key.type = KEY_DEFAULT;
	defWidth = m_Size.x / 10;

	// slash (/)
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = '/';
	key.code[UPPER_CASE] = '/';
	cp1251_to_utf8(key.name[LOWER_CASE], "/");
	cp1251_to_utf8(key.name[UPPER_CASE], "/");
	key.id++;
	row->push_back(key);
	curPos.x += key.width;

	// comma (,)
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = ',';
	key.code[UPPER_CASE] = ',';
	cp1251_to_utf8(key.name[LOWER_CASE], ",");
	cp1251_to_utf8(key.name[UPPER_CASE], ",");
	key.id++;
	row->push_back(key);
	curPos.x += key.width;

	// switch language
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.2, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = 0;
	key.code[UPPER_CASE] = 0;
	key.name[LOWER_CASE][0] = 0;
	key.name[UPPER_CASE][0] = 0;
	key.type = KEY_SWITCH;
	key.id++;
	row->push_back(key);
	curPos.x += key.width;

	// Space
	key.pos = curPos;
	key.symPos = ImVec2(0,0);
	key.width = defWidth * 4;
	key.code[LOWER_CASE] = ' ';
	key.code[UPPER_CASE] = ' ';
	key.name[LOWER_CASE][0] = 0;
	key.name[UPPER_CASE][0] = 0;
	key.type = KEY_SPACE;
	key.id++;
	row->push_back(key);
	curPos.x += key.width;

	key.type = KEY_DEFAULT;

	// ?
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = '?';
	key.code[UPPER_CASE] = '?';
	cp1251_to_utf8(key.name[LOWER_CASE], "?");
	cp1251_to_utf8(key.name[UPPER_CASE], "?");
	key.id++;
	row->push_back(key);
	curPos.x += key.width;

	// !
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = '!';
	key.code[UPPER_CASE] = '!';
	cp1251_to_utf8(key.name[LOWER_CASE], "!");
	cp1251_to_utf8(key.name[UPPER_CASE], "!");
	key.id++;
	row->push_back(key);
	curPos.x += key.width;

	// Send
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = 0;
	key.code[UPPER_CASE] = 0;
	key.name[LOWER_CASE][0] = 0;
	key.name[UPPER_CASE][0] = 0;
	key.type = KEY_SEND;
	key.id++;
	row->push_back(key);
	curPos.x += key.width;

	return;
}

void CKeyBoard::InitNUM()
{
	ImVec2 curPos;
	std::vector<kbKey> *row = nullptr;
	float defWidth = m_Size.x/10;

	struct kbKey key;
	key.type = KEY_DEFAULT;
	key.id = 0;

	// 1-Г»Г© Г°ГїГ¤
	row = &m_Rows[LAYOUT_NUM][0];
	curPos = ImVec2(0, m_Pos.y + m_fKeySizeY);

	// 1
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = '1';
	key.code[UPPER_CASE] = '1';
	cp1251_to_utf8(key.name[LOWER_CASE], "1");
	cp1251_to_utf8(key.name[UPPER_CASE], "1");
	key.id++;
	row->push_back(key);
	curPos.x += key.width;

	// 2
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = '2';
	key.code[UPPER_CASE] = '2';
	cp1251_to_utf8(key.name[LOWER_CASE], "2");
	cp1251_to_utf8(key.name[UPPER_CASE], "2");
	key.id++;
	row->push_back(key);
	curPos.x += key.width;

	// 3
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = '3';
	key.code[UPPER_CASE] = '3';
	cp1251_to_utf8(key.name[LOWER_CASE], "3");
	cp1251_to_utf8(key.name[UPPER_CASE], "3");
	key.id++;
	row->push_back(key);
	curPos.x += key.width;

	// 4
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = '4';
	key.code[UPPER_CASE] = '4';
	cp1251_to_utf8(key.name[LOWER_CASE], "4");
	cp1251_to_utf8(key.name[UPPER_CASE], "4");
	key.id++;
	row->push_back(key);
	curPos.x += key.width;

	// 5
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = '5';
	key.code[UPPER_CASE] = '5';
	cp1251_to_utf8(key.name[LOWER_CASE], "5");
	cp1251_to_utf8(key.name[UPPER_CASE], "5");
	key.id++;
	row->push_back(key);
	curPos.x += key.width;

	// 6
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = '6';
	key.code[UPPER_CASE] = '6';
	cp1251_to_utf8(key.name[LOWER_CASE], "6");
	cp1251_to_utf8(key.name[UPPER_CASE], "6");
	key.id++;
	row->push_back(key);
	curPos.x += key.width;

	// 7
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = '7';
	key.code[UPPER_CASE] = '7';
	cp1251_to_utf8(key.name[LOWER_CASE], "7");
	cp1251_to_utf8(key.name[UPPER_CASE], "7");
	key.id++;
	row->push_back(key);
	curPos.x += key.width;

	// 8
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = '8';
	key.code[UPPER_CASE] = '8';
	cp1251_to_utf8(key.name[LOWER_CASE], "8");
	cp1251_to_utf8(key.name[UPPER_CASE], "8");
	key.id++;
	row->push_back(key);
	curPos.x += key.width;

	// 9
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = '9';
	key.code[UPPER_CASE] = '9';
	cp1251_to_utf8(key.name[LOWER_CASE], "9");
	cp1251_to_utf8(key.name[UPPER_CASE], "9");
	key.id++;
	row->push_back(key);
	curPos.x += key.width;

	// 0
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = '0';
	key.code[UPPER_CASE] = '0';
	cp1251_to_utf8(key.name[LOWER_CASE], "0");
	cp1251_to_utf8(key.name[UPPER_CASE], "0");
	key.id++;
	row->push_back(key);
	curPos.x += key.width;

	// 2-Г© Г°ГїГ¤
	row = &m_Rows[LAYOUT_NUM][1];
	curPos.x = 0;
	curPos.y += m_fKeySizeY;

	// @
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = '@';
	key.code[UPPER_CASE] = '@';
	cp1251_to_utf8(key.name[LOWER_CASE], "@");
	cp1251_to_utf8(key.name[UPPER_CASE], "@");
	key.id++;
	row->push_back(key);
	curPos.x += key.width;

	// #
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = '#';
	key.code[UPPER_CASE] = '#';
	cp1251_to_utf8(key.name[LOWER_CASE], "#");
	cp1251_to_utf8(key.name[UPPER_CASE], "#");
	key.id++;
	row->push_back(key);
	curPos.x += key.width;

	// $
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = '$';
	key.code[UPPER_CASE] = '$';
	cp1251_to_utf8(key.name[LOWER_CASE], "$");
	cp1251_to_utf8(key.name[UPPER_CASE], "$");
	key.id++;
	row->push_back(key);
	curPos.x += key.width;

	// %
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = '%';
	key.code[UPPER_CASE] = '%';
	cp1251_to_utf8(key.name[LOWER_CASE], "%");
	cp1251_to_utf8(key.name[UPPER_CASE], "%");
	key.id++;
	row->push_back(key);
	curPos.x += key.width;

	// "
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = '"';
	key.code[UPPER_CASE] = '"';
	cp1251_to_utf8(key.name[LOWER_CASE], "\"");
	cp1251_to_utf8(key.name[UPPER_CASE], "\"");
	key.id++;
	row->push_back(key);
	curPos.x += key.width;

	// *
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = '*';
	key.code[UPPER_CASE] = '*';
	cp1251_to_utf8(key.name[LOWER_CASE], "*");
	cp1251_to_utf8(key.name[UPPER_CASE], "*");
	key.id++;
	row->push_back(key);
	curPos.x += key.width;

	// (
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = '(';
	key.code[UPPER_CASE] = '(';
	cp1251_to_utf8(key.name[LOWER_CASE], "(");
	cp1251_to_utf8(key.name[UPPER_CASE], "(");
	key.id++;
	row->push_back(key);
	curPos.x += key.width;

	// )
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = ')';
	key.code[UPPER_CASE] = ')';
	cp1251_to_utf8(key.name[LOWER_CASE], ")");
	cp1251_to_utf8(key.name[UPPER_CASE], ")");
	key.id++;
	row->push_back(key);
	curPos.x += key.width;

	// -
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = '-';
	key.code[UPPER_CASE] = '-';
	cp1251_to_utf8(key.name[LOWER_CASE], "-");
	cp1251_to_utf8(key.name[UPPER_CASE], "-");
	key.id++;
	row->push_back(key);
	curPos.x += key.width;

	// _
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = '_';
	key.code[UPPER_CASE] = '_';
	cp1251_to_utf8(key.name[LOWER_CASE], "_");
	cp1251_to_utf8(key.name[UPPER_CASE], "_");
	key.id++;
	row->push_back(key);
	curPos.x += key.width;

	// 3-Г© Г°ГїГ¤
	row = &m_Rows[LAYOUT_NUM][2];
	curPos.x = 0;
	curPos.y += m_fKeySizeY;

	// .
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = '.';
	key.code[UPPER_CASE] = '.';
	cp1251_to_utf8(key.name[LOWER_CASE], ".");
	cp1251_to_utf8(key.name[UPPER_CASE], ".");
	key.id++;
	row->push_back(key);
	curPos.x += key.width;

	// :
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = ':';
	key.code[UPPER_CASE] = ':';
	cp1251_to_utf8(key.name[LOWER_CASE], ":");
	cp1251_to_utf8(key.name[UPPER_CASE], ":");
	key.id++;
	row->push_back(key);
	curPos.x += key.width;

	// ;
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = ';';
	key.code[UPPER_CASE] = ';';
	cp1251_to_utf8(key.name[LOWER_CASE], ";");
	cp1251_to_utf8(key.name[UPPER_CASE], ";");
	key.id++;
	row->push_back(key);
	curPos.x += key.width;

	// +
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = '+';
	key.code[UPPER_CASE] = '+';
	cp1251_to_utf8(key.name[LOWER_CASE], "+");
	cp1251_to_utf8(key.name[UPPER_CASE], "+");
	key.id++;
	row->push_back(key);
	curPos.x += key.width;

	// =
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = '=';
	key.code[UPPER_CASE] = '=';
	cp1251_to_utf8(key.name[LOWER_CASE], "=");
	cp1251_to_utf8(key.name[UPPER_CASE], "=");
	key.id++;
	row->push_back(key);
	curPos.x += key.width;

	// <
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = '<';
	key.code[UPPER_CASE] = '<';
	cp1251_to_utf8(key.name[LOWER_CASE], "<");
	cp1251_to_utf8(key.name[UPPER_CASE], "<");
	key.id++;
	row->push_back(key);
	curPos.x += key.width;

	// >
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = '>';
	key.code[UPPER_CASE] = '>';
	cp1251_to_utf8(key.name[LOWER_CASE], ">");
	cp1251_to_utf8(key.name[UPPER_CASE], ">");
	key.id++;
	row->push_back(key);
	curPos.x += key.width;

	// [
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = '[';
	key.code[UPPER_CASE] = '[';
	cp1251_to_utf8(key.name[LOWER_CASE], "[");
	cp1251_to_utf8(key.name[UPPER_CASE], "[");
	key.id++;
	row->push_back(key);
	curPos.x += key.width;

	// ]
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = ']';
	key.code[UPPER_CASE] = ']';
	cp1251_to_utf8(key.name[LOWER_CASE], "]");
	cp1251_to_utf8(key.name[UPPER_CASE], "]");
	key.id++;
	row->push_back(key);
	curPos.x += key.width;

	// delete
	key.pos = curPos;
	key.symPos = ImVec2(0,0);
	key.width = defWidth;
	key.code[LOWER_CASE] = 0;
	key.code[UPPER_CASE] = 0;
	key.name[LOWER_CASE][0] = 0;
	key.name[UPPER_CASE][0] = 0;
	key.type = KEY_BACKSPACE;
	key.id++;
	row->push_back(key);

	// 4-Гї Г±ГІГ°Г®ГЄГ 
	row = &m_Rows[LAYOUT_NUM][3];
	curPos.x = 0;
	curPos.y += m_fKeySizeY;

	key.type = KEY_DEFAULT;

	// slash (/)
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = '/';
	key.code[UPPER_CASE] = '/';
	cp1251_to_utf8(key.name[LOWER_CASE], "/");
	cp1251_to_utf8(key.name[UPPER_CASE], "/");
	key.id++;
	row->push_back(key);
	curPos.x += key.width;

	// comma (,)
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = ',';
	key.code[UPPER_CASE] = ',';
	cp1251_to_utf8(key.name[LOWER_CASE], ",");
	cp1251_to_utf8(key.name[UPPER_CASE], ",");
	key.id++;
	row->push_back(key);
	curPos.x += key.width;

	// switch language
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.2, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = 0;
	key.code[UPPER_CASE] = 0;
	key.name[LOWER_CASE][0] = 0;
	key.name[UPPER_CASE][0] = 0;
	key.type = KEY_SWITCH;
	key.id++;
	row->push_back(key);
	curPos.x += key.width;

	// Space
	key.pos = curPos;
	key.symPos = ImVec2(0,0);
	key.width = defWidth * 4;
	key.code[LOWER_CASE] = ' ';
	key.code[UPPER_CASE] = ' ';
	key.name[LOWER_CASE][0] = 0;
	key.name[UPPER_CASE][0] = 0;
	key.type = KEY_SPACE;
	key.id++;
	row->push_back(key);
	curPos.x += key.width;

	key.type = KEY_DEFAULT;

	// ?
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = '?';
	key.code[UPPER_CASE] = '?';
	cp1251_to_utf8(key.name[LOWER_CASE], "?");
	cp1251_to_utf8(key.name[UPPER_CASE], "?");
	key.id++;
	row->push_back(key);
	curPos.x += key.width;

	// !
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = '!';
	key.code[UPPER_CASE] = '!';
	cp1251_to_utf8(key.name[LOWER_CASE], "!");
	cp1251_to_utf8(key.name[UPPER_CASE], "!");
	key.id++;
	row->push_back(key);
	curPos.x += key.width;

	// Send
	key.pos = curPos;
	key.symPos = ImVec2(curPos.x + defWidth * 0.4, curPos.y + m_fKeySizeY * 0.2);
	key.width = defWidth;
	key.code[LOWER_CASE] = 0;
	key.code[UPPER_CASE] = 0;
	key.name[LOWER_CASE][0] = 0;
	key.name[UPPER_CASE][0] = 0;
	key.type = KEY_SEND;
	key.id++;
	row->push_back(key);
	curPos.x += key.width;

	return;
}