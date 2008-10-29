/********************************************************************
	created:	2008/10/18
	created:	18:10:2008   23:39
	filename: 	d:\lidengwang\lse\trunk\src\driver\misc.h
	file path:	d:\lidengwang\lse\trunk\src\driver
	file base:	misc
	file ext:	h
	author:		lidengwang
	
	purpose:	

*********************************************************************/

#pragma once


typedef struct _SYSTEM_THREAD_INFORMATION {
    LARGE_INTEGER   KernelTime;             // time spent in kernel mode
    LARGE_INTEGER   UserTime;               // time spent in user mode
    LARGE_INTEGER   CreateTime;             // thread creation time
    ULONG           WaitTime;               // wait time
    PVOID           StartAddress;           // start address
    CLIENT_ID       ClientId;               // thread and process IDs
    KPRIORITY       Priority;               // dynamic priority
    KPRIORITY       BasePriority;           // base priority
    ULONG           ContextSwitchCount;     // number of context switches
    LONG            State;                  // current state
    LONG            WaitReason;             // wait reason
} SYSTEM_THREAD_INFORMATION, * PSYSTEM_THREAD_INFORMATION;

typedef struct _SYSTEM_PROCESS_INFORMATION {
    ULONG           NextEntryDelta;         // offset to the next entry
    ULONG           ThreadCount;            // number of threads
    ULONG           Reserved1[6];           // reserved
    LARGE_INTEGER   CreateTime;             // process creation time
    LARGE_INTEGER   UserTime;               // time spent in user mode
    LARGE_INTEGER   KernelTime;             // time spent in kernel mode
    UNICODE_STRING  ProcessName;            // process name
    KPRIORITY       BasePriority;           // base process priority
    ULONG           ProcessId;              // process identifier
    ULONG           InheritedFromProcessId; // parent process identifier
    ULONG           HandleCount;            // number of handles
    ULONG           Reserved2[2];           // reserved
    VM_COUNTERS     VmCounters;             // virtual memory counters
#if _WIN32_WINNT >= 0x500
    IO_COUNTERS     IoCounters;             // i/o counters
#endif
    SYSTEM_THREAD_INFORMATION Threads[1];   // threads
} SYSTEM_PROCESS_INFORMATION, * PSYSTEM_PROCESS_INFORMATION;



UINT GetWindowsDirectory(
    LPWSTR lpBuffer,
    UINT   uSize);

BOOL GetProcessFullPath(
    PEPROCESS       pPeProcess,
    PUNICODE_STRING pUniCodeFullPath);


BOOL GetProcessFullPathByID(
    UINT uPID,
    PUNICODE_STRING pUniCodeFullPath);

PEPROCESS GetEProcessByHandle  ( HANDLE hProcess );
ULONG     GetPidByProcessHandle( HANDLE ProcessHandle, ULONG *puParentPid);

PVOID GetEProcessByProcName(WCHAR *pwszName);

BOOL GetFullPathNameByHandle(HANDLE FileHandle, WCHAR *pwszFullPathName, ULONG uLength);

BOOL GetFullPathNameByFileObject(
    PFILE_OBJECT pFileObject, 
    WCHAR *pwszFullPathName, 
    ULONG uLength);

int KavRtlGetFullPathNameByHandle(HANDLE FileHandle, WCHAR *pwszFullPathName, ULONG Length);

WCHAR GetDriverLetterFromVolumeDeviceFileName( 
    PUNICODE_STRING pVolumeDeviceFileName );

typedef enum {
    SystemBasicInformation,
    SystemProcessorInformation,             
    SystemPerformanceInformation,
    SystemTimeOfDayInformation,
    SystemPathInformation,
    SystemProcessInformation,
    SystemCallCountInformation,
    SystemDeviceInformation,
    SystemProcessorPerformanceInformation,
    SystemFlagsInformation,
    SystemCallTimeInformation,
    SystemModuleInformation,
    SystemLocksInformation,
    SystemStackTraceInformation,
    SystemPagedPoolInformation,
    SystemNonPagedPoolInformation,
    SystemHandleInformation,
    SystemObjectInformation,
    SystemPageFileInformation,
    SystemVdmInstemulInformation,
    SystemVdmBopInformation,
    SystemFileCacheInformation,
    SystemPoolTagInformation,
    SystemInterruptInformation,
    SystemDpcBehaviorInformation,
    SystemFullMemoryInformation,
    SystemLoadGdiDriverInformation,
    SystemUnloadGdiDriverInformation,
    SystemTimeAdjustmentInformation,
    SystemSummaryMemoryInformation,
    SystemMirrorMemoryInformation,
    SystemPerformanceTraceInformation,
    SystemObsolete0,
    SystemExceptionInformation,
    SystemCrashDumpStateInformation,
    SystemKernelDebuggerInformation,
    SystemContextSwitchInformation,
    SystemRegistryQuotaInformation,
    SystemExtendServiceTableInformation,
    SystemPrioritySeperation,
    SystemVerifierAddDriverInformation,
    SystemVerifierRemoveDriverInformation,
    SystemProcessorIdleInformation,
    SystemLegacyDriverInformation,
    SystemCurrentTimeZoneInformation,
    SystemLookasideInformation,
    SystemTimeSlipNotification,
    SystemSessionCreate,
    SystemSessionDetach,
    SystemSessionInformation,
    SystemRangeStartInformation,
    SystemVerifierInformation,
    SystemVerifierThunkExtend,
    SystemSessionProcessInformation,
    SystemLoadGdiDriverInSystemSpace,
    SystemNumaProcessorMap,
    SystemPrefetcherInformation,
    SystemExtendedProcessInformation,
    SystemRecommendedSharedDataAlignment,
    SystemComPlusPackage,
    SystemNumaAvailableMemory,
    SystemProcessorPowerInformation,
    SystemEmulationBasicInformation,
    SystemEmulationProcessorInformation,
    SystemExtendedHandleInformation,
    SystemLostDelayedWriteInformation,
    SystemBigPoolInformation,
    SystemSessionPoolTagInformation,
    SystemSessionMappedViewInformation,
    SystemHotpatchInformation,
    SystemObjectSecurityMode,
    SystemWatchdogTimerHandler,
    SystemWatchdogTimerInformation,
    SystemLogicalProcessorInformation,
    SystemWow64SharedInformation,
    SystemRegisterFirmwareTableInformationHandler,
    SystemFirmwareTableInformation,
    SystemModuleInformationEx,
    SystemVerifierTriageInformation,
    SystemSuperfetchInformation,
    SystemMemoryListInformation,
    SystemFileCacheInformationEx,
    MaxSystemInfoClass
} SYSTEM_INFORMATION_CLASS;


NTSYSAPI NTSTATUS NTAPI ZwQuerySystemInformation(
    ULONG SystemInformationClass,
    PVOID SystemInformation,
    ULONG SystemInformationLength,
    PULONG ReturnLength);
