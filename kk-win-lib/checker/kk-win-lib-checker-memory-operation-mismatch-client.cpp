/**
 * The MIT License
 * 
 * Copyright (C) 2016 Kiyofumi Kondoh
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

#include "../kk-win-lib-namedpipe.h"
#include "kk-win-lib-checker-memory-operation-mismatch.h"
#include "../kk-win-lib-hook-local.h"
#include "kk-win-lib-checker-memory-operation-mismatch-client.h"

#include "kk-win-lib-checker-memory-operation-mismatch-hook-iat.h"
#include "kk-win-lib-checker-memory-operation-mismatch-hook-crtnewaop.h"
#include "kk-win-lib-checker-memory-operation-mismatch-hook-crtcpp.h"
#include "kk-win-lib-checker-memory-operation-mismatch-hook-usercpp.h"


#include "kk-win-lib-checker-memory-operation-mismatch-internal.h"

#include <assert.h>


namespace kk
{

namespace checker
{

MemoryOperationMismatchClient::MemoryOperationMismatchClient()
{
    ZeroMemory( mCRTOffsetIAT, sizeof(mCRTOffsetIAT) );
    ZeroMemory( mCRTStaticFunc, sizeof(mCRTStaticFunc) );
    ZeroMemory( mUserStaticFunc, sizeof(mUserStaticFunc) );
    mCRTisStaticLinked = false;
    mUseHookIAT = true;
    mUseHookCRTNewArray = true;
    mUseHookCRTCPP = true;
    mUseHookUserCPP = true;
}

MemoryOperationMismatchClient::~MemoryOperationMismatchClient()
{
    this->term();
}

bool
MemoryOperationMismatchClient::term(void)
{
    bool result = false;

    result = MemoryOperationMismatch::term();

    {
        const bool bRet = mHookLocal.grantWriteOperation();
    }

    if ( mUseHookUserCPP )
    {
        const bool bRet = unhookMemoryOperationMismatchUserCPP();
    }
    if ( mUseHookCRTCPP && mCRTisStaticLinked )
    {
        const bool bRet = unhookMemoryOperationMismatchCRTCPP();
    }
    if ( mUseHookCRTNewArray )
    {
        const bool bRet = unhookMemoryOperationMismatchCRTNewAOP();
    }

    if ( mUseHookIAT )
    {
        const bool bRet = unhookMemoryOperationMismatchIAT();
    }

    ZeroMemory( mCRTOffsetIAT, sizeof(mCRTOffsetIAT) );
    ZeroMemory( mCRTStaticFunc, sizeof(mCRTStaticFunc) );
    ZeroMemory( mUserStaticFunc, sizeof(mUserStaticFunc) );
    mCRTisStaticLinked = false;
    mUseHookIAT = true;
    mUseHookCRTNewArray = true;
    mUseHookCRTCPP = true;
    mUseHookUserCPP = true;

    {
        const bool bRet = mHookLocal.term();
    }

    return result;
}


bool
MemoryOperationMismatchClient::init(const bool actionDoBreak)
{
    this->term();

    mDoBreak = actionDoBreak;

    this->mNamedPipe.openClient( szNamedPiepeName, kk::NamedPipe::kModeFullDuplex );

    return true;
}

bool
MemoryOperationMismatchClient::sendProcessId( const DWORD processId )
{
    bool result = false;
    {
        packetProcessId     packet;
        ZeroMemory( &packet, sizeof(packet) );

        packet.header.size = sizeof(packet.data);
        packet.header.mode = kModeProcessId;

        packet.data.processId = processId;

        size_t sendedSize = 0;
        const bool bRet = this->mNamedPipe.send( (const char*)&packet, sizeof(packet), sendedSize );
        if ( !bRet )
        {
            return false;
        }
    }

    {
        packetCRTModule     module;
        ZeroMemory( &module, sizeof(module) );
        size_t recvedSize = 0;
        const bool bRet = this->mNamedPipe.recv( (char *)&module, sizeof(module), recvedSize );
        result = bRet;
        if ( bRet )
        {
            mCRTOffsetIAT[kIndexOperationMalloc]    = module.data.dwMalloc;
            mCRTOffsetIAT[kIndexOperationFree]      = module.data.dwFree;
            mCRTOffsetIAT[kIndexOperationCalloc]    = module.data.dwCalloc;
            mCRTOffsetIAT[kIndexOperationRealloc]   = module.data.dwRealloc;
            mCRTOffsetIAT[kIndexOperationStrdup]   = module.data.dwStrdup;
            mCRTOffsetIAT[kIndexOperationWcsdup]   = module.data.dwWcsdup;
            mCRTOffsetIAT[kIndexOperationReCalloc]  = module.data.dwReCalloc;
            mCRTOffsetIAT[kIndexOperationExpand]    = module.data.dwExpand;
            mCRTOffsetIAT[kIndexOperationNew]           = module.data.dwNew;
            mCRTOffsetIAT[kIndexOperationDelete]        = module.data.dwDelete;
            mCRTOffsetIAT[kIndexOperationNewArray]      = module.data.dwNewArray;
            mCRTOffsetIAT[kIndexOperationDeleteArray]   = module.data.dwDeleteArray;
            mCRTOffsetIAT[kIndexOperationAlignedMalloc]     = module.data.dwAlignedMalloc;
            mCRTOffsetIAT[kIndexOperationAlignedFree]       = module.data.dwAlignedFree;
            mCRTOffsetIAT[kIndexOperationAlignedReCalloc]   = module.data.dwAlignedReCalloc;
            mCRTOffsetIAT[kIndexOperationAlignedRealloc]    = module.data.dwAlignedRealloc;

            mCRTStaticFunc[kIndexCRTStaticFuncNew]                  = module.func.dwCRTStaticNew;
            mCRTStaticFunc[kIndexCRTStaticFuncNewLength]            = module.func.dwCRTStaticNewLength;
            mCRTStaticFunc[kIndexCRTStaticFuncDelete]               = module.func.dwCRTStaticDelete;
            mCRTStaticFunc[kIndexCRTStaticFuncDeleteLength]         = module.func.dwCRTStaticDeleteLength;
            mCRTStaticFunc[kIndexCRTStaticFuncNewArray]             = module.func.dwCRTStaticNewArray;
            mCRTStaticFunc[kIndexCRTStaticFuncNewArrayLength]       = module.func.dwCRTStaticNewArrayLength;
            mCRTStaticFunc[kIndexCRTStaticFuncDeleteArray]          = module.func.dwCRTStaticDeleteArray;
            mCRTStaticFunc[kIndexCRTStaticFuncDeleteArrayLength]    = module.func.dwCRTStaticDeleteArrayLength;

            mCRTStaticFunc[kIndexCRTStaticFuncDeleteSize]               = module.func.dwCRTStaticDeleteSize;
            mCRTStaticFunc[kIndexCRTStaticFuncDeleteSizeLength]         = module.func.dwCRTStaticDeleteSizeLength;
            mCRTStaticFunc[kIndexCRTStaticFuncDeleteArraySize]          = module.func.dwCRTStaticDeleteArraySize;
            mCRTStaticFunc[kIndexCRTStaticFuncDeleteArraySizeLength]    = module.func.dwCRTStaticDeleteArraySizeLength;

            mCRTisStaticLinked = module.data.moduleInfo.info.isCRTStaticLinked;

            mUserStaticFunc[kIndexUserStaticFuncNew]                  = module.user.dwUserStaticNew;
            mUserStaticFunc[kIndexUserStaticFuncNewLength]            = module.user.dwUserStaticNewLength;
            mUserStaticFunc[kIndexUserStaticFuncDelete]               = module.user.dwUserStaticDelete;
            mUserStaticFunc[kIndexUserStaticFuncDeleteLength]         = module.user.dwUserStaticDeleteLength;
            mUserStaticFunc[kIndexUserStaticFuncNewArray]             = module.user.dwUserStaticNewArray;
            mUserStaticFunc[kIndexUserStaticFuncNewArrayLength]       = module.user.dwUserStaticNewArrayLength;
            mUserStaticFunc[kIndexUserStaticFuncDeleteArray]          = module.user.dwUserStaticDeleteArray;
            mUserStaticFunc[kIndexUserStaticFuncDeleteArrayLength]    = module.user.dwUserStaticDeleteArrayLength;

            mUserStaticFunc[kIndexUserStaticFuncDeleteSize]               = module.user.dwUserStaticDeleteSize;
            mUserStaticFunc[kIndexUserStaticFuncDeleteSizeLength]         = module.user.dwUserStaticDeleteSizeLength;
            mUserStaticFunc[kIndexUserStaticFuncDeleteArraySize]          = module.user.dwUserStaticDeleteArraySize;
            mUserStaticFunc[kIndexUserStaticFuncDeleteArraySizeLength]    = module.user.dwUserStaticDeleteArraySizeLength;

            mUserStaticFunc[kIndexUserStaticFuncNewArg3]                  = module.user.dwUserStaticNewArg3;
            mUserStaticFunc[kIndexUserStaticFuncNewArg3Length]            = module.user.dwUserStaticNewArg3Length;
            mUserStaticFunc[kIndexUserStaticFuncDeleteArg3]               = module.user.dwUserStaticDeleteArg3;
            mUserStaticFunc[kIndexUserStaticFuncDeleteArg3Length]         = module.user.dwUserStaticDeleteArg3Length;
            mUserStaticFunc[kIndexUserStaticFuncNewArrayArg3]             = module.user.dwUserStaticNewArrayArg3;
            mUserStaticFunc[kIndexUserStaticFuncNewArrayArg3Length]       = module.user.dwUserStaticNewArrayArg3Length;
            mUserStaticFunc[kIndexUserStaticFuncDeleteArrayArg3]          = module.user.dwUserStaticDeleteArrayArg3;
            mUserStaticFunc[kIndexUserStaticFuncDeleteArrayArg3Length]    = module.user.dwUserStaticDeleteArrayArg3Length;
            mUserStaticFunc[kIndexUserStaticFuncNewArg4]                  = module.user.dwUserStaticNewArg4;
            mUserStaticFunc[kIndexUserStaticFuncNewArg4Length]            = module.user.dwUserStaticNewArg4Length;
            mUserStaticFunc[kIndexUserStaticFuncDeleteArg4]               = module.user.dwUserStaticDeleteArg4;
            mUserStaticFunc[kIndexUserStaticFuncDeleteArg4Length]         = module.user.dwUserStaticDeleteArg4Length;
            mUserStaticFunc[kIndexUserStaticFuncNewArrayArg4]             = module.user.dwUserStaticNewArrayArg4;
            mUserStaticFunc[kIndexUserStaticFuncNewArrayArg4Length]       = module.user.dwUserStaticNewArrayArg4Length;
            mUserStaticFunc[kIndexUserStaticFuncDeleteArrayArg4]          = module.user.dwUserStaticDeleteArrayArg4;
            mUserStaticFunc[kIndexUserStaticFuncDeleteArrayArg4Length]    = module.user.dwUserStaticDeleteArrayArg4Length;

        }
    }

    {
        DWORD64 minOffset = (DWORD64)-1;
        DWORD64 maxOffset = 0;
        size_t hookCount = 0;

        {
            for ( size_t index = 0; index < kIndexCRTStaticFuncMAX; index += 2 )
            {
                const DWORD64 dwOffset = mCRTStaticFunc[index+0];
                const DWORD64 dwSize   = mCRTStaticFunc[index+1];

                if ( 0 != dwOffset || 0 != dwSize )
                {
                    hookCount += 1;
                }

                if ( dwOffset < minOffset )
                {
                    minOffset = dwOffset;
                }
                if ( maxOffset < dwOffset )
                {
                    maxOffset = dwOffset;
                }
            }

            for ( size_t index = 0; index < kIndexUserStaticFuncMAX; index += 2 )
            {
                const DWORD64 dwOffset = mUserStaticFunc[index+0];
                const DWORD64 dwSize   = mUserStaticFunc[index+1];

                if ( 0 != dwOffset || 0 != dwSize )
                {
                    hookCount += 1;
                }

                if ( dwOffset < minOffset )
                {
                    minOffset = dwOffset;
                }
                if ( maxOffset < dwOffset )
                {
                    maxOffset = dwOffset;
                }
            }
        }

        const LPBYTE moduleBase = reinterpret_cast<const LPBYTE>(::GetModuleHandleA( NULL ));
        const LPBYTE minAddr = moduleBase + minOffset;
        const LPBYTE maxAddr = moduleBase + maxOffset;

        const bool bRet = mHookLocal.init( minAddr, maxAddr, hookCount );
        result = bRet;

    }


    if ( mUseHookUserCPP )
    {
        const HMODULE hModule = ::GetModuleHandleA( NULL );
        const bool bRet = hookMemoryOperationMismatchUserCPP( hModule, this );
        result = bRet;
    }

    if ( mCRTisStaticLinked )
    {
        if ( mUseHookCRTCPP )
        {
            const HMODULE hModule = ::GetModuleHandleA( NULL );
            const bool bRet = hookMemoryOperationMismatchCRTCPP( hModule, this );
            result = bRet;
        }
    }
    else
    {
        if ( mUseHookCRTNewArray )
        {
            const HMODULE hModule = ::GetModuleHandleA( NULL );
            const bool bRet = hookMemoryOperationMismatchCRTNewAOP( hModule, this );
            result = bRet;
        }
    }

    if ( mUseHookIAT )
    {
        const HMODULE hModule = ::GetModuleHandleA( NULL );
        const bool bRet = hookMemoryOperationMismatchIAT( hModule, this );
        result = bRet;
    }


    {
        const bool bRet = mHookLocal.dropWriteOperation();
    }

    {
        const HANDLE hProcess = ::GetCurrentProcess();
        const BOOL BRet = ::FlushInstructionCache( hProcess, NULL, 0 );
        if ( !BRet )
        {
            const DWORD dwErr = ::GetLastError();
        }
    }

    return result;
}


bool
MemoryOperationMismatchClient::sendOperation(
    const enumMemoryOperation operation
    , const DWORD64 pointer
)
{
    bool result = false;
    {
        packetMemoryOperation   packet;
        ZeroMemory( &packet, sizeof(packet) );

        packet.header.size = sizeof(packet.data);
        packet.header.mode = kModeMemoryOperation;

        packet.data.funcMemoryOperation = operation;
        packet.data.pointer = pointer;

        size_t sendedSize = 0;
        const bool bRet = this->mNamedPipe.send( (const char*)&packet, sizeof(packet), sendedSize );
        if ( !bRet )
        {
            return false;
        }
    }

    {
        packetAction    action;
        ZeroMemory( &action, sizeof(action) );
        size_t recvedSize = 0;
        const bool bRet = this->mNamedPipe.recv( (char *)&action, sizeof(action), recvedSize );
        if ( bRet )
        {
            if ( kActionBreak == action.data.action )
            {
                if ( mDoBreak )
                {
                    ::DebugBreak();
                }
            }
        }
        result = bRet;
    }

    return result;
}


bool
MemoryOperationMismatchClient::getCRTOffsetIAT(
    DWORD64 CRTOffsetIAT[MemoryOperationMismatch::kIndexOperationMAX]
) const
{
    for ( size_t index = 0; index < kIndexOperationMAX; ++index )
    {
        CRTOffsetIAT[index] = this->mCRTOffsetIAT[index];
    }

    return true;
}

bool
MemoryOperationMismatchClient::getCRTStaticFunc(
    DWORD64 CRTStaticFunc[MemoryOperationMismatch::kIndexCRTStaticFuncMAX]
) const
{
    for ( size_t index = 0; index < kIndexCRTStaticFuncMAX; ++index )
    {
        CRTStaticFunc[index] = this->mCRTStaticFunc[index];
    }

    return true;
}

bool
MemoryOperationMismatchClient::getCRTisStaticLinked( void ) const
{
    return mCRTisStaticLinked;
}

bool
MemoryOperationMismatchClient::getUserStaticFunc(
    DWORD64 UserStaticFunc[MemoryOperationMismatch::kIndexUserStaticFuncMAX]
) const
{
    for ( size_t index = 0; index < kIndexUserStaticFuncMAX; ++index )
    {
        UserStaticFunc[index] = this->mUserStaticFunc[index];
    }

    return true;
}


bool
MemoryOperationMismatchClient::disableHookIAT( const bool disableHook )
{
    const bool oldValue = mUseHookIAT;

    if ( disableHook )
    {
        mUseHookIAT = false;
    }
    else
    {
        mUseHookIAT = true;
    }

    return oldValue;
}

bool
MemoryOperationMismatchClient::disableHookCRTNewArray( const bool disableHook )
{
    const bool oldValue = mUseHookCRTNewArray;

    if ( disableHook )
    {
        mUseHookCRTNewArray = false;
    }
    else
    {
        mUseHookCRTNewArray = true;
    }

    return oldValue;
}

bool
MemoryOperationMismatchClient::disableHookCRTCPP( const bool disableHook )
{
    const bool oldValue = mUseHookCRTCPP;

    if ( disableHook )
    {
        mUseHookCRTCPP = false;
    }
    else
    {
        mUseHookCRTCPP = true;
    }

    return oldValue;
}

bool
MemoryOperationMismatchClient::disableHookUserCPP( const bool disableHook )
{
    const bool oldValue = mUseHookUserCPP;

    if ( disableHook )
    {
        mUseHookUserCPP = false;
    }
    else
    {
        mUseHookUserCPP = true;
    }

    return oldValue;
}




bool
MemoryOperationMismatchClient::hook( size_t& indexHook, const void* moduleBase, const size_t nOrigOffset, const size_t nOrigSize, const void* hookFunc, void** origFunc )
{
    return mHookLocal.hook( indexHook, moduleBase, nOrigOffset, nOrigSize, hookFunc, origFunc );
}

bool
MemoryOperationMismatchClient::unhook( const size_t indexHook )
{
    return mHookLocal.unhook( indexHook );
}






} // namespace checker

} // namespace kk

