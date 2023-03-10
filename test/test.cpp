#include "../MemoryModule/stdafx.h"
#include "../MemoryModule/LoadDllMemoryApi.h"
#include <cstdio>

//PMMP_GLOBAL_DATA MmpGlobalDataPtr = *(PMMP_GLOBAL_DATA*)GetProcAddress(GetModuleHandleA("MemoryModule.dll"), "MmpGlobalDataPtr");

static PVOID ReadDllFile(LPCSTR FileName) {
    LPVOID buffer;
    size_t size;
    FILE* f;
    fopen_s(&f, FileName, "rb");
    if (!f)return 0;
    _fseeki64(f, 0, SEEK_END);
    if (!(size = _ftelli64(f))) {
        fclose(f);
        return 0;
    }
    _fseeki64(f, 0, SEEK_SET);
    fread(buffer = new char[size], 1, size, f);
    fclose(f);
    return buffer;
}

static void DisplayStatus() {
    printf(
        "MemoryModulePP [Version %d.%d]\n\n\tMmpFeatures = %08X\n\n\tLdrpModuleBaseAddressIndex = %p\n\tNtdllLdrEntry = %p\n\tRtlRbInsertNodeEx = %p\n\tRtlRbRemoveNode = %p\n\n\tLdrpInvertedFunctionTable = %p\n\n\tLdrpHashTable = %p\n\n",
        MmpGlobalDataPtr->MajorVersion,
        MmpGlobalDataPtr->MinorVersion,
        MmpGlobalDataPtr->MmpFeatures,
        MmpGlobalDataPtr->MmpBaseAddressIndex->LdrpModuleBaseAddressIndex,
        MmpGlobalDataPtr->MmpBaseAddressIndex->NtdllLdrEntry,
        MmpGlobalDataPtr->MmpBaseAddressIndex->_RtlRbInsertNodeEx,
        MmpGlobalDataPtr->MmpBaseAddressIndex->_RtlRbRemoveNode,
        MmpGlobalDataPtr->MmpInvertedFunctionTable->LdrpInvertedFunctionTable,
        MmpGlobalDataPtr->MmpLdrEntry->LdrpHashTable
    );
}

void test_ic();
void test_ic_mem();
int test() {
    test_ic_mem();
    test_ic();

    LPVOID buffer = ReadDllFile("a.dll");

    HMEMORYMODULE m1 = nullptr, m2 = m1;
    HMODULE hModule = nullptr;
    FARPROC pfn = nullptr;
    DWORD MemoryModuleFeatures = 0;

    typedef int(*_exception)(int code);
    _exception exception = nullptr;
    HRSRC hRsrc;
    DWORD SizeofRes;
    HGLOBAL gRes;
    char str[10];

    LdrQuerySystemMemoryModuleFeatures(&MemoryModuleFeatures);
    if (MemoryModuleFeatures != MEMORY_FEATURE_ALL) {
        printf("not support all features on this version of windows.\n");
    }

    if (!NT_SUCCESS(LdrLoadDllMemoryExW(&m1, nullptr, 0, buffer, 0, L"kernel64", nullptr))) goto end;
    LoadLibraryW(L"wininet.dll");
    if (!NT_SUCCESS(LdrLoadDllMemoryExW(&m2, nullptr, 0, buffer, 0, L"kernel128", nullptr))) goto end;

    //forward export
    hModule = (HMODULE)m1;
    pfn = (decltype(pfn))(GetProcAddress(hModule, "Socket")); //ws2_32.WSASocketW
    pfn = (decltype(pfn))(GetProcAddress(hModule, "VerifyTruse")); //wintrust.WinVerifyTrust
    hModule = (HMODULE)m2;
    pfn = (decltype(pfn))(GetProcAddress(hModule, "Socket"));
    pfn = (decltype(pfn))(GetProcAddress(hModule, "VerifyTruse"));

    //exception
    hModule = (HMODULE)m1;
    exception = (_exception)GetProcAddress(hModule, "exception");
    if (exception) {
        for (int i = 0; i < 5; ++i)exception(i);
    }

    //tls
    pfn = GetProcAddress(hModule, "thread");
    if (pfn && pfn()) {
        printf("thread test failed.\n");
    }

    //resource
    if (!LoadStringA(hModule, 101, str, 10)) {
        printf("load string failed.\n");
    }
    else {
        printf("%s\n", str);
    }
    if (!(hRsrc = FindResourceA(hModule, MAKEINTRESOURCEA(102), "BINARY"))) {
        printf("find binary resource failed.\n");
    }
    else {
        if ((SizeofRes = SizeofResource(hModule, hRsrc)) != 0x10) {
            printf("invalid res size.\n");
        }
        else {
            if (!(gRes = LoadResource(hModule, hRsrc))) {
                printf("load res failed.\n");
            }
            else {
                if (!LockResource(gRes))printf("lock res failed.\n");
                else {
                    printf("resource test success.\n");
                }
            }
        }
    }

end:
    delete[]buffer;
    if (m1)LdrUnloadDllMemory(m1);
    FreeLibrary(LoadLibraryW(L"wininet.dll"));
    FreeLibrary(GetModuleHandleW(L"wininet.dll"));
    if (m2)LdrUnloadDllMemory(m2);

    return 0;
}

void test_ic_mem() {
    LPVOID buffer = ReadDllFile("E:/iCloudDll/objc.dll");
    SetCurrentDirectoryA("E:/iCloudDll/");
    HMEMORYMODULE m1 = nullptr, m2 = m1;
#if 0
    if (!NT_SUCCESS(LdrLoadDllMemoryExW(&m1, nullptr, 0, buffer, 0, L"objc", nullptr))) {
        printf("Failed to load objc.");
    }
    buffer = ReadDllFile("E:/iCloudDll/libdispatch.dll");
    if (!NT_SUCCESS(LdrLoadDllMemoryExW(&m1, nullptr, 0, buffer, 0, L"libdispatch", nullptr))) {
        printf("Failed to load libdispatch.");
    }
    buffer = ReadDllFile("E:/iCloudDll/zlib1.dll");
    if (!NT_SUCCESS(LdrLoadDllMemoryExW(&m1, nullptr, 0, buffer, 0, L"zlib1", nullptr))) {
        printf("Failed to load zlib1.");
    }
    buffer = ReadDllFile("E:/iCloudDll/pthreadVC2.dll");
    if (!NT_SUCCESS(LdrLoadDllMemoryExW(&m1, nullptr, 0, buffer, 0, L"pthreadVC2", nullptr))) {
        printf("Failed to load pthreadVC2.");
    }
    buffer = ReadDllFile("E:/iCloudDll/SQLite3.dll");
    if (!NT_SUCCESS(LdrLoadDllMemoryExW(&m1, nullptr, 0, buffer, 0, L"SQLite3", nullptr))) {
        printf("Failed to load SQLite3.");
    }
    buffer = ReadDllFile("E:/iCloudDll/icudt62.dll");
    if (!NT_SUCCESS(LdrLoadDllMemoryExW(&m1, nullptr, 0, buffer, 0, L"icudt62", nullptr))) {
        printf("Failed to load icudt62.");
    }
    buffer = ReadDllFile("E:/iCloudDll/libicuuc.dll");
    if (!NT_SUCCESS(LdrLoadDllMemoryExW(&m1, nullptr, 0, buffer, 0, L"libicuuc", nullptr))) {
        printf("Failed to load libicuuc.");
    }
    buffer = ReadDllFile("E:/iCloudDll/libicuin.dll");
    if (!NT_SUCCESS(LdrLoadDllMemoryExW(&m1, nullptr, 0, buffer, 0, L"libicuin", nullptr))) {
        printf("Failed to load libicuin.");
    }
    buffer = ReadDllFile("E:/iCloudDll/libxml2.dll");
    if (!NT_SUCCESS(LdrLoadDllMemoryExW(&m1, nullptr, 0, buffer, 0, L"libxml2", nullptr))) {
        printf("Failed to load libxml2.");
    }
    buffer = ReadDllFile("E:/iCloudDll/ASL.dll");
    if (!NT_SUCCESS(LdrLoadDllMemoryExW(&m1, nullptr, 0, buffer, 0, L"ASL", nullptr))) {
        printf("Failed to load ASL.");
    }
    // crash here
    buffer = ReadDllFile("E:/iCloudDll/CoreFoundation.dll");
    if (!NT_SUCCESS(LdrLoadDllMemoryExW(&m1, nullptr, 0, buffer, 0, L"CoreFoundation", nullptr))) {
        printf("Failed to load CoreFoundation.");
    }
    buffer = ReadDllFile("E:/iCloudDll/WTF.dll");
    if (!NT_SUCCESS(LdrLoadDllMemoryExW(&m1, nullptr, 0, buffer, 0, L"WTF", nullptr))) {
        printf("Failed to load WTF.");
    }
    buffer = ReadDllFile("E:/iCloudDll/JavaScriptCore.dll");
    if (!NT_SUCCESS(LdrLoadDllMemoryExW(&m1, nullptr, 0, buffer, 0, L"JavaScriptCore", nullptr))) {
        printf("Failed to load JavaScriptCore.");
    }
#endif
    buffer = ReadDllFile("E:/iCloudDll/CFNetwork.dll");
    if (!NT_SUCCESS(LdrLoadDllMemoryExW(&m1, nullptr, 0, buffer, 0, L"CFNetwork", nullptr))) {
        printf("Failed to load CFNetwork.");
    }
    buffer = ReadDllFile("E:/iCloudDll/AOSKit.dll");
    if (!NT_SUCCESS(LdrLoadDllMemoryExW(&m1, nullptr, 0, buffer, 0, L"AOSKit", nullptr))) {
        printf("Failed to load AOSKit.");
    }
    auto icloud = LoadLibraryA("E:/iCloudDll/iCloud_main.dll");

    FreeLibrary(icloud);
}

void test_ic()
{
    DWORD flags = LOAD_LIBRARY_SEARCH_USER_DIRS | LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR | LOAD_LIBRARY_SEARCH_APPLICATION_DIR | LOAD_LIBRARY_SAFE_CURRENT_DIRS | LOAD_LIBRARY_SEARCH_SYSTEM32 | LOAD_LIBRARY_SEARCH_DEFAULT_DIRS;

    AddDllDirectory(L"C:/Program Files (x86)/Common Files/Apple/Apple Application Support");
    AddDllDirectory(L"C:/Program Files (x86)/Common Files/Apple/Internet Services");

    // Load libraries, test passed
    auto lib = LoadLibraryExA(
        "C:/Program Files (x86)/Common Files/Apple/Apple Application Support/objc.dll",
        NULL,
        flags);
    lib = LoadLibraryExA(
        "C:/Program Files (x86)/Common Files/Apple/Apple Application Support/CoreFoundation.dll",
        NULL,
        flags);
    lib = LoadLibraryExA(
        "C:/Program Files (x86)/Common Files/Apple/Apple Application Support/CFNetwork.dll",
        NULL,
        flags);
}

void test_uef() {
    auto buffer = ReadDllFile("a.dll");

    if (buffer) {
        HMODULE hm = LoadLibraryMemory(buffer);
        delete[]buffer;

        if (hm) {
            printf("Load success: %p\n", hm);
            FreeLibraryMemory(hm);
        }
        else {
            printf("Load fail: %d\n", GetLastError());
        }
    }

    return;
}

int main() {
    DisplayStatus();
    test_ic_mem();

    return 0;
}
