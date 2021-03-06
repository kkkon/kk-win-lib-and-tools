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

#pragma once

namespace kk
{

namespace checker {

class MemoryOperationMismatchClient : public MemoryOperationMismatch
{
public:
    MemoryOperationMismatchClient();
    virtual ~MemoryOperationMismatchClient();

public:
    virtual
    bool
    init( const bool actionDoBreak );

    virtual
    bool
    term( void );

public:
    bool
    sendProcessId( const DWORD processId );

    bool
    sendOperation( const enumMemoryOperation operation, const DWORD64 pointer );


    bool
    getCRTOffsetIAT( DWORD64 CRTOffsetIAT[kIndexOperationMAX] ) const;

    bool
    getCRTStaticFunc( DWORD64 CRTStaticFunc[kIndexCRTStaticFuncMAX] ) const;

    bool
    getCRTisStaticLinked( void ) const;

    bool
    getUserStaticFunc( DWORD64 UserStaticFunc[kIndexUserStaticFuncMAX] ) const;

protected:
    DWORD64             mCRTOffsetIAT[kIndexOperationMAX];
    DWORD64             mCRTStaticFunc[kIndexCRTStaticFuncMAX];
    bool                mCRTisStaticLinked;
    DWORD64             mUserStaticFunc[kIndexUserStaticFuncMAX];

public:
    bool
    disableHookIAT( const bool disableHook );

    bool
    disableHookCRTNewArray( const bool disableHook );

    bool
    disableHookCRTCPP( const bool disableHook );

    bool
    disableHookUserCPP( const bool disableHook );

protected:
    bool                mUseHookIAT;
    bool                mUseHookCRTNewArray;
    bool                mUseHookCRTCPP;
    bool                mUseHookUserCPP;


public:
    bool
    hook( size_t& indexHook, const void* moduleBase, const size_t nOrigOffset, const size_t nOrigSize, const void* hookFunc, void** origFunc );

    bool
    unhook( const size_t indexHook );

protected:
    kk::HookLocal       mHookLocal;


private:
    explicit MemoryOperationMismatchClient(const MemoryOperationMismatchClient&);
    MemoryOperationMismatchClient& operator=(const MemoryOperationMismatchClient&);
}; // class MemoryOperationMismatchClient


} // namespace checker

} // namespace kk

