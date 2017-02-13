/***

   RollRat Software Project.
   Copyright (C) 2015-2017. rollrat. All Rights Reserved.

File name:

   WString.h

Purpose:

   RollRat Framework

Author:

   10-07-2015:   HyunJun Jeong, Creation

***/

#ifndef _WSTRING_9bf1541fdf7efd41b7b39543fd870ac4_
#define _WSTRING_9bf1541fdf7efd41b7b39543fd870ac4_

#include <stdio.h>
#include <type_traits>
#include <algorithm>
#include <ctype.h>
#include <cmath>
#include <stdlib.h>
#include <string.h>
#include <tchar.h>
#include <iostream>
#include <stdint.h>
#include <string>
#include <exception>

#include "..\collection\Array.h"

#ifndef _MSC_VER

// �� ����� ��� ���� �ֻ����� ��ġ�ؾ���.
// undef�� ���������� �� ���̺귯���� ��� ���Ͽ� �����ϱ� �����̴�.

// for another compiler
// de-secure code
#define mbstowcs_s(rv,wcstr,sz,mbstr,max) mbstowcs(wcstr,mbstr,max)
#define wcscpy_s(dest,destlen,src) wcscpy(dest,src)
#define wcsncpy_s(dest,destlen,src,max) wcsncpy(dest,src,destlen)
#define _itow_s(val,buf,radix) _itow(val,buf,radix)
#define _wcsnset_s(dest,destlen,ch,count) _wcsnset(dest,ch,count)

// NO-ISO
#define swprintf_s(dest,fmt,num,numsz) swprintf(dest,fmt,num)

#endif

#if defined(__x86_64__) || defined(__ia64__) || defined(_M_AMD64) || defined(_M_IA64) \
 || defined(_WIN64) || defined(__alpha__) || defined(__s390__)
#define _X64_MODE
typedef uint64_t	checker_type;
#define _X_BRU(x)   x
#define _X_SEL(x,y) x
#else
typedef uint32_t	checker_type;
#define _X_BRU(x)
#define _X_SEL(x,y) y
#endif

namespace Utility {

	typedef enum class _string_Exception_
	{
		// ��, ���� ����� ���� ����� ũ�Ⱑ Ÿ�꺸��
		// ���� �� �߻��˴ϴ�.
		ComparasionSizeException,
		// ���� ũ�⺸�� ū �ε����� �迭�� �����Ϸ�
		// ���� �� �߻��մϴ�.
		OverflowReferenceException,
		// ���� ����� ��ǥ�� ũ�Ⱑ 0�� ��� �߻��˴ϴ�.
		InsertionSizeZeroException,
		// ��ȣ�� ���� ��ȯ���� ��ȣ�� �߰ߵ� ��� �߻��˴ϴ�.
		ToNumericSignException,
        // �Էµ� �� �迭�� ũ�Ⱑ �ٸ� ��� �߻��˴ϴ�.
        DiscordArraySize,
	}	StringErrorCode;

	//
	//	�� Ŭ������ String Ŭ������ 
	//	Exception ������ �����մϴ�.
	//
	class StringException : public std::exception
	{
		const StringErrorCode num;

	public:

		StringException(StringErrorCode sec)
			: num(sec)
		{
		}

		virtual const char *what() const
		{
			switch ( num )
			{
			case StringErrorCode::ComparasionSizeException:
				return "Input value is too large.";
			case StringErrorCode::OverflowReferenceException:
				return "Invalid heap access value.";
			case StringErrorCode::InsertionSizeZeroException:
				return "Inserted length can not be zero.";
			case StringErrorCode::ToNumericSignException:
				return "Sign has been discovered in unsigned conversion.";
            case StringErrorCode::DiscordArraySize:
                return "Size of the two input arrays must be same.";
			}

			return "Undefined error.";
		}
		
	};
	
	//
	//	���ڿ� �����Ͱ� Ŭ���� ������ �ͼӵǵ����Ϸ��� ��쿡 ���˴ϴ�. (�Ҹ��� ȣ���)
	//
	class StringReferencePutWith
	{
	};

	//
	//  �� Ŭ������ ���ڿ� ���� ������ ��Ÿ���ϴ�.
	//  ���� �� Ŭ������ ���� Clone�� Swap�� ������ ������ �Լ�����
	//  ������� �ʽ��ϴ�. ���������ڷ� ĳ�����Ͽ� ���ڿ��� �����ϴ� ���� ��
	//  Ŭ�������� ���� ������ �ƴմϴ�.
	//	����: ���߽� ���ǻ��� ���
	//      1. �� Ŭ������ wchar_t *������ ���ڿ� ������ �����մϴ�.
	//         ���� string api�� ����մϴ�. ���� ��� m_ptr
	//         �迭 ����� ���� null�̿����մϴ�.
	//      2. �������� ������ ����� sizeof(wchar_t)�� 2����
	//         �ݵ�� ����ؾ߸� �մϴ�. ���������� ���ý� wchar_t*
	//         �� ���� �����Ϸ��� ������ ���� �� �����ϴ�.
	//      3. Ư���� ����� ���� ��κ��� �Լ����� �ҷ�, �뷮�� ���ڿ�����
	//         ������ ���� ����, ������ ���Դϴ�. �ǵ����̸�
	//         �������� �ʰ� ����ϼ����� ���ڽ��ϴ�.
	//      4. ��ȯ �� ����ȭ�� �̿��Ͽ� ������带 ���̵��� �ۼ��Ǿ����ϴ�.
	//         ��� �Լ��� ��ȯ ���� ���������� �����ϴ� ��� �Ҹ��ڰ�
	//         ȣ����� ���� �� ������, �Լ��� ȣ���ϴ� �Լ��� ��ȯ ����
	//         �������� ��� �Ҹ��� ȣ��� ���ڿ��� �ջ�� �� �ֽ��ϴ�.
	//         (record. ������ ���� ������ ������� ������ ���ƴ�.)
	//         (��� ���� �ϼ��� �Ǹ� .cpp �и� ����)
	//      5. Length()�� ������ ��� �Լ��� ���ڿ��� ������ġ�� 0���� ����ϴ�.
	//         ����ڰ� ȥ������ �ʵ��� ��ġ�Ͽ����ϴ�. Ư�� ���ڰ�
	//         �߰ߵ��� ������ error(-1)�� ��ȯ�˴ϴ�.
	//      6. strcpy|strncpy vs memcpy: strcpy�� ����Ʈ�� �����Լ��̹Ƿ�,
	//         ũ�⸦ �� �� ���� ���翡 ����������, ũ�Ⱑ ������ ���
	//         strncpy�� �ַ� ����ϰ��ϴµ�, memcpy�� �̸� �ü���� �°�
	//         4 byte, 8 byte�� ����ϹǷ� �޸𸮰����Լ��� ����ϴ°� ��
	//         ���翡 �� ȿ�����Դϴ�.
	//
	//   ����� �Լ�
	//   [Right, Substring] [Left, SubstringReverse] [Mid, Substring Slice]
	//   [RSet, PadRight] [LSet, PadLeft]
	//   [RTrim, TrimStart] [LTrim, TrimEnd]
	//   [IndexOf, FindFirst, FirstContains] [LastIndexfOf, FindLast, LastContains]
	//   [InStr, Contains]
	//
	//   ȥ���Ǵ� �Լ���
	//   Empty: ���̰� 0�϶�
	//   Null:  �����Ͱ� nullptr�϶�
	//   Full:  ���̰� 0���� Ŭ��
	//
	//   ���� ����
	//   1. ���� ���� size_t, bool�� �ƴ� ��� ��ġ ���Լ��� �����ߴ�.
	//
	class WString final
	{
        /**
         *  @var m_ptr    ���ڿ��� ������ ��Ÿ���ϴ�.
         *  @var m_last   ���ڿ��� ��Ʋ ��Ÿ���ϴ�.
         *  @var m_length ���ڿ��� ���̸� ��Ÿ���ϴ�.
         */
		wchar_t *m_ptr;
		wchar_t *m_last;
		size_t   m_length;

        /**
         *  @var with     Ŭ���� ���ο��� ���ڿ� �ͼ� ��ȯ�� 
         *                ����ϴ� ��쿡 ���Դϴ�.
         *  @var srp      �Ҹ��ڸ� ȣ�⿩�θ� �����մϴ�.
         */
		StringReferencePutWith with;
		bool srp = false;

	public:

		typedef ReadOnlyArray<WString *> SplitsArray;
		typedef ReadOnlyArray<WString *> Lines;
		typedef ReadOnlyArray<WString *> Elements;
		typedef ReadOnlyArray<unsigned char> Utf8Array;

		static const size_t error = -1;
        
        /**
         *  @brief  
         *  @param  
         *  @return 
         */

        /**
         *  @brief  �� ���ڿ� ������ �����մϴ�.
         *
         *  �� �����ڴ� null-string�� ��Ÿ���ϴ�. �̴� ����� �� ����
         *  WString ������ ����� ����� �������� ��Ÿ���� �뵵�� ���
         *  �� �� �ֽ��ϴ�.
         */
		WString()
			: m_length(0)
			, m_ptr(nullptr)
			, m_last(m_ptr)
		{
		}
		
        /**
         *  @brief  Ansi ��� ���ڿ� ������ �Է����� �޾� �����ڵ�
         *          ���ڿ����� ��ȯ�� ���ڿ� ������ �����մϴ�.
         *  @param  ptr Ansi ��� ���ڿ� �����Դϴ�.
         *
         *  �� �����ڴ� ���� �������� �䱸�� �Ӹ��ƴ϶� ����ȭ����
         *  ���� ����� ����մϴ�. Windows Api�� MultiByteToWideChar�� 
         *  ���� �Լ��� ����Ͽ� ��ȯ �� ����Ͻʽÿ�.
         */
		WString(const char *ptr)
		{
			AnsiToUnicode(ptr, strlen(ptr));
		}
        
        /**
         *  @brief  Ansi ��� ���ڿ� ������ �Է����� �޾� �����ڵ�
         *          ���ڿ����� ��ȯ�� ���ڿ� ������ �����մϴ�.
         *  @param  ptr Ansi ��� ���ڿ� �����Դϴ�.
         *  @param  len ptr�� �����Դϴ�.
         *
         *  �� �����ڴ� ���� �������� �䱸�� �Ӹ��ƴ϶� ����ȭ����
         *  ���� ����� ����մϴ�. Windows Api�� MultiByteToWideChar�� 
         *  ���� �Լ��� ����Ͽ� ��ȯ �� ����Ͻʽÿ�. ���� �Ű����� len��
         *  ���� ptr�� �����̰� �������Ƿ� ���� �����Ͻʽÿ�.
         */
		WString(const char *ptr, size_t len)
		{
			AnsiToUnicode(ptr, len);
		}
        
        /**
         *  @brief  �����ڵ� ��� ���ڿ� ������ �Է����� �޾� ���ο�
         *          ���ڿ� ������ �����մϴ�.
         *  @param  ptr �����ڵ� ��� ���ڿ� �����Դϴ�.
         *
         *  �� �����ڴ� ���ͳ� ���ڿ�, �Ǵ� const_cast�� ĳ���õ� ���ڿ���
         *  �Ű������� �޵��� ����Ǿ� �ֽ��ϴ�. ��Ÿ �Է¿� ���� ������
         *  ��ȣ���� ���ϹǷ�, WString(const wchar_t *ptr, size_t len)
         *  �� ����Ͽ� �ذ��Ͻʽÿ�.
         */
		WString(const wchar_t *ptr)
		{
			InitString(ptr);
		}
		
		// ���� �� �����ڴ� �� ���̺귯������ ���� ���� ���δ�.
		// �� ����̿ܿ� ������ ������ �� �ִ� ����� �˸� �˷��ָ� �����ϰڽ��ϴ�.

        /**
         *  @brief  �����ڵ� ��� ���ڿ� ������ ������ �Է����� �޾� 
         *          �Ҹ��ڰ� ȣ����� �ʴ� ���ڿ��� �����մϴ�.
         *  @param  ptr �����ڵ� ��� ���ڿ� ������ �����Դϴ�.
         *  @param  len ptr�� �����Դϴ�.
         *
         *  �� �����ڴ� ���ڿ� ������ ���İ� ���̸� �Է����� ������, WString�� 
         *  �����ϴ� �������� ����ϴ� �������� �̿��Ϸ��� ��쿡 ������
         *  ������ �Դϴ�.
         */
		WString(wchar_t *ptr, size_t len)
			: m_length(len)
			, m_ptr(ptr)
			, m_last(ptr + len - 1)
			, srp(true)
		{ // ũ����� �ԷµǴ� ��쿡 ���Ͽ� ������ ���� ����
		}
        
        /**
         *  @brief  �����ڵ� ��� ���ڿ� ������ ������ �Է����� ������
         *          �Ҹ��ڰ� ȣ��Ǵ� ���ڿ��� �����մϴ�.
         *  @param  ptr �����ڵ� ��� ���ڿ� ������ �����Դϴ�.
         *  @param  len ptr�� �����Դϴ�.
         *  @param  <>  ���ڿ� �ͼ� �÷��̽� Ȧ���Դϴ�.
         *
         *  �� �����ڴ� ���ڿ� ������ ���İ� ���̸� �Է����� ������, WStringŬ������
         *  ���ڿ��� �ͼӽ��� ����ϴ� �������� �̿��Ϸ��� ��쿡 ������
         *  ������ �Դϴ�. �����ڴ� ptr�� ���� �˻縦 �������� �ʰ�, �ͼӽ�Ű�Ƿ�,
         *  �ԷµǴ� ���̴� �̿��ڰ� Ȯ���Ͽ� �̿��ؾ��մϴ�.
         */
		WString(wchar_t *ptr, size_t len, StringReferencePutWith)
			: m_length(len)
			, m_ptr(ptr)
			, m_last(ptr + len - 1)
		{
		}
        
        /**
         *  @brief  �����ڵ� ��� ���ڿ� ���İ� ���̸� �Է����� �޾� ���ο�
         *          ���ڿ� ������ �����մϴ�.
         *  @param  ptr �����ڵ� ��� ���ڿ� ������ �����Դϴ�.
         *  @param  len ptr�� �����Դϴ�.
         *
         *  �� �����ڴ� �Է¹��� ���̿� ���� �Էµ� ���ڿ��� �����մϴ�.
         *  ���̸� �̸� Ȯ������ ���� ��� WString(const wchar_t *ptr)
         *  �����ڸ� ����Ͻʽÿ�.
         */
		WString(const wchar_t *ptr, size_t len)
			: m_length(len)
		{
			m_ptr = new wchar_t[m_length + 1];
			m_last = m_ptr + m_length - 1;
			memcpy(m_ptr, ptr, m_length * sizeof(wchar_t));
			m_last[1] = 0;
		}
		
        /**
         *  @brief  ���� �ϳ��� �ݺ��� ���ڿ��� �����մϴ�.
         *  @param  ch �ݺ��� ���ڸ� �����մϴ�.
         *  @param  count �ݺ��� Ƚ���� �����մϴ�.
         */
		WString(wchar_t ch, size_t count)
			: m_length(count)
		{
			m_ptr = new wchar_t[m_length + 1];
			m_last = m_ptr + m_length - 1;
			wcsnset(m_ptr, ch, count);
			m_ptr[m_length] = 0;
		}
		
        /**
         *  @brief  �Է¹��� ���ڷ� �����ϴ� ���̰� 1�� ���ڿ��� �����մϴ�.
         *  @param  ch �����ڵ� ��� ���� �����Դϴ�.
         */
		WString(wchar_t ch)
			: m_length(1)
		{
			m_ptr = new wchar_t[2];
			m_last = m_ptr;
			*m_ptr = ch;
			*(m_last + 1) = 0;
		}
		
        /**
         *  @brief  ���� �ϳ��� �ݺ��� ���ڿ��� �����մϴ�.
         *  @param  ch �ݺ��� ���ڸ� �����մϴ�.
         *  @param  count �ݺ��� Ƚ���� �����մϴ�.
         */
		WString(char ch, size_t count)
			: WString((wchar_t)ch, count)
		{
		}
        /**
         *  @brief  �Է¹��� ���ڷ� �����ϴ� ���̰� 1�� ���ڿ��� �����մϴ�.
         *  @param  ch Ansi ��� ���� �����Դϴ�.
         */
		
		WString(char ch)
			: WString((wchar_t)ch)
		{
		}
		
        /**
         *  @brief  �Է¹��� ���ڷ� �����ϴ� ���̰� 1�� ���ڿ��� �����մϴ�.
         *  @param  ch Ansi ��� ���� �����Դϴ�.
         */
		WString(unsigned char ch)
			: WString((wchar_t)ch)
		{
		}
        
		// short�� �־��µ� �������ؼ� ��������

        /**
         *  @brief  �Է¹��� ������ �Ǵ� �Ǽ������� ���ڿ��� �����մϴ�.
         *  @param  num �Է��� ������ �Ǵ� �Ǽ����� �����մϴ�.
         */
		WString(int num)
		{
			wchar_t buffer[65];
			_itow_s(num, buffer, 10);
			InitString((const wchar_t *)buffer);
		}
		
        /**
         *  @brief  �Է¹��� ������ �Ǵ� �Ǽ������� ���ڿ��� �����մϴ�.
         *  @param  num �Է��� ������ �Ǵ� �Ǽ����� �����մϴ�.
         */
		WString(long int num)
		{
			wchar_t buffer[65];
			_ltow_s(num, buffer, 10);
			InitString((const wchar_t *)buffer);
		}
		
        /**
         *  @brief  �Է¹��� ������ �Ǵ� �Ǽ������� ���ڿ��� �����մϴ�.
         *  @param  num �Է��� ������ �Ǵ� �Ǽ����� �����մϴ�.
         */
		WString(long long int num)
		{
			wchar_t buffer[65];
			swprintf_s(buffer, L"%lld", num, 65);
			InitString((const wchar_t *)buffer);
		}
		
        /**
         *  @brief  �Է¹��� ������ �Ǵ� �Ǽ������� ���ڿ��� �����մϴ�.
         *  @param  num �Է��� ������ �Ǵ� �Ǽ����� �����մϴ�.
         */
		WString(unsigned int num)
		{
			wchar_t buffer[65];
			swprintf_s(buffer, L"%u", num, 65);
			InitString((const wchar_t *)buffer);
		}
		
        /**
         *  @brief  �Է¹��� ������ �Ǵ� �Ǽ������� ���ڿ��� �����մϴ�.
         *  @param  num �Է��� ������ �Ǵ� �Ǽ����� �����մϴ�.
         */
		WString(unsigned long int num)
		{
			wchar_t buffer[65];
			swprintf_s(buffer, L"%lu", num, 65);
			InitString((const wchar_t *)buffer);
		}
		
        /**
         *  @brief  �Է¹��� ������ �Ǵ� �Ǽ������� ���ڿ��� �����մϴ�.
         *  @param  num �Է��� ������ �Ǵ� �Ǽ����� �����մϴ�.
         */
		WString(unsigned long long int num)
		{
			wchar_t buffer[65];
			swprintf_s(buffer, L"%llu", num, 65);
			InitString((const wchar_t *)buffer);
		}
		
        /**
         *  @brief  �Է¹��� ������ �Ǵ� �Ǽ������� ���ڿ��� �����մϴ�.
         *  @param  num �Է��� ������ �Ǵ� �Ǽ����� �����մϴ�.
         */
		WString(float num)
		{
			wchar_t buffer[65];
			swprintf_s(buffer, L"%g", num, 65);
			InitString((const wchar_t *)buffer);
		}
		
        /**
         *  @brief  �Է¹��� ������ �Ǵ� �Ǽ������� ���ڿ��� �����մϴ�.
         *  @param  num �Է��� ������ �Ǵ� �Ǽ����� �����մϴ�.
         */
		WString(double num)
		{
			wchar_t buffer[65];
			swprintf_s(buffer, L"%lg", num, 65);
			InitString((const wchar_t *)buffer);
		}
		
        /**
         *  @brief  �Է¹��� ������ �Ǵ� �Ǽ������� ���ڿ��� �����մϴ�.
         *  @param  num �Է��� ������ �Ǵ� �Ǽ����� �����մϴ�.
         */
		WString(long double num)
		{
			wchar_t buffer[65];
			swprintf_s(buffer, L"%llg", num, 65);
			InitString((const wchar_t *)buffer);
		}
		
        /**
         *  @brief  Ŭ���� ���� �������Դϴ�.
         *  @param  cnt ������ ���ڿ� ���� ������ �����մϴ�.
         *
         *  �� �����ڴ� �Ҹ��ڸ� ȣ���ϴ� ���ο� ���ڿ��� �����մϴ�.
         *  WString(WString& cnt)�� �⺻������ �������� �ʽ��ϴ�.
         */
		WString(const WString& cnt)
			: WString((const wchar_t *)cnt.m_ptr, cnt.m_length)
		{
		}
        
        /**
         *  @brief  ǥ�� ���ڿ��� �̿��� ���� �����͸� �����մϴ�.
         *  @param  str ǥ�� ���ڿ��� �����մϴ�.
         *
         *  �� �����ڴ� ǥ�� ���ڿ��� �Է����� ������, WString�� 
         *  �����ϴ� �������� ����ϴ� �������� �̿��Ϸ��� ��쿡 ������
         *  ������ �Դϴ�.
         */
		WString(std::wstring& str)
			: WString(&str[0], str.length())
		{
		}
        
        /**
         *  @brief  ǥ�� ���ڿ��� �Է����� �޾� ���ο� ���ڿ� ������ �����մϴ�.
         *  @param  wstr ǥ�� ���ڿ��� �����մϴ�.
         */
		WString(const std::string& str)
			: WString(str.c_str(), str.length())
		{
		}
        
        /**
         *  @brief  ǥ�� ���ڿ��� �Է����� �޾� ���ο� ���ڿ� ������ �����մϴ�.
         *  @param  wstr ǥ�� ���ڿ��� �����մϴ�.
         */
		WString(const std::wstring& wstr)
			: WString(wstr.c_str(), wstr.length())
		{
		}

		~WString()
		{
			if ( m_ptr != nullptr && !srp )
			{
				delete[] m_ptr;
				m_ptr = nullptr;
			}
		}
        
        /**
         *  @brief  ���ڿ��� ���̸� �����ɴϴ�.
         *
         *  �� �Լ��� null-terminated string���� ������ ������
         *  '\0'�� ������ ������ �κ��� ���̸� �����ɴϴ�.
         */
		inline size_t Length() const
		{
			return m_length;
		}
		
        /**
         *  @brief  ���ڿ� ������ ũ�Ⱑ 0���� Ȯ���մϴ�.
         */
		inline bool Empty() const
		{
			return m_length == 0;
		}
        
        /**
         *  @brief  ���ڿ� ������ ũ�Ⱑ 0���� ū�� Ȯ���մϴ�.
         */
		inline bool Full() const
		{
			return m_length > 0;
		}
        
        /**
         *  @brief  ���ڿ��� �����Ǿ� �ִ� ���� ���θ� Ȯ���մϴ�.
         */
		inline bool Null() const
		{
			return m_ptr == nullptr;
		}
		
        /**
         *  @brief  ���ڿ� �����͸� �����ɴϴ�.
         *
         *  �� �Լ��� �� Ŭ������ ���Ե� raw-data�� �����ϴ� ������ �Լ��Դϴ�.
         *  �� �Լ��� ���� raw-data�� �����ϴ� ���� �� Ŭ������ �ùٸ�
         *  ������ �ƴմϴ�.
         */
		inline const wchar_t *Reference() const
		{
			return m_ptr;
		}

	private:

		WString Append(const wchar_t *str, size_t len)
		{
			size_t newSize = m_length + len;
			wchar_t * appendString;

			appendString = new wchar_t[newSize + 1];
			
			if (m_length > 0)
				memcpy(appendString, m_ptr, m_length * sizeof(wchar_t));
			memcpy(appendString + m_length, str, len * sizeof(wchar_t));
			appendString[newSize] = 0;
			
			return WString(appendString, newSize, with);
		}

	public:
        
        /**
         *  @brief  ���ڿ� �� �տ� ���ڿ��� �߰��� ���ڿ��� ��ȯ�մϴ�.
         *  @param  str �߰��� ���ڿ��� �����մϴ�.
         *
         *  �� �Լ��� Insert(0, str, len)�� �߻��Դϴ�.
         *  ���� Append�� �����Ϸ��� StringBuilder�� �̿��Ͻʽÿ�.
         */
		WString Append(const wchar_t *str)
		{
			return Append(str, wcslen(str));
		}
        
        /**
         *  @brief  ���ڿ� �� �տ� ���ڿ��� �߰��� ���ڿ��� ��ȯ�մϴ�.
         *  @param  refer �߰��� ���ڿ��� �����մϴ�.
         *
         *  �� �Լ��� Insert(0, str, len)�� �߻��Դϴ�.
         *  ���� Append�� �����Ϸ��� StringBuilder�� �̿��Ͻʽÿ�.
         */
		WString Append(const WString& refer)
		{
			return Append(refer.m_ptr, refer.m_length);
		}
        
        /**
         *  @brief  �� ���ڿ��� �����մϴ�.
         *  @param  t1 ������ ù ��° ���ڿ��Դϴ�.
         *  @param  t2 ������ �� ��° ���ڿ��Դϴ�.
         *
         *  �� �Լ��� ���� ���ڿ� ������ �������� �����Ǿ����ϴ�. �ǵ����̸�
         *  WString�̶�� ���Ŀ� �´� �Ű������� �Է��Ͻñ� �ٶ��ϴ�.
         */
		static WString Concat(const WString& t1, const WString& t2)
		{
			if (t1.Empty())
			{
				if (t2.Empty())
				{
					return WString();
				}
				return WString((const wchar_t *)t2.m_ptr, t2.m_length);
			}

			if (t2.Empty())
			{
				return WString((const wchar_t *)t1.m_ptr, t1.m_length);
			}

			size_t newSize = t1.m_length + t2.m_length;
			wchar_t * mergerString;
			
			mergerString = new wchar_t[newSize + 1];
			memcpy(mergerString, t1.m_ptr, t1.m_length * sizeof(wchar_t));
			memcpy(mergerString + t1.m_length, t2.m_ptr, t2.m_length * sizeof(wchar_t));
			mergerString[newSize] = 0;
			
			StringReferencePutWith with;
			return WString(mergerString, newSize, with);
		}
        
        /**
         *  @brief  �� ���ڿ��� �����մϴ�.
         *  @param  t1 ������ ù ��° ���ڿ��Դϴ�.
         *  @param  t2 ������ �� ��° ���ڿ��Դϴ�.
         *  @param  t3 ������ �� ��° ���ڿ��Դϴ�.
         *
         *  �� �Լ��� ���� ���ڿ� ������ �������� �����Ǿ����ϴ�. �ǵ����̸�
         *  WString�̶�� ���Ŀ� �´� �Ű������� �Է��Ͻñ� �ٶ��ϴ�.
         */
		static WString Concat(const WString& t1, const WString& t2, const WString& t3)
		{
			if (t1.Empty() && t2.Empty() && t3.Empty())
			{
				return WString();
			}

			size_t newSize = t1.m_length + t2.m_length + t3.m_length;
			wchar_t * mergerString;

			mergerString = new wchar_t[newSize + 1];
			memcpy(mergerString, t1.m_ptr, t1.m_length * sizeof(wchar_t));
			memcpy(mergerString + t1.m_length, t2.m_ptr, t2.m_length * sizeof(wchar_t));
			memcpy(mergerString + t1.m_length + t2.m_length, t3.m_ptr, t3.m_length * sizeof(wchar_t));
			mergerString[newSize] = 0;

			StringReferencePutWith with;
			return WString(mergerString, newSize, with);
		}
        
        /**
         *  @brief  �� ���ڿ��� �����մϴ�.
         *  @param  t1 ������ ù ��° ���ڿ��Դϴ�.
         *  @param  t2 ������ �� ��° ���ڿ��Դϴ�.
         *  @param  t3 ������ �� ��° ���ڿ��Դϴ�.
         *  @param  t4 ������ �� ��° ���ڿ��Դϴ�.
         *
         *  �� �Լ��� ���� ���ڿ� ������ �������� �����Ǿ����ϴ�. �ǵ����̸�
         *  WString�̶�� ���Ŀ� �´� �Ű������� �Է��Ͻñ� �ٶ��ϴ�.
         */
		static WString Concat(const WString& t1, const WString& t2, const WString& t3, const WString& t4)
		{
			if (t1.Empty() && t2.Empty() && t3.Empty() && t4.Empty())
			{
				return WString();
			}

			size_t newSize = t1.m_length + t2.m_length + t3.m_length + t4.m_length;
			wchar_t * mergerString;

			mergerString = new wchar_t[newSize + 1];
			memcpy(mergerString, t1.m_ptr, t1.m_length * sizeof(wchar_t));
			memcpy(mergerString + t1.m_length, t2.m_ptr, t2.m_length * sizeof(wchar_t));
			memcpy(mergerString + t1.m_length + t2.m_length, t3.m_ptr, t3.m_length * sizeof(wchar_t));
			memcpy(mergerString + t1.m_length + t2.m_length + t3.m_length, t4.m_ptr, t4.m_length * sizeof(wchar_t));
			mergerString[newSize] = 0;

			StringReferencePutWith with;
			return WString(mergerString, newSize, with);
		}
        
        /**
         *  @brief  �� ���ڿ��� ���� ���մϴ�.
         *  @param  str ���� ���ڿ��� �����մϴ�.
         *  @return wcscmp�Լ��� ��ȯ������ ����մϴ�.
         *
         *  �� �Լ��� Ŭ������ ������ ���ڿ��� wcscmp�� ù ��° �Ű�������
         *  ����մϴ�.
         */
		inline size_t CompareTo(const wchar_t *str) const
		{
			return wcscmp(m_ptr, str);
		}
		
        /**
         *  @brief  �� ���ڿ��� ���� ���մϴ�.
         *  @param  str ���� ���ڿ��� �����մϴ�.
         *  @return wcscmp�Լ��� ��ȯ������ ����մϴ�.
         *
         *  �� �Լ��� Ŭ������ ������ ���ڿ��� wcscmp�� ù ��° �Ű�������
         *  ����մϴ�.
         */
		inline size_t CompareTo(const WString& refer) const
		{
			return CompareTo(refer.m_ptr);
		}
        
        /**
         *  @brief  �� ���ڿ��� ���� ���ϴ� �Լ��Դϴ�.
         *  @param  r1 ���� ù ��° ���ڿ�
         *  @param  r2 ���� �� ��° ���ڿ�
         *  @return wcscmp�Լ��� ��ȯ������ ����մϴ�.
         */
		inline static int Comparer(const WString& r1, const WString& r2)
		{
			return wcscmp(r1.m_ptr, r2.m_ptr);
		}
        
        /**
         *  @brief  �� ���ڿ��� ���� �������� ���θ� Ȯ���մϴ�.
         */
		inline bool Equal(const wchar_t *str) const
		{
			size_t strlen = wcslen(str);

			if ( strlen == m_length )
				return !memcmp(m_ptr, str, m_length * sizeof(wchar_t));

			return false;
		}
        
        /**
         *  @brief  �� ���ڿ��� ���� �������� ���θ� Ȯ���մϴ�.
         */
		inline bool Equal(const WString& refer) const
		{
			if (refer.m_length != this->m_length)
				return false;
			
			return !memcmp(m_ptr, refer.m_ptr, m_length * sizeof(wchar_t));
		}
		
        /**
         *  @brief  �� ���ڿ��� ���� �������� ���θ� Ȯ���մϴ�.
         */
		inline bool operator==(const wchar_t *ptr) const
		{
			return Equal(ptr);
		}
        
        /**
         *  @brief  �� ���ڿ��� ���� �������� ���θ� Ȯ���մϴ�.
         */
		inline bool operator==(const WString& refer) const
		{
			return Equal(refer);
		}
        
        /**
         *  @brief  �� ���ڿ��� ���� �ٸ����� ���θ� Ȯ���մϴ�.
         */
		inline bool operator!=(const wchar_t *ptr) const
		{
			return !Equal(ptr);
		}
        
        /**
         *  @brief  �� ���ڿ��� ���� �ٸ����� ���θ� Ȯ���մϴ�.
         */
		inline bool operator!=(const WString& refer) const
		{
			return !Equal(refer);
		}
		
        /**
         *  @brief  ���ڿ� �տ��� ���� ������ �Ÿ���ŭ ������ ���ڸ� �����ɴϴ�.
         *  @param  pos �Ÿ��� �����մϴ�.
         */
		inline wchar_t First(size_t pos) const
		{
		    // �տ��� ���� �о�´�. ���� ���� ������ Last�� ¦�� ���߱� ���� ����
		    // vb���� Right, Left�� ����ϰ�
		    // ���⼱ Substring�� SubstringReverse�� Remove�� ����ϴ�.
			if (pos >= m_length)
				throw(new StringException(StringErrorCode::ComparasionSizeException));

			return m_ptr[pos];
		}
        
        /**
         *  @brief  ���ڿ� �ڿ��� ���� ������ �Ÿ���ŭ ������ ���ڸ� �����ɴϴ�.
         *  @param  pos �Ÿ��� �����մϴ�.
         */
		inline wchar_t Last(size_t pos) const
		{
			if (pos >= m_length)
				throw(new StringException(StringErrorCode::ComparasionSizeException));

			return *(m_last - pos);
		}
		
	private:
		
		size_t FindFirstHelper(const wchar_t *str, size_t starts) const
		{
			if (starts >= m_length)
				return error;
			
			const wchar_t *ptr = wcsstr(m_ptr + starts, str);

			return ptr != NULL ? ptr - m_ptr : error;
		}
		
		// ������ ȯ�濡���� �����˴ϴ�.
		size_t FindLastHelper(const wchar_t *str, size_t ends, size_t len) const
		{
			if (ends >= m_length)
				return error;
			
			const wchar_t *ptr = wcsrnstrn(m_ptr, m_length - ends, str, len);

			return ptr != NULL ? ptr - m_ptr : error;
		}

	public:
        
        /**
         *  @brief  ���ڿ� �տ��� ������ �Ÿ���ŭ ������ �κк��� ������ ���ڿ��� 
         *          ó�� ��Ÿ���� ��ġ�� �����ɴϴ�.
         *  @param  str ã�� ���ڿ��� �����մϴ�.
         *  @param  starts ���ڿ� �տ������� ������ �Ÿ��� �����մϴ�.
         *  @return ���ڿ��� ó�� ��Ÿ���� ��ġ�Դϴ�. �ش� ���ڿ��� ã�� ���Ѱ��
         *          WString::error�� ��ȯ�˴ϴ�. 
         */
		size_t FindFirst(const wchar_t *str, size_t starts = 0) const
		{
			return FindFirstHelper(str, starts);
		}
        
        /**
         *  @brief  ���ڿ� �տ��� ������ �Ÿ���ŭ ������ �κк��� ������ ���ڿ��� 
         *          ó�� ��Ÿ���� ��ġ�� �����ɴϴ�.
         *  @param  str ã�� ���ڿ��� �����մϴ�.
         *  @param  starts ���ڿ� �տ������� ������ �Ÿ��� �����մϴ�.
         *  @return ���ڿ��� ó�� ��Ÿ���� ��ġ�Դϴ�. �ش� ���ڿ��� ã�� ���Ѱ��
         *          WString::error�� ��ȯ�˴ϴ�. 
         */
		size_t FindFirst(const WString& refer, size_t starts = 0) const
		{
			return FindFirstHelper(refer.m_ptr, starts);
		}
        
        /**
         *  @brief  ���ڿ� �ڿ��� ������ �Ÿ���ŭ ������ �κк��� ������ ���ڿ��� 
         *          ó�� ��Ÿ���� ��ġ�� �����ɴϴ�.
         *  @param  str ã�� ���ڿ��� �����մϴ�.
         *  @param  ends ���ڿ� �ڿ������� ������ �Ÿ��� �����մϴ�.
         *  @return ���ڿ��� ó�� ��Ÿ���� ��ġ�Դϴ�. �ش� ���ڿ��� ã�� ���Ѱ��
         *          WString::error�� ��ȯ�˴ϴ�. 
         */
		size_t FindLast(const wchar_t *str, size_t ends = 0) const
		{
			return FindLastHelper(str, ends, wcslen(str));
		}
        
        /**
         *  @brief  ���ڿ� �ڿ��� ������ �Ÿ���ŭ ������ �κк��� ������ ���ڿ��� 
         *          ó�� ��Ÿ���� ��ġ�� �����ɴϴ�.
         *  @param  str ã�� ���ڿ��� �����մϴ�.
         *  @param  ends ���ڿ� �ڿ������� ������ �Ÿ��� �����մϴ�.
         *  @return ���ڿ��� ó�� ��Ÿ���� ��ġ�Դϴ�. �ش� ���ڿ��� ã�� ���Ѱ��
         *          WString::error�� ��ȯ�˴ϴ�. 
         */
		size_t FindLast(const WString& refer, size_t ends = 0) const
		{
			return FindLastHelper(refer.m_ptr, ends, refer.m_length);
		}
		
        /**
         *  @brief  ���ڿ� �տ��� ������ �Ÿ���ŭ ������ �κк��� ������ ���ڰ�
         *          ó�� ��Ÿ���� ��ġ�� �����ɴϴ�.
         *  @param  ch ã�� ���ڸ� �����մϴ�.
         *  @param  starts ���ڿ� �տ������� ������ �Ÿ��� �����մϴ�.
         *  @return ���ڰ� ó�� ��Ÿ���� ��ġ�Դϴ�. �ش� ���ڸ� ã�� ���Ѱ��
         *          WString::error�� ��ȯ�˴ϴ�.
         */
		size_t FindFirst(const wchar_t ch, size_t starts = 0) const
		{
		    // starts��ġ���� �����Ͽ� ch�� ��ġ�ϴ� ������ ��ġ�� ã���ϴ�.
			if (starts >= m_length)
				throw(new StringException(StringErrorCode::ComparasionSizeException));
			
			const wchar_t *ptr = wcschr(m_ptr + starts, ch);

			return ptr != NULL ? ptr - m_ptr : error;
		}
        
        /**
         *  @brief  ���ڿ� �ڿ��� ������ �Ÿ���ŭ ������ �κк��� ������ ���ڰ�
         *          ó�� ��Ÿ���� ��ġ�� �����ɴϴ�.
         *  @param  ch ã�� ���ڸ� �����մϴ�.
         *  @param  starts ���ڿ� �ڿ������� ������ �Ÿ��� �����մϴ�.
         *  @return ���ڰ� ó�� ��Ÿ���� ��ġ�Դϴ�. �ش� ���ڸ� ã�� ���Ѱ��
         *          WString::error�� ��ȯ�˴ϴ�. 
         */
		size_t FindLast(const wchar_t ch, size_t ends = 0) const
		{
		    // FindLast�� FindLast�� ��ӻ���� �� �ְ�, FindFirst�� FindFirst�� ��ӻ���� �� �ִ�.
			if (ends >= m_length)
				return error;
			
			const wchar_t *ptr = wcsrnchr(m_ptr, m_length - ends, ch);

			return (ptr != NULL) ? ptr - m_ptr : error;
		}

	private:
		
		bool ContainsHelper(const wchar_t *str, size_t len, bool ignorecase = false) const
		{
			if (len > m_length)
				return false;

			const wchar_t *s1, *s2, *ptr = m_ptr;

			if (!ignorecase)
			{
				return wcsstr(ptr, str) != NULL;
			}
			else
			{
				auto _tp_toswitch = [] (wchar_t ch) -> wchar_t
				{
					if ( ch <= L'z' )
					{
						if ( ch <= L'Z' )
							if ( ch >= L'A' )
								return ch | 0x0020;
						if ( ch >= L'a' )
							return ch ^ 0x0020;
					}
					return ch;
				};

				auto _tp_towlower = [] (wchar_t ch) -> wchar_t
				{
					if ( ch <= L'Z' )
						if ( ch >= L'A' )
							return ch ^ 0x0020;
					return ch;
				};

				size_t len_1 = len - 1;
				
				wchar_t *t1 = m_ptr;
				wchar_t *t2 = m_ptr;
				wchar_t *searchMax = m_last - len + 1;
				
				wchar_t strlast = _tp_towlower(str[len_1]);
				wchar_t w = _tp_toswitch(*str);

				t1 = wcschr(t1, *str);
				t2 = wcschr(t2, w);

#define _CONTAINS_HELPER_SKIP(t,ch) {\
				if (_tp_towlower(t[len_1]) == strlast) {\
					s1 = t + 1;s2 = str + 1;\
					while (*s1 && *s2 && !(towlower(*s1) - towlower(*s2)))\
						s1++, s2++;\
					if (*s2 == 0)return true;\
				} t = wcschr(t + 1, ch);}

				// wcschr�� ù ��° ��ġ ������ ã�´�
				// ��/�ҹ��� ��� ã��, ó����ġ�� ����� �� ����
				// �˻��Ѵ�. ���ĺ��� �ƴϸ� ���� ���� ���ϸ�ȴ�.
				// 1. ���ڿ� �ִ�ũ�⸦ �Ѵ°�? 
				//    m_ptr - len�� ������ ���� �� ����. -> false
				// 2. ���ڿ��� ������ ���ڰ� src�� ��ġ�ϴ°�?
				//    ó��, �߰�, ���� ���ϸ� ��������, �� �˰����� 
				//    ó���� ������ ���Ͽ� �񱳺���� ���߾���.
				// ���� �� ��° ���ں��� �񱳸� �����Ѵ�.
				while ( true )
				{
					if ( t1 && t2 )
						if ( t1 < t2 && t1 < searchMax )
							_CONTAINS_HELPER_SKIP(t1, *str)
						else if ( t2 < searchMax )
							_CONTAINS_HELPER_SKIP(t2, w)
						else
							break;
					else
						if ( t1 )
							while ( t1 )
								if ( t1 > searchMax )
									return false;
								else
									_CONTAINS_HELPER_SKIP(t1, *str)
						else if ( t2 )
							while ( t2 )
								if ( t2 > searchMax )
									return false;
								else
									_CONTAINS_HELPER_SKIP(t2, w)
						else
							break;
				}

				return false;
			}

			return false;
		}

	public:
		
        /**
         *  @brief  ���ڿ��� ���ԵǾ��ִ����� ���θ� Ȯ���մϴ�.
         *  @param  str ���ԵǾ��ִ����� ���θ� Ȯ���� ���ڿ��� �����մϴ�.
         *  @param  ignorecase ���ڿ��� ��ҹ��ڸ� ���������ʰ� Ȯ���� �������� ���θ� �����մϴ�.
         */
		bool Contains(const wchar_t *str, bool ignorecase = false) const
		{
			return ContainsHelper(str, wcslen(str), ignorecase);
		}
        
        /**
         *  @brief  ���ڿ��� ���ԵǾ��ִ����� ���θ� Ȯ���մϴ�.
         *  @param  str ���ԵǾ��ִ����� ���θ� Ȯ���� ���ڿ��� �����մϴ�.
         *  @param  ignorecase ���ڿ��� ��ҹ��ڸ� ���������ʰ� Ȯ���� �������� ���θ� �����մϴ�.
         */
		bool Contains(const WString& refer, bool ignorecase = false) const
		{
			return ContainsHelper(refer.m_ptr, refer.m_length, ignorecase);
		}

        /**
         *  @brief  ������ ��ġ�� ���ڸ� �����ɴϴ�.
         *  @param  index ��ġ�� �����մϴ�.
         */
		inline wchar_t operator[](size_t index) const
		{
			// ����� �ʹ� ũ��
			//if (index >= m_length)
			//	throw(new StringException(StringErrorCode::OverflowReferenceException));

			return m_ptr[index];
		}
        
        /**
         *  @brief  starts ��ġ���� ���ڿ��� ������ ��ġ�� ���ڿ� ������ �����ɴϴ�.
         */
		WString Substring(size_t starts)
		{
			if (starts > m_length)
				throw(new StringException(StringErrorCode::ComparasionSizeException));

			return WString((const wchar_t *)(m_ptr + starts), m_length - starts);
		}
        
        /**
         *  @brief  starts��ġ���� len��ŭ�� ũ�⸸ŭ �ڸ� ���ڿ� ������ �����ɴϴ�.
         */
		WString Substring(size_t starts, size_t len)
		{
			if (len + starts > m_length)
				throw(new StringException(StringErrorCode::ComparasionSizeException));

			return WString((const wchar_t *)(m_ptr + starts), len);
		}
        
        /**
         *  @brief  �ڿ��� ������ starts��ŭ �ڸ� ���ڿ� ������ �����ɴϴ�.
         */
		WString SubstringReverse(size_t starts)
		{
			if (starts > m_length)
				throw(new StringException(StringErrorCode::ComparasionSizeException));

			return WString((const wchar_t *)(m_ptr), m_length - starts);
		}
        
        /**
         *  @brief  �ڿ��� ������ starts��ŭ �� ��ġ���� len��ŭ �ڸ� ���ڿ� ������ �����ɴϴ�. 
         */
		WString SubstringReverse(size_t starts, size_t len)
		{
			if (len + starts > m_length)
				throw(new StringException(StringErrorCode::ComparasionSizeException));

			return WString((const wchar_t *)(m_last - starts - len + 1), len);
		}
        
        /**
         *  @brief  ���༱������ ������ ������ �κ��� ����մϴ�.
         */
		size_t TrimStartPos() const
		{
			const wchar_t *ptr = m_ptr;
			while (*ptr)
			{
				if (!isspace((int)*ptr))
					break;
				else
					ptr++;
			}
			return ptr - m_ptr;
		}
        
        /**
         *  @brief  ���༱������ ������ ������ �κ��� ����մϴ�.
         */
		size_t TrimEndPos() const
		{
			wchar_t *ptr = m_last;
			while (ptr >= m_ptr)
			{
				if (!isspace((int)*ptr))
					break;
				else
					ptr--;
			}
			return ptr - m_ptr; // *m_ptr ��������
		}
		
        /**
         *  @brief  �տ��� ���� �ݺ��� ���ڰ� ������ �κ��� ����մϴ�.
         *  @param  ch �ݺ��� ���ڸ� �����մϴ�.
         */
		size_t TrimStartPos(wchar_t ch) const
		{
			const wchar_t *ptr = m_ptr;
			while (*ptr)
			{
				if (*ptr != ch)
					break;
				else
					ptr++;
			}
			return ptr - m_ptr;
		}
        
        /**
         *  @brief  �ڿ��� ���� �ݺ��� ���ڰ� ������ �κ��� ����մϴ�.
         *  @param  ch �ݺ��� ���ڸ� �����մϴ�.
         */
		size_t TrimEndPos(wchar_t ch) const
		{
			wchar_t *ptr = m_last;
			while (ptr >= m_ptr)
			{
				if (*ptr != ch)
					break;
				else
					ptr--;
			}
			return ptr - m_ptr; // *m_ptr ��������
		}
        
        /**
         *  @brief  ���༱�������� ������ ���ڿ� ������ �����ɴϴ�.
         */
		WString TrimStart()
		{
			const wchar_t *ptr = m_ptr;
			while (*ptr)
			{
				if (!isspace((int)*ptr))
					break;
				else
					ptr++;
			}
			return WString(ptr, m_length + m_ptr - ptr);
		}
        
        /**
         *  @brief  ���༱�������� ������ ���ڿ� ������ �����ɴϴ�.
         */
		WString TrimEnd()
		{
			wchar_t *ptr = m_last;
			while (ptr >= m_ptr)
			{
				if (!isspace((int)*ptr))
					break;
				else
					ptr--;
			}
			return WString((const wchar_t *)m_ptr, ptr - m_ptr + 1);
		}
        
        /**
         *  @brief  ���༱������� ���༱�������� ������ ���ڿ� ������ �����ɴϴ�.
         */
		WString Trim()
		{
			const wchar_t *fptr = m_ptr;
			wchar_t *ptr = m_ptr + m_length - 1;
			while (*fptr)
			{
				if (!isspace((int)*fptr))
					break;
				else
					fptr++;
			}
			while (ptr >= m_ptr)
			{
				if (!isspace((int)*ptr))
					break;
				else
					ptr--;
			}
			return WString(fptr, ptr - fptr + 1);
		}
        
        /**
         *  @brief  �տ��� ���� �ݺ��� ���ڸ� ������ ���ڿ� ������ �����ɴϴ�.
         *  @param  ch �ݺ��� ���ڸ� �����մϴ�.
         */
		WString TrimStart(wchar_t ch)
		{
			const wchar_t *ptr = m_ptr;
			while (*ptr)
			{
				if (*ptr != ch)
					break;
				else
					ptr++;
			}
			return WString(ptr, m_length + m_ptr - ptr);
		}
        
        /**
         *  @brief  �ڿ��� ���� �ݺ��� ���ڸ� ������ ���ڿ� ������ �����ɴϴ�.
         *  @param  ch �ݺ��� ���ڸ� �����մϴ�.
         */
		WString TrimEnd(wchar_t ch)
		{
			wchar_t *ptr = m_last;
			while (ptr >= m_ptr)
			{
				if (*ptr != ch)
					break;
				else
					ptr--;
			}
			return WString((const wchar_t *)m_ptr, ptr - m_ptr + 1);
		}
        
        /**
         *  @brief  �հ� ��, ��ο��� ���� �ݺ��� ���ڸ� ������ ���ڿ� ������ �����ɴϴ�.
         *  @param  ch �ݺ��� ���ڸ� �����մϴ�.
         */
		WString Trim(wchar_t ch)
		{
			const wchar_t *fptr = m_ptr;
			wchar_t *ptr = m_ptr + m_length - 1;
			while (*fptr)
			{
				if (*fptr != ch)
					break;
				else
					fptr++;
			}
			while (ptr >= m_ptr)
			{
				if (*ptr != ch)
					break;
				else
					ptr--;
			}
			return WString(fptr, ptr - fptr + 1);
		}

	private:

		size_t CountHelper(const wchar_t *str, size_t len) const
		{
			size_t ret = 0;
			const wchar_t *ptr = m_ptr;
			for ( ; ptr = wcsstr(ptr, str); ptr += len, ret++ )
				;
			return ret;
		}

	public:
        
        /**
         *  @brief  ���ڿ� ���տ� ���Ե� ������ ���ڿ� ������ ������ �����ɴϴ�.
         *  @param  str ������ ���ڿ� �Դϴ�.
         */
		size_t Count(const wchar_t *str) const
		{
		    // ���ڿ� ���տ� ���Ե� str������ ������ �����ɴϴ�.
		    // (Len->Count�� ����)
			return CountHelper(str, wcslen(str));
		}

        /**
         *  @brief  ���ڿ� ���տ� ���Ե� ������ ���ڿ� ������ ������ �����ɴϴ�.
         *  @param  refer ������ ���ڿ� �Դϴ�.
         */
		size_t Count(const WString& refer) const
		{
			return CountHelper(refer.m_ptr, refer.m_length);
		}
        
        /**
         *  @brief  ���ڿ� ���տ� ���Ե� ������ ������ ������ �����ɴϴ�.
         *  @param  ch ������ �����Դϴ�.
         */
		size_t Count(const wchar_t ch) const
		{
			return wcountch(m_ptr, ch);
		}

	private:

		SplitsArray SplitHelper(const wchar_t *src, size_t srclen, size_t max)
		{
			// �޸� ���� ��������, �� �Լ��� ����Ѵٴ� �� ��ü��
			// �̹� �޸𸮿� �Ű��� ���� �ʴ� ���̶� �ؼ��ǹǷ� �׳� �Ҵ���
			// �̸� ���� �ͺ��� �̰Դ� ������ �Ƴ�
            // �������� �� �߿��ϰ� �����Ѵٸ� wcsstr���� �� �� ������ȴ�
			size_t         alloclen = max <= m_length ? max : m_length;
			wchar_t      **position = new wchar_t*[alloclen];
			size_t        *poslen   = new size_t[alloclen];
			size_t         count = 0;
			wchar_t *ptr = m_ptr, *tptr;
			
			for ( ; (tptr = wcsstr(ptr, src)) && max; max--, count++ )
			{
				position[count] = ptr;
				poslen[count] = tptr - ptr;
				ptr = tptr + srclen;
			}

			// max�� 0�� �ƴϸ� ������ �׸��� �����ϹǷ�
			bool max_remain = max > 0;

			WString **n = new WString*[count + max_remain];

			// count�� �� �� �ִٸ� (Count�Լ��� �� ���� ������ �� ������ �� ������) �� ������ ������ �� ������
			// �� ����� �𸣰ڴ�. �׷��� �� SplitHelper�Լ��� ���� ū ���ڿ��񱳿� �����ϴ�.
			// ���� ��� m_length�� 3������ ��� �� �Լ� ����� �������� �� ���̳� �� �Ҵ��Ѵٴ� �鿡�� ��ȿ�����̴�.
			for (size_t i = 0; i < count; i++)
			{
				// poslen[i]�� �뷫 position[i]-position[i-1]-srclen���� �ٲ� �� ���� ���̴�.
				// ������ split�Լ��� new�� ���������ϰ� ȣ���� ���̴�
				// new�� �� ���̵� ������� ����ϵ� �������.
				n[i] = new WString((const wchar_t *)(position[i]), poslen[i]);
			}

			if (max_remain)
			{
				n[count] = new WString((const wchar_t *)(ptr), m_length + m_ptr - ptr );
			}

			delete[] position;
			delete[] poslen;

			return SplitsArray (n, count + max_remain);
		}

        SplitsArray SplitSlowHelper(const wchar_t *src, size_t srclen, size_t max)
        {
            size_t      max_dec = max;
			size_t      count = 0;
            size_t      i = 0;
			wchar_t    *ptr, *tptr;
            
			for ( ptr = m_ptr; (tptr = wcsstr(ptr, src)) && max_dec; max_dec--, count++ )
			{
				ptr = tptr + srclen;
            }

			bool max_remain = max > 0;
            
			WString **n = new WString*[count + max_remain];
            
			for ( ptr = m_ptr; (tptr = wcsstr(ptr, src)) && max; max--, i++ )
			{
				n[i] = new WString((const wchar_t *)(ptr), tptr - ptr);
				ptr = tptr + srclen;
			}
            
			if (max_remain)
			{
				n[count] = new WString((const wchar_t *)(ptr), m_length + m_ptr - ptr );
			}
            
			return SplitsArray (n, count + max_remain);
        }

        WString SplitPositionHelper(const wchar_t *src, size_t srclen, size_t pos)
        {
			wchar_t *ptr = m_ptr, *tptr;
            
			for ( ptr = m_ptr; (tptr = wcsstr(ptr, src)) && pos; pos-- )
			{
				ptr = tptr + srclen;
            }

            if (pos)
				throw(new StringException(StringErrorCode::ComparasionSizeException));

            if (tptr)
            {
                return WString((const wchar_t *)(ptr), tptr - ptr);
            }
            else
            {
                return WString((const wchar_t *)(ptr), m_length + m_ptr - ptr);
            }
        }

	public:
        
        /**
         *  @brief  ������ ���ڿ��� ���� �ڸ� ���ڿ����� ������ ������ ������ŭ �����ɴϴ�.
         *  @param  str ���� ������ ���ڿ��Դϴ�.
         *  @param  max ������ �����Դϴ�.
         *  @return ReadOnlyArray����� �迭�������� ��ȯ�մϴ�. �ڼ��� ������
         *          collection/Array.h�� �����Ͻʽÿ�.
         *
         *  �� Ŭ������ ��� ���ڿ� �ڸ��⸦ Split�� SplitSlow�� �� ���� �������� �����մϴ�.
         *  Split�� �޸𸮸� �� ��� ����ϴ� ��� ó���ӵ��� ������, SplitSlow��
         *  Split�� �޸� �޸𸮸� ������ ����ϸ�, ó���ӵ��� �����ϴ�.
         *  �ڼ��� ������ ������ �ҽ��ڵ带 �����Ͻðų� �ۼ��ڿ��� �����Ͻʽÿ�.
         */
		SplitsArray Split(const wchar_t *str, size_t max = SIZE_MAX)
		{
			return SplitHelper(str, wcslen(str), max);
		}
        
        /**
         *  @brief  ������ ���ڿ��� ���� �ڸ� ���ڿ����� ������ ������ ������ŭ �����ɴϴ�.
         *  @param  refer ���� ������ ���ڿ��Դϴ�.
         *  @param  max ������ �����Դϴ�.
         *  @return ReadOnlyArray����� �迭�������� ��ȯ�մϴ�. �ڼ��� ������
         *          collection/Array.h�� �����Ͻʽÿ�.
         *
         *  �� Ŭ������ ��� ���ڿ� �ڸ��⸦ Split�� SplitSlow�� �� ���� �������� �����մϴ�.
         *  Split�� �޸𸮸� �� ��� ����ϴ� ��� ó���ӵ��� ������, SplitSlow��
         *  Split�� �޸� �޸𸮸� ������ ����ϸ�, ó���ӵ��� �����ϴ�.
         *  �ڼ��� ������ ������ �ҽ��ڵ带 �����Ͻðų� �ۼ��ڿ��� �����Ͻʽÿ�.
         */
		SplitsArray Split(const WString& refer, size_t max = SIZE_MAX)
		{
			return SplitHelper(refer.m_ptr, refer.m_length, max);
		}
        
        /**
         *  @brief  ������ ���ڿ��� ���� �ڸ� ���ڿ����� ������ ������ ������ŭ �����ɴϴ�.
         *  @param  str ���� ������ ���ڿ��Դϴ�.
         *  @param  max ������ �����Դϴ�.
         *  @return ReadOnlyArray����� �迭�������� ��ȯ�մϴ�. �ڼ��� ������
         *          collection/Array.h�� �����Ͻʽÿ�.
         *
         *  �� Ŭ������ ��� ���ڿ� �ڸ��⸦ Split�� SplitSlow�� �� ���� �������� �����մϴ�.
         *  Split�� �޸𸮸� �� ��� ����ϴ� ��� ó���ӵ��� ������, SplitSlow��
         *  Split�� �޸� �޸𸮸� ������ ����ϸ�, ó���ӵ��� �����ϴ�.
         *  �ڼ��� ������ ������ �ҽ��ڵ带 �����Ͻðų� �ۼ��ڿ��� �����Ͻʽÿ�.
         */
		SplitsArray SplitSlow(const wchar_t *str, size_t max = SIZE_MAX)
		{
			return SplitSlowHelper(str, wcslen(str), max);
		}
        
        /**
         *  @brief  ������ ���ڿ��� ���� �ڸ� ���ڿ����� ������ ������ ������ŭ �����ɴϴ�.
         *  @param  refer ���� ������ ���ڿ��Դϴ�.
         *  @param  max ������ �����Դϴ�.
         *  @return ReadOnlyArray����� �迭�������� ��ȯ�մϴ�. �ڼ��� ������
         *          collection/Array.h�� �����Ͻʽÿ�.
         *
         *  �� Ŭ������ ��� ���ڿ� �ڸ��⸦ Split�� SplitSlow�� �� ���� �������� �����մϴ�.
         *  Split�� �޸𸮸� �� ��� ����ϴ� ��� ó���ӵ��� ������, SplitSlow��
         *  Split�� �޸� �޸𸮸� ������ ����ϸ�, ó���ӵ��� �����ϴ�.
         *  �ڼ��� ������ ������ �ҽ��ڵ带 �����Ͻðų� �ۼ��ڿ��� �����Ͻʽÿ�.
         */
		SplitsArray SplitSlow(const WString& refer, size_t max = SIZE_MAX)
		{
			return SplitSlowHelper(refer.m_ptr, refer.m_length, max);
		}
        
        /**
         *  @brief  ������ ���ڿ��� ���� �ڸ� ���ڿ����� ���� �� ������ ��ġ�� ���� �����ɴϴ�.
         *  @param  str ���� ������ ���ڿ��Դϴ�.
         *  @param  pos ������ ��ġ�Դϴ�.
         *  @throw  ���� �ڸ� ���ڿ����� ������ ��� ������ ������ ��ġ���� ������� 
         *          StringErrorCode::ComparasionSizeException�� throw �մϴ�.
         *
         *  �� �Լ��� Split, SplitSlow�� ���� ó���� �ϳ� ���ο��� ������ ��ġ�� �͸���
         *  ������ �� �ֵ��� ����ȭ�Ǿ��ֽ��ϴ�. Count�Լ��� �̸� �˻��Ͽ� ����ϰų�, 
         *  try ... catch ... ������ �̿��Ͽ� ����Ͻʽÿ�.
         */
		WString SplitPosition(const wchar_t *str, size_t pos)
		{
			return SplitPositionHelper(str, wcslen(str), pos);
		}
        
        /**
         *  @brief  ������ ���ڿ��� ���� �ڸ� ���ڿ����� ���� �� ������ ��ġ�� ���� �����ɴϴ�.
         *  @param  refer ���� ������ ���ڿ��Դϴ�.
         *  @param  pos ������ ��ġ�Դϴ�.
         *  @throw  ���� �ڸ� ���ڿ����� ������ ��� ������ ������ ��ġ���� ������� 
         *          StringErrorCode::ComparasionSizeException�� throw �մϴ�.
         *
         *  �� �Լ��� Split, SplitSlow�� ���� ó���� �ϳ� ���ο��� ������ ��ġ�� �͸���
         *  ������ �� �ֵ��� ����ȭ�Ǿ��ֽ��ϴ�. Count�Լ��� �̸� �˻��Ͽ� ����ϰų�, 
         *  try ... catch ... ������ �̿��Ͽ� ����Ͻʽÿ�.
         */
		WString SplitPosition(const WString& refer, size_t pos)
		{
			return SplitPositionHelper(refer.m_ptr, refer.m_length, pos);
		}

	private:
		
		SplitsArray SplitReverseHelper(const wchar_t *src, size_t srclen, size_t max)
		{
			size_t         alloclen = max <= m_length ? max : m_length;
			wchar_t      **position = new wchar_t*[alloclen];
			size_t        *poslen = new size_t[alloclen];
			size_t         count = 0;
			size_t         nowlen = m_length;
			wchar_t *ptr = m_ptr, *tptr, *prev = m_last + 1;
			
			for ( ; (tptr = wcsrnstrn(m_ptr, nowlen, src, srclen)) && max; max--, count++ )
			{
				position[count] = tptr + srclen;
				poslen[count] = prev - tptr - srclen;
				nowlen = tptr - m_ptr;
				prev = tptr;
			}
			
			bool max_remain = max > 0;

			WString **n = new WString*[count + max_remain];
			
			for (size_t i = 0; i < count; i++)
			{
				n[i] = new WString((const wchar_t *)(position[i]), poslen[i]);
			}
			
			if (max_remain)
			{
				n[count] = new WString((const wchar_t *)m_ptr, prev - m_ptr);
			}

			delete[] position;
			delete[] poslen;

			return SplitsArray (n, count + max_remain);
		}

	public:
        
        /**
         *  @brief  �ڿ��� ���� ������ ���ڿ��� ���� �ڸ� ���ڿ����� ������ ������ ������ŭ
         *          �����ɴϴ�.
         *  @param  refer ���� ������ ���ڿ��Դϴ�.
         *  @param  max ������ �����Դϴ�.
         *  @return ReadOnlyArray����� �迭�������� ��ȯ�մϴ�. �ڼ��� ������
         *          collection/Array.h�� �����Ͻʽÿ�.
         */
		SplitsArray SplitReverse(const wchar_t* refer, size_t max = SIZE_MAX)
		{
			return SplitReverseHelper(refer, wcslen(refer), max);
		}
        
        /**
         *  @brief  �ڿ��� ���� ������ ���ڿ��� ���� �ڸ� ���ڿ����� ������ ������ ������ŭ
         *          �����ɴϴ�.
         *  @param  refer ���� ������ ���ڿ��Դϴ�.
         *  @param  max ������ �����Դϴ�.
         *  @return ReadOnlyArray����� �迭�������� ��ȯ�մϴ�. �ڼ��� ������
         *          collection/Array.h�� �����Ͻʽÿ�.
         */
		SplitsArray SplitReverse(const WString& refer, size_t max = SIZE_MAX)
		{
			return SplitReverseHelper(refer.m_ptr, refer.m_length, max);
		}

	private:
		
		WString BetweenHelper(const wchar_t *left, size_t llen, const wchar_t *right, size_t rlen, size_t starts)
		{
			if (starts >= m_length)
				throw(new StringException(StringErrorCode::ComparasionSizeException));

			size_t lefts = FindFirst(left, starts);
			size_t rights = FindFirst(right, lefts);

            if ( (lefts == error) || (rights == error) )
                return WString();

            lefts += llen;

			if (lefts > rights)
				std::swap(lefts, rights);

			return  Slice(lefts, rights - 1);
		}

		SplitsArray BetweensHelper(const wchar_t *left, size_t llen, const wchar_t *right, size_t rlen, size_t starts)
		{
			if (starts >= m_length)
				throw(new StringException(StringErrorCode::ComparasionSizeException));

			// ���ڿ��� asdf{asdf{asdf}asdf ��
			// ������ {��, �������� }���, asdf{asdf�� �����´�.
			// ���ڿ��� asdf{asdf}asdf}asdf ��
			// asdf�� �����´�.
			wchar_t **position = new wchar_t*[m_length];
			size_t   *poslen = new size_t[m_length];
			size_t    count = 0;

			wchar_t *ptr_starts = m_ptr + starts;
			wchar_t *ptr_ends = m_ptr + starts;

			while ( ptr_starts = wcsstr(ptr_ends, left) )
			{
				ptr_starts += llen;

				if ( ptr_ends = wcsstr(ptr_starts, right) )
				{
					position[count] = ptr_starts;
					poslen[count] = size_t(ptr_ends - ptr_starts);
					count++;
					ptr_ends += rlen;
				}
			}

			WString **n = new WString*[count];
			
			for (size_t i = 0; i < count; i++)
			{
				n[i] = new WString((const wchar_t *)(position[i]), poslen[i]);
			}
			
			delete[] position;
			delete[] poslen;
			
			return SplitsArray(n, count);
		}

	public:
        
        /**
         *  @brief  ������ �� ���ڿ� ������ ���ڿ��� �����ɴϴ�.
         *  @param  left ������ ù ��° ���ڿ��Դϴ�.
         *  @param  right ������ �� ��° ���ڿ��Դϴ�.
         *  @param  starts �˻��� ���ڿ��� ������ġ�� �����մϴ�.
         *
         *  �� �Լ��� throw���� ������, ���н� �� ���ڿ��� ��ȯ�մϴ�.
         */
		WString Between(const WString& left, const WString& right, size_t starts = 0)
		{
			return BetweenHelper(left.m_ptr, left.m_length, right.m_ptr, right.m_length, starts);
		}
        
        /**
         *  @brief  ������ �� ���ڿ� ������ ���ڿ��� �����ɴϴ�.
         *  @param  left ������ ù ��° ���ڿ��Դϴ�.
         *  @param  right ������ �� ��° ���ڿ��Դϴ�.
         *  @param  starts �˻��� ���ڿ��� ������ġ�� �����մϴ�.
         *
         *  �� �Լ��� throw���� ������, ���н� �� ���ڿ��� ��ȯ�մϴ�.
         */
		WString Between(const wchar_t *left, const wchar_t *right, size_t starts = 0)
		{
			return BetweenHelper(left, wcslen(left), right, wcslen(right), starts);
		}
		
        /**
         *  @brief  ������ �� ���ڿ� ������ ���ڿ����� ������ �����ɴϴ�.
         *  @param  left ������ ù ��° ���ڿ��Դϴ�.
         *  @param  right ������ �� ��° ���ڿ��Դϴ�.
         *  @param  starts �˻��� ���ڿ��� ������ġ�� �����մϴ�.
         *  @return ReadOnlyArray����� �迭�������� ��ȯ�մϴ�. �ڼ��� ������
         *          collection/Array.h�� �����Ͻʽÿ�.
         *
         *  �� �Լ��� ������Ž������ ������� �ۼ��Ǿ����ϴ�.
         */
		SplitsArray Betweens(const WString& left, const WString& right, size_t starts = 0)
		{
			return BetweensHelper(left.m_ptr, left.m_length, right.m_ptr, right.m_length, starts);
		}
        
        /**
         *  @brief  ������ �� ���ڿ� ������ ���ڿ����� ������ �����ɴϴ�.
         *  @param  left ������ ù ��° ���ڿ��Դϴ�.
         *  @param  right ������ �� ��° ���ڿ��Դϴ�.
         *  @param  starts �˻��� ���ڿ��� ������ġ�� �����մϴ�.
         *
         *  �� �Լ��� ������Ž������ �̿��մϴ�.
         */
		SplitsArray Betweens(const wchar_t *left, const wchar_t *right, size_t starts = 0)
		{
			return BetweensHelper(left, wcslen(left), right, wcslen(right), starts);
		}
        
        /**
         *  @brief  ������ �� ���� ������ ���ڿ��� �����ɴϴ�.
         *  @param  left ������ ù ��° �����Դϴ�.
         *  @param  right ������ �� ��° �����Դϴ�.
         *  @param  starts �˻��� ���ڿ��� ������ġ�� �����մϴ�.
         *
         *  �� �Լ��� throw���� ������, ���н� �� ���ڿ��� ��ȯ�մϴ�.
         */
		WString Between(wchar_t left, wchar_t right, size_t starts = 0)
		{
			if (starts >= m_length)
				throw(new StringException(StringErrorCode::ComparasionSizeException));

			size_t lefts = FindFirst(left, starts) + 1;
			size_t rights = FindFirst(right, lefts);

			if (lefts > rights)
				std::swap(lefts, rights);

			return (lefts != error) && (rights != error) ? Slice(lefts, rights - 1) : WString();
		}
        
        /**
         *  @brief  ������ �� ���� ������ ���ڿ����� ������ �����ɴϴ�.
         *  @param  left ������ ù ��° �����Դϴ�.
         *  @param  right ������ �� ��° �����Դϴ�.
         *  @param  starts �˻��� ���ڿ��� ������ġ�� �����մϴ�.
         *  @return ReadOnlyArray����� �迭�������� ��ȯ�մϴ�. �ڼ��� ������
         *          collection/Array.h�� �����Ͻʽÿ�.
         *
         *  �� �Լ��� ������Ž������ ������� �ۼ��Ǿ����ϴ�.
         */
		SplitsArray Betweens(wchar_t left, wchar_t right, size_t starts = 0)
		{
			if (starts >= m_length)
				throw(new StringException(StringErrorCode::ComparasionSizeException));

			wchar_t **position = new wchar_t*[m_length];
			size_t   *poslen = new size_t[m_length];
			size_t    count = 0;

			wchar_t *ptr_starts = m_ptr + starts;
			wchar_t *ptr_ends = m_ptr + starts;

			while ( ptr_starts = wcschr(ptr_ends, left) )
			{
				ptr_starts++;

				if ( ptr_ends = wcschr(ptr_starts, right) )
				{
					position[count] = ptr_starts;
					poslen[count] = size_t(ptr_ends - ptr_starts);
					count++;
					ptr_ends++;
				}
			}

			WString **n = new WString*[count];
			
			for (size_t i = 0; i < count; i++)
			{
				n[i] = new WString((const wchar_t *)(position[i]), poslen[i]);
			}
			
			delete[] position;
			delete[] poslen;
			
			return SplitsArray(n, count);
		}
        
        /**
         *  @brief  ���ڿ��� ���Ե� ��� ���ڸ� �ҹ��ڷ� ��ȯ�� ���ڿ��� �����ɴϴ�.
         */
		WString ToLower()
		{
			wchar_t *ret = new wchar_t[m_length + 1];
			for (size_t i = 0; i < m_length; i++)
				ret[i] = towlower(m_ptr[i]);
			ret[m_length] = 0;
			return WString(ret, m_length, with);
		}
		
        /**
         *  @brief  ���ڿ��� ���Ե� ��� ���ڸ� �빮�ڷ� ��ȯ�� ���ڿ��� �����ɴϴ�.
         */
		WString ToUpper()
		{
			wchar_t *ret = new wchar_t[m_length + 1];
			for (size_t i = 0; i < m_length; i++)
				ret[i] = towupper(m_ptr[i]);
			ret[m_length] = 0;
			return WString(ret, m_length, with);
		}

        /**
         *  @brief  ù ��° ���ڸ� �빮�ڷ� ��ȯ�� ���ڿ��� ��ȯ�մϴ�.
         */
		WString Capitalize()
		{
			wchar_t *ret = this->ToArray();
			*ret = towupper(*ret);
			return WString(ret, m_length, with);
		}
        
        /**
         *  @brief  ��� �ܾ��� ù ��° ���ڸ� �빮�ڷ� ��ȯ�� ���ڿ��� ��ȯ�մϴ�.
         */
		WString Title()
		{
			wchar_t *ret = this->ToArray();
			*ret = towupper(*ret);
			for (size_t i = 0; i < m_length - 1; i++)
				if (m_ptr[i] == L' ')
					ret[i + 1] = towupper(m_ptr[i + 1]);
			return WString(ret, m_length, with);
		}
		
	private:
		
		inline bool StartsWithHelper(const wchar_t *str, size_t starts, size_t len) const
		{
			if (starts >= m_length)
				return false;

			// len�� ��ġ�� ��Ȯ�� �� �� �ֱ� ������ ���ܿ� �������� �ƴ��ߴ�.
			// starts�� ���� Ȯ���� ���� ������ ������ ������ ��
			if (m_length < len + starts)
				return false;

			return !memcmp(m_ptr + starts, str, len * sizeof(wchar_t));
		}
		
	public:
        
        /**
         *  @brief  ���ڿ��� ���� �κ��� ������ ���ڿ��� ��ġ�ϴ����� ���θ� �����ɴϴ�.
         *  @param  str ������ ���ڿ��Դϴ�.
         *  @param  starts ���ۺκ��� �����մϴ�.
         */
		bool StartsWith(const wchar_t *str, size_t starts = 0) const
		{
			return StartsWithHelper(str, starts, wcslen(str));
		}
		
        /**
         *  @brief  ���ڿ��� ���� �κ��� ������ ���ڿ��� ��ġ�ϴ����� ���θ� �����ɴϴ�.
         *  @param  refer ������ ���ڿ��Դϴ�.
         *  @param  starts ���ۺκ��� �����մϴ�.
         */
		bool StartsWith(const WString& refer, size_t starts = 0) const
		{
			return StartsWithHelper(refer.m_ptr, starts, refer.m_length);
		}
        
        /**
         *  @brief  ���ڿ��� ���� �κ��� ������ ���ڿ� ��ġ�ϴ����� ���θ� �����ɴϴ�.
         *  @param  ch ������ �����Դϴ�.
         *  @param  starts ���ۺκ��� �����մϴ�.
         */
		bool StartsWith(const wchar_t ch, size_t starts) const
		{
			if (starts >= m_length)
				throw(new StringException(StringErrorCode::ComparasionSizeException));

			return m_ptr[starts] == ch;
		}
        
        /**
         *  @brief  ���ڿ��� ���� �κ��� ������ ���ڿ� ��ġ�ϴ����� ���θ� �����ɴϴ�.
         *  @param  ch ������ �����Դϴ�.
         */
		inline bool StartsWith(const wchar_t ch) const
		{
			return *m_ptr == ch;
		}

	private:

		inline bool EndsWithHelper(const wchar_t *str, size_t ends, size_t len) const
		{
			if (ends >= m_length)
				return false;

			if (m_length < len - ends)
				return false;

			return !memcmp(m_last - ends - len + 1, str, len * sizeof(wchar_t));
		}

	public:
        
        /**
         *  @brief  ���ڿ��� �� �κ��� ������ ���ڿ��� ��ġ�ϴ����� ���θ� �����ɴϴ�.
         *  @param  str ������ ���ڿ��Դϴ�.
         *  @param  ends �˻��� ������ ������ ��ġ�� �����մϴ�.
         *
         *  �� �Լ��� �Ű����� ends�� �� �κп������� ������ ��ġ�� ����ŵ�ϴ�.
         */
		bool EndsWith(const wchar_t *str, size_t ends = 0) const
		{
			return EndsWithHelper(str, ends, wcslen(str));
		}
        
        /**
         *  @brief  ���ڿ��� �� �κ��� ������ ���ڿ��� ��ġ�ϴ����� ���θ� �����ɴϴ�.
         *  @param  refer ������ ���ڿ��Դϴ�.
         *  @param  ends �˻��� ������ ������ ��ġ�� �����մϴ�.
         *
         *  �� �Լ��� �Ű����� ends�� �� �κп������� ������ ��ġ�� ����ŵ�ϴ�.
         */
		bool EndsWith(const WString& refer, size_t ends = 0) const
		{
			return EndsWithHelper(refer.m_ptr, ends, refer.m_length);
		}
        
        /**
         *  @brief  ���ڿ��� �� �κ��� ������ ���ڿ� ��ġ�ϴ����� ���θ� �����ɴϴ�.
         *  @param  ch ������ �����Դϴ�.
         *  @param  ends �˻��� ������ ������ ��ġ�� �����մϴ�.
         *
         *  �� �Լ��� �Ű����� ends�� �� �κп������� ������ ��ġ�� ����ŵ�ϴ�.
         */
		inline bool EndsWith(const wchar_t ch, size_t ends) const
		{
			if (ends >= m_length)
				return false;

			return *(m_last - ends) == ch;
		}
        
        /**
         *  @brief  ���ڿ��� �� �κ��� ������ ���ڿ� ��ġ�ϴ����� ���θ� �����ɴϴ�.
         *  @param  ch ������ �����Դϴ�.
         */
		inline bool EndsWith(const wchar_t ch) const
		{
			return *m_last == ch;
		}
        
        /**
         *  @brief  ���ڿ� ������ ũ�⸦ ������ ũ�⸸ŭ �����ϰ� ������ ���� ���������� 
         *          �����ѵ� ���� ���� ������ ������ ���ڸ� ������ ���ڿ� ������ �����ɴϴ�.
         *  @param  len ������ ũ���Դϴ�.
         *  @param  pad ������ �����Դϴ�.
         *  @return len�� ���ڿ��� ũ�⺸�� ���� ��� ���ڿ� ������ ���纻�� �����ɴϴ�.
         */
		WString PadLeft(size_t len, wchar_t pad = L' ')
		{
			if (len > m_length)
			{
				wchar_t* ret = new wchar_t[len+1];
				size_t   padlen = len - m_length;

				wcsnset(ret, pad, padlen);
				memcpy(ret + padlen, m_ptr, m_length * sizeof(wchar_t));

				ret[len] = 0; // (m_length + 1) * wchar_t�ε� ����

				return WString(ret, len, with);
			}
			else
			{
				return WString((const wchar_t *)m_ptr, m_length);
			}
		}
		
        /**
         *  @brief  ���ڿ� ������ ũ�⸦ ������ ũ�⸸ŭ �����ϰ� ������ ���� �������� 
         *          �����ѵ� ���� ������ ������ ������ ���ڸ� ������ ���ڿ� ������ �����ɴϴ�.
         *  @param  len ������ ũ���Դϴ�.
         *  @param  pad ������ �����Դϴ�.
         *  @return len�� ���ڿ��� ũ�⺸�� ���� ��� ���ڿ� ������ ���纻�� �����ɴϴ�.
         */
		WString PadRight(size_t len, wchar_t pad = L' ')
		{
			if (len > m_length)
			{
				wchar_t *ret = new wchar_t[len+1];

				memcpy(ret, m_ptr, m_length * sizeof(wchar_t));
				wcsnset(ret + m_length, pad, len - m_length);

				ret[len] = 0;

				return WString(ret, len, with);
			}
			else
			{
				return WString((const wchar_t *)m_ptr, m_length);
			}
		}
        
        /**
         *  @brief  ���ڿ� ������ ũ�⸦ ������ ũ�⸸ŭ �����ϰ� ������ ���� �����
         *          �����ѵ� ���� ���� ������ ������ ���ڸ� ������ ���ڿ� ������ �����ɴϴ�.
         *  @param  len ������ ũ���Դϴ�.
         *  @param  pad ������ �����Դϴ�.
         *  @param  lefts ���� ������ Ȧ����ŭ�ΰ�� ������ �� ��������� ���θ� �����մϴ�.
         *  @return len�� ���ڿ��� ũ�⺸�� ���� ��� ���ڿ� ������ ���纻�� �����ɴϴ�.
         */
		WString PadCenter(size_t len, wchar_t pad = L' ', bool lefts = true)
		{
			if (len > m_length)
			{
				size_t padlen = len - m_length;
				size_t lpadlen = padlen / 2 + ((padlen & 1) && lefts);
				size_t rpadlen = padlen - lpadlen;
				
				wchar_t *ret = new wchar_t[len+1];

				wcsnset(ret, pad, lpadlen);
				memcpy(ret + lpadlen, m_ptr, m_length * sizeof(wchar_t));
				wcsnset(ret + lpadlen + m_length, pad, rpadlen);

				ret[len] = 0;

				return WString(ret, len, with);
			}
			else
			{
				return WString((const wchar_t *)m_ptr, m_length);
			}
		}

	private:

		WString InsertLeftHelper(size_t separation, const wchar_t *str, size_t strlen)
		{
			size_t   sizeof_diff = (m_length - 1) / separation;
			size_t   len = sizeof_diff * strlen + m_length;
			size_t   dstrlen = strlen * sizeof(wchar_t);
			wchar_t *totalString = new wchar_t[len + 1];
			wchar_t *totalPtr = totalString + 1;

			for (size_t pos = 1; pos < m_length; pos++)
			{
				if (pos % separation == 0)
				{
					memcpy(totalPtr, str, dstrlen);
					totalPtr += strlen;
				}
				*totalPtr++ = m_ptr[pos];
			}
			
			*totalString = *m_ptr;
			totalString[len] = 0;

			return WString(totalString, len, with);
		}
		
		WString InsertRightHelper(size_t separation, const wchar_t *str, size_t strlen)
		{
			size_t   sizeof_diff = (m_length - 1) / separation;
			size_t   len = sizeof_diff * strlen + m_length;
			size_t   dstrlen = strlen * sizeof(wchar_t);
			wchar_t *totalString = new wchar_t[len + 1];
			wchar_t *totalPtr = totalString + len;
			
			for (size_t pos = m_length - 1; pos > 0; pos--)
			{
				*--totalPtr = m_ptr[pos];
				if ((m_length - pos) && (m_length - pos) % separation == 0)
				{
					memcpy(totalPtr -= strlen, str, dstrlen);
				}
			}
			
			*totalString = *m_ptr;
			totalString[len] = 0;

			return WString(totalString, len, with);
		}

	public:
		
        /**
         *  @brief  ���ڿ��� ���ʺ��� ������ ũ�⸸ŭ ���ڸ� �ǳʶٸ鼭 ������ ���ڿ���
         *          ������ ���ڿ��� �����ɴϴ�.
         *  @param  separation ������ ũ���Դϴ�.
         *  @param  str ������ ���ڿ��Դϴ�.
         *
         *  �� �Լ��� ����ϴ� ��쿡 ���� ������ �����Ͻʽÿ�.
         *  ����> "1234567890" ���� InsertLeft(3, L"++")�� ����� ���
         *       "123++456++789++0"���� ��µ�.
         *  ������ �׸� ������ ���ڿ��� �߰����� �ʽ��ϴ�.
         */
		WString InsertLeft(size_t separation, const wchar_t *str)
		{
			return InsertLeftHelper(separation, str, wcslen(str));
		}
        
        /**
         *  @brief  ���ڿ��� ���ʺ��� ������ ũ�⸸ŭ ���ڸ� �ǳʶٸ鼭 ������ ���ڿ���
         *          ������ ���ڿ��� �����ɴϴ�.
         *  @param  separation ������ ũ���Դϴ�.
         *  @param  refer ������ ���ڿ��Դϴ�.
         *
         *  �� �Լ��� ����ϴ� ��쿡 ���� ������ �����Ͻʽÿ�.
         *  ����> "1234567890" ���� InsertLeft(3, L"++")�� ����� ���
         *       "123++456++789++0"���� ��µ�.
         *  ������ �׸� ������ ���ڿ��� �߰����� �ʽ��ϴ�.
         */
		WString InsertLeft(size_t separation, const WString& refer)
		{
			return InsertLeftHelper(separation, refer.m_ptr, refer.m_length);
		}
		
        /**
         *  @brief  ���ڿ��� �����ʺ��� ������ ũ�⸸ŭ ���ڸ� �ǳʶٸ鼭 ������ ���ڿ���
         *          ������ ���ڿ��� �����ɴϴ�.
         *  @param  separation ������ ũ���Դϴ�.
         *  @param  str ������ ���ڿ��Դϴ�.
         *
         *  �� �Լ��� ����ϴ� ��쿡 ���� ������ �����Ͻʽÿ�.
         *  ����> "1234567890" ���� InsertRight(3, L",")�� ����� ���
         *       "1,234,567,890"���� ��µ�.
         *  ������ �׸� ������ ���ڿ��� �߰����� �ʽ��ϴ�.
         */
		WString InsertRight(size_t separation, const wchar_t *str)
		{
			return InsertRightHelper(separation, str, wcslen(str));
		}
        
        /**
         *  @brief  ���ڿ��� �����ʺ��� ������ ũ�⸸ŭ ���ڸ� �ǳʶٸ鼭 ������ ���ڿ���
         *          ������ ���ڿ��� �����ɴϴ�.
         *  @param  separation ������ ũ���Դϴ�.
         *  @param  refer ������ ���ڿ��Դϴ�.
         *
         *  �� �Լ��� ����ϴ� ��쿡 ���� ������ �����Ͻʽÿ�.
         *  ����> "1234567890" ���� InsertRight(3, L",")�� ����� ���
         *       "1,234,567,890"���� ��µ�.
         *  ������ �׸� ������ ���ڿ��� �߰����� �ʽ��ϴ�.
         */
		WString InsertRight(size_t separation, const WString& refer)
		{
			return InsertRightHelper(separation, refer.m_ptr, refer.m_length);
		}
        
        /**
         *  @brief  ���ڿ��� ���ʺ��� ������ ũ�⸸ŭ ���ڸ� �ǳʶٸ鼭 ������ ���ڸ�
         *          ������ ���ڿ��� �����ɴϴ�.
         *  @param  separation ������ ũ���Դϴ�.
         *  @param  ch ������ �����Դϴ�.
         *
         *  �� �Լ��� ����ϴ� ��쿡 ���� ������ �����Ͻʽÿ�.
         *  ����> "1234567890" ���� InsertLeft(2, L'%')�� ����� ���
         *       "12%34%56%78%90"���� ��µ�.
         *  ������ �׸� ������ ���ڸ� �߰����� �ʽ��ϴ�.
         */
		WString InsertLeft(size_t separation, wchar_t ch)
		{
			size_t   sizeof_diff = (m_length - 1) / separation;
			size_t   len = sizeof_diff + m_length;
			wchar_t *totalString = new wchar_t[len + 1];
			wchar_t *totalPtr = totalString + 1;

			for (size_t pos = 1; pos < m_length; pos++)
			{
				if (pos % separation == 0)
				{
					*totalPtr++ = ch;
				}
				*totalPtr++ = m_ptr[pos];
			}
			
			*totalString = *m_ptr;
			totalString[len] = 0;

			return WString(totalString, len, with);
		}
        
        /**
         *  @brief  ���ڿ��� �����ʺ��� ������ ũ�⸸ŭ ���ڸ� �ǳʶٸ鼭 ������ ���ڸ�
         *          ������ ���ڿ��� �����ɴϴ�.
         *  @param  separation ������ ũ���Դϴ�.
         *  @param  ch ������ �����Դϴ�.
         *
         *  �� �Լ��� ����ϴ� ��쿡 ���� ������ �����Ͻʽÿ�.
         *  ����> "1234567890" ���� InsertRight(3, L',')�� ����� ���
         *       "1,234,567,890"���� ��µ�.
         *  ������ �׸� ������ ���ڸ� �߰����� �ʽ��ϴ�.
         */
		WString InsertRight(size_t separation, wchar_t ch)
		{
			size_t   sizeof_diff = (m_length - 1) / separation;
			size_t   len = sizeof_diff + m_length;
			wchar_t *totalString = new wchar_t[len + 1];
			wchar_t *totalPtr = totalString + len;
			
			for (size_t pos = m_length - 1; pos > 0; pos--)
			{
				*--totalPtr = m_ptr[pos];
				if ((m_length - pos) && (m_length - pos) % separation == 0)
				{
					*--totalPtr = ch;
				}
			}
			
			*totalString = *m_ptr;
			totalString[len] = 0;

			return WString(totalString, len, with);
		}

	private:

		// ���ڿ� ���տ� ���Ե� src�� ��� dest�� 
		// ġȯ�� ���ڿ� ������ �����ɴϴ�.
		WString ReplaceHelper(const wchar_t *src, const wchar_t *dest, size_t srclen, size_t destlen, size_t max)
		{
			size_t         alloclen = max <= m_length ? max : m_length;
			wchar_t      **position = new wchar_t*[alloclen];
			size_t         count = 0;
			size_t         sourceLength;
			size_t         index = 0;
			size_t         ddestlen = destlen * sizeof(wchar_t);
			size_t         tlen;
			size_t         rest;
			const wchar_t *ptr = m_ptr;
			const wchar_t *tptr;
			wchar_t       *mergerString;
			wchar_t       *mergerPointer;
			const wchar_t *iter = m_ptr;

			for ( ; (tptr = wcsstr(ptr, src)) && max; )
			{
				position[count] = (wchar_t *)tptr;
				ptr = tptr + srclen;
				max--;
				count++;
			}
			
			sourceLength = m_length + (destlen - srclen) * count;
			mergerPointer = mergerString = new wchar_t[sourceLength + 1];

			for ( ; index < count;
					index++,
					iter += srclen + tlen,
					mergerPointer += destlen )
			{
				tlen = (size_t)((const wchar_t *)position[index] - iter);

				if (tlen > 0)
				{
					memcpy(mergerPointer, iter, tlen * sizeof(wchar_t));
				}

				memcpy(mergerPointer += tlen, dest, ddestlen);
			}
            
			rest = m_ptr + m_length - iter;

			if ( rest > 0 )
			{
				memcpy(mergerPointer, iter, rest * sizeof(wchar_t));
			}
			
			delete[] position;

			mergerString[sourceLength] = 0;

			return WString(mergerString, sourceLength, with);
		}
        
		WString ReplaceSlowHelper(const wchar_t *src, const wchar_t *dest, size_t srclen, size_t destlen, size_t max)
        {
			size_t         tlen;
			size_t         rest;
			size_t         ddestlen = destlen * sizeof(wchar_t);
            size_t         max_dec = max;
			size_t         count = 0;
			size_t         sourceLength;
			wchar_t       *ptr, *tptr;
			wchar_t       *mergerString;
			wchar_t       *mergerPointer;
			const wchar_t *iter = m_ptr;
            
			for ( ptr = m_ptr; (tptr = wcsstr(ptr, src)) && max_dec; max_dec--, count++ )
			{
				ptr = tptr + srclen;
            }

			sourceLength = m_length + (destlen - srclen) * count;
			mergerPointer = mergerString = new wchar_t[sourceLength + 1];
            
			for ( ptr = m_ptr; (tptr = wcsstr(ptr, src)) && max; 
                     max--, 
                     iter += srclen + tlen,
                     ptr = tptr + srclen, 
					 mergerPointer += destlen )
			{
                tlen = (size_t)(tptr - ptr);
                
				if (tlen > 0)
				{
					memcpy(mergerPointer, iter, tlen * sizeof(wchar_t));
				}
                
				memcpy(mergerPointer += tlen, dest, ddestlen);
			}
			
			rest = m_ptr + m_length - iter;

			if ( rest > 0 )
			{
				memcpy(mergerPointer, iter, rest * sizeof(wchar_t));
			}
			
			mergerString[sourceLength] = 0;

			return WString(mergerString, sourceLength, with);
        }

	public:
        
        /**
         *  @brief  ���ڿ��� ���Ե� Ư�� ���ڿ��� ������ ���ڿ��� ġȯ�մϴ�.
         *  @param  src � ���ڿ��� �ٲ� ������ �����մϴ�.
         *  @param  dest � ���ڿ��� �ٲ� ������ �����մϴ�.
         *  @param  max �ٲ� Ƚ���� �����մϴ�.
         *
         *  �� Ŭ������ ���ڿ� ġȯ�� Replace�� ReplaceSlow�� �� ���� �������� �����մϴ�.
         *  Replace�� �޸𸮸� �� ��� ����ϴ� ��� ó���ӵ��� ������, ReplaceSlow��
         *  Replace�� �޸� �޸𸮸� ������ ����ϸ�, ó���ӵ��� �����ϴ�.
         *  �ڼ��� ������ ������ �ҽ��ڵ带 �����Ͻðų� �ۼ��ڿ��� �����Ͻʽÿ�.
         */
		WString Replace(const wchar_t *src, const wchar_t *dest, size_t max = SIZE_MAX)
		{
			return ReplaceHelper(src, dest, wcslen(src), wcslen(dest), max);
		}
        
        /**
         *  @brief  ���ڿ��� ���Ե� Ư�� ���ڿ��� ������ ���ڿ��� ġȯ�մϴ�.
         *  @param  refer0 � ���ڿ��� �ٲ� ������ �����մϴ�.
         *  @param  refer1 � ���ڿ��� �ٲ� ������ �����մϴ�.
         *  @param  max �ٲ� Ƚ���� �����մϴ�.
         *
         *  �� Ŭ������ ���ڿ� ġȯ�� Replace�� ReplaceSlow�� �� ���� �������� �����մϴ�.
         *  Replace�� �޸𸮸� �� ��� ����ϴ� ��� ó���ӵ��� ������, ReplaceSlow��
         *  Replace�� �޸� �޸𸮸� ������ ����ϸ�, ó���ӵ��� �����ϴ�.
         *  �ڼ��� ������ ������ �ҽ��ڵ带 �����Ͻðų� �ۼ��ڿ��� �����Ͻʽÿ�.
         */
		WString Replace(const WString& refer0, const WString& refer1, size_t max = SIZE_MAX)
		{
			return ReplaceHelper(refer0.m_ptr, refer1.m_ptr, refer0.m_length, refer1.m_length, max);
		}
        
        /**
         *  @brief  ���ڿ��� ���Ե� Ư�� ���ڿ��� ������ ���ڿ��� ġȯ�մϴ�.
         *  @param  src � ���ڿ��� �ٲ� ������ �����մϴ�.
         *  @param  dest � ���ڿ��� �ٲ� ������ �����մϴ�.
         *  @param  max �ٲ� Ƚ���� �����մϴ�.
         *
         *  �� Ŭ������ ���ڿ� ġȯ�� Replace�� ReplaceSlow�� �� ���� �������� �����մϴ�.
         *  Replace�� �޸𸮸� �� ��� ����ϴ� ��� ó���ӵ��� ������, ReplaceSlow��
         *  Replace�� �޸� �޸𸮸� ������ ����ϸ�, ó���ӵ��� �����ϴ�.
         *  �ڼ��� ������ ������ �ҽ��ڵ带 �����Ͻðų� �ۼ��ڿ��� �����Ͻʽÿ�.
         */
		WString ReplaceSlow(const wchar_t *src, const wchar_t *dest, size_t max = SIZE_MAX)
		{
			return ReplaceSlowHelper(src, dest, wcslen(src), wcslen(dest), max);
		}
        
        /**
         *  @brief  ���ڿ��� ���Ե� Ư�� ���ڿ��� ������ ���ڿ��� ġȯ�մϴ�.
         *  @param  refer0 � ���ڿ��� �ٲ� ������ �����մϴ�.
         *  @param  refer1 � ���ڿ��� �ٲ� ������ �����մϴ�.
         *  @param  max �ٲ� Ƚ���� �����մϴ�.
         *
         *  �� Ŭ������ ���ڿ� ġȯ�� Replace�� ReplaceSlow�� �� ���� �������� �����մϴ�.
         *  Replace�� �޸𸮸� �� ��� ����ϴ� ��� ó���ӵ��� ������, ReplaceSlow��
         *  Replace�� �޸� �޸𸮸� ������ ����ϸ�, ó���ӵ��� �����ϴ�.
         *  �ڼ��� ������ ������ �ҽ��ڵ带 �����Ͻðų� �ۼ��ڿ��� �����Ͻʽÿ�.
         */
		WString ReplaceSlow(const WString& refer0, const WString& refer1, size_t max = SIZE_MAX)
		{
			return ReplaceSlowHelper(refer0.m_ptr, refer1.m_ptr, refer0.m_length, refer1.m_length, max);
		}

	private:

        WString TrimHelper(const wchar_t *src, size_t srclen, size_t max)
        {
            size_t         alloclen = max <= m_length ? max : m_length;
			wchar_t      **position = new wchar_t*[alloclen];
			size_t         count = 0;
			size_t         sourceLength;
			size_t         index = 0;
			size_t         tlen;
			size_t         rest;
			const wchar_t *ptr = m_ptr;
			const wchar_t *tptr;
			wchar_t       *mergerString;
			wchar_t       *mergerPointer;
			const wchar_t *iter = m_ptr;

			for ( ; (tptr = wcsstr(ptr, src)) && max; )
			{
				position[count] = (wchar_t *)tptr;
				ptr = tptr + srclen;
				max--;
				count++;
			}
			
			sourceLength = m_length - srclen * count;
			mergerPointer = mergerString = new wchar_t[sourceLength + 1];

			for ( ; index < count;
					index++,
					iter += srclen + tlen,
					mergerPointer += tlen )
			{
				tlen = (size_t)((const wchar_t *)position[index] - iter);

				if (tlen > 0)
				{
					memcpy(mergerPointer, iter, tlen * sizeof(wchar_t));
				}
			}
            
			rest = m_ptr + m_length - iter;

			if ( rest > 0 )
			{
				memcpy(mergerPointer, iter, rest * sizeof(wchar_t));
			}
			
			delete[] position;

			mergerString[sourceLength] = 0;

			return WString(mergerString, sourceLength, with);
        }
        
	public:
        
        /**
         *  @brief  ���ڿ��� ���Ե� Ư�� ���ڿ��� �����մϴ�.
         *  @param  src ������ ���ڿ��� �����մϴ�.
         */
		WString Trim(const wchar_t *src)
		{
			return TrimHelper(src, wcslen(src), SIZE_MAX);
		}
		
        /**
         *  @brief  ���ڿ��� ���Ե� Ư�� ���ڿ��� �����մϴ�.
         *  @param  refer ������ ���ڿ��� �����մϴ�.
         */
		WString Trim(const WString& refer)
		{
			return TrimHelper(refer.m_ptr, refer.m_length, SIZE_MAX);
		}
        
        /**
         *  @brief  ���ڿ��� ���ۺκк��� ������ ũ�⸸ŭ�� ���ڿ��� �����ɴϴ�.
         *  @param  len ������ ���ڿ��� ũ�⸦ �����մϴ�.
         */
		WString Remove(size_t len)
		{ // ���ڿ� ������ ó������ index���� �����ϴ� ���ڿ� ������ �����ɴϴ�.
			if (len >= m_length) // exceptrion from this method
				throw(new StringException(StringErrorCode::ComparasionSizeException));

			return this->Substring(0, len);
		}
        
        /**
         *  @brief  ���ڿ��� ������ �κк��� ������ ũ�⸸ŭ�� ���ڸ� ������ 
         *          ���ڿ��� �����ɴϴ�.
         *  @param  starts ������ �κ��� ������ġ�� �����մϴ�.
         *  @param  len ������ �κ��� ũ�⸦ �����մϴ�.
         */
		WString Remove(size_t starts, size_t len)
		{
            // ���ڿ� ������ starts ��ġ ���� count ��ŭ�� �����ϴ� ���հ� ���ڿ�
		    // ������ �������� �����ɴϴ�. (�ٱ� ���)
			if (starts + len > m_length) // exceptrion from this method
				throw(new StringException(StringErrorCode::ComparasionSizeException));

			size_t retlen = m_length - len;
			wchar_t *newString = new wchar_t[retlen + 1];

			memcpy(newString, m_ptr, starts * sizeof(wchar_t));
			memcpy(newString + starts, m_ptr + starts + len, (retlen - starts) * sizeof(wchar_t));

			newString[retlen] = 0;

			return WString(newString, retlen, with);
		}
        
        /**
         *  @brief  ���ڿ��� ���κк��� ������ ũ�⸸ŭ�� ���ڿ��� �����ɴϴ�.
         *  @param  len ������ ���ڿ��� ũ�⸦ �����մϴ�.
         */
		WString RemoveReverse(size_t len)
		{
			if (len >= m_length) // exceptrion from this method
				throw(new StringException(StringErrorCode::ComparasionSizeException));

			return this->SubstringReverse(0, len);
		}
        
        /**
         *  @brief  ���ڿ��� ������ �κк��� ������ ũ�⸸ŭ�� ���ڸ� ������ 
         *          ���ڿ��� �����ɴϴ�.
         *  @param  starts ������ �κ��� ������ġ�� �����մϴ�. �� ��ġ��
         *          ���ڿ��� �� �κк��� ���˴ϴ�.
         *  @param  len ������ �κ��� ũ�⸦ �����մϴ�.
         */
		WString RemoveReverse(size_t starts, size_t len)
		{
			if (starts + len > m_length) // exceptrion from this method
				throw(new StringException(StringErrorCode::ComparasionSizeException));

			return this->Remove(m_length - starts - len, len);
		}
        
        /**
         *  @brief  ���ڿ��� Ư���κп� ���ڿ��� �����մϴ�.
         *  @param  starts ������ �κ��� ������ġ�� �����մϴ�.
         *  @param  str ������ ���ڿ��� �����մϴ�.
         *  @param  len ������ ���ڿ��� ũ�⸦ �����մϴ�.
         *
         *  �� �Լ��� �Ű����� len�� str�� ���̺��� ª�ų� ���ƾ��մϴ�.
         */
		WString Insert(size_t starts, const wchar_t *str, size_t len)
		{
			// wchar_t�� ���� �����ε��� ���ø����� ����ؾ��ϹǷ�
			// ������� ����� �߰����� �ʾҴ�.
			if (starts >= m_length)
				throw(new StringException(StringErrorCode::ComparasionSizeException));

			if (len == 0)
				throw(new StringException(StringErrorCode::InsertionSizeZeroException));

			size_t newLen = m_length + len;

			if (newLen == 0)
				return *new WString();

			wchar_t *newString = new wchar_t[newLen + 1];
			
			memcpy(newString, m_ptr, starts * sizeof(wchar_t));
			memcpy(newString + starts, str, len * sizeof(wchar_t));
			memcpy(newString + starts + len, m_ptr + starts, (m_length - starts) * sizeof(wchar_t));

			newString[newLen] = 0;
			
			return WString(newString, newLen, with);
		}
        
        /**
         *  @brief  ���ڿ��� Ư���κп� ���ڿ��� �����մϴ�.
         *  @param  starts ������ �κ��� ������ġ�� �����մϴ�.
         *  @param  refer ������ ���ڿ��� �����մϴ�.
         *  @param  len ������ ���ڿ��� ũ�⸦ �����մϴ�.
         *
         *  �� �Լ��� �Ű����� len�� refer�� ���̺��� ª�ų� ���ƾ��մϴ�.
         */
		WString Insert(size_t starts, const WString& refer, size_t len)
		{
			return Insert(starts, refer.m_ptr, len);
		}
        
        /**
         *  @brief  ���ڿ��� Ư���κп� ���ڿ��� �����մϴ�.
         *  @param  starts ������ �κ��� ������ġ�� �����մϴ�.
         *  @param  str ������ ���ڿ��� �����մϴ�.
         */
		WString Insert(size_t starts, const wchar_t *str)
		{
			return Insert(starts, str, wcslen(str));
		}
        
        /**
         *  @brief  ���ڿ��� Ư���κп� ���ڿ��� �����մϴ�.
         *  @param  starts ������ �κ��� ������ġ�� �����մϴ�.
         *  @param  refer ������ ���ڿ��� �����մϴ�.
         */
		WString Insert(size_t starts, const WString& refer)
		{
			return Insert(starts, refer.m_ptr, refer.m_length);
		}
        
        /**
         *  @brief  ���ڿ��� ������ Ƚ����ŭ �ø��ϴ�.
         *  @param  count �ø� Ƚ���� �����մϴ�.
         */
		WString Repeat(size_t count)
		{
			size_t newLen = count * m_length;
			size_t dm_length = m_length * sizeof(wchar_t);
			wchar_t *newString = new wchar_t[newLen + 1];

			for ( size_t i = 0; i < count; i++ )
			{
				memcpy(newString + i * m_length, m_ptr, dm_length);
			}

			newString[newLen] = 0;
			
			return WString(newString, newLen, with);
		}
        
        /**
         *  @brief  ���ڿ��� �������ϴ�.
         */
		WString Reverse()
		{
			wchar_t *ret = this->ToArray();
			_wcsrev(ret);
			return WString(ret, m_length, with);
		}
		
        /**
         *  @brief  ���ڿ��� �Ϻθ� �����ɴϴ�.
         *  @param  first ������ ���ڿ��� �� �����մϴ�.
         *  @param  last ������ ���ڿ��� ����ġ�� �����մϴ�.
         *
         *  �� �Լ��� �Ű����� last�� 0���� ���� ��� �ڿ������� �����ɴϴ�.
         */
		WString Slice(size_t first, size_t last)
		{
			size_t pure = (int)last > 0 ? last : ~last + 1;
			if (first > m_length || pure > m_length || (first > pure && last > 0))
				throw(new StringException(StringErrorCode::ComparasionSizeException));

			if ((int)last > 0)
			{
				return WString((const wchar_t *)(m_ptr + first), last - first + 1);
			}
			else
			{
				return WString((const wchar_t *)(m_ptr + first), m_length - first + last);
			}
		}

        /**
         *  @brief  ���ڿ��� �Ϻθ� �����ɴϴ�.
         *  @param  skip ������ ���ڿ��� ��ġ�� �����մϴ�.
         *
         *  �� �Լ��� ���ڿ��� �� �� ���� skip��ŭ ������ �κб��� ������ ���ڿ���
         *  �����ɴϴ�.
         */
		WString Slice(size_t skip)
		{
			if ((skip << 1) > m_length)
				throw(new StringException(StringErrorCode::ComparasionSizeException));

			return WString((const wchar_t *)(m_ptr + skip), m_length - (skip << 1));
		}
        
        /**
         *  @brief  ������ ũ�⸸ŭ �ǳʶ� ���ڿ��� �����ɴϴ�.
         *  @param  jmp �ǳʶ� ũ�⸦ �����մϴ�.
         *  @param  starts �˻��� ������ġ�� �����մϴ�.
         *  @param  len �ǳʶ� �� ������ ���ڿ��� ũ�⸦ �����մϴ�.
         *  @param  remain ������ �׸��� ������ Ʋ�� ���� ���� ��� �ش� ���ڿ��� 
         *          ���������� ���θ� �����մϴ�.
         *
         *  �� �Լ��� ����ϴ� ��쿡 ���� ������ �����Ͻʽÿ�.
         *  WString("01234567").Slicing(1); // -> "0246"
         *  WString("%0123456").Slicing(1,1); // -> "0246"
         *  WString("%01%23%45%67").Slicing(1,1,2); // -> "01234567"
         *  WString("%01%23%45%6789").Slicing(1,1,2); // -> "012345679"
         *  WString("%01%23%45%6789").Slicing(1,1,2,false); // -> "01234567"
         */
		WString Slicing(size_t jmp, size_t starts = 0, size_t len = 1, bool remain = true)
		{
			size_t   searchLen = m_length - starts;

			if ( searchLen == 0 && searchLen > m_length  && len == 0 && jmp == 0 )
				throw(new StringException(StringErrorCode::ComparasionSizeException));

			if ( len <= searchLen )
			{
				size_t   chunkLen = jmp + len;
				size_t   fixedLen = (searchLen / chunkLen) * len;
				size_t   lastRemain = searchLen % chunkLen;
				size_t   remainLen = lastRemain >= len ? len : (remain ? lastRemain : 0);
				size_t   totalLen = fixedLen + remainLen;
				wchar_t* collect = new wchar_t[totalLen+1];
				wchar_t* colptr = collect;

				size_t   countLen = starts;
				size_t   putLen2 = len * sizeof(wchar_t);

				size_t   copyLen = m_length - lastRemain;

				for ( ; countLen < copyLen; countLen += chunkLen )
				{
					memcpy(colptr, m_ptr + countLen, putLen2);
					colptr += len;
				}
				
				if ( remainLen )
				{
					memcpy(colptr, m_ptr + countLen, remainLen * sizeof(wchar_t));
				}

				collect[totalLen] = 0;
				
				return WString(collect, totalLen);
			}
			else if ( remain )
			{
				return WString((const wchar_t *)(m_ptr + starts), searchLen);
			}

			return WString();
		}

	private:

		Lines LineSplitHelper(size_t len, const wchar_t *front, size_t front_len, const wchar_t *end, size_t end_len)
		{
			size_t remainLen = m_length % len;
			size_t countLineLen = m_length / len + (remainLen != 0);
			size_t eachLineLen = len + front_len + end_len;

			size_t dfront_len = front_len * sizeof(wchar_t);
			size_t dend_len = end_len * sizeof(wchar_t);
			size_t dlen = len * sizeof(wchar_t);
			
			WString **n = new WString*[countLineLen];

			for (size_t i = 0; i < countLineLen; i++)
			{
				wchar_t *partialLine = new wchar_t[eachLineLen + 1];

				if ( front_len )
					memcpy(partialLine, front, dfront_len);
				memcpy(partialLine + front_len, m_ptr + i * len, dlen);
				if ( end_len )
					memcpy(partialLine + front_len + len, end, dend_len);

				partialLine[eachLineLen] = 0;
				
				n[i] = new WString(partialLine, eachLineLen, with);
			}

			if ( remainLen != 0 )
			{
				if ( end_len )
				{
					wcsnset(n[countLineLen - 1]->m_ptr + front_len + remainLen, L' ', len - remainLen);
				}
				else
				{
					n[countLineLen - 1]->m_length = remainLen + front_len;
				}
			}

			return Lines(n, countLineLen);
		}

	public:
        
        /**
         *  @brief  ������ ũ�⸸ŭ �߶� ���๮�ڸ� ������ ���ڿ� ������ �����ɴϴ�.
         *  @param  len �ڸ� ���ڿ��� ũ�⸦ �����մϴ�.
         *  @return ReadOnlyArray����� �迭�������� ��ȯ�մϴ�. �ڼ��� ������
         *          collection/Array.h�� �����Ͻʽÿ�.
         */
		Lines LineSplit(size_t len)
		{
			return LineSplitHelper(len, nullptr, 0, nullptr, 0);
		}
		
        /**
         *  @brief  ������ ũ�⸸ŭ �߶� ���๮�ڸ� ������ �� ������ ���ڿ���
         *          �� ���ڿ� �� �տ� ������ ���ڿ� ������ �����ɴϴ�. 
         *  @param  len �ڸ� ���ڿ��� ũ�⸦ �����մϴ�.
         *  @param  front �տ� ������ ���ڿ��� �����մϴ�.
         *  @return ReadOnlyArray����� �迭�������� ��ȯ�մϴ�. �ڼ��� ������
         *          collection/Array.h�� �����Ͻʽÿ�.
         */
		Lines LineSplit(size_t len, const WString& front)
		{
			return LineSplitHelper(len, front.Reference(), front.m_length, nullptr, 0);
		}
        
        /**
         *  @brief  ������ ũ�⸸ŭ �߶� ���๮�ڸ� ������ �� ������ ���ڿ���
         *          �� ���ڿ� �� �տ� ������ ���ڿ� ������ �����ɴϴ�. 
         *  @param  len �ڸ� ���ڿ��� ũ�⸦ �����մϴ�.
         *  @param  front �տ� ������ ���ڿ��� �����մϴ�.
         *  @return ReadOnlyArray����� �迭�������� ��ȯ�մϴ�. �ڼ��� ������
         *          collection/Array.h�� �����Ͻʽÿ�.
         */
		Lines LineSplit(size_t len, const wchar_t *front)
		{
			return LineSplitHelper(len, front, wcslen(front), nullptr, 0);
		}
		
        /**
         *  @brief  ������ ũ�⸸ŭ �߶� ���๮�ڸ� ������ �� ������ ���ڿ���
         *          ���� ���ڿ� �� ��, �ڿ� ������ ���ڿ� ������ �����ɴϴ�. 
         *  @param  len �ڸ� ���ڿ��� ũ�⸦ �����մϴ�.
         *  @param  front �տ� ������ ���ڿ��� �����մϴ�.
         *  @param  end �ڿ� ������ ���ڿ��� �����մϴ�.
         *  @return ReadOnlyArray����� �迭�������� ��ȯ�մϴ�. �ڼ��� ������
         *          collection/Array.h�� �����Ͻʽÿ�.
         */
		Lines LineSplit(size_t len, const WString& front, const WString& end)
		{
			return LineSplitHelper(len, front.Reference(), front.m_length, end.Reference(), end.m_length);
		}
        
        /**
         *  @brief  ������ ũ�⸸ŭ �߶� ���๮�ڸ� ������ �� ������ ���ڿ���
         *          ���� ���ڿ� �� ��, �ڿ� ������ ���ڿ� ������ �����ɴϴ�. 
         *  @param  len �ڸ� ���ڿ��� ũ�⸦ �����մϴ�.
         *  @param  front �տ� ������ ���ڿ��� �����մϴ�.
         *  @param  end �ڿ� ������ ���ڿ��� �����մϴ�.
         *  @return ReadOnlyArray����� �迭�������� ��ȯ�մϴ�. �ڼ��� ������
         *          collection/Array.h�� �����Ͻʽÿ�.
         */
		Lines LineSplit(size_t len, const wchar_t *front, const wchar_t *end)
		{
			return LineSplitHelper(len, front, wcslen(front), end, wcslen(end));
		}
		
        /**
         *  @brief  \r\n �Ǵ� \n�� �����ڷ� �Ͽ� �ڸ� ���ڿ����� �����ɴϴ�.
         *  @param  last ������ ���� ��������� ��������� ���θ� �����մϴ�.
         *  @return ReadOnlyArray����� �迭�������� ��ȯ�մϴ�. �ڼ��� ������
         *          collection/Array.h�� �����Ͻʽÿ�.
         */
		Lines LineSplit(bool last = false)
		{
			wchar_t      **position = new wchar_t*[m_length];
			size_t        *poslen = new size_t[m_length];
			size_t         count = 0;
			size_t         szTotal;
			const wchar_t *ptr = m_ptr;
			const wchar_t *rptr = m_ptr;

			for ( ; *ptr; ptr++ )
			{
				if ( *ptr == L'\r' || *ptr == L'\n' )
				{
					position[count] = (wchar_t *)rptr;
					poslen[count] = (size_t)(ptr - rptr);
					count++;

					if ( ptr[1] == L'\n')
					{
						ptr += 1;
					}
					rptr = ptr + 1;
				}
			}
			
			szTotal = count + (last || (ptr - rptr));

			WString **n = new WString*[szTotal];
			
			for (size_t i = 0; i < count; i++)
			{
				n[i] = new WString((const wchar_t *)(position[i]), poslen[i]);
			}

			if ( last || (ptr - rptr) )
			{
				n[count] = new WString((const wchar_t *)(rptr), ptr - rptr);
			}

			delete[] position;
			delete[] poslen;

			return SplitsArray (n, szTotal);
		}
        
        /**
         *  @brief  �� �� �� len��ŭ�� ���ڼ��� �����ϰ� ���๮�ڸ� ������ 
         *          ���ڿ����� �����ɴϴ�.
         *
         *  �� �Լ����� ���๮�ڷ� \r\n�� ����մϴ�.
         */
		WString LineBreak(size_t len)
		{
			size_t remainLen = m_length % len;         // �� ������ �ٿ� ���� ���� ��
			size_t fullinsertLen = m_length / len;     // ���ڿ��� �� ��ü�� ä������ ���� ��
			size_t countLine = fullinsertLen + (remainLen != 0); // ��� �� ��

			size_t totalLen = countLine * 2 - (countLine ? 2 : 0) + m_length;

			wchar_t *origin = new wchar_t[totalLen + 1];
			wchar_t *pointer = origin;

			const wchar_t *mpointer = m_ptr;

			size_t dlen = len * sizeof(wchar_t);

			if ( fullinsertLen )
			{
				for ( size_t line = 0; line < fullinsertLen - 1; line++)
				{
					memcpy(pointer, mpointer, dlen);
					pointer[len] = L'\r';
					pointer[len + 1] = L'\n';
					pointer += len + 2;
					mpointer += len;
				}
				
				memcpy(pointer, mpointer, dlen);

				if (remainLen)
				{
					pointer[len] = L'\r';
					pointer[len + 1] = L'\n';
					pointer += len + 2;
					mpointer += len;
					memcpy(pointer, mpointer, remainLen * sizeof(wchar_t));
				}
			}
			else
			{
				memcpy(origin, m_ptr, m_length * sizeof(wchar_t));
			}

			origin[totalLen] = 0;

			return WString(origin, totalLen, with);
		}
        
        /**
         *  @brief  ���ڿ��� �ؽð��� ����մϴ�.
         *
         *  �� �Լ��� ���ɰ� ���ȿ� ������ ���� �� �ֽ��ϴ�.
         */
		uint64_t Hash(uint64_t seed = 0x8538dcfb7617fe9f) const
		{
			uint64_t num_hash = seed;
			size_t   length = m_length;
			size_t   count = 0;

			while ( length-- >= count++ )
			{
				num_hash += m_ptr[count];
				num_hash ^= m_ptr[length] * seed;
			}

			return num_hash * ((seed << 16) + (num_hash >> 16) + (num_hash << 32));
		}
        
        /**
         *  @brief  ���ڿ��� c���� �����ϴ� ���� ���ڷ� �ٲ� �� �ִ�����
         *          ���θ� �����ɴϴ�.
         */
		bool IsNumeric() const
		{
			const wchar_t *ptr = m_ptr;

			if (*ptr == L'-' || *ptr == L'+')
				ptr++;

			while (iswdigit(*ptr) && *ptr)
				ptr++;
			
			if (*ptr == L'.')
				ptr++;

			while (iswdigit(*ptr) && *ptr)
				ptr++;

			if (*ptr == L'e' || *ptr == L'E')
			{
				ptr++;
				if (*ptr == L'+' || *ptr == L'-' || iswdigit(*ptr))
				{
					ptr++;
					while (iswdigit(*ptr) && *ptr)
						ptr++;
				}
			}

			return *ptr == 0;
		}
        
        /**
         *  @brief  ���ڿ��� c���� �����ϴ� 16���� ���ڷ� �ٲ� �� �ִ�����
         *          ���θ� �����ɴϴ�.
         */
		bool IsHexDigit() const
		{
			const wchar_t *ptr = m_ptr;

			if (*ptr == L'0' && (ptr[1] == L'x' || ptr[1] == L'X'))
				ptr += 2;

			while (iswxdigit(*ptr) && *ptr)
				ptr++;

			return *ptr == 0;
		}
        
        /**
         *  @brief  16������ ���ڿ��� ���ڷ� �ٲ� ���� ����մϴ�.
         */
		unsigned long long int ToHexDigit() const
		{
			unsigned long long int ret = 0;
			const wchar_t *ptr = m_ptr;

			if (*ptr == L'0' && (ptr[1] == L'x' || ptr[1] == L'X'))
				ptr += 2;

			while (*ptr)
			{
				if (iswdigit(*ptr))
					ret = ret * 16 + *ptr++ - L'0';
				else // no check
					ret = ret * 16 + (*ptr++ & 15) + 9;
			}

			return ret;
		}
        
        /**
         *  @brief  ���ڿ��� ���ڷ� ��ȯ�մϴ�.
         *  @return ���ڿ� ������ ù ��Ҹ� ��ȯ�մϴ�.
         *  @throw  ���ڿ��� ũ�Ⱑ 1�̿����ϸ�, �׷��� ���� ���
         *          StringErrorCode::OverflowReferenceException�� throw�˴ϴ�.
         */
		inline wchar_t ToChar() const
		{
			if (m_length != 1)
				throw(new StringException(StringErrorCode::OverflowReferenceException));

			return m_ptr[0];
		}
        
        /**
         *  @brief  10�� ������ ǥ���� ���ڿ��� ���ڷ� ��ȯ�մϴ�.
         */
		long long int ToLongLong() const
		{
			long long int ret = 0, mark = 1;
			const wchar_t *ptr = m_ptr;

			if (*ptr == L'-')
				mark = -1, ptr++;
			else if (*ptr == L'+')
				ptr++;

			while (*ptr)
				ret = ret * 10 + (*ptr++ & 0xf);

			return ret * mark;
		}
        
        /**
         *  @brief  10�� ������ ǥ���� ���ڿ��� ���ڷ� ��ȯ�մϴ�.
         */
		unsigned long long int ToULongLong() const
		{
			long long int ret = 0;
			const wchar_t *ptr = m_ptr;

			if (*ptr == L'-' || *ptr == L'+')
				throw(new StringException(StringErrorCode::ToNumericSignException));

			while (*ptr)
				ret = ret * 10 + (*ptr++ & 0xf);

			return ret;
		}
        
        /**
         *  @brief  10�� ������ ǥ���� ���ڿ��� ���ڷ� ��ȯ�մϴ�.
         */
		long int ToLong() const
		{
			return (long)ToLongLong();
		}
        
        /**
         *  @brief  10�� ������ ǥ���� ���ڿ��� ���ڷ� ��ȯ�մϴ�.
         */
		unsigned long int ToULong() const
		{
			return (unsigned long)ToULongLong();
		}
        
        /**
         *  @brief  10�� ������ ǥ���� ���ڿ��� ���ڷ� ��ȯ�մϴ�.
         */
		int ToInteger() const
		{
			return (int)ToLongLong();
		}
        
        /**
         *  @brief  10�� ������ ǥ���� ���ڿ��� ���ڷ� ��ȯ�մϴ�.
         */
		unsigned int ToUInteger() const
		{
			return (int)ToULongLong();
		}
        
        /**
         *  @brief  10�� ������ ǥ���� ���ڿ��� ���ڷ� ��ȯ�մϴ�.
         */
		short int ToShort() const
		{
			return (short)ToLongLong();
		}
        
        /**
         *  @brief  10�� ������ ǥ���� ���ڿ��� ���ڷ� ��ȯ�մϴ�.
         */
		unsigned short int ToUShort() const
		{
			return (unsigned)ToULongLong();
		}

	private:

		long double ToLongDoubleHelper(const wchar_t *ptr) const
		{
			unsigned char sign = 0;
			long double   digit = 0.;
			long double   base = 1.;
			long double   step = 1.;
			wchar_t       token;

			while (token = *ptr++)
			{
				switch (token)
				{
				case L'-':
					sign = 0x80;
					break;
				case L'.':
					step = 10;
					break;
				case L'E':
				case L'e':
					((unsigned char*)&digit)[sizeof(long double) - 1] |= sign;
					return  digit / base * powl(10., ToLongDoubleHelper(ptr));
				default:
					if (token >= L'0' && token <= L'9')
					{
						digit = digit * 10. + (token & 0xF);
						base *= step;
					}
				}
			}

			((unsigned char*)&digit)[sizeof(long double) - 1] |= sign;
			return  digit / base;
		}

	public:
        
        /**
         *  @brief  10�� �Ǽ��� ǥ���� ���ڿ��� ���ڷ� ��ȯ�մϴ�.
         */
		long double ToLongDouble() const
		{
			return ToLongDoubleHelper(m_ptr);
		}
        
        /**
         *  @brief  10�� �Ǽ��� ǥ���� ���ڿ��� ���ڷ� ��ȯ�մϴ�.
         */
		double ToDouble() const
		{
			return (double)ToLongDouble();
		}
        
        /**
         *  @brief  10�� �Ǽ��� ǥ���� ���ڿ��� ���ڷ� ��ȯ�մϴ�.
         */
		float ToFloat() const
		{
			return (float)ToLongDouble();
		}
        
        /**
         *  @brief  ���ڿ��� ���纻�� �����ɴϴ�.
         */
		inline wchar_t *ToArray() // ����Ʈ�� ������ �̰� ĳ��������.
		{
			wchar_t *ret = new wchar_t[m_length + 1];
			memcpy(ret, m_ptr, (m_length + 1) * sizeof(wchar_t));
			return ret;
		}

        /**
         *  @brief  ���ڿ��� ���纻�� �����ɴϴ�.
         */
		inline const wchar_t *ToArray() const
		{
			wchar_t *ret = new wchar_t[m_length + 1];
			memcpy(ret, m_ptr, (m_length + 1) * sizeof(wchar_t));
			return ret;
		}
        
        /**
         *  @brief  ���ڿ��� Ansi�� ��ȯ�� ���ڿ��� �����ɴϴ�.
         */
		char *ToAnsi()
		{
			return UnicodeToAnsi();
		}
        
        /**
         *  @brief  ���ڿ��� ToUtf8�� ��ȯ�� ���ڿ��� �����ɴϴ�.
         *  @return ReadOnlyArray����� �迭�������� ��ȯ�մϴ�. �ڼ��� ������
         *          collection/Array.h�� �����Ͻʽÿ�.
         *
         *  �� �Լ��� ��ȯ������ ����Ʈ�迭�Դϴ�.
         */
		Utf8Array ToUtf8(bool file_bom = false)
		{
		    // �� ��ȯ�� �ִ��� ������ ������ ������� ũ���� ������ �Ұ��� �ϴٴ� ���̴�.
		    // �̸� �ذ��Ϸ��� �� ������ �� �������ų� bytebuffer�� ����°� �ּ���������.
		    // �׷��� ���⼱ �� ���� ������ ���� �����Ͽ���.
		    // ������ ȯ�濡���� �����ȴ�.
			size_t szReal = file_bom + (file_bom << 1);
			size_t size = szReal + m_length;
			unsigned long *tmp = new unsigned long[size];
			unsigned long *ptr = tmp;

			if ( file_bom )
			{
				ptr[0] = 0xef; ptr[1] = 0xbb; ptr[2] = 0xbf;

				ptr += 3;
			}

			// 3 byte�̻��� Encoding::ToUtf8�� ����.
			for ( size_t i = 0; i < m_length; i++ )
			{
				wchar_t ch = m_ptr[i];
				unsigned long put = ch;

				if (ch > 0x7f)
				{
					put = (ch & 0x3f) | ((ch << 2) & 0x3f00);
					if (ch < 0x800)
						szReal += 2, put |= 0xc080;
					else
						szReal += 3, put |= ((ch << 4) & 0x3f0000) | 0xe08080;
				}
				else
					szReal++;

				ptr[i] = put;
			}

			unsigned char *bytes = new unsigned char[szReal];

			for ( size_t i = 0, j = 0; i < size; i++ )
			{
				if (tmp[i] >= 0x10000)
				{
					bytes[j  ] = (unsigned char)((tmp[i] & 0xff0000) >> 16);
					bytes[j+1] = (unsigned char)((tmp[i] & 0x00ff00) >> 8);
					bytes[j+2] = (unsigned char)((tmp[i] & 0x0000ff));
					j += 3;
				}
				else if (tmp[i] >= 0x100)
				{
					bytes[j  ] = (unsigned char)((tmp[i] & 0xff00) >> 8);
					bytes[j+1] = (unsigned char)((tmp[i] & 0xff));
					j += 2;
				}
				else
				{
					bytes[j] = (unsigned char)((tmp[i] & 0xff));
					j += 1;
				}
			}

			delete[] tmp;

			return Utf8Array(bytes, szReal);
		}
        
        /**
         *  @brief  �� ���ڿ��� �����մϴ�.
         */
		WString operator&(const WString& concat)
		{
			return this->Concat(*this, concat);
		}

        /**
         *  @brief  �� ���ڿ��� �����մϴ�.
         */
		WString operator+(const WString& concat)
		{
			return this->Concat(*this, concat);
		}
        
        /**
         *  @brief  �� ���ڿ��� ���մϴ�.
         */
		inline bool operator>(const WString& compare)
		{
			return wcscmp(m_ptr, compare.m_ptr) > 0;
		}

        /**
         *  @brief  �� ���ڿ��� ���մϴ�.
         */
		inline bool operator<(const WString& compare)
		{
			return wcscmp(m_ptr, compare.m_ptr) < 0;
		}

        /**
         *  @brief  �� ���ڿ��� ���մϴ�.
         */
		inline bool operator>=(const WString& compare)
		{
			return !this->operator<(compare);
		}

        /**
         *  @brief  �� ���ڿ��� ���մϴ�.
         */
		inline bool operator<=(const WString& compare)
		{
			return !this->operator>(compare);
		}
        
        /**
         *  @brief  �� ���ڿ��� ���ιٲߴϴ�.
         */
		inline void Swap(WString& refer)
		{
			std::swap(m_ptr, refer.m_ptr);
			std::swap(m_last, refer.m_last);
			std::swap(m_length, refer.m_length);
		}
        
        /**
         *  @brief  ������ ���ڿ��� �����մϴ�.
         */
		void operator=(const WString& refer)
		{
			if (m_ptr != nullptr)
				delete[] m_ptr;
			if (m_length = refer.m_length)
			{
				m_ptr = new wchar_t[m_length + 1];
				m_last = m_ptr + m_length - 1;
				memcpy(m_ptr, refer.m_ptr, (m_length + 1) * sizeof(wchar_t));
			}
		}
        
        /**
         *  @brief  ������ ���ڿ��� Ŭ���� ����ϴ�.
         */
		inline void Clone(const WString& refer)
		{
			if (m_ptr != nullptr)
				delete[] m_ptr;
			m_ptr = nullptr;
			srp = true;
			m_ptr = refer.m_ptr;
			m_last = refer.m_last;
			m_length = refer.m_length;
		}
        
        /**
         *  @brief  ������ ���ڿ��� Ŭ���� ����ϴ�.
         */
		inline WString Clone()
		{
			WString nstr;
			nstr.Clone(*this);
			return nstr;
		}
        
        friend std::wostream& operator<<(std::wostream& os, const WString& refer)
        {
            if (refer.Null())
            {
                os << L"Null-(0)";
            }
            else
            {
                os << refer.Reference();
                return os;
            }
        }

		WString operator+=(const WString&) = delete;
		WString operator&=(const WString&) = delete;

	private:

		void InitString(const wchar_t *str)
		{
			m_length = wcslen(str);
			m_ptr = new wchar_t[m_length + 1];
			m_last = m_ptr + m_length - 1;
			memcpy(m_ptr, str, (m_length + 1) * sizeof(wchar_t));
		}
		
		void InitString(const char *str)
		{
			m_length = strlen(str);
			m_ptr = new wchar_t[m_length + 1];
			size_t converted = 0;
            // �̰� ǥ����? �𸣰���
			mbstowcs_s(&converted, m_ptr, m_length + 1, str, SIZE_MAX);
		}
		
		void AnsiToUnicode(const char *ansi, size_t len)
		{
			wchar_t *ptr = m_ptr = new wchar_t[len + 1];
			size_t rlen = len;
			m_length = len;
            // ����
			while (rlen--)
				*ptr++ = (wchar_t)*ansi++;
			*ptr = 0;
			m_last = m_ptr + len - 1;
		}

		// perfect ansi�����Ѵ�.
		char *UnicodeToAnsi()
		{
			char *ret = new char[m_length + 1];
			char *ptr = ret;
			wchar_t *unicode = m_ptr;
			size_t rlen = m_length;
			while (rlen--)
				*ptr++ = (char)*unicode++;
			*ptr = 0;
			return ret;
		}
		
        //
        //                  * warning *
        //
        //  ����� ���⼭���� �� �� ���� �ð��� ��ǳ�� ������ �˴ϴ�.
        //  ǥ�ؿ� ���ǵ� �°� ����ȯ ��Ģ�� ���� ��Ģ�� ����� ȥ����
        //  ī������ ������ �� �� �Դϴ�.
        //

#define _MAGIC	checker_type(~0ULL/0xff)
#define _WMAGIC	checker_type(~0ULL/0xffff)
		
		inline checker_type HazHelper(checker_type src) const
		{
			// 0x0101010101010101
			// 0x8080808080808080
			return (src - ( _MAGIC)) & ((~src & (( _MAGIC) <<  7)));
		}

		inline checker_type HawzHelper(checker_type src) const
		{
			// 0x0001000100010001
			// 0x8000800080008000
			return (src - (_WMAGIC)) & ((~src & ((_WMAGIC) << 15)));
		}

		// xxxlen �ݺ� ��ũ��
#define _K(n)    if(ptr[n] == 0) return(n);
#define _ZIF(n)  if( HazHelper(trim[n])) {trim += n; break;}
#define _ZWIF(n) if(HawzHelper(trim[n])) {trim += n; break;}

		size_t strlen(const char* ptr) const
		{
			_K(0) _K(1) _K(2) _K(3)
	_X_BRU( _K(4) _K(5) _K(6) _K(7) )
			
			checker_type *trim = 
#ifdef _X64_MODE
				(checker_type *)(((checker_type)ptr & -8LL) + 8);
#else
				(checker_type *)(((checker_type)ptr & -4L) + 4);
#endif
			while (true)
			{
				_ZIF( 0)_ZIF( 1)_ZIF( 2)_ZIF( 3)
				_ZIF( 4)_ZIF( 5)_ZIF( 6)_ZIF( 7)
				_ZIF( 8)_ZIF( 9)_ZIF(10)_ZIF(11)
				_ZIF(12)_ZIF(13)_ZIF(14)_ZIF(15)

				trim += 16;
			}
			
			if (!(trim[0] & (checker_type)( 0xffUL << (8 * 0)))) return ((size_t)trim - (size_t)ptr) + 0;
			if (!(trim[0] & (checker_type)( 0xffUL << (8 * 1)))) return ((size_t)trim - (size_t)ptr) + 1;
			if (!(trim[0] & (checker_type)( 0xffUL << (8 * 2)))) return ((size_t)trim - (size_t)ptr) + 2;
            // ������ ��Ģ�� ������ else�� �ٲ� �� ����
			if (!(trim[0] & (checker_type)( 0xffUL << (8 * 3)))) return ((size_t)trim - (size_t)ptr) + 3;
#ifdef _X64_MODE
			if (!(trim[0] & (checker_type)(0xffULL << (8 * 4)))) return ((size_t)trim - (size_t)ptr) + 4;
			if (!(trim[0] & (checker_type)(0xffULL << (8 * 5)))) return ((size_t)trim - (size_t)ptr) + 5;
			if (!(trim[0] & (checker_type)(0xffULL << (8 * 6)))) return ((size_t)trim - (size_t)ptr) + 6;
            // ������ ��Ģ�� ������ else�� �ٲ� �� ����
			if (!(trim[0] & (checker_type)(0xffULL << (8 * 7)))) return ((size_t)trim - (size_t)ptr) + 7;
#endif
            // ������ ��ȯ������ ���Ƿ� �������� �ƴ���
		}

		size_t wcslen(const wchar_t *ptr) const
		{
			_K(0) _K(1)
	_X_BRU( _K(3) _K(4) )
			
			checker_type *trim = 
#ifdef _X64_MODE
				(checker_type *)(((checker_type)ptr & -8LL) + 8);
#else
				(checker_type *)(((checker_type)ptr & -4L) + 4);
#endif
			while (true)
			{
				_ZWIF( 0)_ZWIF( 1)_ZWIF( 2)_ZWIF( 3)
				_ZWIF( 4)_ZWIF( 5)_ZWIF( 6)_ZWIF( 7)
				_ZWIF( 8)_ZWIF( 9)_ZWIF(10)_ZWIF(11)
				_ZWIF(12)_ZWIF(13)_ZWIF(14)_ZWIF(15)

				trim += 16;
			}
			
			if (!(trim[0] & (checker_type)( 0xffffUL << (16 * 0)))) return (((size_t)trim - (size_t)ptr) >> 1) + 0;
            // ������ ��Ģ�� ������ else�� �ٲ� �� ����
			if (!(trim[0] & (checker_type)( 0xffffUL << (16 * 1)))) return (((size_t)trim - (size_t)ptr) >> 1) + 1;
#ifdef _X64_MODE
			if (!(trim[0] & (checker_type)(0xffffULL << (16 * 2)))) return (((size_t)trim - (size_t)ptr) >> 1) + 2;
            // ������ ��Ģ�� ������ else�� �ٲ� �� ����
			if (!(trim[0] & (checker_type)(0xffffULL << (16 * 3)))) return (((size_t)trim - (size_t)ptr) >> 1) + 3;
#endif
            // ������ ��ȯ������ ���Ƿ� �������� �ƴ���
		}

		wchar_t *wcsrnchr(wchar_t *ptr, size_t len, wchar_t ch) const
		{
			wchar_t *lptr = ptr + len - 1;
			wchar_t *aptr = (wchar_t *)
				_X_SEL(((checker_type)lptr & -8LL),((checker_type)lptr & -4L));
			
			checker_type wide_checker = 
	   _X_BRU ( ((checker_type)ch << 48) | )
	   _X_BRU ( ((checker_type)ch << 32) | )
				((checker_type)ch << 16) |
				((checker_type)ch <<  0  );
			
			if (aptr < ptr) aptr = ptr;

			for ( ; lptr >= aptr; --lptr )
				if (*lptr == ch) return  lptr;

			for (lptr = aptr - _X_SEL(4, 2); lptr >= ptr; lptr -= _X_SEL(4, 2)) 
			{
				checker_type nptr = *(checker_type *)lptr ^ wide_checker;

				if ( HawzHelper(nptr) ) 
				{
			_X_BRU( if (lptr[3] == ch) return lptr + 3; )
			_X_BRU( if (lptr[2] == ch) return lptr + 2; )
					if (lptr[1] == ch) return lptr + 1;
					return lptr; 
				} 
			}

			for (lptr += _X_SEL(7, 3); lptr >= aptr; --lptr)
				if (*lptr == ch) return  lptr;

			return NULL;
		}
		
		wchar_t *wcsrnstrn(wchar_t *ptr, size_t ptrlen, const wchar_t *dest, size_t destlen) const
		{
			wchar_t *tptr;
			size_t len2 = destlen  * sizeof(wchar_t);

			while (tptr = wcsrnchr(m_ptr, ptrlen, *dest))
			{
				ptrlen = tptr - m_ptr;
				if (!memcmp(tptr, dest, len2))
					return tptr;
			}

			return NULL;
		}

		void wcsnset(wchar_t *ptr, wchar_t ch, size_t len) const
		{
			wchar_t *trim = (wchar_t *)
				_X_SEL((((checker_type)ptr & -8LL) + 8),(((checker_type)ptr & -4L) + 4));

			if (trim < ptr) trim = ptr;
			
			for ( ; ptr < trim && len; ptr++, len-- )
				*ptr = ch;

			if ( len > 0 )
			{
				checker_type put =
				   _X_BRU ( ((checker_type)ch << 48) | )
				   _X_BRU ( ((checker_type)ch << 32) | )
							((checker_type)ch << 16) |
							((checker_type)ch <<  0  );

				while ( len >= sizeof(checker_type)/2 )
				{
					*(checker_type *)trim = put;
					trim += sizeof(checker_type) / 2;
					len  -= sizeof(checker_type) / 2;
				}

	   _X_BRU ( if ( len == 3 ) {*(uint32_t *)trim = put; trim[2] = ch; return;} )
	   _X_BRU ( if ( len == 2 ) {*(uint32_t *)trim = put; return;} )
				if ( len == 1 ) {*(uint16_t *)trim = put; return;}
			}
		}

		size_t wcountch(wchar_t *ptr, wchar_t ch) const
		{
			size_t count = 0;
			wchar_t *trim = (wchar_t *)
				_X_SEL((((checker_type)ptr & -8LL) + 8), (((checker_type)ptr & -4L) + 4));

			checker_type wide_checker = 
	   _X_BRU ( ((checker_type)ch << 48) | )
	   _X_BRU ( ((checker_type)ch << 32) | )
				((checker_type)ch << 16) |
				((checker_type)ch <<  0  );
			
			if (trim < ptr) trim = ptr;

			for (; ptr < trim; ptr++)
				if (*ptr == ch) count++;

			for (ptr = trim; m_last - _X_SEL(4, 2) >= ptr; ptr += _X_SEL(4, 2))
			{
				checker_type nptr = *(checker_type *)ptr ^ wide_checker;

				if (HawzHelper(nptr))
				{
#if 0
					checker_type switchZero = 0;
					
					switchZero =
                       ((nptr & 0x000000000000ffff) - (_WMAGIC & 0x000000000000ffff)) & 0x000000000000ffff
                     | ((nptr & 0x00000000ffff0000) - (_WMAGIC & 0x00000000ffff0000)) & 0x00000000ffff0000
           _X_BRU (  | ((nptr & 0x0000ffff00000000) - (_WMAGIC & 0x0000ffff00000000)) & 0x0000ffff00000000 )
           _X_BRU (  | ((nptr & 0xffff000000000000) - (_WMAGIC & 0xffff000000000000)) & 0xffff000000000000 )
					;

					count += (switchZero & ((~nptr & ((_WMAGIC) << 15)))) / 0x8000 % 0xffff;
#else
					if ( ptr[0] == ch ) count++;
					if ( ptr[1] == ch ) count++;
		   _X_BRU ( if ( ptr[2] == ch ) count++;)
		   _X_BRU ( if ( ptr[3] == ch ) count++;)
#endif
				}
			}
			
			for (; ptr <= m_last; ptr++)
				if (*ptr == ch) count++;
            
			return count;
		}

	};
    
    inline WString operator"" _ws(const wchar_t* str, size_t length)
    {
        return WString{str, length};
    }

    inline WString operator"" _ws(const char* str, size_t length)
    {
        return WString{str, length};
    }

    inline WString operator"" _ws(unsigned long long i)
    {
        return WString{i};
    }

}

#endif