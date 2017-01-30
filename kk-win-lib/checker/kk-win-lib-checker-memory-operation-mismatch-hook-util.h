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

#pragma once

namespace kk
{

namespace checker
{

namespace hookutil
{

const DWORD
getPageSize( void );

LPVOID
trampolinePageAllocate( LPVOID minAddr, LPVOID maxAddr );

bool
trampolinePageDropWriteOperation( LPVOID pTrampolinePage, const DWORD changeSize );

bool
trampolinePageDeallocate( LPVOID pTrampolinePage );


#include <pshpack1.h>
struct TrampolineHeader
{
    WORD    origSize;
    BYTE    origCodeSize;
    BYTE    hookCodeSize;
};


struct Trampoline
{
    union
    {
        BYTE                data[0x10];
        TrampolineHeader    header;
    } uh;
    BYTE    origCode[0x10];
    BYTE    hookCode[0x20];

    BYTE    longJump[0x20];
};
#include <poppack.h>


} // namespace hookutil

} // namespace checker

} // namespace kk

