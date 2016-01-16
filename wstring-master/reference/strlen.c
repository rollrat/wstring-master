/***

   RollRat Software Project.
   Copyright (C) 2015. rollrat. All Rights Reserved.

File name:
   
   strlen.c

Author:

   12-28-2015:   HyunJun Jeong, Creation

***/

#include <stdint.h>

// Get a pointer type can be sure.
#if defined(__x86_64__) || defined(__ia64__) || defined(_M_AMD64) || defined(_M_IA64) \
    || defined(_WIN64) || defined(__alpha__) || defined(__s390__)
#define _X64_MODE
typedef uint64_t	ptr_type;
#else
typedef uint32_t	ptr_type;
#endif

#define align_address(n) \
    ((ptr_type *)( (ptr_type)(n) & ~(ptr_type)( sizeof(ptr_type) - 1 ) ) + 1)

// find zero in string
static const char* findzero(const char* str)
{
    if (!str[0])
        return str;
    else if (!str[1])
        return str + 1;
    else if (!str[2])
        return str + 2;
    else if (!str[3])
        return str + 3;
    if ( sizeof(ptr_type) == sizeof(uint64_t) )
    {
        if (!str[4])
            return str + 4;
        else if (!str[5])
            return str + 5;
        else if (!str[6])
            return str + 6;
        else if (!str[7])
            return str + 7;
    }
}

/* this function provide counting string-length method by null-terminated string 
   buf is too big, to use this function should be considered prior using.
   or you can make partition routines. */
size_t strlen( str )
    const char* str;
{
    ptr_type* trim;

    // set has zero checker byte
    const ptr_type less_magic = (ptr_type)(~0ULL / 0xff);
    const ptr_type most_magic = (less_magic << 7);

    trim = align_address(str);

    // Routine 1 Find 0 and 128~255 Byte value in pointer type value.
    if ( (*(ptr_type *)str - less_magic) & most_magic )
    {
        if ( (*(ptr_type *)str - less_magic) & (~*(ptr_type *)str & most_magic) )
            return findzero(str) - str;
        goto FIND_BY_HASZERO;
    }

    while ( 1 )
    {
        if ( (trim[0] - less_magic) & most_magic )
            { trim = &trim[0];  goto FIND_BY_HASZERO; }
        if ( (trim[1] - less_magic) & most_magic )
            { trim = &trim[1];  goto FIND_BY_HASZERO; }
        if ( (trim[2] - less_magic) & most_magic )
            { trim = &trim[2];  goto FIND_BY_HASZERO; }
        if ( (trim[3] - less_magic) & most_magic )
            { trim = &trim[3];  goto FIND_BY_HASZERO; }
        if ( sizeof(ptr_type) == sizeof(uint32_t) )
        {
            if ( (trim[4] - less_magic) & most_magic )
                { trim = &trim[4];  goto FIND_BY_HASZERO; }
            if ( (trim[5] - less_magic) & most_magic )
                { trim = &trim[5];  goto FIND_BY_HASZERO; }
            if ( (trim[6] - less_magic) & most_magic )
                { trim = &trim[6];  goto FIND_BY_HASZERO; }
            if ( (trim[7] - less_magic) & most_magic )
                { trim = &trim[7];  goto FIND_BY_HASZERO; }
        }
        trim += (sizeof(uint64_t) + sizeof(uint32_t) - sizeof(ptr_type));
    }

    // Routine 2 Find Zero Byte in pointer type value.
FIND_BY_HASZERO:

    if ( (*trim - less_magic) & (~*trim & most_magic) )
    {
        return findzero((const char *)trim) - str;
    }

    while ( 1 )
    {
        if ( (trim[0] - less_magic) & (~trim[0] & most_magic) )
                return findzero(trim + 0) - str;
        if ( (trim[1] - less_magic) & (~trim[1] & most_magic) )
                return findzero(trim + 1) - str;
        if ( (trim[2] - less_magic) & (~trim[2] & most_magic) )
                return findzero(trim + 2) - str;
        if ( (trim[3] - less_magic) & (~trim[3] & most_magic) )
                return findzero(trim + 3) - str;
        if ( sizeof(ptr_type) == sizeof(uint32_t) )
        {
            if ( (trim[4] - less_magic) & (~trim[4] & most_magic) )
                return findzero(trim + 4) - str;
            if ( (trim[5] - less_magic) & (~trim[5] & most_magic) )
                return findzero(trim + 5) - str;
            if ( (trim[6] - less_magic) & (~trim[6] & most_magic) )
                return findzero(trim + 6) - str;
            if ( (trim[7] - less_magic) & (~trim[7] & most_magic) )
                return findzero(trim + 7) - str;
        }
        trim += (sizeof(uint64_t) + sizeof(uint32_t) - sizeof(ptr_type));
    }
}

#include <malloc.h>
#include <memory.h>
#include <stdio.h>

mainc()
{
#define L1(x) x x x x
#define L2(x) L1(x) L1(x) L1(x) L1(x)
#define L3(x) L2(x) L2(x) L2(x) L2(x)
#define L4(x) L3(x) L3(x) L3(x) L3(x)
#define L5(x) L4(x) L4(x) L4(x) L4(x)
#define L6(x) L5(x) L5(x) L5(x) L5(x)
    const char testTarget[] =  L6("rollrat");
    char* testCopy = malloc(sizeof(testTarget));
    memcpy(testCopy, testTarget, sizeof(testTarget));

    size_t a = xstrlen(testCopy);
    free(testCopy);

    printf("%d %d", a, strlen(testTarget));
}