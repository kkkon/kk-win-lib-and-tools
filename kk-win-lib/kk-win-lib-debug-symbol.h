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

class DebugSymbol
{
public:
    DebugSymbol();
    virtual ~DebugSymbol();

public:
    bool
    init( const size_t processId );

    bool
    term( void );

public:
    bool
    findGlobalReplacements( void );

    enum enumIndexOperation
    {
        kIndexOperationNew = 0
        , kIndexOperationDelete = 1
        , kIndexOperationNewArray = 2
        , kIndexOperationDeleteArray = 3

        , kIndexOperationDeleteSize = 4
        , kIndexOperationDeleteArraySize = 5

        , kIndexOperationNewArg3 = 6
        , kIndexOperationDeleteArg3 = 7
        , kIndexOperationNewArrayArg3 = 8
        , kIndexOperationDeleteArrayArg3 = 9
        , kIndexOperationNewArg4 = 10
        , kIndexOperationDeleteArg4 = 11
        , kIndexOperationNewArrayArg4 = 12
        , kIndexOperationDeleteArrayArg4 = 13

        , kIndexOperationMax = 14
    };

    struct FuncInfo
    {
        DWORD64     dwAddr;
        DWORD       size;
        bool        isCRT;
    };

    bool
    getGlobalReplacementsCount( size_t count[kIndexOperationMax] ) const;

    bool
    getGlobalReplacementsRVA( const enumIndexOperation indexOperation, FuncInfo* funcArray ) const;

    bool
    isIncludeCRTNewArray( void ) const;

    bool
    getCRTNewArrayRVA( FuncInfo* funcInfo ) const;

public:
    bool
    setCRTCPPisStaticLinked( const bool isStaticLinked );

    bool
    getCRTCPPisStaticLinked( void ) const;

protected:
    class DebugSymbolImpl;
    DebugSymbolImpl*    mImpl;
    kk::RemoteProcess   mProcess;

private:
    explicit DebugSymbol(const DebugSymbol&);
    DebugSymbol& operator=(const DebugSymbol&);
}; // class DebugSymbol


} // namespace kk

