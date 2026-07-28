#include <xtl.h>

// XInputCapsPatch -- Xbox 360 kernel build 17559 (DashLaunch plugin).
//
// Makes generic (class-2) XInput controllers report GAMEPAD capabilities
// instead of "Unknown", so games that check XINPUT_CAPABILITIES accept them.
// Two in-place edits inside the class-2 capabilities filler (0x800F9F68):
//   1) 0x800F9F98  skip the "descriptor received" bail -> Type/SubType = GAMEPAD
//   2) 0x800F9FC8  emit Flags = 0x000E instead of the (zero) source byte
// RAM-only; nothing is written to persistent storage.

typedef struct _XBOX_KRNL_VERSION {
	WORD Major, Minor, Build, Qfe;
} XBOX_KRNL_VERSION, *PXBOX_KRNL_VERSION;

extern "C" { extern PXBOX_KRNL_VERSION XboxKrnlVersion; }

// Loaded-module entry; only LoadCount (at offset 0x40) is needed here.
typedef struct _LDR_DATA_TABLE_ENTRY {
	BYTE Reserved[0x40];
	WORD LoadCount;
} LDR_DATA_TABLE_ENTRY, *PLDR_DATA_TABLE_ENTRY;

// Patch sites in the class-2 capabilities filler, with their expected original
// words. The originals are checked before writing so a wrong address can never
// corrupt a live kernel instruction.
#define BAIL_BRANCH     0x800F9F98   // beq 0x800FA074   -> nop
#define BAIL_ORIGINAL   0x418200DC
#define PPC_NOP         0x60000000

#define FLAGS_LOAD      0x800F9FC8   // lbz r11,0xA2(r10) -> li r11,0x0E
#define FLAGS_ORIGINAL  0x896A00A2
#define LI_R11_0E       0x3960000E

BOOL APIENTRY DllMain(HANDLE hInstDLL, DWORD dwReason, LPVOID lpReserved)
{
	if (dwReason != DLL_PROCESS_ATTACH)
		return TRUE;

	if (XboxKrnlVersion->Build == 17559) {
		PDWORD pBail  = (PDWORD)BAIL_BRANCH;
		PDWORD pFlags = (PDWORD)FLAGS_LOAD;

		if (*pBail  == BAIL_ORIGINAL)  *pBail  = PPC_NOP;
		if (*pFlags == FLAGS_ORIGINAL) *pFlags = LI_R11_0E;
	}

	((PLDR_DATA_TABLE_ENTRY)hInstDLL)->LoadCount = 1;  // keep resident
	return FALSE;
}
