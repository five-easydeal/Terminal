/*++
Copyright (c) Microsoft Corporation

Module Name:
- conddkrefs.h

Abstract:
- Contains headers that are a part of the public DDK.
- We don't include both the DDK and the SDK at the same time because they mesh poorly
  and it's easier to include a copy of the infrequently changing defs here.
--*/

#pragma once

#pragma region wdm.h (public DDK)
//
// Define the base asynchronous I/O argument types
//
extern "C"
{
    typedef struct _IO_STATUS_BLOCK {
        union {
            NTSTATUS Status;
            PVOID Pointer;
        } DUMMYUNIONNAME;

        ULONG_PTR Information;
    } IO_STATUS_BLOCK, *PIO_STATUS_BLOCK;


    //
    // ClientId
    //

    typedef struct _CLIENT_ID {
        HANDLE UniqueProcess;
        HANDLE UniqueThread;
    } CLIENT_ID;
    typedef CLIENT_ID *PCLIENT_ID;


#pragma region LIST_ENTRY manipulation
    FORCEINLINE
        VOID
        InitializeListHead(
            _Out_ PLIST_ENTRY ListHead
        )

    {

        ListHead->Flink = ListHead->Blink = ListHead;
        return;
    }

    _Must_inspect_result_
        BOOLEAN
        CFORCEINLINE
        IsListEmpty(
            _In_ const LIST_ENTRY * ListHead
        )

    {

        return (BOOLEAN)(ListHead->Flink == ListHead);
    }

    FORCEINLINE
        VOID
        InsertHeadList(
            _Inout_ PLIST_ENTRY ListHead,
            _Out_ PLIST_ENTRY Entry
        )

    {

        PLIST_ENTRY NextEntry;

        NextEntry = ListHead->Flink;
        if (NextEntry->Blink != ListHead) {
            assert(false);
        }

        Entry->Flink = NextEntry;
        Entry->Blink = ListHead;
        NextEntry->Blink = Entry;
        ListHead->Flink = Entry;
        return;
    }

    FORCEINLINE
        VOID
        InsertTailList(
            _Inout_ PLIST_ENTRY ListHead,
            _Out_ __drv_aliasesMem PLIST_ENTRY Entry
        )
    {

        PLIST_ENTRY PrevEntry;


        PrevEntry = ListHead->Blink;
        if (PrevEntry->Flink != ListHead) {
            assert(false);
        }

        Entry->Flink = ListHead;
        Entry->Blink = PrevEntry;
        PrevEntry->Flink = Entry;
        ListHead->Blink = Entry;
        return;
    }

    FORCEINLINE
        BOOLEAN
        RemoveEntryList(
            _In_ PLIST_ENTRY Entry
        )

    {

        PLIST_ENTRY PrevEntry;
        PLIST_ENTRY NextEntry;

        NextEntry = Entry->Flink;
        PrevEntry = Entry->Blink;
        if ((NextEntry->Blink != Entry) || (PrevEntry->Flink != Entry)) {
            assert(false);
        }

        PrevEntry->Flink = NextEntry;
        NextEntry->Blink = PrevEntry;
        return (BOOLEAN)(PrevEntry == NextEntry);
    }
#pragma endregion

#pragma region IOCTL codes
    //
    // Define the various device type values.  Note that values used by Microsoft
    // Corporation are in the range 0-32767, and 32768-65535 are reserved for use
    // by customers.
    //

#ifndef FILE_DEVICE_CONSOLE
#define FILE_DEVICE_CONSOLE             0x00000050
#endif

    //
    // Macro definition for defining IOCTL and FSCTL function control codes.  Note
    // that function codes 0-2047 are reserved for Microsoft Corporation, and
    // 2048-4095 are reserved for customers.
    //

#ifndef CTL_CODE
#define CTL_CODE( DeviceType, Function, Method, Access ) (                 \
    ((DeviceType) << 16) | ((Access) << 14) | ((Function) << 2) | (Method) \
)
#endif

    //
    // Define the method codes for how buffers are passed for I/O and FS controls
    //

#define METHOD_BUFFERED                 0
#define METHOD_IN_DIRECT                1
#define METHOD_OUT_DIRECT               2

#ifndef METHOD_NEITHER
#define METHOD_NEITHER                  3
#endif

    //
    // Define some easier to comprehend aliases:
    //   METHOD_DIRECT_TO_HARDWARE (writes, aka METHOD_IN_DIRECT)
    //   METHOD_DIRECT_FROM_HARDWARE (reads, aka METHOD_OUT_DIRECT)
    //

#define METHOD_DIRECT_TO_HARDWARE       METHOD_IN_DIRECT
#define METHOD_DIRECT_FROM_HARDWARE     METHOD_OUT_DIRECT

    //
    // Define the access check value for any access
    //
    //
    // The FILE_READ_ACCESS and FILE_WRITE_ACCESS constants are also defined in
    // ntioapi.h as FILE_READ_DATA and FILE_WRITE_DATA. The values for these
    // constants *MUST* always be in sync.
    //
    //
    // FILE_SPECIAL_ACCESS is checked by the NT I/O system the same as FILE_ANY_ACCESS.
    // The file systems, however, may add additional access checks for I/O and FS controls
    // that use this value.
    //


#define FILE_ANY_ACCESS                 0
#define FILE_SPECIAL_ACCESS    (FILE_ANY_ACCESS)
#define FILE_READ_ACCESS          ( 0x0001 )    // file & pipe
#define FILE_WRITE_ACCESS         ( 0x0002 )    // file & pipe
#pragma endregion

};
#pragma endregion

#pragma region winternl.h (public no link, DDK)
// Modified from NtDeviceIoControlFile for now.
FORCEINLINE
NTSTATUS WINAPI IoControlFile(
    _In_ HANDLE FileHandle,
    _In_ ULONG IoControlCode,
    _In_ PVOID InputBuffer,
    _In_ ULONG InputBufferLength,
    _Out_ PVOID OutputBuffer,
    _In_ ULONG OutputBufferLength
)
{
    NTSTATUS Status = STATUS_SUCCESS;

    DWORD BytesReturned;

    if (FALSE == DeviceIoControl(FileHandle,
                                 IoControlCode,
                                 InputBuffer,
                                 InputBufferLength,
                                 OutputBuffer,
                                 OutputBufferLength,
                                 &BytesReturned,
                                 nullptr))
    {
        Status = NTSTATUS_FROM_WIN32(GetLastError());
    }

    return Status;
}
#pragma endregion

#pragma region ntifs.h (public DDK)

extern "C"
{
    #define RtlOffsetToPointer(B,O)  ((PCHAR)( ((PCHAR)(B)) + ((ULONG_PTR)(O))  ))  
};

#pragma endregion