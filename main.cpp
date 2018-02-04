#include "../skse64/PluginAPI.h"
#include "../skse64_common/skse_version.h"
#include "../skse64_common/Relocation.h"
#include "../skse64_common/SafeWrite.h"
#include <shlobj.h>

/*
sprint check code in jump handler, 1.5.23:

.text:0000000140708FBC loc_140708FBC:                          ; CODE XREF: sub_140708EE0+CA↑j
.text:0000000140708FBC                 mov     rcx, cs:qword_142F4CE68
.text:0000000140708FC3                 call    sub_140608640
.text:0000000140708FC8                 test    al, al
.text:0000000140708FCA                 jnz     short loc_140708FF9     
.text:0000000140708FCC                 mov     rcx, cs:qword_142F4CE68
.text:0000000140708FD3                 mov     edx, 100h
.text:0000000140708FD8                 add     rcx, 0B8h
.text:0000000140708FDF                 call    sub_14063C170
.text:0000000140708FE4                 test    al, al
.text:0000000140708FE6                 jnz     short loc_140708FF9     
.text:0000000140708FE8                 mov     rcx, cs:qword_142F4CE68
.text:0000000140708FEF                 add     rsp, 20h
.text:0000000140708FF3                 pop     rbx
.text:0000000140708FF4                 jmp     sub_1405D1DC0

jmp from 708FBC to 708FE8 skips checks
*/
		
RelocAddr <uintptr_t *> sprintCheckStart = 0x708FBC;
RelocAddr <uintptr_t *> sprintCheckEnd = 0x708FE6;

extern "C" {
	bool SKSEPlugin_Query(const SKSEInterface * skse, PluginInfo * info)
	{
		gLog.OpenRelative(CSIDL_MYDOCUMENTS, "\\My Games\\Skyrim Special Edition\\SKSE\\SprintJump.log");
		gLog.SetPrintLevel(IDebugLog::kLevel_Error);
		gLog.SetLogLevel(IDebugLog::kLevel_DebugMessage);

		_MESSAGE("Sprint Jump SSE");

		// populate info structure
		info->infoVersion = PluginInfo::kInfoVersion;
		info->name = "sprint jump sse";
		info->version = 1;

		if (skse->isEditor)
		{
			_MESSAGE("loaded in editor, marking as incompatible");
			return false;
		}
		else if (skse->runtimeVersion != RUNTIME_VERSION_1_5_23)
		{
			_MESSAGE("unsupported runtime version $08X", skse->runtimeVersion);
			return false;
		}

		return true;
	}

	bool SKSEPlugin_Load(const SKSEInterface * skse) {
		_MESSAGE("Patching sprint check in JumpHandler");

		SafeWrite8(sprintCheckStart.GetUIntPtr(), 0xEB); // short jmp
		SafeWrite8(sprintCheckStart.GetUIntPtr() + 0x01, sprintCheckEnd.GetUIntPtr() - sprintCheckStart.GetUIntPtr()); // to end of sprint check
		
		_MESSAGE("Patched");
		return true;
	}
};

