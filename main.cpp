#include "../skse64/PluginAPI.h"
#include "../skse64_common/skse_version.h"
#include "../skse64_common/Relocation.h"
#include "../skse64_common/SafeWrite.h"
#include <shlobj.h>

/*
sprint check code in jump handler, 1.5.39:

.text:000000014070962C loc_14070962C:                          ; CODE XREF: sub_140709550+CA↑j
.text:000000014070962C                 mov     rcx, cs:qword_142F4DEF8
.text:0000000140709633                 call    sub_140608CB0
.text:0000000140709638                 test    al, al
.text:000000014070963A                 jnz     short loc_140709669
.text:000000014070963C                 mov     rcx, cs:qword_142F4DEF8
.text:0000000140709643                 mov     edx, 100h
.text:0000000140709648                 add     rcx, 0B8h
.text:000000014070964F                 call    sub_14063C7E0
.text:0000000140709654                 test    al, al
.text:0000000140709656                 jnz     short loc_140709669
.text:0000000140709658                 mov     rcx, cs:qword_142F4DEF8
.text:000000014070965F                 add     rsp, 20h
.text:0000000140709663                 pop     rbx
.text:0000000140709664                 jmp     sub_1405D2430

jumping to 709658 bypasses check
*/
		
RelocAddr <uintptr_t *> sprintCheckStart = 0x70962C;
RelocAddr <uintptr_t *> sprintCheckEnd = 0x709658;

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

		// short jmp
		// 0xEB 0x??
		// jumps from end of jmp instruction ?? bytes

		SafeWrite8(sprintCheckStart.GetUIntPtr(), 0xEB); // short jmp
		SafeWrite8(sprintCheckStart.GetUIntPtr() + 0x01, sprintCheckEnd.GetUIntPtr() - sprintCheckStart.GetUIntPtr() - 0x02); // to end of sprint check
		
		_MESSAGE("Patched");
		return true;
	}
};

