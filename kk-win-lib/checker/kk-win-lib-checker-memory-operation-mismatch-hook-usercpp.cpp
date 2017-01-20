/**
 * The MIT License
 * 
 * Copyright (C) 2017 Kiyofumi Kondoh
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */


#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "kk-win-lib-checker-memory-operation-mismatch-hook-usercpp.h"
#include "kk-win-lib-checker-memory-operation-mismatch-hook-util.h"



#include "../kk-win-lib-namedpipe.h"
#include "kk-win-lib-checker-memory-operation-mismatch.h"
#include "kk-win-lib-checker-memory-operation-mismatch-client.h"


#include <stdlib.h>

#include <assert.h>

namespace kk
{

namespace checker
{


static
HMODULE
sModule = NULL;

static
DWORD64
sUserStaticFunc[MemoryOperationMismatch::kIndexUserStaticFuncMAX];

static
MemoryOperationMismatchClient*
sMemoryOperationMismatch = NULL;

typedef void* (*PFN_new)(size_t size);
static
PFN_new             pfn_new = NULL;

typedef void* (*PFN_new_array)(size_t size);
static
PFN_new_array       pfn_new_array = NULL;

typedef void (*PFN_delete)(void* p);
static
PFN_delete          pfn_delete = NULL;

typedef void (*PFN_delete_array)(void* p);
static
PFN_delete_array    pfn_delete_array = NULL;

typedef void (*PFN_delete_size)(void* p,size_t size);
static
PFN_delete_size         pfn_delete_size = NULL;

typedef void (*PFN_delete_array_size)(void* p,size_t size);
static
PFN_delete_array_size   pfn_delete_array_size = NULL;

static
void*
my_new( size_t size )
{
    void* p = pfn_new(size);

    sMemoryOperationMismatch->sendOperation( MemoryOperationMismatch::kOperationUserStaticNew, (DWORD64)p );

    return p;
}

static
void*
my_new_array( size_t size )
{
    void* p = pfn_new_array(size);

    sMemoryOperationMismatch->sendOperation( MemoryOperationMismatch::kOperationUserStaticNewArray, (DWORD64)p );

    return p;
}

static
void
my_delete( void* p )
{
    sMemoryOperationMismatch->sendOperation( MemoryOperationMismatch::kOperationUserStaticDelete, (DWORD64)p );

    pfn_delete(p);

    return;
}

static
void
my_delete_array( void* p )
{
    sMemoryOperationMismatch->sendOperation( MemoryOperationMismatch::kOperationUserStaticDeleteArray, (DWORD64)p );

    pfn_delete_array(p);

    return;
}

static
void
my_delete_size( void* p, size_t size )
{
    sMemoryOperationMismatch->sendOperation( MemoryOperationMismatch::kOperationUserStaticDeleteSize, (DWORD64)p );

    pfn_delete_size(p,size);

    return;
}

static
void
my_delete_array_size( void* p, size_t size )
{
    sMemoryOperationMismatch->sendOperation( MemoryOperationMismatch::kOperationUserStaticDeleteArraySize, (DWORD64)p );

    pfn_delete_array_size(p,size);

    return;
}


typedef void* (*PFN_new_arg3)(size_t size,const char* fn,size_t line);
static
PFN_new_arg3            pfn_new_arg3 = NULL;

typedef void* (*PFN_new_array_arg3)(size_t size,const char* fn,size_t line);
static
PFN_new_array_arg3      pfn_new_array_arg3 = NULL;

typedef void (*PFN_delete_arg3)(void* p,const char* fn,size_t line);
static
PFN_delete_arg3         pfn_delete_arg3 = NULL;

typedef void (*PFN_delete_array_arg3)(void* p,const char* fn,size_t line);
static
PFN_delete_array_arg3   pfn_delete_array_arg3 = NULL;

typedef void* (*PFN_new_arg4)(size_t size,const char* fn,size_t line,const char* tag);
static
PFN_new_arg4            pfn_new_arg4 = NULL;

typedef void* (*PFN_new_array_arg4)(size_t size,const char* fn,size_t line,const char* tag);
static
PFN_new_array_arg4      pfn_new_array_arg4 = NULL;

typedef void (*PFN_delete_arg4)(void* p,const char* fn,size_t line,const char* tag);
static
PFN_delete_arg4         pfn_delete_arg4 = NULL;

typedef void (*PFN_delete_array_arg4)(void* p,const char* fn,size_t line,const char* tag);
static
PFN_delete_array_arg4   pfn_delete_array_arg4 = NULL;

static
void*
my_new_arg3( size_t size, const char* fn, size_t line )
{
    void* p = pfn_new_arg3(size,fn,line);

    sMemoryOperationMismatch->sendOperation( MemoryOperationMismatch::kOperationUserStaticNew, (DWORD64)p );

    return p;
}

static
void*
my_new_array_arg3( size_t size, const char* fn, size_t line )
{
    void* p = pfn_new_array_arg3(size,fn,line);

    sMemoryOperationMismatch->sendOperation( MemoryOperationMismatch::kOperationUserStaticNewArray, (DWORD64)p );

    return p;
}

static
void
my_delete_arg3( void* p, const char* fn, size_t line )
{
    sMemoryOperationMismatch->sendOperation( MemoryOperationMismatch::kOperationUserStaticDelete, (DWORD64)p );

    pfn_delete_arg3(p,fn,line);

    return;
}

static
void
my_delete_array_arg3( void* p, const char* fn, size_t line )
{
    sMemoryOperationMismatch->sendOperation( MemoryOperationMismatch::kOperationUserStaticDeleteArray, (DWORD64)p );

    pfn_delete_array_arg3(p,fn,line);

    return;
}

static
void*
my_new_arg4( size_t size, const char* fn, size_t line, const char* tag )
{
    void* p = pfn_new_arg4(size,fn,line,tag);

    sMemoryOperationMismatch->sendOperation( MemoryOperationMismatch::kOperationUserStaticNew, (DWORD64)p );

    return p;
}

static
void*
my_new_array_arg4( size_t size, const char* fn, size_t line, const char* tag )
{
    void* p = pfn_new_array_arg4(size,fn,line,tag);

    sMemoryOperationMismatch->sendOperation( MemoryOperationMismatch::kOperationUserStaticNewArray, (DWORD64)p );

    return p;
}

static
void
my_delete_arg4( void* p, const char* fn, size_t line, const char* tag )
{
    sMemoryOperationMismatch->sendOperation( MemoryOperationMismatch::kOperationUserStaticDelete, (DWORD64)p );

    pfn_delete_arg4(p,fn,line,tag);

    return;
}

static
void
my_delete_array_arg4( void* p, const char* fn, size_t line, const char* tag )
{
    sMemoryOperationMismatch->sendOperation( MemoryOperationMismatch::kOperationUserStaticDeleteArray, (DWORD64)p );

    pfn_delete_array_arg4(p,fn,line,tag);

    return;
}




static
LPVOID      sPageTrampoline  = NULL;


#include <pshpack1.h>
struct HookJump {
    BYTE        opJmp;
    DWORD       relAddr;
};
#include <poppack.h>

static
const
HookJump    sHookJump = { 0xe9, 0 };

#if defined(_M_X64)
#include <pshpack1.h>
struct LongJump {
    BYTE        opPushR;

    WORD        opMovRim64;
    DWORD64     absAddr;

    DWORD       opXchgR;

    BYTE        opRet;
};
#include <poppack.h>

static
const
LongJump    sLongJump = { 0x50, 0xb848, 0, 0x24048748, 0xc3 };

#endif // defined(_M_X64)



static
bool
hookUserCPP( const HMODULE hModule )
{
    if ( NULL == hModule )
    {
        return false;
    }

    const DWORD64   pageSize = hookutil::getPageSize();

    sPageTrampoline = hookutil::trampolinePageAllocate( hModule, hModule );

    if ( NULL == sPageTrampoline )
    {
        return false;
    }

    assert( sizeof(size_t) == sizeof(void*) );

    for ( size_t indexOperation = 0; indexOperation < MemoryOperationMismatch::kIndexUserStaticFuncMAX; indexOperation += 2 )
    {
        DWORD64     minOffset = -1;
        DWORD64     maxOffset = 0;

        {
            minOffset = sUserStaticFunc[indexOperation+0];
            maxOffset = minOffset + sizeof(sHookJump);
        }

        {
            bool haveSpace = false;

            const DWORD64 funcSize = sUserStaticFunc[indexOperation+1];
            if ( 0 == funcSize )
            {
                continue;
            }
            if ( sizeof(HookJump) <= funcSize )
            {
                haveSpace = true;
            }

            if ( false == haveSpace )
            {
                assert(false);
            }

            if ( false == haveSpace )
            {
                return false;
            }
        }


        HANDLE hHeap = ::HeapCreate( 0, 0, 0 );
        if ( NULL != hHeap )
        {
            const DWORD64   minPageCount = minOffset / pageSize;
            const DWORD64   maxPageCount = (maxOffset + (pageSize-1)) / pageSize;

            const size_t    pageCount = static_cast<const size_t>( (maxPageCount - minPageCount) );

            const BYTE* pAddrModule = reinterpret_cast<const BYTE*>(hModule);
            const BYTE* pAddrMin = pAddrModule + pageSize*minPageCount;
            const BYTE* pAddrMax = pAddrModule + pageSize*maxPageCount;

            DWORD*   protect = reinterpret_cast<DWORD*>( ::HeapAlloc( hHeap, 0, sizeof(DWORD) * pageCount ) );
            if ( NULL != protect )
            {
                ZeroMemory( protect, sizeof(DWORD) * pageCount );

                // query protect
                {
                    MEMORY_BASIC_INFORMATION    mbi;
                    const BYTE* pAddr = pAddrMin;
                    for ( size_t index = 0; index < pageCount; ++index, pAddr += pageSize )
                    {
                        const size_t nRet = ::VirtualQuery( pAddr, &mbi, sizeof(mbi) );
                        if ( 0 == nRet )
                        {
                            const DWORD dwErr = ::GetLastError();
                        }
                        else
                        {
                            protect[index] = mbi.Protect;
                        }
                    }
                }
                // change protect
                {
                    const BYTE* pAddr = pAddrMin;
                    for ( size_t index = 0; index < pageCount; ++index, pAddr += pageSize )
                    {
                        if ( PAGE_EXECUTE_READWRITE != protect[index] )
                        {
                            DWORD   dwProtect;
                            const BOOL BRet = ::VirtualProtect( (LPVOID)pAddr, (size_t)pageSize, PAGE_EXECUTE_READWRITE, &dwProtect );
                            if ( !BRet )
                            {
                                const DWORD dwErr = ::GetLastError();
                            }
                            else
                            {
                                assert( dwProtect == protect[index] );
                                protect[index] = dwProtect;
                            }
                        }
                    }
                }

                {
                    assert( sizeof(size_t) == sizeof(void*) );
                    BYTE*   p = reinterpret_cast<BYTE*>(hModule);

                    if ( 0 != sUserStaticFunc[indexOperation+0] )
                    {
                        BYTE* pCode = reinterpret_cast<BYTE*>(p + sUserStaticFunc[indexOperation+0]);
                        hookutil::Trampoline* pTrampoline = reinterpret_cast<hookutil::Trampoline*>((LPBYTE)sPageTrampoline + sizeof(hookutil::Trampoline)*indexOperation/2);
                        BYTE* pOrigCode = pTrampoline->origCode;
                        BYTE* pHookCode = pTrampoline->hookCode;

                        size_t indexPatch = 0;
                        size_t indexOrig = 0;
                        size_t indexHook = 0;
                        bool    lastInstJump = false;
                        for ( indexOrig = 0; indexOrig < sizeof(HookJump); )
                        {
                            const BYTE p = pCode[indexOrig];
                            lastInstJump = true;
#if defined(_M_X64)
                            // REX prefix?
                            if ( 0x40 == (p & 0xf0) )
                            {
                                pOrigCode[indexOrig] = p;
                                pHookCode[indexHook] = p;
                                indexOrig += 1;
                                indexHook += 1;

                                continue;
                            }
#endif // defined(_M_X64)

                            switch ( p )
                            {
                            case 0x50: // push eax
                            case 0x51: // push ecx
                            case 0x52: // push edx
                            case 0x53: // push rbx
                            case 0x54: // push esp
                            case 0x55: // push ebp
                            case 0x56: // push esi
                            case 0x57: // push edi
                                pOrigCode[indexOrig] = p;
                                pHookCode[indexHook] = p;
                                indexOrig += 1;
                                indexHook += 1;
                                lastInstJump = false;
                                break;
                            case 0x58: // pop eax
                            case 0x59: // pop ecx
                            case 0x5a: // pop edx
                            case 0x5b: // pop ebx
                            case 0x5c: // pop esp
                            case 0x5d: // pop ebp
                            case 0x5e: // pop esi
                            case 0x5f: // pop edi
                                pOrigCode[indexOrig] = p;
                                pHookCode[indexHook] = p;
                                indexOrig += 1;
                                indexHook += 1;
                                lastInstJump = false;
                                break;
                            case 0x6a: // push imm8
                                pOrigCode[indexOrig+0] = p;
                                pOrigCode[indexOrig+1] = pCode[indexOrig+1];
                                pHookCode[indexHook+0] = p;
                                pHookCode[indexHook+1] = pCode[indexOrig+1];
                                indexOrig += 2;
                                indexHook += 2;
                                lastInstJump = false;
                                break;
                            case 0x81:
                                if ( 0xec == pCode[indexOrig+1] )
                                {
                                    // sub esp, imm32
                                    pOrigCode[indexOrig+0] = p;
                                    pOrigCode[indexOrig+1] = pCode[indexOrig+1];
                                    pOrigCode[indexOrig+2] = pCode[indexOrig+2];
                                    pOrigCode[indexOrig+3] = pCode[indexOrig+3];
                                    pOrigCode[indexOrig+4] = pCode[indexOrig+4];
                                    pOrigCode[indexOrig+5] = pCode[indexOrig+5];
                                    pHookCode[indexHook+0] = p;
                                    pHookCode[indexHook+1] = pCode[indexOrig+1];
                                    pHookCode[indexHook+2] = pCode[indexOrig+2];
                                    pHookCode[indexHook+3] = pCode[indexOrig+3];
                                    pHookCode[indexHook+4] = pCode[indexOrig+4];
                                    pHookCode[indexHook+5] = pCode[indexOrig+5];
                                    indexOrig += 6;
                                    indexHook += 6;
                                    lastInstJump = false;
                                }
                                else
                                {
                                    assert( false );
                                }
                                break;
                            case 0x83:
                                if ( 0xec == pCode[indexOrig+1] )
                                {
                                    // sub esp, imm8
                                    pOrigCode[indexOrig+0] = p;
                                    pOrigCode[indexOrig+1] = pCode[indexOrig+1];
                                    pOrigCode[indexOrig+2] = pCode[indexOrig+2];
                                    pHookCode[indexHook+0] = p;
                                    pHookCode[indexHook+1] = pCode[indexOrig+1];
                                    pHookCode[indexHook+2] = pCode[indexOrig+2];
                                    indexOrig += 3;
                                    indexHook += 3;
                                    lastInstJump = false;
                                }
                                else
                                {
                                    assert( false );
                                }
                                break;
                            case 0x89:
                                if (
                                    (0x89 == pCode[indexOrig+1])
                                    || (0x8b == pCode[indexOrig+1])
                                )
                                {
                                    if (
                                        (0x4c == pCode[indexOrig+2])
                                        && (0x24 == pCode[indexOrig+3])
                                    )
                                    {
                                        // 89 4c 24: mov [rsp+imm8],rcx
                                        // 8b 4c 24: mov rcx,[rsp+imm8]
                                        pOrigCode[indexOrig+0] = p;
                                        pOrigCode[indexOrig+1] = pCode[indexOrig+1];
                                        pOrigCode[indexOrig+2] = pCode[indexOrig+2];
                                        pOrigCode[indexOrig+3] = pCode[indexOrig+3];
                                        pOrigCode[indexOrig+4] = pCode[indexOrig+4];
                                        pHookCode[indexHook+0] = p;
                                        pHookCode[indexHook+1] = pCode[indexOrig+1];
                                        pHookCode[indexHook+2] = pCode[indexOrig+2];
                                        pHookCode[indexHook+3] = pCode[indexOrig+3];
                                        pHookCode[indexHook+4] = pCode[indexOrig+4];
                                        indexOrig += 5;
                                        indexHook += 5;
                                        lastInstJump = false;
                                    }
                                    else
                                    {
                                        assert( false );
                                    }
                                }
                                else
                                {
                                    assert( false );
                                }
                                break;
                            case 0x8b:
                                if (
                                    (0xff == pCode[indexOrig+1]) // mov edi,edi
                                    || (0xec == pCode[indexOrig+1]) // mov ebp, esp
                                )
                                {
                                    pOrigCode[indexOrig+0] = p;
                                    pOrigCode[indexOrig+1] = pCode[indexOrig+1];
                                    pHookCode[indexHook+0] = p;
                                    pHookCode[indexHook+1] = pCode[indexOrig+1];
                                    indexOrig += 2;
                                    indexHook += 2;
                                    lastInstJump = false;
                                }
                                else
                                if (
                                    (0x45 == pCode[indexOrig+1]) // mov eax,[ebp+imm8]
                                )
                                {
                                    pOrigCode[indexOrig+0] = p;
                                    pOrigCode[indexOrig+1] = pCode[indexOrig+1];
                                    pOrigCode[indexOrig+2] = pCode[indexOrig+2];
                                    pHookCode[indexHook+0] = p;
                                    pHookCode[indexHook+1] = pCode[indexOrig+1];
                                    pHookCode[indexHook+2] = pCode[indexOrig+2];
                                    indexOrig += 3;
                                    indexHook += 3;
                                    lastInstJump = false;
                                }
                                else
                                if (
                                    (0x74 == pCode[indexOrig+1])
                                    && (0x24 == pCode[indexOrig+2])
                                )
                                {
                                    // mov esi,[esp+imm8]
                                    pOrigCode[indexOrig+0] = p;
                                    pOrigCode[indexOrig+1] = pCode[indexOrig+1];
                                    pOrigCode[indexOrig+2] = pCode[indexOrig+2];
                                    pOrigCode[indexOrig+3] = pCode[indexOrig+3];
                                    pHookCode[indexHook+0] = p;
                                    pHookCode[indexHook+1] = pCode[indexOrig+1];
                                    pHookCode[indexHook+2] = pCode[indexOrig+2];
                                    pHookCode[indexHook+3] = pCode[indexOrig+3];
                                    indexOrig += 4;
                                    indexHook += 4;
                                    lastInstJump = false;
                                }
                                else
                                {
                                    assert( false );
                                }
                                break;
                            case 0xeb:
                                {
                                    // jmp rel8
                                    pOrigCode[indexOrig+0] = p;
                                    pOrigCode[indexOrig+1] = pCode[indexOrig+1];
                                    pHookCode[indexHook+0] = 0xe9;
                                    DWORD* pAddr = reinterpret_cast<DWORD*>(&pHookCode[indexHook+1]);
                                    const LPBYTE addr = reinterpret_cast<LPBYTE>(&pCode[indexOrig+2] + *(char*)(&pCode[indexOrig+1]));
                                    const LPBYTE pAddrBase = reinterpret_cast<LPBYTE>(&pHookCode[indexHook+sizeof(HookJump)]);
                                    *pAddr = static_cast<DWORD>(addr - pAddrBase);
                                    indexOrig += 2;
                                    indexHook += sizeof(HookJump);
                                }
                                break;
                            case 0xe9:
                                {
                                    // jmp rel32
                                    pOrigCode[indexOrig+0] = p;
                                    pOrigCode[indexOrig+1] = pCode[indexOrig+1];
                                    pOrigCode[indexOrig+2] = pCode[indexOrig+2];
                                    pOrigCode[indexOrig+3] = pCode[indexOrig+3];
                                    pOrigCode[indexOrig+4] = pCode[indexOrig+4];

#if defined(_M_IX86)
                                    pHookCode[indexHook+0] = 0xe9;
                                    DWORD* pAddr = reinterpret_cast<DWORD*>(&pHookCode[indexHook+1]);
                                    const LPBYTE addr = reinterpret_cast<LPBYTE>(&pCode[indexOrig+sizeof(HookJump)] + *((LONG*)&pCode[indexOrig+1]));
                                    const LPBYTE pAddrBase = reinterpret_cast<LPBYTE>(&pHookCode[indexHook+sizeof(HookJump)]);
                                    *pAddr = addr - pAddrBase;
                                    indexOrig += sizeof(HookJump);
                                    indexHook += sizeof(HookJump);
#endif // defined(_M_IX86)
#if defined(_M_X64)
                                    memcpy( &pHookCode[indexHook], &sLongJump, sizeof(sLongJump) );
                                    LongJump* pAddrJump = reinterpret_cast<LongJump*>(&pHookCode[indexHook]);
                                    const LPBYTE addr = reinterpret_cast<LPBYTE>(&pCode[indexOrig+sizeof(HookJump)] + *((LONG*)&pCode[indexOrig+1]));
                                    pAddrJump->absAddr = reinterpret_cast<DWORD64>(addr);
                                    indexOrig += sizeof(HookJump);
                                    indexHook += sizeof(sLongJump);
#endif // defined(_M_X64)

                                    if ( indexOrig == sUserStaticFunc[indexOperation+1] )
                                    {
                                        indexPatch = indexOrig - sizeof(HookJump);
                                    }
                                }
                                break;
                            case 0xcc:
                                // int 3
                                {
                                    char temp[128];
                                    ::wsprintfA( temp, "hook-usercpp: Detect Breakpoint of Debugger. operation=%u\n", indexOperation );
                                    ::OutputDebugStringA( temp );
                                }
                                break;
                            case 0xff:
                                if ( 0x75 == pCode[indexOrig+1] )
                                {
                                    // push [ebp+imm8]
                                    pOrigCode[indexOrig+0] = p;
                                    pOrigCode[indexOrig+1] = pCode[indexOrig+1];
                                    pOrigCode[indexOrig+2] = pCode[indexOrig+2];
                                    pHookCode[indexHook+0] = p;
                                    pHookCode[indexHook+1] = pCode[indexOrig+1];
                                    pHookCode[indexHook+2] = pCode[indexOrig+2];
                                    indexOrig += 3;
                                    indexHook += 3;
                                    lastInstJump = false;
                                }
                                else
                                if ( 0x15 == pCode[indexOrig+1] )
                                {
                                    // call imm32
                                    pOrigCode[indexOrig+0] = p;
                                    pOrigCode[indexOrig+1] = pCode[indexOrig+1];
                                    pOrigCode[indexOrig+2] = pCode[indexOrig+2];
                                    pOrigCode[indexOrig+3] = pCode[indexOrig+3];
                                    pOrigCode[indexOrig+4] = pCode[indexOrig+4];
                                    pOrigCode[indexOrig+5] = pCode[indexOrig+5];
                                    pHookCode[indexHook+0] = p;
                                    pHookCode[indexHook+1] = pCode[indexOrig+1];
                                    pHookCode[indexHook+2] = pCode[indexOrig+2];
                                    pHookCode[indexHook+3] = pCode[indexOrig+3];
                                    pHookCode[indexHook+4] = pCode[indexOrig+3];
                                    pHookCode[indexHook+5] = pCode[indexOrig+4];
                                    indexOrig += 6;
                                    indexHook += 6;
                                    lastInstJump = false;
                                }
                                else
                                {
                                    assert( false );
                                }
                                break;
                            default:
                                assert( false );
                                break;
                            }
                        }
                        for ( size_t index = indexOrig; index < sizeof(pTrampoline->origCode); ++index )
                        {
                            pOrigCode[index] = 0xcc;
                        }

                        if ( lastInstJump )
                        {
                        }
                        else
                        {
#if defined(_M_IX86)
                            pHookCode[indexHook] = sHookJump.opJmp;
                            DWORD* pAddr = reinterpret_cast<DWORD*>(&pHookCode[indexHook+1]);
                            const LPBYTE addr = reinterpret_cast<LPBYTE>(&pCode[indexPatch+0+sizeof(HookJump)]);
                            const LPBYTE pAddrBase = reinterpret_cast<LPBYTE>(&pHookCode[indexHook+sizeof(HookJump)]);
                            *pAddr = addr - pAddrBase;
                            indexHook += sizeof(HookJump);
#endif defined(_M_IX86)
#if defined(_M_X64)
                            memcpy( &pHookCode[indexHook], &sLongJump, sizeof(sLongJump) );
                            LongJump* pAddrJump = reinterpret_cast<LongJump*>(&pHookCode[indexHook]);
                            pAddrJump->absAddr = reinterpret_cast<DWORD64>(&pCode[indexOrig]);
                            indexHook += sizeof(sLongJump);
#endif // defined(_M_X64)

                        }
                        for ( size_t index = indexHook; index < sizeof(pTrampoline->hookCode); ++index )
                        {
                            pHookCode[index] = 0xcc;
                        }

#if defined(_M_IX86)
                        {
                            pCode[indexPatch+0] = sHookJump.opJmp;
                            DWORD* pAddr = reinterpret_cast<DWORD*>(&pCode[indexPatch+1]);
                            const LPBYTE pAddrBase = reinterpret_cast<const LPBYTE>(&pCode[indexPatch+0+sizeof(HookJump)]);
                            LPBYTE addr = NULL;
                            switch ( indexOperation )
                            {
                            case MemoryOperationMismatch::kIndexUserStaticFuncNew:
                                addr = reinterpret_cast<LPBYTE>(my_new);
                                break;
                            case MemoryOperationMismatch::kIndexUserStaticFuncNewArray:
                                addr = reinterpret_cast<LPBYTE>(my_new_array);
                                break;
                            case MemoryOperationMismatch::kIndexUserStaticFuncDelete:
                                addr = reinterpret_cast<LPBYTE>(my_delete);
                                break;
                            case MemoryOperationMismatch::kIndexUserStaticFuncDeleteArray:
                                addr = reinterpret_cast<LPBYTE>(my_delete_array);
                                break;
                            case MemoryOperationMismatch::kIndexUserStaticFuncDeleteSize:
                                addr = reinterpret_cast<LPBYTE>(my_delete_size);
                                break;
                            case MemoryOperationMismatch::kIndexUserStaticFuncDeleteArraySize:
                                addr = reinterpret_cast<LPBYTE>(my_delete_array_size);
                                break;
                            case MemoryOperationMismatch::kIndexUserStaticFuncNewArg3:
                                addr = reinterpret_cast<LPBYTE>(my_new_arg3);
                                break;
                            case MemoryOperationMismatch::kIndexUserStaticFuncNewArrayArg3:
                                addr = reinterpret_cast<LPBYTE>(my_new_array_arg3);
                                break;
                            case MemoryOperationMismatch::kIndexUserStaticFuncDeleteArg3:
                                addr = reinterpret_cast<LPBYTE>(my_delete_arg3);
                                break;
                            case MemoryOperationMismatch::kIndexUserStaticFuncDeleteArrayArg3:
                                addr = reinterpret_cast<LPBYTE>(my_delete_array_arg3);
                                break;
                            case MemoryOperationMismatch::kIndexUserStaticFuncNewArg4:
                                addr = reinterpret_cast<LPBYTE>(my_new_arg4);
                                break;
                            case MemoryOperationMismatch::kIndexUserStaticFuncNewArrayArg4:
                                addr = reinterpret_cast<LPBYTE>(my_new_array_arg4);
                                break;
                            case MemoryOperationMismatch::kIndexUserStaticFuncDeleteArg4:
                                addr = reinterpret_cast<LPBYTE>(my_delete_arg4);
                                break;
                            case MemoryOperationMismatch::kIndexUserStaticFuncDeleteArrayArg4:
                                addr = reinterpret_cast<LPBYTE>(my_delete_array_arg4);
                                break;
                            default:
                                assert( false );
                                break;
                            }
                            *pAddr = addr - pAddrBase;
                        }
#endif // defined(_M_IX86)

#if defined(_M_X64)
                        {
                            {
                                pCode[indexPatch+0] = sHookJump.opJmp;
                                DWORD* pAddr = reinterpret_cast<DWORD*>(&pCode[indexPatch+1]);
                                const LPBYTE pAddrBase = reinterpret_cast<const LPBYTE>(&pCode[indexPatch+0+sizeof(HookJump)]);
                                const LPBYTE addr = reinterpret_cast<const LPBYTE>(pTrampoline->longJump);
                                *pAddr = static_cast<DWORD>(addr - pAddrBase);
                            }

                            {
                                memcpy( pTrampoline->longJump, &sLongJump, sizeof(sLongJump) );
                                LongJump* pAddrJump = reinterpret_cast<LongJump*>(pTrampoline->longJump);

                                LPBYTE addr = NULL;
                                switch ( indexOperation )
                                {
                                case MemoryOperationMismatch::kIndexUserStaticFuncNew:
                                    addr = reinterpret_cast<LPBYTE>(my_new);
                                    break;
                                case MemoryOperationMismatch::kIndexUserStaticFuncNewArray:
                                    addr = reinterpret_cast<LPBYTE>(my_new_array);
                                    break;
                                case MemoryOperationMismatch::kIndexUserStaticFuncDelete:
                                    addr = reinterpret_cast<LPBYTE>(my_delete);
                                    break;
                                case MemoryOperationMismatch::kIndexUserStaticFuncDeleteArray:
                                    addr = reinterpret_cast<LPBYTE>(my_delete_array);
                                    break;
                                case MemoryOperationMismatch::kIndexUserStaticFuncDeleteSize:
                                    addr = reinterpret_cast<LPBYTE>(my_delete_size);
                                    break;
                                case MemoryOperationMismatch::kIndexUserStaticFuncDeleteArraySize:
                                    addr = reinterpret_cast<LPBYTE>(my_delete_array_size);
                                    break;
                                case MemoryOperationMismatch::kIndexUserStaticFuncNewArg3:
                                    addr = reinterpret_cast<LPBYTE>(my_new_arg3);
                                    break;
                                case MemoryOperationMismatch::kIndexUserStaticFuncNewArrayArg3:
                                    addr = reinterpret_cast<LPBYTE>(my_new_array_arg3);
                                    break;
                                case MemoryOperationMismatch::kIndexUserStaticFuncDeleteArg3:
                                    addr = reinterpret_cast<LPBYTE>(my_delete_arg3);
                                    break;
                                case MemoryOperationMismatch::kIndexUserStaticFuncDeleteArrayArg3:
                                    addr = reinterpret_cast<LPBYTE>(my_delete_array_arg3);
                                    break;
                                case MemoryOperationMismatch::kIndexUserStaticFuncNewArg4:
                                    addr = reinterpret_cast<LPBYTE>(my_new_arg4);
                                    break;
                                case MemoryOperationMismatch::kIndexUserStaticFuncNewArrayArg4:
                                    addr = reinterpret_cast<LPBYTE>(my_new_array_arg4);
                                    break;
                                case MemoryOperationMismatch::kIndexUserStaticFuncDeleteArg4:
                                    addr = reinterpret_cast<LPBYTE>(my_delete_arg4);
                                    break;
                                case MemoryOperationMismatch::kIndexUserStaticFuncDeleteArrayArg4:
                                    addr = reinterpret_cast<LPBYTE>(my_delete_array_arg4);
                                    break;
                                default:
                                    assert( false );
                                    break;
                                }
                                pAddrJump->absAddr = reinterpret_cast<DWORD64>(addr);
                            }
                        }
#endif // defined(_M_X64)

                        {
                            for ( size_t index = indexPatch + sizeof(HookJump); index < indexOrig; ++index )
                            {
                                pCode[index] = 0x90;
                            }
                        }

                        switch ( indexOperation )
                        {
                        case MemoryOperationMismatch::kIndexUserStaticFuncNew:
                            pfn_new = reinterpret_cast<PFN_new>(pHookCode);
                            break;
                        case MemoryOperationMismatch::kIndexUserStaticFuncNewArray:
                            pfn_new_array = reinterpret_cast<PFN_new_array>(pHookCode);
                            break;
                        case MemoryOperationMismatch::kIndexUserStaticFuncDelete:
                            pfn_delete = reinterpret_cast<PFN_delete>(pHookCode);
                            break;
                        case MemoryOperationMismatch::kIndexUserStaticFuncDeleteArray:
                            pfn_delete_array = reinterpret_cast<PFN_delete_array>(pHookCode);
                            break;
                        case MemoryOperationMismatch::kIndexUserStaticFuncDeleteSize:
                            pfn_delete_size = reinterpret_cast<PFN_delete_size>(pHookCode);
                            break;
                        case MemoryOperationMismatch::kIndexUserStaticFuncDeleteArraySize:
                            pfn_delete_array_size = reinterpret_cast<PFN_delete_array_size>(pHookCode);
                            break;
                        case MemoryOperationMismatch::kIndexUserStaticFuncNewArg3:
                            pfn_new_arg3 = reinterpret_cast<PFN_new_arg3>(pHookCode);
                            break;
                        case MemoryOperationMismatch::kIndexUserStaticFuncNewArrayArg3:
                            pfn_new_array_arg3 = reinterpret_cast<PFN_new_array_arg3>(pHookCode);
                            break;
                        case MemoryOperationMismatch::kIndexUserStaticFuncDeleteArg3:
                            pfn_delete_arg3 = reinterpret_cast<PFN_delete_arg3>(pHookCode);
                            break;
                        case MemoryOperationMismatch::kIndexUserStaticFuncDeleteArrayArg3:
                            pfn_delete_array_arg3 = reinterpret_cast<PFN_delete_array_arg3>(pHookCode);
                            break;
                        case MemoryOperationMismatch::kIndexUserStaticFuncNewArg4:
                            pfn_new_arg4 = reinterpret_cast<PFN_new_arg4>(pHookCode);
                            break;
                        case MemoryOperationMismatch::kIndexUserStaticFuncNewArrayArg4:
                            pfn_new_array_arg4 = reinterpret_cast<PFN_new_array_arg4>(pHookCode);
                            break;
                        case MemoryOperationMismatch::kIndexUserStaticFuncDeleteArg4:
                            pfn_delete_arg4 = reinterpret_cast<PFN_delete_arg4>(pHookCode);
                            break;
                        case MemoryOperationMismatch::kIndexUserStaticFuncDeleteArrayArg4:
                            pfn_delete_array_arg4 = reinterpret_cast<PFN_delete_array_arg4>(pHookCode);
                            break;
                        }

                        hookutil::TrampolineHeader* pHeader = &(pTrampoline->uh.header);
                        pHeader->origSize = static_cast<WORD>(sUserStaticFunc[indexOperation+1]);
                        pHeader->origCodeSize = static_cast<BYTE>(indexOrig);
                        pHeader->hookCodeSize = static_cast<BYTE>(indexHook);

                    }
                }

                // restore protect
                {
                    const BYTE* pAddr = pAddrMin;
                    for ( size_t index = 0; index < pageCount; ++index, pAddr += pageSize )
                    {
                        if ( PAGE_EXECUTE_READWRITE != protect[index] )
                        {
                            DWORD   dwProtect;
                            const BOOL BRet = ::VirtualProtect( (LPVOID)pAddr, (size_t)pageSize, protect[index], &dwProtect );
                            if ( !BRet )
                            {
                                const DWORD dwErr = ::GetLastError();
                            }
                            else
                            {
                            }
                        }
                    }
                }
            }

            if ( NULL != protect )
            {
                const BOOL BRet = ::HeapFree( hHeap, 0, protect );
                if ( !BRet )
                {
                    const DWORD dwErr = ::GetLastError();
                }
                else
                {
                    protect = NULL;
                }
            }

            ::OutputDebugStringA("");
        }

        if ( NULL != hHeap )
        {
            const BOOL BRet = ::HeapDestroy( hHeap );
            if ( !BRet )
            {
                const DWORD dwErr = ::GetLastError();
            }
            else
            {
                hHeap = NULL;
            }
        }
    }

    hookutil::tranpolinePageDropWriteOperation( sPageTrampoline, (const DWORD)pageSize );

    return true;
}

static
bool
unhookUserCPP( void )
{
    if ( NULL == sModule )
    {
        return false;
    }

    const DWORD64   pageSize = hookutil::getPageSize();

    assert( sizeof(size_t) == sizeof(void*) );

    for ( size_t indexOperation = 0; indexOperation < MemoryOperationMismatch::kIndexUserStaticFuncMAX; indexOperation += 2 )
    {
        DWORD64     minOffset = -1;
        DWORD64     maxOffset = 0;

        {
            minOffset = sUserStaticFunc[indexOperation+0];
            maxOffset = minOffset + sizeof(sHookJump);
        }

        HANDLE hHeap = ::HeapCreate( 0, 0, 0 );
        if ( NULL != hHeap )
        {
            const DWORD64   minPageCount = minOffset / pageSize;
            const DWORD64   maxPageCount = (maxOffset + (pageSize-1)) / pageSize;

            const size_t    pageCount = static_cast<const size_t>( (maxPageCount - minPageCount) );

            const BYTE* pAddrBase = reinterpret_cast<const BYTE*>(sModule);
            const BYTE* pAddrMin = pAddrBase + pageSize*minPageCount;
            const BYTE* pAddrMax = pAddrBase + pageSize*maxPageCount;

            DWORD*   protect = reinterpret_cast<DWORD*>( ::HeapAlloc( hHeap, 0, sizeof(DWORD) * pageCount ) );
            if ( NULL != protect )
            {
                ZeroMemory( protect, sizeof(DWORD) * pageCount );

                // query protect
                {
                    MEMORY_BASIC_INFORMATION    mbi;
                    const BYTE* pAddr = pAddrMin;
                    for ( size_t index = 0; index < pageCount; ++index, pAddr += pageSize )
                    {
                        const size_t nRet = ::VirtualQuery( pAddr, &mbi, sizeof(mbi) );
                        if ( 0 == nRet )
                        {
                            const DWORD dwErr = ::GetLastError();
                        }
                        else
                        {
                            protect[index] = mbi.Protect;
                        }
                    }
                }
                // change protect
                {
                    const BYTE* pAddr = pAddrMin;
                    for ( size_t index = 0; index < pageCount; ++index, pAddr += pageSize )
                    {
                        if ( PAGE_EXECUTE_READWRITE != protect[index] )
                        {
                            DWORD   dwProtect;
                            const BOOL BRet = ::VirtualProtect( (LPVOID)pAddr, (size_t)pageSize, PAGE_EXECUTE_READWRITE, &dwProtect );
                            if ( !BRet )
                            {
                                const DWORD dwErr = ::GetLastError();
                            }
                            else
                            {
                                assert( dwProtect == protect[index] );
                                protect[index] = dwProtect;
                            }
                        }
                    }
                }

                {
                    assert( sizeof(size_t) == sizeof(void*) );
                    BYTE*   p = reinterpret_cast<BYTE*>(sModule);

                    if ( 0 != sUserStaticFunc[indexOperation+0] )
                    {
                        BYTE* pCode = reinterpret_cast<BYTE*>(p + sUserStaticFunc[indexOperation+0]);
                        hookutil::Trampoline* pTrampoline = reinterpret_cast<hookutil::Trampoline*>((LPBYTE)sPageTrampoline + sizeof(hookutil::Trampoline)*indexOperation/2);
                        BYTE* pOrigCode = pTrampoline->origCode;
                        BYTE* pHookCode = pTrampoline->hookCode;
                        hookutil::TrampolineHeader* pHeader = &(pTrampoline->uh.header);

                        for ( size_t index = 0; index < pHeader->origCodeSize; ++index )
                        {
                            pCode[index] = pOrigCode[index];
                        }
                    }
                }

                // restore protect
                {
                    const BYTE* pAddr = pAddrMin;
                    for ( size_t index = 0; index < pageCount; ++index, pAddr += pageSize )
                    {
                        if ( PAGE_EXECUTE_READWRITE != protect[index] )
                        {
                            DWORD   dwProtect;
                            const BOOL BRet = ::VirtualProtect( (LPVOID)pAddr, (size_t)pageSize, protect[index], &dwProtect );
                            if ( !BRet )
                            {
                                const DWORD dwErr = ::GetLastError();
                            }
                            else
                            {
                            }
                        }
                    }
                }
            }

            if ( NULL != protect )
            {
                const BOOL BRet = ::HeapFree( hHeap, 0, protect );
                if ( !BRet )
                {
                    const DWORD dwErr = ::GetLastError();
                }
                else
                {
                    protect = NULL;
                }
            }

            ::OutputDebugStringA("");
        }

        if ( NULL != hHeap )
        {
            const BOOL BRet = ::HeapDestroy( hHeap );
            if ( !BRet )
            {
                const DWORD dwErr = ::GetLastError();
            }
            else
            {
                hHeap = NULL;
            }
        }
    }


    pfn_new = NULL;
    pfn_delete = NULL;
    pfn_new_array = NULL;
    pfn_delete_array = NULL;
    pfn_delete_size = NULL;
    pfn_delete_array_size = NULL;
    pfn_new_arg3 = NULL;
    pfn_delete_arg3 = NULL;
    pfn_new_array_arg3 = NULL;
    pfn_delete_array_arg3 = NULL;
    pfn_new_arg4 = NULL;
    pfn_delete_arg4 = NULL;
    pfn_new_array_arg4 = NULL;
    pfn_delete_array_arg4 = NULL;

    hookutil::trampolinePageDeallocate( sPageTrampoline );
    sPageTrampoline = NULL;

    return true;
}



bool
hookMemoryOperationMismatchUserCPP( const HMODULE hModule, MemoryOperationMismatchClient* pMOM )
{
    bool result = true;
    {
        const bool bRet = pMOM->getUserStaticFunc( sUserStaticFunc );
        if ( !bRet )
        {
            result = false;
        }
        else
        {
            sMemoryOperationMismatch = pMOM;
        }
    }

    if ( result )
    {
        const bool bRet = hookUserCPP( hModule );
        if ( !bRet )
        {
            result = false;
        }
        else
        {
            sModule = hModule;
        }
    }

    return result;
}


bool
unhookMemoryOperationMismatchUserCPP( void )
{
    bool result = true;
    {
        const bool bRet = unhookUserCPP();
        if ( !bRet )
        {
            result = false;
        }
    }

    sMemoryOperationMismatch = NULL;
    sModule = NULL;

    return result;
}



} // namespace checker

} // namespace kk
