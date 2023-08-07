//---------------------------------------------------------------------------
/*
	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
// CPU idetification / features detection routine
//---------------------------------------------------------------------------
#include "tjsCommHead.h"

#if 0
#include <Windows.h>
#endif
#include "tvpgl_ia32_intf.h"
#include "DebugIntf.h"
#include "SysInitIntf.h"

#include "ThreadIntf.h"

#include "Exception.h"
#if 0
#include <intrin.h>
#endif
#include "MsgIntf.h"
#include <SDL_platform.h>

#if 0
#ifdef _MSC_VER
#pragma intrinsic(__rdtsc)
#endif
#endif

/*
	Note: CPU clock measuring routine is in EmergencyExit.cpp, reusing
	hot-key watching thread.
*/

//---------------------------------------------------------------------------
extern "C"
{
	tjs_uint32 TVPCPUType = 0; // CPU type
}

static bool TVPCPUChecked = false;
//---------------------------------------------------------------------------





#ifdef _WIN32
#if defined(_M_IX86) || defined(_M_X64) || defined(__i386__) || defined(__x86_64__)
#ifdef _MSC_VER
#include <windows.h>
#include <intrin.h>
extern "C" unsigned __int64 __xgetbv(int);
static bool __os_has_avx_support() {
	// Check if the OS will save the YMM registers
	unsigned long long xcrFeatureMask = __xgetbv(_XCR_XFEATURE_ENABLED_MASK);
	return (xcrFeatureMask & 6) == 6;
}
#else
// VC 以外は動作未確認
static inline int __cpuid(int CPUInfo[4],int InfoType) {
  int highest;
  asm volatile("cpuid":"=a"(*CPUInfo),"=b"(*(CPUInfo+1)),
               "=c"(*(CPUInfo+2)),"=d"(*(CPUInfo+3)):"0"(InfoType));
  return highest;
}
static inline int __cpuidex(int CPUInfo[4],int InfoType,int ECXValue) {
  int highest;
	asm volatile("xchg{l}\t{%%}ebx, %1\n\t"
		"cpuid\n\t"
		"xchg{l}\t{%%}ebx, %1\n\t"
		: "=a" (CPUInfo[0]), "=r" (CPUInfo[1]), "=c" (CPUInfo[2]), "=d" (CPUInfo[3])
		: "0" (InfoType), "2" (ECXValue));
  return highest;
}
#include <x86intrin.h>
#endif
#endif

extern "C" {
tjs_uint32 TVPCPUFeatures;
tjs_uint32 TVPCPUID1_EAX;
tjs_uint32 TVPCPUID1_EBX;
tjs_nchar TVPCPUVendor[16];
tjs_nchar TVPCPUName[52];
tjs_uint32 TVPCPUPhysicalCore;
extern tjs_uint32 TVPCPUType;
}

#if defined(_M_IX86) || defined(_M_X64) || defined(__i386__) || defined(__x86_64__)
static void GetCpuid( int op, int& eax, int& ebx, int& ecx, int& edx) {
	int info[4] = {0,0,0,0};
	__cpuid( info, op );
	eax = info[0];
	ebx = info[1];
	ecx = info[2];
	edx = info[3];
}
static void GetCpuidEx( int op, int& eax, int& ebx, int& ecx, int& edx, int ecxv ) {
	int info[4] = {0,0,0,0};
	__cpuidex( info, op, ecxv );
	eax = info[0];
	ebx = info[1];
	ecx = info[2];
	edx = info[3];
}

static int GetCpuVendor( int& max )
{
	int ret = TVP_CPU_IS_UNKNOWN;
	int eax, ebx, ecx, edx;
	GetCpuid( 0, eax, ebx, ecx, edx );
	max = eax;

#define MAKE_VENDOR_CODE( a, b, c, d )	( ((d) << 24) | ((c) << 16) | ((b) << 8) | (a) )
	// GenuineIntel
	static const int Intel_EBX = MAKE_VENDOR_CODE( 'G', 'e', 'n', 'u' );
	static const int Intel_EDX = MAKE_VENDOR_CODE( 'i', 'n', 'e', 'I' );
	static const int Intel_ECX = MAKE_VENDOR_CODE( 'n', 't', 'e', 'l' );

	// AuthenticAMD
	static const int AMD_EBX = MAKE_VENDOR_CODE( 'A', 'u', 't', 'h' );
	static const int AMD_EDX = MAKE_VENDOR_CODE( 'e', 'n', 't', 'i' );
	static const int AMD_ECX = MAKE_VENDOR_CODE( 'c', 'A', 'M', 'D' );

	// GenuineTMx86
	static const int Transmeta_EBX = MAKE_VENDOR_CODE( 'G', 'e', 'n', 'u' );
	static const int Transmeta_EDX = MAKE_VENDOR_CODE( 'i', 'n', 'e', 'T' );
	static const int Transmeta_ECX = MAKE_VENDOR_CODE( 'M', 'x', '8', '6' );

	// CyrixInstead
	static const int Cyrix_EBX = MAKE_VENDOR_CODE( 'C', 'y', 'r', 'i' );
	static const int Cyrix_EDX = MAKE_VENDOR_CODE( 'x', 'I', 'n', 's' );
	static const int Cyrix_ECX = MAKE_VENDOR_CODE( 't', 'e', 'a', 'd' );

	// CentaurHauls
	static const int IDT_EBX = MAKE_VENDOR_CODE( 'C', 'e', 'n', 't' );
	static const int IDT_EDX = MAKE_VENDOR_CODE( 'a', 'u', 'r', 'H' );
	static const int IDT_ECX = MAKE_VENDOR_CODE( 'a', 'u', 'l', 's' );

	// RiseRiseRise
	static const int Rise_EBX = MAKE_VENDOR_CODE( 'R', 'i', 's', 'e' );
	static const int Rise_EDX = MAKE_VENDOR_CODE( 'R', 'i', 's', 'e' );
	static const int Rise_ECX = MAKE_VENDOR_CODE( 'R', 'i', 's', 'e' );

	// NexGenDriven
	static const int NexGen_EBX = MAKE_VENDOR_CODE( 'N', 'e', 'x', 'G' );
	static const int NexGen_EDX = MAKE_VENDOR_CODE( 'e', 'n', 'D', 'r' );
	static const int NexGen_ECX = MAKE_VENDOR_CODE( 'i', 'v', 'e', 'n' );

	// UMC UMC UMC
	static const int UMC_EBX = MAKE_VENDOR_CODE( 'U', 'M', 'C', ' ' );
	static const int UMC_EDX = MAKE_VENDOR_CODE( 'U', 'M', 'C', ' ' );
	static const int UMC_ECX = MAKE_VENDOR_CODE( 'U', 'M', 'C', ' ' );

	// Geode By NSC
	static const int NSC_EBX = MAKE_VENDOR_CODE( 'G', 'e', 'o', 'd' );
	static const int NSC_EDX = MAKE_VENDOR_CODE( 'e', ' ', 'B', 'y' );
	static const int NSC_ECX = MAKE_VENDOR_CODE( 'N', 'S', 'C', ' ' );

	// Compaq FX32!
	static const int Compaq_EBX = MAKE_VENDOR_CODE( 'C', 'o', 'm', 'p' );
	static const int Compaq_EDX = MAKE_VENDOR_CODE( 'a', 'q', ' ', 'F' );
	static const int Compaq_ECX = MAKE_VENDOR_CODE( 'X', '3', '2', '!' );
#undef MAKE_VENDOR_CODE

	if( ebx == Intel_EBX && edx == Intel_EDX && ecx == Intel_ECX ) {
		ret = TVP_CPU_IS_INTEL;
	} else if( ebx == AMD_EBX && edx == AMD_EDX && ecx == AMD_ECX ) {
		ret = TVP_CPU_IS_AMD;
	} else if( ebx == Transmeta_EBX && edx == Transmeta_EDX && ecx == Transmeta_ECX ) {
		ret = TVP_CPU_IS_TRANSMETA;
	} else if( ebx == Cyrix_EBX && edx == Cyrix_EDX && ecx == Cyrix_ECX ) {
		ret = TVP_CPU_IS_CYRIX;
	} else if( ebx == IDT_EBX && edx == IDT_EDX && ecx == IDT_ECX ) {
		ret = TVP_CPU_IS_IDT;
	} else if( ebx == Rise_EBX && edx == Rise_EDX && ecx == Rise_ECX ) {
		ret = TVP_CPU_IS_RISE;
	} else if( ebx == NexGen_EBX && edx == NexGen_EDX && ecx == NexGen_ECX ) {
		ret = TVP_CPU_IS_NEXGEN;
	} else if( ebx == UMC_EBX && edx == UMC_EDX && ecx == UMC_ECX ) {
		ret = TVP_CPU_IS_UMC;
	} else if( ebx == NSC_EBX && edx == NSC_EDX && ecx == NSC_ECX ) {
		ret = TVP_CPU_IS_NSC;
	} else if( ebx == Compaq_EBX && edx == Compaq_EDX && ecx == Compaq_ECX ) {
		ret = TVP_CPU_IS_COMPAQ;
	}
	int* vendor = (int*)TVPCPUVendor;
	vendor[0] = ebx;
	vendor[1] = edx;
	vendor[2] = ecx;
	return ret;
}
//---------------------------------------------------------------------------
static void GetCPUName() {
	int eax, ebx, ecx, edx;
	GetCpuid( 0x80000000, eax, ebx, ecx, edx );
	if( eax >= 0x80000004 ) {
		int* name = (int*)TVPCPUName;
		GetCpuid( 0x80000002, eax, ebx, ecx, edx );
		name[0] = eax;
		name[1] = ebx;
		name[2] = ecx;
		name[3] = edx;
		GetCpuid( 0x80000003, eax, ebx, ecx, edx );
		name[4] = eax;
		name[5] = ebx;
		name[6] = ecx;
		name[7] = edx;
		GetCpuid( 0x80000004, eax, ebx, ecx, edx );
		name[8] = eax;
		name[9] = ebx;
		name[10] = ecx;
		name[11] = edx;
	}
}
#endif
//---------------------------------------------------------------------------
// TVPCheckCPULocal
//---------------------------------------------------------------------------
static tjs_uint32 TVPCheckCPULocal()
{
	TVPCPUFeatures = TVPCPUID1_EAX = TVPCPUID1_EBX = 0;
	TVPCPUPhysicalCore = 1;
	memset( TVPCPUVendor, 0, sizeof(TVPCPUVendor) );
	memset( TVPCPUName, 0, sizeof(TVPCPUName) );

#if defined(_M_IX86) || defined(_M_X64) || defined(__i386__) || defined(__x86_64__)
	int maxCpuId = 0;
	int vendor = GetCpuVendor( maxCpuId );

	unsigned long flags = 0;

	int eax, ebx, ecx, edx;
	GetCpuid( 0x80000000, eax, ebx, ecx, edx );
	int maxCpuIdEx = eax;

	int featureEx = 0, featureExEcx = 0;
	if( maxCpuIdEx > 0x80000000 ) {
		GetCpuid( 0x80000001, eax, ebx, ecx, edx );
		featureExEcx = ecx;
		featureEx = edx;

		if( featureEx & 1 ) flags |= TVP_CPU_HAS_FPU;
		if( featureEx & (1<<22) ) flags |= TVP_CPU_HAS_EMMX;
		if( featureEx & (1<<27) ) flags |= TVP_CPU_HAS_TSCP;
		if( featureEx & (1<<31) ) flags |= TVP_CPU_HAS_3DN;
		if( featureEx & (1<<30) ) flags |= TVP_CPU_HAS_E3DN;
		if( featureExEcx & (1<<6) ) flags |= TVP_CPU_HAS_SSE4a;
	}

	GetCpuid(1,eax,ebx,ecx,edx);
	TVPCPUID1_EAX = eax;
	TVPCPUID1_EBX = ebx;
	int featureEcx = ecx;
	int feature = edx;

	if( feature & 1 ) flags |= TVP_CPU_HAS_FPU;
	if( feature & (1<<23) ) flags |= TVP_CPU_HAS_MMX;
	if( feature & (1<<15) ) flags |= TVP_CPU_HAS_CMOV;
	if( feature & (1<<25) ) {
		if( vendor == TVP_CPU_IS_INTEL ) flags |= TVP_CPU_HAS_EMMX|TVP_CPU_HAS_SSE;
		else  flags |= TVP_CPU_HAS_SSE;
	}
	if( feature & (1<<26) ) flags |= TVP_CPU_HAS_SSE2;
	if( featureEcx & 1 ) flags |= TVP_CPU_HAS_SSE3;
	if( featureEcx & (1<<9) ) flags |= TVP_CPU_HAS_SSSE3;
	if( featureEcx & (1<<12) ) flags |= TVP_CPU_HAS_FMA3;
	if( featureEcx & (1<<19) ) flags |= TVP_CPU_HAS_SSE41;
	if( featureEcx & (1<<20) ) flags |= TVP_CPU_HAS_SSE42;
	if( featureEcx & (1<<25) ) flags |= TVP_CPU_HAS_AES;
	if( featureEcx & (1<<28) ) flags |= TVP_CPU_HAS_AVX;
	if( featureEcx & (1<<30) ) flags |= TVP_CPU_HAS_RDRAND;
	if( feature & (1<<4) ) flags |= TVP_CPU_HAS_TSC;

	GetCpuidEx( 7, eax, ebx, ecx, edx, 0 );
	int featureEbx = ebx;
	if( featureEbx & (1<<5) ) flags |= TVP_CPU_HAS_AVX2;
	if( featureEbx & (1<<18) ) flags |= TVP_CPU_HAS_RDSEED;

	if( vendor == TVP_CPU_IS_INTEL && maxCpuId >= 0x00000004 ) {
		GetCpuid( 0x00000004, eax, ebx, ecx, edx );
		TVPCPUPhysicalCore = ((eax >> 26) & 0x3F) + 1;
	}

	if( vendor == TVP_CPU_IS_AMD && maxCpuIdEx >= 0x80000008 ) {
		GetCpuid( 0x80000008, eax, ebx, ecx, edx );
		TVPCPUPhysicalCore = (ecx & 0xFF) + 1;
	}
	GetCPUName();

	// OS Check
#ifdef _MSC_VER
	if( flags & (TVP_CPU_HAS_AVX|TVP_CPU_HAS_AVX2) ) {
		__try {
			// YMMレジスタ(AVX)はWindowsなら7 SP1以降
			if( !__os_has_avx_support() ) {
				flags &= ~(TVP_CPU_HAS_AVX|TVP_CPU_HAS_AVX2);
			}
		} __except(EXCEPTION_EXECUTE_HANDLER) {
			// exception had been ocured
			flags &= ~(TVP_CPU_HAS_AVX|TVP_CPU_HAS_AVX2);
		} 
	}
#endif

	TVPCPUFeatures = flags | vendor;
	return flags;
#else
	return 0;
#endif
}

//---------------------------------------------------------------------------
// TVPGetCPUTypeForOne
//---------------------------------------------------------------------------
static void TVPGetCPUTypeForOne()
{
#if 1
	TVPCheckCPULocal(); // in detect_cpu.nas
#else
	__try {
		TVPCheckCPULocal(); // in detect_cpu.nas
	} __except(EXCEPTION_EXECUTE_HANDLER) {
		// exception had been ocured
		throw Exception( TVPCpuCheckFailure );
	}
#endif

	// check OSFXSR WinXP以降ならサポートしているので、もうこのチェックは無意味かな
#if 0 && !defined(TJS_64BIT_OS)
	if(TVPCPUFeatures & TVP_CPU_HAS_SSE)
	{
		__try {
			//__emit__(0x0f, 0x57, 0xc0); // xorps xmm0, xmm0   (SSE)
			__asm xorps xmm0, xmm0
		} __except(EXCEPTION_EXECUTE_HANDLER) {
			// exception had been ocured
			// execution of 'xorps' is failed (XMM registers not available)
			TVPCPUFeatures &=~ TVP_CPU_HAS_SSE;
			TVPCPUFeatures &=~ TVP_CPU_HAS_SSE2;
			TVPCPUFeatures &=~ TVP_CPU_HAS_SSE3;
			TVPCPUFeatures &=~ TVP_CPU_HAS_SSSE3;
			TVPCPUFeatures &=~ TVP_CPU_HAS_SSE41;
			TVPCPUFeatures &=~ TVP_CPU_HAS_SSE42;
		}
	}
#endif
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
// tTVPCPUCheckThread
//---------------------------------------------------------------------------
class tTVPCPUCheckThread : public tTVPThread
{
	bool Succeeded;
public:
	tTVPCPUCheckThread(DWORD tam)
	{
		// set thread affinity mask
		Succeeded = true;

		SetThreadAffinityMask((HANDLE)GetHandle(), tam);

		StartTread();
	}

	~tTVPCPUCheckThread()
	{
	}

	void WaitEnd()
	{
		Terminate();
		WaitFor();
	}

	bool GetSucceeded() const { return Succeeded; }

	void Execute(void);
} ;
//---------------------------------------------------------------------------
void tTVPCPUCheckThread::Execute(void)
{
	try
	{
		TVPGetCPUTypeForOne();
	}
	catch(...)
	{
		Succeeded = false;
	}
}
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
static ttstr TVPDumpCPUFeatures(tjs_uint32 features)
{
	ttstr ret;
#define TVP_DUMP_CPU(x, n) { ret += TJS_W("  ") TJS_W(n);  \
	if(features & x) ret += TJS_W(":yes"); else ret += TJS_W(":no"); }

	TVP_DUMP_CPU(TVP_CPU_HAS_FPU, "FPU");
	TVP_DUMP_CPU(TVP_CPU_HAS_MMX, "MMX");
	TVP_DUMP_CPU(TVP_CPU_HAS_3DN, "3DN");
	TVP_DUMP_CPU(TVP_CPU_HAS_SSE, "SSE");
	TVP_DUMP_CPU(TVP_CPU_HAS_CMOV, "CMOVcc");
	TVP_DUMP_CPU(TVP_CPU_HAS_E3DN, "E3DN");
	TVP_DUMP_CPU(TVP_CPU_HAS_EMMX, "EMMX");
	TVP_DUMP_CPU(TVP_CPU_HAS_SSE2, "SSE2");
	TVP_DUMP_CPU(TVP_CPU_HAS_TSC, "TSC");

	TVP_DUMP_CPU(TVP_CPU_HAS_TSCP, "TSCP");
	TVP_DUMP_CPU(TVP_CPU_HAS_SSE3, "SSE3");
	TVP_DUMP_CPU(TVP_CPU_HAS_SSSE3, "SSSE3");
	TVP_DUMP_CPU(TVP_CPU_HAS_SSE41, "SSE41");
	TVP_DUMP_CPU(TVP_CPU_HAS_SSE42, "SSE42");
	TVP_DUMP_CPU(TVP_CPU_HAS_SSE4a, "SSE4A");
	TVP_DUMP_CPU(TVP_CPU_HAS_AVX, "AVX");
	TVP_DUMP_CPU(TVP_CPU_HAS_AVX2, "AVX2");
	TVP_DUMP_CPU(TVP_CPU_HAS_FMA3, "FMA3");
	TVP_DUMP_CPU(TVP_CPU_HAS_AES, "AES");
	TVP_DUMP_CPU(TVP_CPU_HAS_RDRAND, "RDRAND");
	TVP_DUMP_CPU(TVP_CPU_HAS_RDSEED, "RDSEED");

	return ret;
}
//---------------------------------------------------------------------------
static ttstr TVPDumpCPUInfo(tjs_int cpu_num)
{
	// dump detected cpu type
	ttstr features( TVPFormatMessage(TVPInfoCpuNumber,ttstr(cpu_num)) );

	features += TVPDumpCPUFeatures(TVPCPUFeatures);

	tjs_uint32 vendor = TVPCPUFeatures & TVP_CPU_VENDOR_MASK;

#undef TVP_DUMP_CPU
#define TVP_DUMP_CPU(x, n) { \
	if(vendor == x) features += TJS_W("  ") TJS_W(n); }

	TVP_DUMP_CPU(TVP_CPU_IS_INTEL, "Intel");
	TVP_DUMP_CPU(TVP_CPU_IS_AMD, "AMD");
	TVP_DUMP_CPU(TVP_CPU_IS_IDT, "IDT");
	TVP_DUMP_CPU(TVP_CPU_IS_CYRIX, "Cyrix");
	TVP_DUMP_CPU(TVP_CPU_IS_NEXGEN, "NexGen");
	TVP_DUMP_CPU(TVP_CPU_IS_RISE, "Rise");
	TVP_DUMP_CPU(TVP_CPU_IS_UMC, "UMC");
	TVP_DUMP_CPU(TVP_CPU_IS_TRANSMETA, "Transmeta");
	TVP_DUMP_CPU(TVP_CPU_IS_NSC, "NSC");
	TVP_DUMP_CPU(TVP_CPU_IS_COMPAQ, "Compaq");

	TVP_DUMP_CPU(TVP_CPU_IS_UNKNOWN, "Unknown");

#undef TVP_DUMP_CPU

	features += TJS_W("(") + ttstr((const tjs_nchar *)TVPCPUVendor) + TJS_W(")");

	if(TVPCPUName[0]!=0)
		features += TJS_W(" [") + ttstr((const tjs_nchar *)TVPCPUName) + TJS_W("]");

	features += TJS_W("  CPUID(1)/EAX=") + TJSInt32ToHex(TVPCPUID1_EAX);
	features += TJS_W(" CPUID(1)/EBX=") + TJSInt32ToHex(TVPCPUID1_EBX);

	TVPAddImportantLog(features);

	if(((TVPCPUID1_EAX >> 8) & 0x0f) <= 4)
		throw Exception( TVPFormatMessage( TVPCpuCheckFailureCpuFamilyOrLesserIsNotSupported, features).c_str() );

	return features;
}
//---------------------------------------------------------------------------
#endif





//---------------------------------------------------------------------------
// TVPDetectCPU
//---------------------------------------------------------------------------
static void TVPDisableCPU(tjs_uint32 featurebit, const tjs_char *name)
{
	tTJSVariant val;
	ttstr str;
	if(TVPGetCommandLine(name, &val))
	{
		str = val;
		if(str == TJS_W("no"))
			TVPCPUType &=~ featurebit;
		else if(str == TJS_W("force"))
			TVPCPUType |= featurebit;
	}
}
//---------------------------------------------------------------------------
void TVPDetectCPU()
{
	if(TVPCPUChecked) return;
	TVPCPUChecked = true;

#ifdef _WIN32
#ifdef TJS_64BIT_OS

	// get process affinity mask
	ULONGLONG pam = 1;
	HANDLE hp = ::OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, ::GetCurrentProcessId());
	if(hp)
	{
		ULONGLONG sam = 1;
		::GetProcessAffinityMask(hp, (PDWORD_PTR)&pam, (PDWORD_PTR)&sam);
		::CloseHandle(hp);
	}

	// for each CPU...
	ttstr cpuinfo;
	bool first = true;
	tjs_uint32 features = 0;
	for(tjs_int cpu = 0; cpu < 64; cpu++)
	{
		if(pam & (1ULL<<cpu))
		{
			tTVPCPUCheckThread * thread = new tTVPCPUCheckThread(1<<cpu);
			thread->WaitEnd();
			bool succeeded = thread->GetSucceeded();
			delete thread;
			if(!succeeded) throw Exception(TJS_W("CPU check failure"));
			cpuinfo += TVPDumpCPUInfo(cpu) + TJS_W("\r\n");

			// mask features
			if(first)
			{
				features =  (TVPCPUFeatures & TVP_CPU_FEATURE_MASK);
				TVPCPUType = TVPCPUFeatures;
				first = false;
			}
			else
			{
				features &= (TVPCPUFeatures & TVP_CPU_FEATURE_MASK);
			}
		}
	}
#else
	// get process affinity mask
	DWORD pam = 1;
	HANDLE hp = ::OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, ::GetCurrentProcessId());
	if(hp)
	{
		DWORD sam = 1;
		::GetProcessAffinityMask(hp, &pam, &sam);
		::CloseHandle(hp);
	}

	// for each CPU...
	ttstr cpuinfo;
	bool first = true;
	tjs_uint32 features = 0;
	for(tjs_int cpu = 0; cpu < 32; cpu++)
	{
		if(pam & (1<<cpu))
		{
			tTVPCPUCheckThread * thread = new tTVPCPUCheckThread(1<<cpu);
			thread->WaitEnd();
			bool succeeded = thread->GetSucceeded();
			delete thread;
			if(!succeeded) throw Exception(TJS_W("CPU check failure"));
			cpuinfo += TVPDumpCPUInfo(cpu) + TJS_W("\r\n");

			// mask features
			if(first)
			{
				features =  (TVPCPUFeatures & TVP_CPU_FEATURE_MASK);
				TVPCPUType = TVPCPUFeatures;
				first = false;
			}
			else
			{
				features &= (TVPCPUFeatures & TVP_CPU_FEATURE_MASK);
			}
		}
	}
#endif
#endif
	TVPCPUType &= ~ TVP_CPU_FEATURE_MASK;
#ifdef _WIN32
	TVPCPUType |= features;
#endif

#ifndef _WIN32
#ifdef __i386__
	TVPCPUType |= TVP_CPU_FAMILY_X86;
#endif

#ifdef __x86_64__
	TVPCPUType |= TVP_CPU_FAMILY_X86_64;
#endif

#ifdef __MMX__
	TVPCPUType |= TVP_CPU_HAS_MMX;
#endif

#ifdef __3dNOW__
	TVPCPUType |= TVP_CPU_HAS_3DN;
#endif

#ifdef __SSE__
	TVPCPUType |= TVP_CPU_HAS_SSE;
#endif

#ifdef __SSE2__
	TVPCPUType |= TVP_CPU_HAS_SSE2;
#endif

#ifdef __SSE3__
	TVPCPUType |= TVP_CPU_HAS_SSE3;
#endif

#ifdef __SSSE3__
	TVPCPUType |= TVP_CPU_HAS_SSSE3;
#endif

#ifdef __SSE4_1__
	TVPCPUType |= TVP_CPU_HAS_SSE41;
#endif

#ifdef __SSE4_2__
	TVPCPUType |= TVP_CPU_HAS_SSE42;
#endif

#ifdef __SSE4a__
	TVPCPUType |= TVP_CPU_HAS_SSE4a;
#endif

#ifdef __AVX__
	TVPCPUType |= TVP_CPU_HAS_AVX;
#endif

#ifdef __AVX2__
	TVPCPUType |= TVP_CPU_HAS_AVX2;
#endif

#ifdef __FMA__
	TVPCPUType |= TVP_CPU_HAS_FMA3;
#endif

#ifdef __AES__
	TVPCPUType |= TVP_CPU_HAS_AES;
#endif

#if !defined(__EMSCRIPTEN__) && !defined(__IPHONEOS__) && !defined(__ANDROID__)
#ifndef DISABLE_USE_VECTOR_CODEPATHS
	TVPCPUType |= TVP_CPU_HAS_MMX;
	TVPCPUType |= TVP_CPU_HAS_3DN;
	TVPCPUType |= TVP_CPU_HAS_SSE;
	TVPCPUType |= TVP_CPU_HAS_SSE2;
	TVPCPUType |= TVP_CPU_HAS_SSE3;
	TVPCPUType |= TVP_CPU_HAS_SSSE3;
	TVPCPUType |= TVP_CPU_HAS_SSE41;
	TVPCPUType |= TVP_CPU_HAS_SSE42;
	TVPCPUType |= TVP_CPU_HAS_SSE4a;
#endif
#endif
#endif

	// Disable or enable cpu features by option
	TVPDisableCPU(TVP_CPU_HAS_MMX,  TJS_W("-cpummx"));
	TVPDisableCPU(TVP_CPU_HAS_3DN,  TJS_W("-cpu3dn"));
	TVPDisableCPU(TVP_CPU_HAS_SSE,  TJS_W("-cpusse"));
	TVPDisableCPU(TVP_CPU_HAS_CMOV, TJS_W("-cpucmov"));
	TVPDisableCPU(TVP_CPU_HAS_E3DN, TJS_W("-cpue3dn"));
	TVPDisableCPU(TVP_CPU_HAS_EMMX, TJS_W("-cpuemmx"));
	TVPDisableCPU(TVP_CPU_HAS_SSE2, TJS_W("-cpusse2"));

	TVPDisableCPU(TVP_CPU_HAS_SSE3, TJS_W("-cpusse3"));
	TVPDisableCPU(TVP_CPU_HAS_SSSE3, TJS_W("-cpussse3"));
	TVPDisableCPU(TVP_CPU_HAS_SSE41, TJS_W("-cpusse41"));
	TVPDisableCPU(TVP_CPU_HAS_SSE42, TJS_W("-cpusse42"));
	TVPDisableCPU(TVP_CPU_HAS_SSE4a, TJS_W("-cpusse4a"));
	TVPDisableCPU(TVP_CPU_HAS_AVX, TJS_W("-cpuavx"));
	TVPDisableCPU(TVP_CPU_HAS_AVX2, TJS_W("-cpuavx2"));
	TVPDisableCPU(TVP_CPU_HAS_FMA3, TJS_W("-cpufma3"));
	TVPDisableCPU(TVP_CPU_HAS_AES, TJS_W("-cpuaes"));

#ifdef _WIN32
	if(TVPCPUType == 0)
		throw Exception( TVPFormatMessage(TVPCpuCheckFailureNotSupprtedCpu, cpuinfo).c_str() );

	TVPAddImportantLog( TVPFormatMessage(TVPInfoFinallyDetectedCpuFeatures,TVPDumpCPUFeatures(TVPCPUType)) );
#endif
}
//---------------------------------------------------------------------------





#if 0
//---------------------------------------------------------------------------
// jpeg and png loader support functions
//---------------------------------------------------------------------------
unsigned long MMXReady = 0;
extern "C"
{
	void __fastcall CheckMMX(void)
	{
		TVPDetectCPU();
		MMXReady = TVPCPUType & TVP_CPU_HAS_MMX;
	}
}
//---------------------------------------------------------------------------
#endif





//---------------------------------------------------------------------------
// TVPGetCPUType
//---------------------------------------------------------------------------
tjs_uint32 TVPGetCPUType()
{
	TVPDetectCPU();
	return TVPCPUType;
}
//---------------------------------------------------------------------------

