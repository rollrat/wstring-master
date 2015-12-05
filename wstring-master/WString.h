/***

   RollRat Software Project.
   Copyright (C) 2015. rollrat. All Rights Reserved.

File name:
   
   WString.h

Purpose:

	RollRat Library

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

#include "Array.h"

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
	//      7. �ڵ�� ���� ���� �ѵ� SIMD�� ���� �� �����ϴ�.
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
		wchar_t *m_ptr;
		wchar_t *m_last;
		size_t   m_length;

		StringReferencePutWith with;
		bool srp = false;

	public:

		typedef ReadOnlyArray<WString *> SplitsArray;
		typedef ReadOnlyArray<WString *> Lines;
		typedef ReadOnlyArray<unsigned char> Utf8Array;

		static const size_t error = -1;

		WString()
			: m_length(0)
			, m_ptr(nullptr)
			, m_last(m_ptr)
		{
		}
		
		WString(const char *ptr)
		{
			AnsiToUnicode(ptr, strlen(ptr));
		}

		WString(const char *ptr, size_t len)
		{
			AnsiToUnicode(ptr, len);
		}

		WString(const wchar_t *ptr)
		{
			InitString(ptr);
		}
		
		// ���� �� �����ڴ� �� ���̺귯������ ���� ���� ���δ�.
		// �� ����̿ܿ� ������ ������ �� �ִ� ����� �˸� �˷��ָ� �����ϰڴ�.

		// ũ����� �ԷµǴ� ��쿡 ���Ͽ� ������ ���� ����
		WString(wchar_t *ptr, size_t len)
			: m_length(len)
			, m_ptr(ptr)
			, m_last(ptr + len - 1)
			, srp(true)
		{
		}

		WString(wchar_t *ptr, size_t len, StringReferencePutWith)
			: m_length(len)
			, m_ptr(ptr)
			, m_last(ptr + len - 1)
		{
		}

		WString(const wchar_t *ptr, size_t len)
			: m_length(len)
		{
			m_ptr = new wchar_t[m_length + 1];
			m_last = m_ptr + m_length - 1;
			memcpy(m_ptr, ptr, m_length * sizeof(wchar_t));
			m_last[1] = 0;
		}
		
		WString(wchar_t ch, size_t count)
			: m_length(count)
		{
			m_ptr = new wchar_t[m_length + 1];
			m_last = m_ptr + m_length - 1;
			wcsnset(m_ptr, ch, count);
			m_ptr[m_length] = 0;
		}
		
		WString(wchar_t ch)
			: m_length(1)
		{
			m_ptr = new wchar_t[2];
			m_last = m_ptr;
			*m_ptr = ch;
			*(m_last + 1) = 0;
		}
		
		WString(char ch, size_t count)
			: WString((wchar_t)ch, count)
		{
		}
		
		WString(char ch)
			: WString((wchar_t)ch)
		{
		}
		
		WString(unsigned char ch)
			: WString((wchar_t)ch)
		{
		}

		// short�� �־��µ� �������ؼ� ��������
		WString(int num)
		{
			wchar_t buffer[65];
			_itow_s(num, buffer, 10);
			InitString((const wchar_t *)buffer);
		}
		
		WString(long int num)
		{
			wchar_t buffer[65];
			_ltow_s(num, buffer, 10);
			InitString((const wchar_t *)buffer);
		}
		
		WString(long long int num)
		{
			wchar_t buffer[65];
			swprintf_s(buffer, L"%lld", num, 65);
			InitString((const wchar_t *)buffer);
		}
		
		WString(unsigned int num)
		{
			wchar_t buffer[65];
			swprintf_s(buffer, L"%u", num, 65);
			InitString((const wchar_t *)buffer);
		}
		
		WString(unsigned long int num)
		{
			wchar_t buffer[65];
			swprintf_s(buffer, L"%lu", num, 65);
			InitString((const wchar_t *)buffer);
		}
		
		WString(unsigned long long int num)
		{
			wchar_t buffer[65];
			swprintf_s(buffer, L"%llu", num, 65);
			InitString((const wchar_t *)buffer);
		}
		
		WString(float num)
		{
			wchar_t buffer[65];
			swprintf_s(buffer, L"%g", num, 65);
			InitString((const wchar_t *)buffer);
		}
		
		WString(double num)
		{
			wchar_t buffer[65];
			swprintf_s(buffer, L"%lg", num, 65);
			InitString((const wchar_t *)buffer);
		}
		
		WString(long double num)
		{
			wchar_t buffer[65];
			swprintf_s(buffer, L"%llg", num, 65);
			InitString((const wchar_t *)buffer);
		}
		
		WString(const WString& cnt)
			: WString((const wchar_t *)cnt.m_ptr, cnt.m_length)
		{
		}

		WString(std::wstring& str)
			: WString(&str[0], str.length())
		{
		}

		WString(const std::string& str)
			: WString(str.c_str(), str.length())
		{
		}

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

		// ���ڿ��� ���̸� �����ɴϴ�.
		inline size_t Length() const
		{
			return m_length;
		}
		
		// ���ڿ� ������ ũ�Ⱑ 0���� Ȯ���մϴ�.
		inline bool Empty() const
		{
			return m_length == 0;
		}

		// ���ڿ� ������ ũ�Ⱑ 0���� ū�� Ȯ���մϴ�.
		inline bool Full() const
		{
			return m_length > 0;
		}

		// ���ڿ��� �����Ǿ� �ִ� ���� ���θ� Ȯ���մϴ�.
		inline bool Null() const
		{
			return m_ptr == nullptr;
		}
		
		// ���ڿ��� �����մϴ�.
		inline const wchar_t *Reference() const
		{
			return m_ptr;
		}

	private:

		// �� �Լ��� Insert(0, str, len)�� �߻��Դϴ�.
		// ���� Append�� �����Ϸ��� StringBuilder�� �̿��Ͻʽÿ�.
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

		WString Append(const wchar_t *str)
		{
			return Append(str, wcslen(str));
		}

		WString Append(const WString& refer)
		{
			return Append(refer.m_ptr, refer.m_length);
		}

		// �� ���ڿ��� �����մϴ�.
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
			memcpy(mergerString + t2.m_length, t2.m_ptr, t2.m_length * sizeof(wchar_t));
			mergerString[newSize] = 0;
			
			StringReferencePutWith with;
			return WString(mergerString, newSize, with);
		}

		// �� ���ڿ��� �����մϴ�.
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

		// �� ���ڿ��� �����մϴ�.
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

		// �� ���ڿ��� ���� ���մϴ�.
		inline size_t CompareTo(const wchar_t *str) const
		{
			return wcscmp(m_ptr, str);
		}
		
		inline size_t CompareTo(const WString& refer) const
		{
			return CompareTo(refer.m_ptr);
		}

		inline static int Comparer(const WString& r1, const WString& r2)
		{
			return wcscmp(r1.m_ptr, r2.m_ptr);
		}

		// �� ���ڿ��� ���� ������ Ȯ���մϴ�.
		inline bool Equal(const wchar_t *str) const
		{
			size_t strlen = wcslen(str);

			if ( strlen == m_length )
				return !memcmp(m_ptr, str, m_length * sizeof(wchar_t));

			return false;
		}

		inline bool Equal(const WString& refer) const
		{
			if (refer.m_length != this->m_length)
				return false;
			
			return !memcmp(m_ptr, refer.m_ptr, m_length * sizeof(wchar_t));
		}
		
		inline bool operator==(const wchar_t *ptr) const
		{
			return Equal(ptr);
		}

		inline bool operator==(const WString& refer) const
		{
			return Equal(refer);
		}

		inline bool operator!=(const wchar_t *ptr) const
		{
			return !Equal(ptr);
		}

		inline bool operator!=(const WString& refer) const
		{
			return !Equal(refer);
		}
		
		// �տ��� ���� �о�´�. ���� ���� ������ Last�� ¦�� ���߱� ���� ����
		// vb���� Right, Left�� ����ϰ�
		// ���⼱ Substring�� SubstringReverse�� Remove�� ����ϴ�.
		inline wchar_t First(size_t pos) const
		{
			if (pos >= m_length)
				throw(new StringException(StringErrorCode::ComparasionSizeException));

			return m_ptr[pos];
		}

		// �ڿ��� ���� �ܾ�´�.
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

		size_t FindFirst(const wchar_t *str, size_t starts = 0) const
		{
			return FindFirstHelper(str, starts);
		}

		size_t FindFirst(const WString& refer, size_t starts = 0) const
		{
			return FindFirstHelper(refer.m_ptr, starts);
		}

		size_t FindLast(const wchar_t *str, size_t ends = 0) const
		{
			return FindLastHelper(str, ends, wcslen(str));
		}

		size_t FindLast(const WString& refer, size_t ends = 0) const
		{
			return FindLastHelper(refer.m_ptr, ends, refer.m_length);
		}
		
		// starts��ġ���� �����Ͽ� ch�� ��ġ�ϴ� ������ ��ġ�� ã���ϴ�.
		size_t FindFirst(const wchar_t ch, size_t starts = 0) const
		{
			if (starts >= m_length)
				throw(new StringException(StringErrorCode::ComparasionSizeException));
			
			const wchar_t *ptr = wcschr(m_ptr + starts, ch);

			return ptr != NULL ? ptr - m_ptr : error;
		}

		// FindLast�� FindLast�� ��ӻ���� �� �ְ�, FindFirst�� FindFirst�� ��ӻ���� �� �ִ�.
		size_t FindLast(const wchar_t ch, size_t ends = 0) const
		{
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
		
		// ���ڿ��� ���ԵǾ��ִ��� Ȯ���մϴ�.
		bool Contains(const wchar_t *str, bool ignorecase = false) const
		{
			return ContainsHelper(str, wcslen(str), ignorecase);
		}

		bool Contains(const WString& refer, bool ignorecase = false) const
		{
			return ContainsHelper(refer.m_ptr, refer.m_length, ignorecase);
		}

		// �ε��� ���� (charAt)
		inline wchar_t operator[](size_t index) const
		{
			// ����� �ʹ� ũ��
			//if (index >= m_length)
			//	throw(new StringException(StringErrorCode::OverflowReferenceException));

			return m_ptr[index];
		}

		// starts ��ġ���� ���ڿ��� ������ ��ġ�� ���ڿ� ������ �����ɴϴ�.
		WString Substring(size_t starts)
		{
			if (starts > m_length)
				throw(new StringException(StringErrorCode::ComparasionSizeException));

			return WString((const wchar_t *)(m_ptr + starts), m_length - starts);
		}

		// starts��ġ���� len��ŭ�� ũ�⸸ŭ �ڸ� ���ڿ� ������ �����ɴϴ�.
		WString Substring(size_t starts, size_t len)
		{
			if (len + starts > m_length)
				throw(new StringException(StringErrorCode::ComparasionSizeException));

			return WString((const wchar_t *)(m_ptr + starts), len);
		}

		// ������ ��ҿ��� ���۵Ǿ� starts��ŭ �ڸ� ���ڿ� ������ �����ɴϴ�.
		WString SubstringReverse(size_t starts)
		{
			if (starts > m_length)
				throw(new StringException(StringErrorCode::ComparasionSizeException));

			return WString((const wchar_t *)(m_ptr), m_length - starts);
		}

		// ������ ��ҿ��� ���۵Ǿ� starts��ŭ �� ��ġ���� len��ŭ �ڸ� ���ڿ� ������ �����ɴϴ�. 
		WString SubstringReverse(size_t starts, size_t len)
		{
			if (len + starts > m_length)
				throw(new StringException(StringErrorCode::ComparasionSizeException));

			return WString((const wchar_t *)(m_last - starts - len + 1), len);
		}

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

		// ���༱�������� ������ ���ڿ� ������ �����ɴϴ�.
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

		// ���༱�������� ������ ���ڿ� ������ �����ɴϴ�.
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

		// ��/���༱�������� ������ ���ڿ� ������ �����ɴϴ�.
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

		// ���ڿ� ���տ� ���Ե� str������ ������ �����ɴϴ�.
		// (Len->Count�� ����)
		size_t Count(const wchar_t *str) const
		{
			return CountHelper(str, wcslen(str));
		}

		size_t Count(const WString& refer) const
		{
			return CountHelper(refer.m_ptr, refer.m_length);
		}

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

	public:

		// str�� ���� �ڸ� ���ڿ� ���յ��� ������ max��ŭ �����ɴϴ�.
		SplitsArray Split(const wchar_t *str, size_t max = SIZE_MAX)
		{
			return SplitHelper(str, wcslen(str), max);
		}

		SplitsArray Split(const WString& refer, size_t max = SIZE_MAX)
		{
			return SplitHelper(refer.m_ptr, refer.m_length, max);
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

		// �ڿ��� ���� str�� ���� �ڸ� ���ڿ� ���յ��� ������ max��ŭ �����ɴϴ�.
		SplitsArray SplitReverse(const wchar_t* refer, size_t max = SIZE_MAX)
		{
			return SplitReverseHelper(refer, wcslen(refer), max);
		}

		SplitsArray SplitReverse(const WString& refer, size_t max = SIZE_MAX)
		{
			return SplitReverseHelper(refer.m_ptr, refer.m_length, max);
		}

	private:
		
		WString BetweenHelper(const wchar_t *left, size_t llen, const wchar_t *right, size_t rlen, size_t starts)
		{
			if (starts >= m_length)
				throw(new StringException(StringErrorCode::ComparasionSizeException));

			size_t lefts = FindFirst(left, starts) + llen;
			size_t rights = FindFirst(right, lefts);

			if (lefts > rights)
				std::swap(lefts, rights);

			return (lefts != error) && (rights != error) ? Slice(lefts, rights - 1) : WString();
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

		// left�� right ���̿� �ִ� ���ڿ��� �����ɴϴ�.
		WString Between(const WString& left, const WString& right, size_t starts = 0)
		{
			return BetweenHelper(left.m_ptr, left.m_length, right.m_ptr, right.m_length, starts);
		}

		WString Between(const wchar_t *left, const wchar_t *right, size_t starts = 0)
		{
			return BetweenHelper(left, wcslen(left), right, wcslen(right), starts);
		}
		
		// left�� right ���̿� �ִ� ���ڿ����� �����ɴϴ�.
		SplitsArray Betweens(const WString& left, const WString& right, size_t starts = 0)
		{
			return BetweensHelper(left.m_ptr, left.m_length, right.m_ptr, right.m_length, starts);
		}

		SplitsArray Betweens(const wchar_t *left, const wchar_t *right, size_t starts = 0)
		{
			return BetweensHelper(left, wcslen(left), right, wcslen(right), starts);
		}

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

		// �ʹ� Ŀ�� ����...
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

		// ���ڿ� ������ �ҹ��ڷ� ��ȯ�� ������ �����ɴϴ�.
		WString ToLower()
		{
			wchar_t *ret = new wchar_t[m_length + 1];
			for (size_t i = 0; i < m_length; i++)
				ret[i] = towlower(m_ptr[i]);
			ret[m_length] = 0;
			return WString(ret, m_length, with);
		}
		
		// ���ڿ� ������ �빮�ڷ� ��ȯ�� ������ �����ɴϴ�.
		WString ToUpper()
		{
			wchar_t *ret = new wchar_t[m_length + 1];
			for (size_t i = 0; i < m_length; i++)
				ret[i] = towupper(m_ptr[i]);
			ret[m_length] = 0;
			return WString(ret, m_length, with);
		}

		// ù ���� ���ڸ� �빮�ڷ� ġȯ�� ���ڿ��� ��ȯ�մϴ�.
		// (��� �ܾ �����Ϸ��� Title�Լ��� �̿��Ͻʽÿ�)
		WString Capitalize()
		{
			wchar_t *ret = this->ToArray();
			*ret = towupper(*ret);
			return WString(ret, m_length, with);
		}

		// ��� �ܾ��� ù ��° ���ڸ� �빮�ڷ� ġȯ�� ���ڿ��� ��ȯ�մϴ�.
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

		// ���ڿ� ������ ���� �κ��� ��� ���ڿ��� ��ġ�ϴ��� Ȯ���մϴ�.
		// starts�� EndsWith�� �˻��� ������ ù ��° ��ġ�Դϴ�.
		bool StartsWith(const wchar_t *str, size_t starts = 0) const
		{
			return StartsWithHelper(str, starts, wcslen(str));
		}
		
		bool StartsWith(const WString& refer, size_t starts = 0) const
		{
			return StartsWithHelper(refer.m_ptr, starts, refer.m_length);
		}

		// �̰� ����Ϸ��� ����� �ɸ���?
		bool StartsWith(const wchar_t ch, size_t starts) const
		{
			if (starts >= m_length)
				throw(new StringException(StringErrorCode::ComparasionSizeException));

			return m_ptr[starts] == ch;
		}

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

		// ���ڿ� ������ �� �κ��� ��� ���ڿ��� ��ġ�ϴ��� Ȯ���մϴ�. 
		// ends�� EndsWith�� �˻��� ������ ������ ��ġ�Դϴ�.
		bool EndsWith(const wchar_t *str, size_t ends = 0) const
		{
			return EndsWithHelper(str, ends, wcslen(str));
		}

		bool EndsWith(const WString& refer, size_t ends = 0) const
		{
			return EndsWithHelper(refer.m_ptr, ends, refer.m_length);
		}

		// �̰� ������� ������ ����
		inline bool EndsWith(const wchar_t ch, size_t ends) const
		{
			if (ends >= m_length)
				return false;

			return *(m_last - ends) == ch;
		}

		inline bool EndsWith(const wchar_t ch) const
		{
			return *m_last == ch;
		}

		// ���ڿ� ������ ũ�⸦ len��ŭ �����ϰ� ������ ���� ���������� 
		// �����ѵ� ���� ���� ������ pad�� ������ ���ڿ� ������ �����ɴϴ�.
		// len�� ���ڿ��� ũ�⺸�� ���� ��� ���ڿ� ������ ���纻�� �����ɴϴ�.
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
		
		// ���ڿ� ������ ũ�⸦ len��ŭ �����ϰ� ������ ���� �������� 
		// �����ѵ� ���� ������ ������ pad�� ������ ���ڿ� ������ �����ɴϴ�.
		// len�� ���ڿ��� ũ�⺸�� ���� ��� ���ڿ� ������ ���纻�� �����ɴϴ�.
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

		WString PadMiddle(size_t len, wchar_t pad = L' ', bool lefts = true)
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
		
		// ���� ���� separation��ŭ ��� ptr�� �����մϴ�.
		// ���� InsertLeft(3, L"++", 2)�� ���
		// 1234567890 => 123++456++789++0 ���� ��ȯ��
		// (�� ������ �׸� �߰� �ȵ�)
		WString InsertLeft(size_t separation, const wchar_t *str)
		{
			return InsertLeftHelper(separation, str, wcslen(str));
		}

		WString InsertLeft(size_t separation, const WString& refer)
		{
			return InsertLeftHelper(separation, refer.m_ptr, refer.m_length);
		}
		
		// ������ ���� separation��ŭ ��� ptr�� �����մϴ�.
		// ���� InsertRight(3, L"++", 2)�� ���
		// 1234567890 => 1++234++567++890 ���� ��ȯ��
		// (�� ������ �׸� �߰� �ȵ�)
		WString InsertRight(size_t separation, const wchar_t *str)
		{
			return InsertRightHelper(separation, str, wcslen(str));
		}

		WString InsertRight(size_t separation, const WString& refer)
		{
			return InsertRightHelper(separation, refer.m_ptr, refer.m_length);
		}

		// �̷� �Լ����� ���ڿ� ��ǥ���� ���� ���ϵ�
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

	public:

		WString Replace(const wchar_t *src, const wchar_t *dest, size_t max = SIZE_MAX)
		{
			return ReplaceHelper(src, dest, wcslen(src), wcslen(dest), max);
		}

		WString Replace(const WString& refer0, const WString& refer1, size_t max = SIZE_MAX)
		{
			return ReplaceHelper(refer0.m_ptr, refer1.m_ptr, refer0.m_length, refer1.m_length, max);
		}

		// ���Ե� ���ڿ� ����
		WString Trim(const wchar_t *src)
		{
			return ReplaceHelper(src, L"", wcslen(src), 0, SIZE_MAX);
		}
		
		WString Trim(const WString& refer)
		{
			return ReplaceHelper(refer.m_ptr, L"", refer.m_length, 0, SIZE_MAX);
		}

		// ���ڿ� ������ ó������ index���� �����ϴ� ���ڿ� ������ �����ɴϴ�.
		WString Remove(size_t len)
		{
			if (len >= m_length) // exceptrion from this method
				throw(new StringException(StringErrorCode::ComparasionSizeException));

			return this->Substring(0, len);
		}

		// ���ڿ� ������ starts ��ġ ���� count ��ŭ�� �����ϴ� ���հ� ���ڿ�
		// ������ �������� �����ɴϴ�. (�ٱ� ���)
		WString Remove(size_t starts, size_t len)
		{
			if (starts + len > m_length) // exceptrion from this method
				throw(new StringException(StringErrorCode::ComparasionSizeException));

			size_t retlen = m_length - len;
			wchar_t *newString = new wchar_t[retlen + 1];

			memcpy(newString, m_ptr, starts * sizeof(wchar_t));
			memcpy(newString + starts, m_ptr + starts + len, (retlen - starts) * sizeof(wchar_t));

			newString[retlen] = 0;

			return WString(newString, retlen, with);
		}

		WString RemoveReverse(size_t len)
		{
			if (len >= m_length) // exceptrion from this method
				throw(new StringException(StringErrorCode::ComparasionSizeException));

			return this->SubstringReverse(0, len);
		}

		WString RemoveReverse(size_t starts, size_t len)
		{
			if (starts + len > m_length) // exceptrion from this method
				throw(new StringException(StringErrorCode::ComparasionSizeException));

			return this->Remove(m_length - starts - len, len);
		}

		// ���ڿ� ������ starts�ε��� ���� str�� len��ŭ�� ���̸� �߰� ��Ų
		// ���ڿ� ������ �����ɴϴ�.
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

		WString Insert(size_t starts, const WString& refer, size_t len)
		{
			return Insert(starts, refer.m_ptr, len);
		}

		WString Insert(size_t starts, const wchar_t *str)
		{
			return Insert(starts, str, wcslen(str));
		}

		WString Insert(size_t starts, const WString& refer)
		{
			return Insert(starts, refer.m_ptr, refer.m_length);
		}

		// ���ڿ� ������ count�����ŭ �ø� ���ڿ� ������ �����ɴϴ�.
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

		// ������ ���ڿ��� �����ɴϴ�.
		WString Reverse()
		{
			wchar_t *ret = this->ToArray();
			_wcsrev(ret);
			return WString(ret, m_length, with);
		}
		
		// [first, last]�� �����ɴϴ�.
		// (��, last�� 0���� �۰ų� ���� ��� �ڿ��� ���� ������)
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

		// skip ~ - skip ��ŭ �����´�
		WString Slice(size_t skip)
		{
			if ((skip << 1) > m_length)
				throw(new StringException(StringErrorCode::ComparasionSizeException));

			return WString((const wchar_t *)(m_ptr + skip), m_length - (skip << 1));
		}

		// jmp��ŭ �ǳʶٸ鼭 ���� ���ڵ��� �����´�.
		WString Slicing(size_t jmp, size_t starts = 0, bool jmpstarts = true)
		{
			if (starts >= m_length)
				throw(new StringException(StringErrorCode::ComparasionSizeException));

			if ( jmp > 0 && jmp + starts < m_length )
			{
				jmp += 1;

				size_t   retlen = (m_length - starts) / jmp + ((m_length - starts) % jmp && jmpstarts);
				wchar_t* collect = new wchar_t[retlen+1];
				size_t   jmpcnt = (jmpstarts ? 0 : jmp-1) + starts;

				for ( size_t i = 0 ; jmpcnt < m_length; i++, jmpcnt += jmp )
				{
					collect[i] = m_ptr[jmpcnt];
				}

				collect[retlen] = 0;

				return WString(collect, retlen);
			}
			else if ( jmp == 0 )
			{
				return WString((const wchar_t *)(m_ptr + starts), m_length - starts);
			}
			else
			{
				return WString();
			}
		}

		// skip��ŭ �ǳʶٸ鼭 �ǳʶ� ���ڵ��� �����´�.
		WString SlicingInverse(size_t skip, size_t starts = 0, bool skipstarts = true)
		{
			if (starts >= m_length)
				throw(new StringException(StringErrorCode::ComparasionSizeException));

			if ( skip > 0 && skip + starts < m_length )
			{
				skip += 1;
				
				size_t   retlen = m_length - starts - (m_length - starts) / skip - ((m_length - starts) % skip && skipstarts);
				wchar_t* collect = new wchar_t[retlen+1];
				wchar_t* colptr = collect;
				size_t   skipcnt = (skipstarts ? 1 : 0) + starts;
				size_t   skip2 = (skip - 1) * sizeof(wchar_t);

				for ( ; skipcnt < m_length; skipcnt += skip )
				{
					if ( skipcnt + skip < m_length )
						memcpy(colptr, m_ptr + skipcnt, skip2);
					else
						memcpy(colptr, m_ptr + skipcnt, (m_length - skipcnt) << 1);
					colptr += skip - 1;
				}

				collect[retlen] = 0;
				
				return WString(collect, retlen);
			}
			else if ( skip == 0 )
			{
				return WString();
			}
			else
			{
				return WString((const wchar_t *)(m_ptr + starts), m_length - starts);
			}
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

		// len��ŭ �ڸ� ���ڿ����� �����ɴϴ�.
		Lines LineSplit(size_t len)
		{
			return LineSplitHelper(len, nullptr, 0, nullptr, 0);
		}
		
		// len��ŭ �ڸ� ���ڿ����� ���ڿ� �տ� front�� �߰��� ���ڿ����� �����ɴϴ�.
		Lines LineSplit(size_t len, const WString& front)
		{
			return LineSplitHelper(len, front.Reference(), front.m_length, nullptr, 0);
		}

		Lines LineSplit(size_t len, const wchar_t *front)
		{
			return LineSplitHelper(len, front, wcslen(front), nullptr, 0);
		}
		
		// len��ŭ �ڸ� ���ڿ����� ���ڿ� �տ� front��, �ڿ� end�� �߰��� ���ڿ����� �����ɴϴ�.
		Lines LineSplit(size_t len, const WString& front, const WString& end)
		{
			return LineSplitHelper(len, front.Reference(), front.m_length, end.Reference(), end.m_length);
		}

		Lines LineSplit(size_t len, const wchar_t *front, const wchar_t *end)
		{
			return LineSplitHelper(len, front, wcslen(front), end, wcslen(end));
		}
		
		// \r\n �Ǵ� \n�� �����ڷ� �Ͽ� �ڸ� ���ڿ����� �����ɴϴ�.
		// (last == ture�� ��� ������ ���� ��������� ������� �ʽ��ϴ�.)
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

		// �� �� �� len��ŭ�� ���ڼ��� ������ ���ڿ����� �����ɴϴ�.
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

			return num_hash * ((seed << 16) + (num_hash >> 16));
		}

		// ���ڿ��� ������ Ȯ���մϴ�.
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

		bool IsHexDigit() const
		{
			const wchar_t *ptr = m_ptr;

			if (*ptr == L'0' && (ptr[1] == L'x' || ptr[1] == L'X'))
				ptr += 2;

			while (iswxdigit(*ptr) && *ptr)
				ptr++;

			return *ptr == 0;
		}

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

		// ���ڿ��� ���ڷ� ��ȯ�մϴ�.
		inline wchar_t ToChar() const
		{
			if (m_length != 1)
				throw(new StringException(StringErrorCode::OverflowReferenceException));

			return m_ptr[0];
		}

		// ���ڿ��� ������ ��ȯ�մϴ�.
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

		long int ToLong() const
		{
			return (long)ToLongLong();
		}

		unsigned long int ToULong() const
		{
			return (unsigned long)ToULongLong();
		}

		int ToInteger() const
		{
			return (int)ToLongLong();
		}

		unsigned int ToUInteger() const
		{
			return (int)ToULongLong();
		}

		short int ToShort() const
		{
			return (short)ToLongLong();
		}

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

		// ���ڿ��� �Ǽ��� ��ȯ�մϴ�.
		long double ToLongDouble() const
		{
			return ToLongDoubleHelper(m_ptr);
		}

		double ToDouble() const
		{
			return (double)ToLongDouble();
		}

		float ToFloat() const
		{
			return (float)ToLongDouble();
		}

		inline wchar_t *ToArray() // ����Ʈ�� ������ �̰� ĳ��������.
		{
			wchar_t *ret = new wchar_t[m_length + 1];
			memcpy(ret, m_ptr, (m_length + 1) * sizeof(wchar_t));
			return ret;
		}
		inline const wchar_t *ToArray() const
		{
			wchar_t *ret = new wchar_t[m_length + 1];
			memcpy(ret, m_ptr, (m_length + 1) * sizeof(wchar_t));
			return ret;
		}

		char *ToAnsi()
		{
			return UnicodeToAnsi();
		}

		// �� ��ȯ�� �ִ��� ������ ������ ������� ũ���� ������ �Ұ��� �ϴٴ� ���̴�.
		// �̸� �ذ��Ϸ��� �� ������ �� �������ų� bytebuffer�� ����°� �ּ���������.
		// �׷��� ���⼱ �� ���� ������ ���� �����Ͽ���.
		// ������ ȯ�濡���� �����˴ϴ�.
		Utf8Array ToUtf8(bool file_bom = false)
		{
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

		WString operator&(const WString& concat)
		{
			return this->Concat(*this, concat);
		}
		WString operator+(const WString& concat)
		{
			return this->Concat(*this, concat);
		}

		// ���ڿ� ��
		inline bool operator>(const WString& compare)
		{
			return wcscmp(m_ptr, compare.m_ptr) > 0;
		}
		inline bool operator<(const WString& compare)
		{
			return wcscmp(m_ptr, compare.m_ptr) < 0;
		}
		inline bool operator>=(const WString& compare)
		{
			return !this->operator<(compare);
		}
		inline bool operator<=(const WString& compare)
		{
			return !this->operator>(compare);
		}

		inline void Swap(WString& refer)
		{
			std::swap(m_ptr, refer.m_ptr);
			std::swap(m_last, refer.m_last);
			std::swap(m_length, refer.m_length);
		}

		void operator=(const WString& refer)
		{
			if (m_ptr != nullptr)
				delete[] m_ptr;
			m_length = refer.m_length;
			m_ptr = new wchar_t[m_length + 1];
			m_last = m_ptr + m_length - 1;
			memcpy(m_ptr, refer.m_ptr, (m_length + 1) * sizeof(wchar_t));
		}

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

		inline WString Clone()
		{
			WString nstr;
			nstr.Clone(*this);
			return nstr;
		}

		friend std::wostream& operator<<(std::wostream& os, const WString& refer);

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
			mbstowcs_s(&converted, m_ptr, m_length + 1, str, SIZE_MAX);
		}
		
		void AnsiToUnicode(const char *ansi, size_t len)
		{
			wchar_t *ptr = m_ptr = new wchar_t[len + 1];
			size_t rlen = len;
			m_length = len;
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
		
#define _MAGIC	checker_type(~0ULL/0xff)
#define _WMAGIC	checker_type(~0ULL/0xffff)
		
		// src�� ���Ե� null byte�� ã���ϴ�.
		inline checker_type HazHelper(checker_type src) const
		{
			// 0x0101010101010101
			// 0x8080808080808080
			return (src - ( _MAGIC)) & ((~src & (( _MAGIC) <<  7)));
		}

		// src�� ���Ե� null wide byte�� ã���ϴ�.
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
			if (!(trim[0] & (checker_type)( 0xffUL << (8 * 3)))) return ((size_t)trim - (size_t)ptr) + 3;
#ifdef _X64_MODE
			if (!(trim[0] & (checker_type)(0xffULL << (8 * 4)))) return ((size_t)trim - (size_t)ptr) + 4;
			if (!(trim[0] & (checker_type)(0xffULL << (8 * 5)))) return ((size_t)trim - (size_t)ptr) + 5;
			if (!(trim[0] & (checker_type)(0xffULL << (8 * 6)))) return ((size_t)trim - (size_t)ptr) + 6;
			if (!(trim[0] & (checker_type)(0xffULL << (8 * 7)))) return ((size_t)trim - (size_t)ptr) + 7;
#endif
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
			if (!(trim[0] & (checker_type)( 0xffffUL << (16 * 1)))) return (((size_t)trim - (size_t)ptr) >> 1) + 1;
#ifdef _X64_MODE
			if (!(trim[0] & (checker_type)(0xffffULL << (16 * 2)))) return (((size_t)trim - (size_t)ptr) >> 1) + 2;
			if (!(trim[0] & (checker_type)(0xffffULL << (16 * 3)))) return (((size_t)trim - (size_t)ptr) >> 1) + 3;
#endif
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

}

#endif