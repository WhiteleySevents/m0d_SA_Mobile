#pragma once

#include <cstdlib>
#include <string>
#include <vector>
#include <list>
#include <unistd.h>

#define SAMP_VERSION	"0.3.7"
#define PORT_VERSION	"1.0.0"

#define MAX_PLAYERS		10040
#define MAX_VEHICLES	20000
#define MAX_PLAYER_NAME	24

#define RAKSAMP_CLIENT
#define NETCODE_CONNCOOKIELULZ 0x6969
#include "vendor/RakNet/SAMP/samp_netencr.h"
#include "vendor/RakNet/SAMP/SAMPRPC.h"

#include "util/util.h"

extern uintptr_t g_libGTASA;

extern const char* g_pszStorage;

void Log(const char *fmt, ...);
uint32_t GetTickCount();