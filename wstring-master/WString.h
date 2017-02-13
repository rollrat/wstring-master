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

// 이 헤더는 모든 포함 최상위에 위치해야함.
// undef를 안한이유는 이 라이브러리의 모든 파일에 적용하기 위함이다.

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
		// 비교, 삭제 연산시 연산 대상의 크기가 타깃보다
		// 작을 때 발생됩니다.
		ComparasionSizeException,
		// 원래 크기보다 큰 인덱스로 배열에 접근하려
		// 했을 때 발생합니다.
		OverflowReferenceException,
		// 삽입 연산시 목표의 크기가 0일 경우 발생됩니다.
		InsertionSizeZeroException,
		// 부호가 없는 변환에서 부호가 발견될 경우 발생됩니다.
		ToNumericSignException,
        // 입력된 두 배열의 크기가 다를 경우 발생됩니다.
        DiscordArraySize,
	}	StringErrorCode;

	//
	//	이 클래스는 String 클래스용 
	//	Exception 구문을 정의합니다.
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
	//	문자열 포인터가 클래스 생성시 귀속되도록하려는 경우에 사용됩니다. (소멸자 호출됨)
	//
	class StringReferencePutWith
	{
	};

	//
	//  이 클래스는 문자열 도구 집합을 나타냅니다.
	//  따라서 이 클래스의 값은 Clone과 Swap을 제외한 나머지 함수에선
	//  변경되지 않습니다. 참조연산자로 캐스팅하여 문자열을 변경하는 것은 이
	//  클래스에선 옳은 접근이 아닙니다.
	//	주의: 개발시 주의사항 기록
	//      1. 이 클래스는 wchar_t *형식의 문자열 집합을 공유합니다.
	//         또한 string api를 사용합니다. 따라서 모든 m_ptr
	//         배열 요소의 끝은 null이여야합니다.
	//      2. 직접적인 포인터 연산시 sizeof(wchar_t)이 2임을
	//         반드시 고려해야만 합니다. 정수형으로 포팅시 wchar_t*
	//         에 의한 컴파일러의 참조를 얻을 수 없습니다.
	//      3. 특별한 언급이 없는 대부분의 함수들은 소량, 대량의 문자열에서
	//         실험을 거쳐 제작, 수정된 것입니다. 되도록이면
	//         수정하지 않고 사용하셨으면 좋겠습니다.
	//      4. 반환 값 최적화를 이용하여 오버헤드를 줄이도록 작성되었습니다.
	//         모든 함수의 반환 값을 참조형으로 리턴하는 경우 소멸자가
	//         호출되지 않을 수 있으며, 함수를 호출하는 함수의 반환 값이
	//         참조형인 경우 소멸자 호출로 문자열이 손상될 수 있습니다.
	//         (record. 수능이 끝난 다음날 대대적인 수정을 거쳤다.)
	//         (어느 정도 완성이 되면 .cpp 분리 예정)
	//      5. Length()를 제외한 모든 함수는 문자열의 시작위치를 0으로 여깁니다.
	//         사용자가 혼동하지 않도록 조치하였습니다. 특정 문자가
	//         발견되지 않으면 error(-1)이 반환됩니다.
	//      6. strcpy|strncpy vs memcpy: strcpy는 바이트별 복사함수이므로,
	//         크기를 알 수 없는 복사에 적합하지만, 크기가 정해진 경우
	//         strncpy를 주로 사용하곤하는데, memcpy는 이를 운영체제에 맞게
	//         4 byte, 8 byte를 사용하므로 메모리관련함수를 사용하는게 더
	//         복사에 더 효율적입니다.
	//
	//   비슷한 함수
	//   [Right, Substring] [Left, SubstringReverse] [Mid, Substring Slice]
	//   [RSet, PadRight] [LSet, PadLeft]
	//   [RTrim, TrimStart] [LTrim, TrimEnd]
	//   [IndexOf, FindFirst, FirstContains] [LastIndexfOf, FindLast, LastContains]
	//   [InStr, Contains]
	//
	//   혼동되는 함수들
	//   Empty: 길이가 0일때
	//   Null:  포인터가 nullptr일때
	//   Full:  길이가 0보다 클때
	//
	//   예외 조건
	//   1. 리턴 값이 size_t, bool이 아닌 모든 위치 비교함수에 적용했다.
	//
	class WString final
	{
        /**
         *  @var m_ptr    문자열의 시작을 나타냅니다.
         *  @var m_last   문자열의 끄틀 나타냅니다.
         *  @var m_length 문자열의 길이를 나타냅니다.
         */
		wchar_t *m_ptr;
		wchar_t *m_last;
		size_t   m_length;

        /**
         *  @var with     클래스 내부에서 문자열 귀속 반환을 
         *                사용하는 경우에 쓰입니다.
         *  @var srp      소멸자를 호출여부를 결정합니다.
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
         *  @brief  빈 문자열 집합을 생성합니다.
         *
         *  이 생성자는 null-string을 나타냅니다. 이는 변경될 수 없는
         *  WString 집합을 사용한 요소의 마지막을 나타내는 용도로 사용
         *  될 수 있습니다.
         */
		WString()
			: m_length(0)
			, m_ptr(nullptr)
			, m_last(m_ptr)
		{
		}
		
        /**
         *  @brief  Ansi 기반 문자열 형식을 입력으로 받아 유니코드
         *          문자열으로 변환한 문자열 집합을 생성합니다.
         *  @param  ptr Ansi 기반 문자열 형식입니다.
         *
         *  이 생성자는 많은 계산과정이 요구될 뿐만아니라 최적화되지
         *  않은 방법을 사용합니다. Windows Api의 MultiByteToWideChar와 
         *  같은 함수를 사용하여 변환 후 사용하십시오.
         */
		WString(const char *ptr)
		{
			AnsiToUnicode(ptr, strlen(ptr));
		}
        
        /**
         *  @brief  Ansi 기반 문자열 형식을 입력으로 받아 유니코드
         *          문자열으로 변환한 문자열 집합을 생성합니다.
         *  @param  ptr Ansi 기반 문자열 형식입니다.
         *  @param  len ptr의 길이입니다.
         *
         *  이 생성자는 많은 계산과정이 요구될 뿐만아니라 최적화되지
         *  않은 방법을 사용합니다. Windows Api의 MultiByteToWideChar와 
         *  같은 함수를 사용하여 변환 후 사용하십시오. 또한 매개변수 len에
         *  따라 ptr의 사용길이가 정해지므로 사용시 유의하십시오.
         */
		WString(const char *ptr, size_t len)
		{
			AnsiToUnicode(ptr, len);
		}
        
        /**
         *  @brief  유니코드 기반 문자열 형식을 입력으로 받아 새로운
         *          문자열 집합을 생성합니다.
         *  @param  ptr 유니코드 기반 문자열 형식입니다.
         *
         *  이 생성자는 리터널 문자열, 또는 const_cast로 캐스팅된 문자열을
         *  매개변수로 받도록 설계되어 있습니다. 기타 입력에 대한 문제는
         *  보호되지 못하므로, WString(const wchar_t *ptr, size_t len)
         *  을 사용하여 해결하십시오.
         */
		WString(const wchar_t *ptr)
		{
			InitString(ptr);
		}
		
		// 다음 두 생성자는 이 라이브러리에서 가장 많이 쓰인다.
		// 이 방법이외에 적절히 생성할 수 있는 방법을 알면 알려주면 감사하겠습니다.

        /**
         *  @brief  유니코드 기반 문자열 포인터 형식을 입력으로 받아 
         *          소멸자가 호출되지 않는 문자열을 생성합니다.
         *  @param  ptr 유니코드 기반 문자열 포인터 형식입니다.
         *  @param  len ptr의 길이입니다.
         *
         *  이 생성자는 문자열 포인터 형식과 길이를 입력으로 받으며, WString이 
         *  제공하는 도구만을 사용하는 목적으로 이용하려는 경우에 적합한
         *  생성자 입니다.
         */
		WString(wchar_t *ptr, size_t len)
			: m_length(len)
			, m_ptr(ptr)
			, m_last(ptr + len - 1)
			, srp(true)
		{ // 크기까지 입력되는 경우에 한하여 포인터 복사 생성
		}
        
        /**
         *  @brief  유니코드 기반 문자열 포인터 형식을 입력으로 받으며
         *          소멸자가 호출되는 문자열을 생성합니다.
         *  @param  ptr 유니코드 기반 문자열 포인터 형식입니다.
         *  @param  len ptr의 길이입니다.
         *  @param  <>  문자열 귀속 플레이스 홀더입니다.
         *
         *  이 생성자는 문자열 포인터 형식과 길이를 입력으로 받으며, WString클래스에
         *  문자열을 귀속시켜 사용하는 목적으로 이용하려는 경우에 적합한
         *  생성자 입니다. 생성자는 ptr에 대한 검사를 진행하지 않고, 귀속시키므로,
         *  입력되는 길이는 이용자가 확정하여 이용해야합니다.
         */
		WString(wchar_t *ptr, size_t len, StringReferencePutWith)
			: m_length(len)
			, m_ptr(ptr)
			, m_last(ptr + len - 1)
		{
		}
        
        /**
         *  @brief  유니코드 기반 문자열 형식과 길이를 입력으로 받아 새로운
         *          문자열 집합을 생성합니다.
         *  @param  ptr 유니코드 기반 문자열 포인터 형식입니다.
         *  @param  len ptr의 길이입니다.
         *
         *  이 생성자는 입력받은 길이에 따라 입력된 문자열을 복사합니다.
         *  길이를 미리 확정하지 않은 경우 WString(const wchar_t *ptr)
         *  생성자를 사용하십시오.
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
         *  @brief  문자 하나를 반복한 문자열을 생성합니다.
         *  @param  ch 반복할 문자를 지정합니다.
         *  @param  count 반복할 횟수를 지정합니다.
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
         *  @brief  입력받은 문자로 시작하는 길이가 1인 문자열을 생성합니다.
         *  @param  ch 유니코드 기반 문자 형식입니다.
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
         *  @brief  문자 하나를 반복한 문자열을 생성합니다.
         *  @param  ch 반복할 문자를 지정합니다.
         *  @param  count 반복할 횟수를 지정합니다.
         */
		WString(char ch, size_t count)
			: WString((wchar_t)ch, count)
		{
		}
        /**
         *  @brief  입력받은 문자로 시작하는 길이가 1인 문자열을 생성합니다.
         *  @param  ch Ansi 기반 문자 형식입니다.
         */
		
		WString(char ch)
			: WString((wchar_t)ch)
		{
		}
		
        /**
         *  @brief  입력받은 문자로 시작하는 길이가 1인 문자열을 생성합니다.
         *  @param  ch Ansi 기반 문자 형식입니다.
         */
		WString(unsigned char ch)
			: WString((wchar_t)ch)
		{
		}
        
		// short도 있었는데 지저분해서 지워버림

        /**
         *  @brief  입력받은 정수형 또는 실수형으로 문자열을 생성합니다.
         *  @param  num 입력할 정수형 또는 실수형을 지정합니다.
         */
		WString(int num)
		{
			wchar_t buffer[65];
			_itow_s(num, buffer, 10);
			InitString((const wchar_t *)buffer);
		}
		
        /**
         *  @brief  입력받은 정수형 또는 실수형으로 문자열을 생성합니다.
         *  @param  num 입력할 정수형 또는 실수형을 지정합니다.
         */
		WString(long int num)
		{
			wchar_t buffer[65];
			_ltow_s(num, buffer, 10);
			InitString((const wchar_t *)buffer);
		}
		
        /**
         *  @brief  입력받은 정수형 또는 실수형으로 문자열을 생성합니다.
         *  @param  num 입력할 정수형 또는 실수형을 지정합니다.
         */
		WString(long long int num)
		{
			wchar_t buffer[65];
			swprintf_s(buffer, L"%lld", num, 65);
			InitString((const wchar_t *)buffer);
		}
		
        /**
         *  @brief  입력받은 정수형 또는 실수형으로 문자열을 생성합니다.
         *  @param  num 입력할 정수형 또는 실수형을 지정합니다.
         */
		WString(unsigned int num)
		{
			wchar_t buffer[65];
			swprintf_s(buffer, L"%u", num, 65);
			InitString((const wchar_t *)buffer);
		}
		
        /**
         *  @brief  입력받은 정수형 또는 실수형으로 문자열을 생성합니다.
         *  @param  num 입력할 정수형 또는 실수형을 지정합니다.
         */
		WString(unsigned long int num)
		{
			wchar_t buffer[65];
			swprintf_s(buffer, L"%lu", num, 65);
			InitString((const wchar_t *)buffer);
		}
		
        /**
         *  @brief  입력받은 정수형 또는 실수형으로 문자열을 생성합니다.
         *  @param  num 입력할 정수형 또는 실수형을 지정합니다.
         */
		WString(unsigned long long int num)
		{
			wchar_t buffer[65];
			swprintf_s(buffer, L"%llu", num, 65);
			InitString((const wchar_t *)buffer);
		}
		
        /**
         *  @brief  입력받은 정수형 또는 실수형으로 문자열을 생성합니다.
         *  @param  num 입력할 정수형 또는 실수형을 지정합니다.
         */
		WString(float num)
		{
			wchar_t buffer[65];
			swprintf_s(buffer, L"%g", num, 65);
			InitString((const wchar_t *)buffer);
		}
		
        /**
         *  @brief  입력받은 정수형 또는 실수형으로 문자열을 생성합니다.
         *  @param  num 입력할 정수형 또는 실수형을 지정합니다.
         */
		WString(double num)
		{
			wchar_t buffer[65];
			swprintf_s(buffer, L"%lg", num, 65);
			InitString((const wchar_t *)buffer);
		}
		
        /**
         *  @brief  입력받은 정수형 또는 실수형으로 문자열을 생성합니다.
         *  @param  num 입력할 정수형 또는 실수형을 지정합니다.
         */
		WString(long double num)
		{
			wchar_t buffer[65];
			swprintf_s(buffer, L"%llg", num, 65);
			InitString((const wchar_t *)buffer);
		}
		
        /**
         *  @brief  클래스 복사 생성자입니다.
         *  @param  cnt 복사할 문자열 도구 집합을 지정합니다.
         *
         *  이 생성자는 소멸자를 호출하는 새로운 문자열을 생성합니다.
         *  WString(WString& cnt)은 기본적으로 제공되지 않습니다.
         */
		WString(const WString& cnt)
			: WString((const wchar_t *)cnt.m_ptr, cnt.m_length)
		{
		}
        
        /**
         *  @brief  표준 문자열을 이용해 복사 포인터를 생성합니다.
         *  @param  str 표준 문자열을 지정합니다.
         *
         *  이 생성자는 표준 문자열을 입력으로 받으며, WString이 
         *  제공하는 도구만을 사용하는 목적으로 이용하려는 경우에 적합한
         *  생성자 입니다.
         */
		WString(std::wstring& str)
			: WString(&str[0], str.length())
		{
		}
        
        /**
         *  @brief  표준 문자열을 입력으로 받아 새로운 문자열 집합을 생성합니다.
         *  @param  wstr 표준 문자열을 지정합니다.
         */
		WString(const std::string& str)
			: WString(str.c_str(), str.length())
		{
		}
        
        /**
         *  @brief  표준 문자열을 입력으로 받아 새로운 문자열 집합을 생성합니다.
         *  @param  wstr 표준 문자열을 지정합니다.
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
         *  @brief  문자열의 길이를 가져옵니다.
         *
         *  이 함수는 null-terminated string에서 마지막 문자인
         *  '\0'를 제외한 나머지 부분의 길이를 가져옵니다.
         */
		inline size_t Length() const
		{
			return m_length;
		}
		
        /**
         *  @brief  문자열 집합의 크기가 0인지 확인합니다.
         */
		inline bool Empty() const
		{
			return m_length == 0;
		}
        
        /**
         *  @brief  문자열 집합의 크기가 0보다 큰지 확인합니다.
         */
		inline bool Full() const
		{
			return m_length > 0;
		}
        
        /**
         *  @brief  문자열이 생성되어 있는 지의 여부를 확인합니다.
         */
		inline bool Null() const
		{
			return m_ptr == nullptr;
		}
		
        /**
         *  @brief  문자열 포인터를 가져옵니다.
         *
         *  이 함수는 이 클래스에 포함된 raw-data를 제공하는 유일한 함수입니다.
         *  이 함수를 통해 raw-data를 변경하는 것은 이 클래스의 올바른
         *  사용법이 아닙니다.
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
         *  @brief  문자열 맨 앞에 문자열을 추가한 문자열을 반환합니다.
         *  @param  str 추가할 문자열을 지정합니다.
         *
         *  이 함수는 Insert(0, str, len)의 추상입니다.
         *  빠른 Append를 실행하려면 StringBuilder를 이용하십시오.
         */
		WString Append(const wchar_t *str)
		{
			return Append(str, wcslen(str));
		}
        
        /**
         *  @brief  문자열 맨 앞에 문자열을 추가한 문자열을 반환합니다.
         *  @param  refer 추가할 문자열을 지정합니다.
         *
         *  이 함수는 Insert(0, str, len)의 추상입니다.
         *  빠른 Append를 실행하려면 StringBuilder를 이용하십시오.
         */
		WString Append(const WString& refer)
		{
			return Append(refer.m_ptr, refer.m_length);
		}
        
        /**
         *  @brief  두 문자열을 병합합니다.
         *  @param  t1 병합할 첫 번째 문자열입니다.
         *  @param  t2 병합할 두 번째 문자열입니다.
         *
         *  이 함수는 빠른 문자열 병합을 목적으로 생성되었습니다. 되도록이면
         *  WString이라는 형식에 맞는 매개변수를 입력하시기 바랍니다.
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
         *  @brief  세 문자열을 병합합니다.
         *  @param  t1 병합할 첫 번째 문자열입니다.
         *  @param  t2 병합할 두 번째 문자열입니다.
         *  @param  t3 병합할 세 번째 문자열입니다.
         *
         *  이 함수는 빠른 문자열 병합을 목적으로 생성되었습니다. 되도록이면
         *  WString이라는 형식에 맞는 매개변수를 입력하시기 바랍니다.
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
         *  @brief  네 문자열을 병합합니다.
         *  @param  t1 병합할 첫 번째 문자열입니다.
         *  @param  t2 병합할 두 번째 문자열입니다.
         *  @param  t3 병합할 세 번째 문자열입니다.
         *  @param  t4 병합할 네 번째 문자열입니다.
         *
         *  이 함수는 빠른 문자열 병합을 목적으로 생성되었습니다. 되도록이면
         *  WString이라는 형식에 맞는 매개변수를 입력하시기 바랍니다.
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
         *  @brief  두 문자열을 서로 비교합니다.
         *  @param  str 비교할 문자열을 지정합니다.
         *  @return wcscmp함수의 반환형식을 사용합니다.
         *
         *  이 함수는 클래스에 지정된 문자열을 wcscmp의 첫 번째 매개변수로
         *  사용합니다.
         */
		inline size_t CompareTo(const wchar_t *str) const
		{
			return wcscmp(m_ptr, str);
		}
		
        /**
         *  @brief  두 문자열을 서로 비교합니다.
         *  @param  str 비교할 문자열을 지정합니다.
         *  @return wcscmp함수의 반환형식을 사용합니다.
         *
         *  이 함수는 클래스에 지정된 문자열을 wcscmp의 첫 번째 매개변수로
         *  사용합니다.
         */
		inline size_t CompareTo(const WString& refer) const
		{
			return CompareTo(refer.m_ptr);
		}
        
        /**
         *  @brief  두 문자열을 서로 비교하는 함수입니다.
         *  @param  r1 비교할 첫 번째 문자열
         *  @param  r2 비교할 두 번째 문자열
         *  @return wcscmp함수의 반환형식을 사용합니다.
         */
		inline static int Comparer(const WString& r1, const WString& r2)
		{
			return wcscmp(r1.m_ptr, r2.m_ptr);
		}
        
        /**
         *  @brief  두 문자열이 서로 같은지의 여부를 확인합니다.
         */
		inline bool Equal(const wchar_t *str) const
		{
			size_t strlen = wcslen(str);

			if ( strlen == m_length )
				return !memcmp(m_ptr, str, m_length * sizeof(wchar_t));

			return false;
		}
        
        /**
         *  @brief  두 문자열이 서로 같은지의 여부를 확인합니다.
         */
		inline bool Equal(const WString& refer) const
		{
			if (refer.m_length != this->m_length)
				return false;
			
			return !memcmp(m_ptr, refer.m_ptr, m_length * sizeof(wchar_t));
		}
		
        /**
         *  @brief  두 문자열이 서로 같은지의 여부를 확인합니다.
         */
		inline bool operator==(const wchar_t *ptr) const
		{
			return Equal(ptr);
		}
        
        /**
         *  @brief  두 문자열이 서로 같은지의 여부를 확인합니다.
         */
		inline bool operator==(const WString& refer) const
		{
			return Equal(refer);
		}
        
        /**
         *  @brief  두 문자열이 서로 다른지의 여부를 확인합니다.
         */
		inline bool operator!=(const wchar_t *ptr) const
		{
			return !Equal(ptr);
		}
        
        /**
         *  @brief  두 문자열이 서로 다른지의 여부를 확인합니다.
         */
		inline bool operator!=(const WString& refer) const
		{
			return !Equal(refer);
		}
		
        /**
         *  @brief  문자열 앞에서 부터 지정된 거리만큼 떨어진 문자를 가져옵니다.
         *  @param  pos 거리를 지정합니다.
         */
		inline wchar_t First(size_t pos) const
		{
		    // 앞에서 부터 읽어온다. 굳이 만든 이유는 Last와 짝을 맞추기 위한 정도
		    // vb에선 Right, Left와 비슷하고
		    // 여기선 Substring과 SubstringReverse나 Remove랑 비슷하다.
			if (pos >= m_length)
				throw(new StringException(StringErrorCode::ComparasionSizeException));

			return m_ptr[pos];
		}
        
        /**
         *  @brief  문자열 뒤에서 부터 지정된 거리만큼 떨어진 문자를 가져옵니다.
         *  @param  pos 거리를 지정합니다.
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
		
		// 윈도우 환경에서만 지원됩니다.
		size_t FindLastHelper(const wchar_t *str, size_t ends, size_t len) const
		{
			if (ends >= m_length)
				return error;
			
			const wchar_t *ptr = wcsrnstrn(m_ptr, m_length - ends, str, len);

			return ptr != NULL ? ptr - m_ptr : error;
		}

	public:
        
        /**
         *  @brief  문자열 앞에서 지정된 거리만큼 떨어진 부분부터 지정된 문자열이 
         *          처음 나타나는 위치를 가져옵니다.
         *  @param  str 찾을 문자열을 지정합니다.
         *  @param  starts 문자열 앞에서부터 떨어진 거리를 지정합니다.
         *  @return 문자열이 처음 나타내는 위치입니다. 해당 문자열을 찾지 못한경우
         *          WString::error가 반환됩니다. 
         */
		size_t FindFirst(const wchar_t *str, size_t starts = 0) const
		{
			return FindFirstHelper(str, starts);
		}
        
        /**
         *  @brief  문자열 앞에서 지정된 거리만큼 떨어진 부분부터 지정된 문자열이 
         *          처음 나타나는 위치를 가져옵니다.
         *  @param  str 찾을 문자열을 지정합니다.
         *  @param  starts 문자열 앞에서부터 떨어진 거리를 지정합니다.
         *  @return 문자열이 처음 나타내는 위치입니다. 해당 문자열을 찾지 못한경우
         *          WString::error가 반환됩니다. 
         */
		size_t FindFirst(const WString& refer, size_t starts = 0) const
		{
			return FindFirstHelper(refer.m_ptr, starts);
		}
        
        /**
         *  @brief  문자열 뒤에서 지정된 거리만큼 떨어진 부분부터 지정된 문자열이 
         *          처음 나타나는 위치를 가져옵니다.
         *  @param  str 찾을 문자열을 지정합니다.
         *  @param  ends 문자열 뒤에서부터 떨어진 거리를 지정합니다.
         *  @return 문자열이 처음 나타내는 위치입니다. 해당 문자열을 찾지 못한경우
         *          WString::error가 반환됩니다. 
         */
		size_t FindLast(const wchar_t *str, size_t ends = 0) const
		{
			return FindLastHelper(str, ends, wcslen(str));
		}
        
        /**
         *  @brief  문자열 뒤에서 지정된 거리만큼 떨어진 부분부터 지정된 문자열이 
         *          처음 나타나는 위치를 가져옵니다.
         *  @param  str 찾을 문자열을 지정합니다.
         *  @param  ends 문자열 뒤에서부터 떨어진 거리를 지정합니다.
         *  @return 문자열이 처음 나타내는 위치입니다. 해당 문자열을 찾지 못한경우
         *          WString::error가 반환됩니다. 
         */
		size_t FindLast(const WString& refer, size_t ends = 0) const
		{
			return FindLastHelper(refer.m_ptr, ends, refer.m_length);
		}
		
        /**
         *  @brief  문자열 앞에서 지정된 거리만큼 떨어진 부분부터 지정된 문자가
         *          처음 나타나는 위치를 가져옵니다.
         *  @param  ch 찾을 문자를 지정합니다.
         *  @param  starts 문자열 앞에서부터 떨어진 거리를 지정합니다.
         *  @return 문자가 처음 나타내는 위치입니다. 해당 문자를 찾지 못한경우
         *          WString::error가 반환됩니다.
         */
		size_t FindFirst(const wchar_t ch, size_t starts = 0) const
		{
		    // starts위치부터 시작하여 ch와 일치하는 문자의 위치를 찾습니다.
			if (starts >= m_length)
				throw(new StringException(StringErrorCode::ComparasionSizeException));
			
			const wchar_t *ptr = wcschr(m_ptr + starts, ch);

			return ptr != NULL ? ptr - m_ptr : error;
		}
        
        /**
         *  @brief  문자열 뒤에서 지정된 거리만큼 떨어진 부분부터 지정된 문자가
         *          처음 나타나는 위치를 가져옵니다.
         *  @param  ch 찾을 문자를 지정합니다.
         *  @param  starts 문자열 뒤에서부터 떨어진 거리를 지정합니다.
         *  @return 문자가 처음 나타내는 위치입니다. 해당 문자를 찾지 못한경우
         *          WString::error가 반환됩니다. 
         */
		size_t FindLast(const wchar_t ch, size_t ends = 0) const
		{
		    // FindLast는 FindLast를 계속사용할 수 있고, FindFirst는 FindFirst를 계속사용할 수 있다.
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

				// wcschr로 첫 번째 일치 문장을 찾는다
				// 대/소문자 모두 찾고, 처음위치와 가까운 것 먼저
				// 검색한다. 알파벳이 아니면 같을 때로 비교하면된다.
				// 1. 문자열 최대크기를 넘는가? 
				//    m_ptr - len이 넘으면 비교할 수 없다. -> false
				// 2. 문자열의 마지막 글자가 src와 일치하는가?
				//    처음, 중간, 끝을 비교하면 좋겠지만, 이 알고리즘은 
				//    처음과 끝만을 비교하여 비교비용을 낮추었다.
				// 이제 두 번째 문자부터 비교를 시작한다.
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
         *  @brief  문자열이 포함되어있는지의 여부를 확인합니다.
         *  @param  str 포함되어있는지의 여부를 확인할 문자열을 지정합니다.
         *  @param  ignorecase 문자열의 대소문자를 구분하지않고 확인할 것인지의 여부를 지정합니다.
         */
		bool Contains(const wchar_t *str, bool ignorecase = false) const
		{
			return ContainsHelper(str, wcslen(str), ignorecase);
		}
        
        /**
         *  @brief  문자열이 포함되어있는지의 여부를 확인합니다.
         *  @param  str 포함되어있는지의 여부를 확인할 문자열을 지정합니다.
         *  @param  ignorecase 문자열의 대소문자를 구분하지않고 확인할 것인지의 여부를 지정합니다.
         */
		bool Contains(const WString& refer, bool ignorecase = false) const
		{
			return ContainsHelper(refer.m_ptr, refer.m_length, ignorecase);
		}

        /**
         *  @brief  지정된 위치의 문자를 가져옵니다.
         *  @param  index 위치를 지정합니다.
         */
		inline wchar_t operator[](size_t index) const
		{
			// 비용이 너무 크다
			//if (index >= m_length)
			//	throw(new StringException(StringErrorCode::OverflowReferenceException));

			return m_ptr[index];
		}
        
        /**
         *  @brief  starts 위치부터 문자열의 끝까지 위치한 문자열 집합을 가져옵니다.
         */
		WString Substring(size_t starts)
		{
			if (starts > m_length)
				throw(new StringException(StringErrorCode::ComparasionSizeException));

			return WString((const wchar_t *)(m_ptr + starts), m_length - starts);
		}
        
        /**
         *  @brief  starts위치부터 len만큼의 크기만큼 자른 문자열 집합을 가져옵니다.
         */
		WString Substring(size_t starts, size_t len)
		{
			if (len + starts > m_length)
				throw(new StringException(StringErrorCode::ComparasionSizeException));

			return WString((const wchar_t *)(m_ptr + starts), len);
		}
        
        /**
         *  @brief  뒤에서 시작해 starts만큼 자른 문자열 집합을 가져옵니다.
         */
		WString SubstringReverse(size_t starts)
		{
			if (starts > m_length)
				throw(new StringException(StringErrorCode::ComparasionSizeException));

			return WString((const wchar_t *)(m_ptr), m_length - starts);
		}
        
        /**
         *  @brief  뒤에서 시작해 starts만큼 뒤 위치에서 len만큼 자른 문자열 집합을 가져옵니다. 
         */
		WString SubstringReverse(size_t starts, size_t len)
		{
			if (len + starts > m_length)
				throw(new StringException(StringErrorCode::ComparasionSizeException));

			return WString((const wchar_t *)(m_last - starts - len + 1), len);
		}
        
        /**
         *  @brief  선행선형공백 영역이 끝나는 부분을 계산합니다.
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
         *  @brief  후행선형공백 영역이 끝나는 부분을 계산합니다.
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
			return ptr - m_ptr; // *m_ptr 기준으로
		}
		
        /**
         *  @brief  앞에서 부터 반복된 문자가 끝나는 부분을 계산합니다.
         *  @param  ch 반복된 문자를 지정합니다.
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
         *  @brief  뒤에서 부터 반복된 문자가 끝나는 부분을 계산합니다.
         *  @param  ch 반복된 문자를 지정합니다.
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
			return ptr - m_ptr; // *m_ptr 기준으로
		}
        
        /**
         *  @brief  선행선형공백을 제거한 문자열 집합을 가져옵니다.
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
         *  @brief  후행선형공백을 제거한 문자열 집합을 가져옵니다.
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
         *  @brief  선행선형공백과 후행선형공백을 제거한 문자열 집합을 가져옵니다.
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
         *  @brief  앞에서 부터 반복된 문자를 제거한 문자열 집합을 가져옵니다.
         *  @param  ch 반복된 문자를 지정합니다.
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
         *  @brief  뒤에서 부터 반복된 문자를 제거한 문자열 집합을 가져옵니다.
         *  @param  ch 반복된 문자를 지정합니다.
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
         *  @brief  앞과 뒤, 모두에서 부터 반복된 문자를 제거한 문자열 집합을 가져옵니다.
         *  @param  ch 반복된 문자를 지정합니다.
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
         *  @brief  문자열 집합에 포함된 지정된 문자열 집합의 개수를 가져옵니다.
         *  @param  str 지정된 문자열 입니다.
         */
		size_t Count(const wchar_t *str) const
		{
		    // 문자열 집합에 포함된 str집합의 개수를 가져옵니다.
		    // (Len->Count로 변경)
			return CountHelper(str, wcslen(str));
		}

        /**
         *  @brief  문자열 집합에 포함된 지정된 문자열 집합의 개수를 가져옵니다.
         *  @param  refer 지정된 문자열 입니다.
         */
		size_t Count(const WString& refer) const
		{
			return CountHelper(refer.m_ptr, refer.m_length);
		}
        
        /**
         *  @brief  문자열 집합에 포함된 지정된 문자의 개수를 가져옵니다.
         *  @param  ch 지정된 문자입니다.
         */
		size_t Count(const wchar_t ch) const
		{
			return wcountch(m_ptr, ch);
		}

	private:

		SplitsArray SplitHelper(const wchar_t *src, size_t srclen, size_t max)
		{
			// 메모리 낭비가 심하지만, 이 함수를 사용한다는 것 자체가
			// 이미 메모리에 신경을 쓰지 않는 것이라 해석되므로 그냥 할당함
			// 미리 세는 것보다 이게더 빠를거 아녀
            // 안정성을 더 중요하게 생각한다면 wcsstr구문 두 번 돌리면된다
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

			// max가 0이 아니면 마지막 항목이 들어가야하므로
			bool max_remain = max > 0;

			WString **n = new WString*[count + max_remain];

			// count만 셀 수 있다면 (Count함수로 셀 수는 있으나 저 루프가 더 빠르다) 위 루프와 병합할 수 있으나
			// 그 방법을 모르겠다. 그래서 이 SplitHelper함수는 비교적 큰 문자열비교에 적합하다.
			// 예를 들어 m_length가 3글자인 경우 이 함수 사용은 기존보다 세 번이나 더 할당한다는 면에서 비효율적이다.
			for (size_t i = 0; i < count; i++)
			{
				// poslen[i]는 대략 position[i]-position[i-1]-srclen으로 바뀔 수 있을 것이다.
				// 어차피 split함수는 new를 무지막지하게 호출할 것이니
				// new든 위 식이든 어느쪽을 사용하든 상관없다.
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
         *  @brief  지정된 문자열을 경계로 자른 문자열들의 집합을 지정된 개수만큼 가져옵니다.
         *  @param  str 경계로 지정할 문자열입니다.
         *  @param  max 지정할 개수입니다.
         *  @return ReadOnlyArray기반의 배열형식으로 반환합니다. 자세한 사항은
         *          collection/Array.h를 참고하십시오.
         *
         *  이 클래스는 경계 문자열 자르기를 Split와 SplitSlow의 두 가지 형식으로 제공합니다.
         *  Split는 메모리를 두 배로 사용하는 대신 처리속도는 빠르며, SplitSlow는
         *  Split와 달리 메모리를 정량만 사용하며, 처리속도는 느립니다.
         *  자세한 사항은 제공된 소스코드를 참고하시거나 작성자에게 문의하십시오.
         */
		SplitsArray Split(const wchar_t *str, size_t max = SIZE_MAX)
		{
			return SplitHelper(str, wcslen(str), max);
		}
        
        /**
         *  @brief  지정된 문자열을 경계로 자른 문자열들의 집합을 지정된 개수만큼 가져옵니다.
         *  @param  refer 경계로 지정할 문자열입니다.
         *  @param  max 지정할 개수입니다.
         *  @return ReadOnlyArray기반의 배열형식으로 반환합니다. 자세한 사항은
         *          collection/Array.h를 참고하십시오.
         *
         *  이 클래스는 경계 문자열 자르기를 Split와 SplitSlow의 두 가지 형식으로 제공합니다.
         *  Split는 메모리를 두 배로 사용하는 대신 처리속도는 빠르며, SplitSlow는
         *  Split와 달리 메모리를 정량만 사용하며, 처리속도는 느립니다.
         *  자세한 사항은 제공된 소스코드를 참고하시거나 작성자에게 문의하십시오.
         */
		SplitsArray Split(const WString& refer, size_t max = SIZE_MAX)
		{
			return SplitHelper(refer.m_ptr, refer.m_length, max);
		}
        
        /**
         *  @brief  지정된 문자열을 경계로 자른 문자열들의 집합을 지정된 개수만큼 가져옵니다.
         *  @param  str 경계로 지정할 문자열입니다.
         *  @param  max 지정할 개수입니다.
         *  @return ReadOnlyArray기반의 배열형식으로 반환합니다. 자세한 사항은
         *          collection/Array.h를 참고하십시오.
         *
         *  이 클래스는 경계 문자열 자르기를 Split와 SplitSlow의 두 가지 형식으로 제공합니다.
         *  Split는 메모리를 두 배로 사용하는 대신 처리속도는 빠르며, SplitSlow는
         *  Split와 달리 메모리를 정량만 사용하며, 처리속도는 느립니다.
         *  자세한 사항은 제공된 소스코드를 참고하시거나 작성자에게 문의하십시오.
         */
		SplitsArray SplitSlow(const wchar_t *str, size_t max = SIZE_MAX)
		{
			return SplitSlowHelper(str, wcslen(str), max);
		}
        
        /**
         *  @brief  지정된 문자열을 경계로 자른 문자열들의 집합을 지정된 개수만큼 가져옵니다.
         *  @param  refer 경계로 지정할 문자열입니다.
         *  @param  max 지정할 개수입니다.
         *  @return ReadOnlyArray기반의 배열형식으로 반환합니다. 자세한 사항은
         *          collection/Array.h를 참고하십시오.
         *
         *  이 클래스는 경계 문자열 자르기를 Split와 SplitSlow의 두 가지 형식으로 제공합니다.
         *  Split는 메모리를 두 배로 사용하는 대신 처리속도는 빠르며, SplitSlow는
         *  Split와 달리 메모리를 정량만 사용하며, 처리속도는 느립니다.
         *  자세한 사항은 제공된 소스코드를 참고하시거나 작성자에게 문의하십시오.
         */
		SplitsArray SplitSlow(const WString& refer, size_t max = SIZE_MAX)
		{
			return SplitSlowHelper(refer.m_ptr, refer.m_length, max);
		}
        
        /**
         *  @brief  지정된 문자열을 경계로 자른 문자열들의 집합 중 지정된 위치의 것을 가져옵니다.
         *  @param  str 경계로 지정할 문자열입니다.
         *  @param  pos 지정할 위치입니다.
         *  @throw  경계로 자른 문자열들의 집합의 요소 개수가 지정한 위치보다 작은경우 
         *          StringErrorCode::ComparasionSizeException로 throw 합니다.
         *
         *  이 함수는 Split, SplitSlow와 같은 처리를 하나 내부에서 지정된 위치의 것만을
         *  가져올 수 있도록 최적화되어있습니다. Count함수로 미리 검사하여 사용하거나, 
         *  try ... catch ... 구문을 이용하여 사용하십시오.
         */
		WString SplitPosition(const wchar_t *str, size_t pos)
		{
			return SplitPositionHelper(str, wcslen(str), pos);
		}
        
        /**
         *  @brief  지정된 문자열을 경계로 자른 문자열들의 집합 중 지정된 위치의 것을 가져옵니다.
         *  @param  refer 경계로 지정할 문자열입니다.
         *  @param  pos 지정할 위치입니다.
         *  @throw  경계로 자른 문자열들의 집합의 요소 개수가 지정한 위치보다 작은경우 
         *          StringErrorCode::ComparasionSizeException로 throw 합니다.
         *
         *  이 함수는 Split, SplitSlow와 같은 처리를 하나 내부에서 지정된 위치의 것만을
         *  가져올 수 있도록 최적화되어있습니다. Count함수로 미리 검사하여 사용하거나, 
         *  try ... catch ... 구문을 이용하여 사용하십시오.
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
         *  @brief  뒤에서 부터 지정된 문자열을 경계로 자른 문자열들의 집합을 지정된 개수만큼
         *          가져옵니다.
         *  @param  refer 경계로 지정할 문자열입니다.
         *  @param  max 지정할 개수입니다.
         *  @return ReadOnlyArray기반의 배열형식으로 반환합니다. 자세한 사항은
         *          collection/Array.h를 참고하십시오.
         */
		SplitsArray SplitReverse(const wchar_t* refer, size_t max = SIZE_MAX)
		{
			return SplitReverseHelper(refer, wcslen(refer), max);
		}
        
        /**
         *  @brief  뒤에서 부터 지정된 문자열을 경계로 자른 문자열들의 집합을 지정된 개수만큼
         *          가져옵니다.
         *  @param  refer 경계로 지정할 문자열입니다.
         *  @param  max 지정할 개수입니다.
         *  @return ReadOnlyArray기반의 배열형식으로 반환합니다. 자세한 사항은
         *          collection/Array.h를 참고하십시오.
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

			// 문자열이 asdf{asdf{asdf}asdf 면
			// 왼쪽이 {고, 오른쪽이 }라면, asdf{asdf를 가져온다.
			// 문자열이 asdf{asdf}asdf}asdf 면
			// asdf를 가져온다.
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
         *  @brief  지정된 두 문자열 사이의 문자열을 가져옵니다.
         *  @param  left 지정할 첫 번째 문자열입니다.
         *  @param  right 지정할 두 번째 문자열입니다.
         *  @param  starts 검사할 문자열의 시작위치를 지정합니다.
         *
         *  이 함수는 throw되지 않으며, 실패시 빈 문자열을 반환합니다.
         */
		WString Between(const WString& left, const WString& right, size_t starts = 0)
		{
			return BetweenHelper(left.m_ptr, left.m_length, right.m_ptr, right.m_length, starts);
		}
        
        /**
         *  @brief  지정된 두 문자열 사이의 문자열을 가져옵니다.
         *  @param  left 지정할 첫 번째 문자열입니다.
         *  @param  right 지정할 두 번째 문자열입니다.
         *  @param  starts 검사할 문자열의 시작위치를 지정합니다.
         *
         *  이 함수는 throw되지 않으며, 실패시 빈 문자열을 반환합니다.
         */
		WString Between(const wchar_t *left, const wchar_t *right, size_t starts = 0)
		{
			return BetweenHelper(left, wcslen(left), right, wcslen(right), starts);
		}
		
        /**
         *  @brief  지정된 두 문자열 사이의 문자열들의 집합을 가져옵니다.
         *  @param  left 지정할 첫 번째 문자열입니다.
         *  @param  right 지정할 두 번째 문자열입니다.
         *  @param  starts 검사할 문자열의 시작위치를 지정합니다.
         *  @return ReadOnlyArray기반의 배열형식으로 반환합니다. 자세한 사항은
         *          collection/Array.h를 참고하십시오.
         *
         *  이 함수는 게으른탐색법을 기반으로 작성되었습니다.
         */
		SplitsArray Betweens(const WString& left, const WString& right, size_t starts = 0)
		{
			return BetweensHelper(left.m_ptr, left.m_length, right.m_ptr, right.m_length, starts);
		}
        
        /**
         *  @brief  지정된 두 문자열 사이의 문자열들의 집합을 가져옵니다.
         *  @param  left 지정할 첫 번째 문자열입니다.
         *  @param  right 지정할 두 번째 문자열입니다.
         *  @param  starts 검사할 문자열의 시작위치를 지정합니다.
         *
         *  이 함수는 게으른탐색법을 이용합니다.
         */
		SplitsArray Betweens(const wchar_t *left, const wchar_t *right, size_t starts = 0)
		{
			return BetweensHelper(left, wcslen(left), right, wcslen(right), starts);
		}
        
        /**
         *  @brief  지정된 두 문자 사이의 문자열을 가져옵니다.
         *  @param  left 지정할 첫 번째 문자입니다.
         *  @param  right 지정할 두 번째 문자입니다.
         *  @param  starts 검사할 문자열의 시작위치를 지정합니다.
         *
         *  이 함수는 throw되지 않으며, 실패시 빈 문자열을 반환합니다.
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
         *  @brief  지정된 두 문자 사이의 문자열들의 집합을 가져옵니다.
         *  @param  left 지정할 첫 번째 문자입니다.
         *  @param  right 지정할 두 번째 문자입니다.
         *  @param  starts 검사할 문자열의 시작위치를 지정합니다.
         *  @return ReadOnlyArray기반의 배열형식으로 반환합니다. 자세한 사항은
         *          collection/Array.h를 참고하십시오.
         *
         *  이 함수는 게으른탐색법을 기반으로 작성되었습니다.
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
         *  @brief  문자열에 포함된 모든 문자를 소문자로 변환한 문자열을 가져옵니다.
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
         *  @brief  문자열에 포함된 모든 문자를 대문자로 변환한 문자열을 가져옵니다.
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
         *  @brief  첫 번째 문자를 대문자로 변환한 문자열을 반환합니다.
         */
		WString Capitalize()
		{
			wchar_t *ret = this->ToArray();
			*ret = towupper(*ret);
			return WString(ret, m_length, with);
		}
        
        /**
         *  @brief  모든 단어의 첫 번째 문자를 대문자로 변환한 문자열을 반환합니다.
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

			// len의 위치를 정확히 모를 수 있기 때문에 예외에 포함하지 아니했다.
			// starts는 사용될 확률이 거의 없으니 있으나 마나한 듯
			if (m_length < len + starts)
				return false;

			return !memcmp(m_ptr + starts, str, len * sizeof(wchar_t));
		}
		
	public:
        
        /**
         *  @brief  문자열의 시작 부분이 지정된 문자열과 일치하는지의 여부를 가져옵니다.
         *  @param  str 지정할 문자열입니다.
         *  @param  starts 시작부분을 지정합니다.
         */
		bool StartsWith(const wchar_t *str, size_t starts = 0) const
		{
			return StartsWithHelper(str, starts, wcslen(str));
		}
		
        /**
         *  @brief  문자열의 시작 부분이 지정된 문자열과 일치하는지의 여부를 가져옵니다.
         *  @param  refer 지정할 문자열입니다.
         *  @param  starts 시작부분을 지정합니다.
         */
		bool StartsWith(const WString& refer, size_t starts = 0) const
		{
			return StartsWithHelper(refer.m_ptr, starts, refer.m_length);
		}
        
        /**
         *  @brief  문자열의 시작 부분이 지정된 문자와 일치하는지의 여부를 가져옵니다.
         *  @param  ch 지정할 문자입니다.
         *  @param  starts 시작부분을 지정합니다.
         */
		bool StartsWith(const wchar_t ch, size_t starts) const
		{
			if (starts >= m_length)
				throw(new StringException(StringErrorCode::ComparasionSizeException));

			return m_ptr[starts] == ch;
		}
        
        /**
         *  @brief  문자열의 시작 부분이 지정된 문자와 일치하는지의 여부를 가져옵니다.
         *  @param  ch 지정할 문자입니다.
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
         *  @brief  문자열의 끝 부분이 지정된 문자열과 일치하는지의 여부를 가져옵니다.
         *  @param  str 지정할 문자열입니다.
         *  @param  ends 검색을 시작할 마지막 위치를 지정합니다.
         *
         *  이 함수의 매개변수 ends는 끝 부분에서부터 시작할 위치를 가리킵니다.
         */
		bool EndsWith(const wchar_t *str, size_t ends = 0) const
		{
			return EndsWithHelper(str, ends, wcslen(str));
		}
        
        /**
         *  @brief  문자열의 끝 부분이 지정된 문자열과 일치하는지의 여부를 가져옵니다.
         *  @param  refer 지정할 문자열입니다.
         *  @param  ends 검색을 시작할 마지막 위치를 지정합니다.
         *
         *  이 함수의 매개변수 ends는 끝 부분에서부터 시작할 위치를 가리킵니다.
         */
		bool EndsWith(const WString& refer, size_t ends = 0) const
		{
			return EndsWithHelper(refer.m_ptr, ends, refer.m_length);
		}
        
        /**
         *  @brief  문자열의 끝 부분이 지정된 문자와 일치하는지의 여부를 가져옵니다.
         *  @param  ch 지정할 문자입니다.
         *  @param  ends 검색을 시작할 마지막 위치를 지정합니다.
         *
         *  이 함수의 매개변수 ends는 끝 부분에서부터 시작할 위치를 가리킵니다.
         */
		inline bool EndsWith(const wchar_t ch, size_t ends) const
		{
			if (ends >= m_length)
				return false;

			return *(m_last - ends) == ch;
		}
        
        /**
         *  @brief  문자열의 끝 부분이 지정된 문자와 일치하는지의 여부를 가져옵니다.
         *  @param  ch 지정할 문자입니다.
         */
		inline bool EndsWith(const wchar_t ch) const
		{
			return *m_last == ch;
		}
        
        /**
         *  @brief  문자열 집합의 크기를 지정된 크기만큼 설정하고 원래의 것을 오른쪽으로 
         *          정렬한뒤 남은 왼쪽 공간에 지정된 문자를 삽입한 문자열 집합을 가져옵니다.
         *  @param  len 지정할 크기입니다.
         *  @param  pad 지정할 문자입니다.
         *  @return len이 문자열의 크기보다 작은 경우 문자열 집합의 복사본을 가져옵니다.
         */
		WString PadLeft(size_t len, wchar_t pad = L' ')
		{
			if (len > m_length)
			{
				wchar_t* ret = new wchar_t[len+1];
				size_t   padlen = len - m_length;

				wcsnset(ret, pad, padlen);
				memcpy(ret + padlen, m_ptr, m_length * sizeof(wchar_t));

				ret[len] = 0; // (m_length + 1) * wchar_t로도 가능

				return WString(ret, len, with);
			}
			else
			{
				return WString((const wchar_t *)m_ptr, m_length);
			}
		}
		
        /**
         *  @brief  문자열 집합의 크기를 지정된 크기만큼 설정하고 원래의 것을 왼쪽으로 
         *          정렬한뒤 남은 오른쪽 공간에 지정된 문자를 삽입한 문자열 집합을 가져옵니다.
         *  @param  len 지정할 크기입니다.
         *  @param  pad 지정할 문자입니다.
         *  @return len이 문자열의 크기보다 작은 경우 문자열 집합의 복사본을 가져옵니다.
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
         *  @brief  문자열 집합의 크기를 지정된 크기만큼 설정하고 원래의 것을 가운데로
         *          정렬한뒤 남은 양쪽 공간에 지정된 문자를 삽입한 문자열 집합을 가져옵니다.
         *  @param  len 지정할 크기입니다.
         *  @param  pad 지정할 문자입니다.
         *  @param  lefts 남은 영역이 홀수만큼인경우 왼쪽을 더 길게할지의 여부를 설정합니다.
         *  @return len이 문자열의 크기보다 작은 경우 문자열 집합의 복사본을 가져옵니다.
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
         *  @brief  문자열의 왼쪽부터 지정된 크기만큼 문자를 건너뛰면서 지정된 문자열을
         *          삽입한 문자열을 가져옵니다.
         *  @param  separation 지정할 크기입니다.
         *  @param  str 지정할 문자열입니다.
         *
         *  이 함수를 사용하는 경우에 다음 예제를 참고하십시오.
         *  예제> "1234567890" 에서 InsertLeft(3, L"++")를 사용할 경우
         *       "123++456++789++0"으로 출력됨.
         *  마지막 항목엔 지정된 문자열을 추가하지 않습니다.
         */
		WString InsertLeft(size_t separation, const wchar_t *str)
		{
			return InsertLeftHelper(separation, str, wcslen(str));
		}
        
        /**
         *  @brief  문자열의 왼쪽부터 지정된 크기만큼 문자를 건너뛰면서 지정된 문자열을
         *          삽입한 문자열을 가져옵니다.
         *  @param  separation 지정할 크기입니다.
         *  @param  refer 지정할 문자열입니다.
         *
         *  이 함수를 사용하는 경우에 다음 예제를 참고하십시오.
         *  예제> "1234567890" 에서 InsertLeft(3, L"++")를 사용할 경우
         *       "123++456++789++0"으로 출력됨.
         *  마지막 항목엔 지정된 문자열을 추가하지 않습니다.
         */
		WString InsertLeft(size_t separation, const WString& refer)
		{
			return InsertLeftHelper(separation, refer.m_ptr, refer.m_length);
		}
		
        /**
         *  @brief  문자열의 오른쪽부터 지정된 크기만큼 문자를 건너뛰면서 지정된 문자열을
         *          삽입한 문자열을 가져옵니다.
         *  @param  separation 지정할 크기입니다.
         *  @param  str 지정할 문자열입니다.
         *
         *  이 함수를 사용하는 경우에 다음 예제를 참고하십시오.
         *  예제> "1234567890" 에서 InsertRight(3, L",")를 사용할 경우
         *       "1,234,567,890"으로 출력됨.
         *  마지막 항목엔 지정된 문자열을 추가하지 않습니다.
         */
		WString InsertRight(size_t separation, const wchar_t *str)
		{
			return InsertRightHelper(separation, str, wcslen(str));
		}
        
        /**
         *  @brief  문자열의 오른쪽부터 지정된 크기만큼 문자를 건너뛰면서 지정된 문자열을
         *          삽입한 문자열을 가져옵니다.
         *  @param  separation 지정할 크기입니다.
         *  @param  refer 지정할 문자열입니다.
         *
         *  이 함수를 사용하는 경우에 다음 예제를 참고하십시오.
         *  예제> "1234567890" 에서 InsertRight(3, L",")를 사용할 경우
         *       "1,234,567,890"으로 출력됨.
         *  마지막 항목엔 지정된 문자열을 추가하지 않습니다.
         */
		WString InsertRight(size_t separation, const WString& refer)
		{
			return InsertRightHelper(separation, refer.m_ptr, refer.m_length);
		}
        
        /**
         *  @brief  문자열의 왼쪽부터 지정된 크기만큼 문자를 건너뛰면서 지정된 문자를
         *          삽입한 문자열을 가져옵니다.
         *  @param  separation 지정할 크기입니다.
         *  @param  ch 지정할 문자입니다.
         *
         *  이 함수를 사용하는 경우에 다음 예제를 참고하십시오.
         *  예제> "1234567890" 에서 InsertLeft(2, L'%')를 사용할 경우
         *       "12%34%56%78%90"으로 출력됨.
         *  마지막 항목엔 지정된 문자를 추가하지 않습니다.
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
         *  @brief  문자열의 오른쪽부터 지정된 크기만큼 문자를 건너뛰면서 지정된 문자를
         *          삽입한 문자열을 가져옵니다.
         *  @param  separation 지정할 크기입니다.
         *  @param  ch 지정할 문자입니다.
         *
         *  이 함수를 사용하는 경우에 다음 예제를 참고하십시오.
         *  예제> "1234567890" 에서 InsertRight(3, L',')를 사용할 경우
         *       "1,234,567,890"으로 출력됨.
         *  마지막 항목엔 지정된 문자를 추가하지 않습니다.
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

		// 문자열 집합에 포함된 src를 모두 dest로 
		// 치환한 문자열 집합을 가져옵니다.
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
         *  @brief  문자열에 포함된 특정 문자열을 지정된 문자열로 치환합니다.
         *  @param  src 어떤 문자열을 바꿀 것인지 지정합니다.
         *  @param  dest 어떤 문자열로 바꿀 것인지 지정합니다.
         *  @param  max 바꿀 횟수를 지정합니다.
         *
         *  이 클래스는 문자열 치환을 Replace와 ReplaceSlow의 두 가지 형식으로 제공합니다.
         *  Replace는 메모리를 두 배로 사용하는 대신 처리속도는 빠르며, ReplaceSlow는
         *  Replace와 달리 메모리를 정량만 사용하며, 처리속도는 느립니다.
         *  자세한 사항은 제공된 소스코드를 참고하시거나 작성자에게 문의하십시오.
         */
		WString Replace(const wchar_t *src, const wchar_t *dest, size_t max = SIZE_MAX)
		{
			return ReplaceHelper(src, dest, wcslen(src), wcslen(dest), max);
		}
        
        /**
         *  @brief  문자열에 포함된 특정 문자열을 지정된 문자열로 치환합니다.
         *  @param  refer0 어떤 문자열을 바꿀 것인지 지정합니다.
         *  @param  refer1 어떤 문자열로 바꿀 것인지 지정합니다.
         *  @param  max 바꿀 횟수를 지정합니다.
         *
         *  이 클래스는 문자열 치환을 Replace와 ReplaceSlow의 두 가지 형식으로 제공합니다.
         *  Replace는 메모리를 두 배로 사용하는 대신 처리속도는 빠르며, ReplaceSlow는
         *  Replace와 달리 메모리를 정량만 사용하며, 처리속도는 느립니다.
         *  자세한 사항은 제공된 소스코드를 참고하시거나 작성자에게 문의하십시오.
         */
		WString Replace(const WString& refer0, const WString& refer1, size_t max = SIZE_MAX)
		{
			return ReplaceHelper(refer0.m_ptr, refer1.m_ptr, refer0.m_length, refer1.m_length, max);
		}
        
        /**
         *  @brief  문자열에 포함된 특정 문자열을 지정된 문자열로 치환합니다.
         *  @param  src 어떤 문자열을 바꿀 것인지 지정합니다.
         *  @param  dest 어떤 문자열로 바꿀 것인지 지정합니다.
         *  @param  max 바꿀 횟수를 지정합니다.
         *
         *  이 클래스는 문자열 치환을 Replace와 ReplaceSlow의 두 가지 형식으로 제공합니다.
         *  Replace는 메모리를 두 배로 사용하는 대신 처리속도는 빠르며, ReplaceSlow는
         *  Replace와 달리 메모리를 정량만 사용하며, 처리속도는 느립니다.
         *  자세한 사항은 제공된 소스코드를 참고하시거나 작성자에게 문의하십시오.
         */
		WString ReplaceSlow(const wchar_t *src, const wchar_t *dest, size_t max = SIZE_MAX)
		{
			return ReplaceSlowHelper(src, dest, wcslen(src), wcslen(dest), max);
		}
        
        /**
         *  @brief  문자열에 포함된 특정 문자열을 지정된 문자열로 치환합니다.
         *  @param  refer0 어떤 문자열을 바꿀 것인지 지정합니다.
         *  @param  refer1 어떤 문자열로 바꿀 것인지 지정합니다.
         *  @param  max 바꿀 횟수를 지정합니다.
         *
         *  이 클래스는 문자열 치환을 Replace와 ReplaceSlow의 두 가지 형식으로 제공합니다.
         *  Replace는 메모리를 두 배로 사용하는 대신 처리속도는 빠르며, ReplaceSlow는
         *  Replace와 달리 메모리를 정량만 사용하며, 처리속도는 느립니다.
         *  자세한 사항은 제공된 소스코드를 참고하시거나 작성자에게 문의하십시오.
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
         *  @brief  문자열에 포함된 특정 문자열을 삭제합니다.
         *  @param  src 삭제할 문자열을 지정합니다.
         */
		WString Trim(const wchar_t *src)
		{
			return TrimHelper(src, wcslen(src), SIZE_MAX);
		}
		
        /**
         *  @brief  문자열에 포함된 특정 문자열을 삭제합니다.
         *  @param  refer 삭제할 문자열을 지정합니다.
         */
		WString Trim(const WString& refer)
		{
			return TrimHelper(refer.m_ptr, refer.m_length, SIZE_MAX);
		}
        
        /**
         *  @brief  문자열의 시작부분부터 지정된 크기만큼의 문자열을 가져옵니다.
         *  @param  len 가져올 문자열의 크기를 지정합니다.
         */
		WString Remove(size_t len)
		{ // 문자열 집합의 처음부터 index까지 포함하는 문자열 집합을 가져옵니다.
			if (len >= m_length) // exceptrion from this method
				throw(new StringException(StringErrorCode::ComparasionSizeException));

			return this->Substring(0, len);
		}
        
        /**
         *  @brief  문자열의 지정된 부분부터 지정된 크기만큼의 문자를 삭제한 
         *          문자열을 가져옵니다.
         *  @param  starts 삭제할 부분의 시작위치를 지정합니다.
         *  @param  len 삭제할 부분의 크기를 지정합니다.
         */
		WString Remove(size_t starts, size_t len)
		{
            // 문자열 집합의 starts 위치 부터 count 만큼을 포함하는 집합과 문자열
		    // 집합의 차집합을 가져옵니다. (바깥 블록)
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
         *  @brief  문자열의 끝부분부터 지정된 크기만큼의 문자열을 가져옵니다.
         *  @param  len 가져올 문자열의 크기를 지정합니다.
         */
		WString RemoveReverse(size_t len)
		{
			if (len >= m_length) // exceptrion from this method
				throw(new StringException(StringErrorCode::ComparasionSizeException));

			return this->SubstringReverse(0, len);
		}
        
        /**
         *  @brief  문자열의 지정된 부분부터 지정된 크기만큼의 문자를 삭제한 
         *          문자열을 가져옵니다.
         *  @param  starts 삭제할 부분의 시작위치를 지정합니다. 이 위치는
         *          문자열의 끝 부분부터 계산됩니다.
         *  @param  len 삭제할 부분의 크기를 지정합니다.
         */
		WString RemoveReverse(size_t starts, size_t len)
		{
			if (starts + len > m_length) // exceptrion from this method
				throw(new StringException(StringErrorCode::ComparasionSizeException));

			return this->Remove(m_length - starts - len, len);
		}
        
        /**
         *  @brief  문자열의 특정부분에 문자열을 삽입합니다.
         *  @param  starts 삽입할 부분의 시작위치를 지정합니다.
         *  @param  str 삽입할 문자열을 지정합니다.
         *  @param  len 삽입할 문자열의 크기를 지정합니다.
         *
         *  이 함수의 매개변수 len은 str의 길이보다 짧거나 같아야합니다.
         */
		WString Insert(size_t starts, const wchar_t *str, size_t len)
		{
			// wchar_t에 대한 오버로딩은 템플릿까지 사용해야하므로
			// 여러모로 곤란해 추가하지 않았다.
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
         *  @brief  문자열의 특정부분에 문자열을 삽입합니다.
         *  @param  starts 삽입할 부분의 시작위치를 지정합니다.
         *  @param  refer 삽입할 문자열을 지정합니다.
         *  @param  len 삽입할 문자열의 크기를 지정합니다.
         *
         *  이 함수의 매개변수 len은 refer의 길이보다 짧거나 같아야합니다.
         */
		WString Insert(size_t starts, const WString& refer, size_t len)
		{
			return Insert(starts, refer.m_ptr, len);
		}
        
        /**
         *  @brief  문자열의 특정부분에 문자열을 삽입합니다.
         *  @param  starts 삽입할 부분의 시작위치를 지정합니다.
         *  @param  str 삽입할 문자열을 지정합니다.
         */
		WString Insert(size_t starts, const wchar_t *str)
		{
			return Insert(starts, str, wcslen(str));
		}
        
        /**
         *  @brief  문자열의 특정부분에 문자열을 삽입합니다.
         *  @param  starts 삽입할 부분의 시작위치를 지정합니다.
         *  @param  refer 삽입할 문자열을 지정합니다.
         */
		WString Insert(size_t starts, const WString& refer)
		{
			return Insert(starts, refer.m_ptr, refer.m_length);
		}
        
        /**
         *  @brief  문자열을 지정된 횟수만큼 늘립니다.
         *  @param  count 늘릴 횟수를 지정합니다.
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
         *  @brief  문자열을 뒤집습니다.
         */
		WString Reverse()
		{
			wchar_t *ret = this->ToArray();
			_wcsrev(ret);
			return WString(ret, m_length, with);
		}
		
        /**
         *  @brief  문자열의 일부를 가져옵니다.
         *  @param  first 가져올 문자열의 를 지정합니다.
         *  @param  last 가져올 문자열의 끝위치를 지정합니다.
         *
         *  이 함수의 매개변수 last가 0보다 작을 경우 뒤에서부터 가져옵니다.
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
         *  @brief  문자열의 일부를 가져옵니다.
         *  @param  skip 가져올 문자열의 위치를 지정합니다.
         *
         *  이 함수는 문자열의 양 끝 부터 skip만큼 떨어진 부분까지 삭제한 문자열을
         *  가져옵니다.
         */
		WString Slice(size_t skip)
		{
			if ((skip << 1) > m_length)
				throw(new StringException(StringErrorCode::ComparasionSizeException));

			return WString((const wchar_t *)(m_ptr + skip), m_length - (skip << 1));
		}
        
        /**
         *  @brief  지정된 크기만큼 건너뛴 문자열을 가져옵니다.
         *  @param  jmp 건너뛸 크기를 지정합니다.
         *  @param  starts 검색할 시작위치를 지정합니다.
         *  @param  len 건너뛴 후 가져올 문자열의 크기를 지정합니다.
         *  @param  remain 마지막 항목이 지정된 틀에 맞지 않은 경우 해당 문자열을 
         *          가져올지의 여부를 지정합니다.
         *
         *  이 함수를 사용하는 경우에 다음 예제를 참고하십시오.
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
         *  @brief  지정된 크기만큼 잘라 개행문자를 삽입한 문자열 집합을 가져옵니다.
         *  @param  len 자를 문자열의 크기를 지정합니다.
         *  @return ReadOnlyArray기반의 배열형식으로 반환합니다. 자세한 사항은
         *          collection/Array.h를 참고하십시오.
         */
		Lines LineSplit(size_t len)
		{
			return LineSplitHelper(len, nullptr, 0, nullptr, 0);
		}
		
        /**
         *  @brief  지정된 크기만큼 잘라 개행문자를 삽입한 뒤 지정된 문자열을
         *          각 문자열 행 앞에 삽입한 문자열 집합을 가져옵니다. 
         *  @param  len 자를 문자열의 크기를 지정합니다.
         *  @param  front 앞에 삽입할 문자열을 지정합니다.
         *  @return ReadOnlyArray기반의 배열형식으로 반환합니다. 자세한 사항은
         *          collection/Array.h를 참고하십시오.
         */
		Lines LineSplit(size_t len, const WString& front)
		{
			return LineSplitHelper(len, front.Reference(), front.m_length, nullptr, 0);
		}
        
        /**
         *  @brief  지정된 크기만큼 잘라 개행문자를 삽입한 뒤 지정된 문자열을
         *          각 문자열 행 앞에 삽입한 문자열 집합을 가져옵니다. 
         *  @param  len 자를 문자열의 크기를 지정합니다.
         *  @param  front 앞에 삽입할 문자열을 지정합니다.
         *  @return ReadOnlyArray기반의 배열형식으로 반환합니다. 자세한 사항은
         *          collection/Array.h를 참고하십시오.
         */
		Lines LineSplit(size_t len, const wchar_t *front)
		{
			return LineSplitHelper(len, front, wcslen(front), nullptr, 0);
		}
		
        /**
         *  @brief  지정된 크기만큼 잘라 개행문자를 삽입한 뒤 지정된 문자열을
         *          각각 문자열 행 앞, 뒤에 삽입한 문자열 집합을 가져옵니다. 
         *  @param  len 자를 문자열의 크기를 지정합니다.
         *  @param  front 앞에 삽입할 문자열을 지정합니다.
         *  @param  end 뒤에 삽입할 문자열을 지정합니다.
         *  @return ReadOnlyArray기반의 배열형식으로 반환합니다. 자세한 사항은
         *          collection/Array.h를 참고하십시오.
         */
		Lines LineSplit(size_t len, const WString& front, const WString& end)
		{
			return LineSplitHelper(len, front.Reference(), front.m_length, end.Reference(), end.m_length);
		}
        
        /**
         *  @brief  지정된 크기만큼 잘라 개행문자를 삽입한 뒤 지정된 문자열을
         *          각각 문자열 행 앞, 뒤에 삽입한 문자열 집합을 가져옵니다. 
         *  @param  len 자를 문자열의 크기를 지정합니다.
         *  @param  front 앞에 삽입할 문자열을 지정합니다.
         *  @param  end 뒤에 삽입할 문자열을 지정합니다.
         *  @return ReadOnlyArray기반의 배열형식으로 반환합니다. 자세한 사항은
         *          collection/Array.h를 참고하십시오.
         */
		Lines LineSplit(size_t len, const wchar_t *front, const wchar_t *end)
		{
			return LineSplitHelper(len, front, wcslen(front), end, wcslen(end));
		}
		
        /**
         *  @brief  \r\n 또는 \n를 구분자로 하여 자른 문자열들을 가져옵니다.
         *  @param  last 마지막 줄이 비어있으면 출력할지의 여부를 설정합니다.
         *  @return ReadOnlyArray기반의 배열형식으로 반환합니다. 자세한 사항은
         *          collection/Array.h를 참고하십시오.
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
         *  @brief  한 줄 당 len만큼의 문자수로 분할하고 개행문자를 삽입한 
         *          문자열들을 가져옵니다.
         *
         *  이 함수에서 개행문자로 \r\n을 사용합니다.
         */
		WString LineBreak(size_t len)
		{
			size_t remainLen = m_length % len;         // 맨 마지막 줄에 남은 문자 수
			size_t fullinsertLen = m_length / len;     // 문자열이 줄 전체에 채워지는 줄의 수
			size_t countLine = fullinsertLen + (remainLen != 0); // 모든 줄 수

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
         *  @brief  문자열의 해시값을 계산합니다.
         *
         *  이 함수는 성능과 보안에 문제가 있을 수 있습니다.
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
         *  @brief  문자열이 c언어에서 제공하는 십진 숫자로 바꿀 수 있는지의
         *          여부를 가져옵니다.
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
         *  @brief  문자열이 c언어에서 제공하는 16진법 숫자로 바꿀 수 있는지의
         *          여부를 가져옵니다.
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
         *  @brief  16진수인 문자열을 숫자로 바꾼 수를 출력합니다.
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
         *  @brief  문자열을 문자로 변환합니다.
         *  @return 문자열 집합의 첫 요소를 반환합니다.
         *  @throw  문자열의 크기가 1이여야하며, 그렇지 않을 경우
         *          StringErrorCode::OverflowReferenceException로 throw됩니다.
         */
		inline wchar_t ToChar() const
		{
			if (m_length != 1)
				throw(new StringException(StringErrorCode::OverflowReferenceException));

			return m_ptr[0];
		}
        
        /**
         *  @brief  10진 정수로 표현된 문자열을 숫자로 변환합니다.
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
         *  @brief  10진 정수로 표현된 문자열을 숫자로 변환합니다.
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
         *  @brief  10진 정수로 표현된 문자열을 숫자로 변환합니다.
         */
		long int ToLong() const
		{
			return (long)ToLongLong();
		}
        
        /**
         *  @brief  10진 정수로 표현된 문자열을 숫자로 변환합니다.
         */
		unsigned long int ToULong() const
		{
			return (unsigned long)ToULongLong();
		}
        
        /**
         *  @brief  10진 정수로 표현된 문자열을 숫자로 변환합니다.
         */
		int ToInteger() const
		{
			return (int)ToLongLong();
		}
        
        /**
         *  @brief  10진 정수로 표현된 문자열을 숫자로 변환합니다.
         */
		unsigned int ToUInteger() const
		{
			return (int)ToULongLong();
		}
        
        /**
         *  @brief  10진 정수로 표현된 문자열을 숫자로 변환합니다.
         */
		short int ToShort() const
		{
			return (short)ToLongLong();
		}
        
        /**
         *  @brief  10진 정수로 표현된 문자열을 숫자로 변환합니다.
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
         *  @brief  10진 실수로 표현된 문자열을 숫자로 변환합니다.
         */
		long double ToLongDouble() const
		{
			return ToLongDoubleHelper(m_ptr);
		}
        
        /**
         *  @brief  10진 실수로 표현된 문자열을 숫자로 변환합니다.
         */
		double ToDouble() const
		{
			return (double)ToLongDouble();
		}
        
        /**
         *  @brief  10진 실수로 표현된 문자열을 숫자로 변환합니다.
         */
		float ToFloat() const
		{
			return (float)ToLongDouble();
		}
        
        /**
         *  @brief  문자열의 복사본을 가져옵니다.
         */
		inline wchar_t *ToArray() // 바이트를 쓰려면 이걸 캐스팅하자.
		{
			wchar_t *ret = new wchar_t[m_length + 1];
			memcpy(ret, m_ptr, (m_length + 1) * sizeof(wchar_t));
			return ret;
		}

        /**
         *  @brief  문자열의 복사본을 가져옵니다.
         */
		inline const wchar_t *ToArray() const
		{
			wchar_t *ret = new wchar_t[m_length + 1];
			memcpy(ret, m_ptr, (m_length + 1) * sizeof(wchar_t));
			return ret;
		}
        
        /**
         *  @brief  문자열을 Ansi로 변환한 문자열을 가져옵니다.
         */
		char *ToAnsi()
		{
			return UnicodeToAnsi();
		}
        
        /**
         *  @brief  문자열의 ToUtf8로 변환한 문자열을 가져옵니다.
         *  @return ReadOnlyArray기반의 배열형식으로 반환합니다. 자세한 사항은
         *          collection/Array.h를 참고하십시오.
         *
         *  이 함수의 반환형식은 바이트배열입니다.
         */
		Utf8Array ToUtf8(bool file_bom = false)
		{
		    // 이 변환의 최대의 단점은 간단한 방법으론 크기의 예측이 불가능 하다는 것이다.
		    // 이를 해결하려면 이 루프를 두 번돌리거나 bytebuffer를 만드는게 최선일테지만.
		    // 그래서 여기선 두 번의 루프를 돌려 구현하였다.
		    // 윈도우 환경에서만 지원된다.
			size_t szReal = file_bom + (file_bom << 1);
			size_t size = szReal + m_length;
			unsigned long *tmp = new unsigned long[size];
			unsigned long *ptr = tmp;

			if ( file_bom )
			{
				ptr[0] = 0xef; ptr[1] = 0xbb; ptr[2] = 0xbf;

				ptr += 3;
			}

			// 3 byte이상은 Encoding::ToUtf8을 쓰자.
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
         *  @brief  두 문자열을 결합합니다.
         */
		WString operator&(const WString& concat)
		{
			return this->Concat(*this, concat);
		}

        /**
         *  @brief  두 문자열을 결합합니다.
         */
		WString operator+(const WString& concat)
		{
			return this->Concat(*this, concat);
		}
        
        /**
         *  @brief  두 문자열을 비교합니다.
         */
		inline bool operator>(const WString& compare)
		{
			return wcscmp(m_ptr, compare.m_ptr) > 0;
		}

        /**
         *  @brief  두 문자열을 비교합니다.
         */
		inline bool operator<(const WString& compare)
		{
			return wcscmp(m_ptr, compare.m_ptr) < 0;
		}

        /**
         *  @brief  두 문자열을 비교합니다.
         */
		inline bool operator>=(const WString& compare)
		{
			return !this->operator<(compare);
		}

        /**
         *  @brief  두 문자열을 비교합니다.
         */
		inline bool operator<=(const WString& compare)
		{
			return !this->operator>(compare);
		}
        
        /**
         *  @brief  두 문자열을 서로바꿉니다.
         */
		inline void Swap(WString& refer)
		{
			std::swap(m_ptr, refer.m_ptr);
			std::swap(m_last, refer.m_last);
			std::swap(m_length, refer.m_length);
		}
        
        /**
         *  @brief  지정된 문자열을 복사합니다.
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
         *  @brief  지정된 문자열의 클론을 만듭니다.
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
         *  @brief  지정된 문자열의 클론을 만듭니다.
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
            // 이거 표준임? 모르겠음
			mbstowcs_s(&converted, m_ptr, m_length + 1, str, SIZE_MAX);
		}
		
		void AnsiToUnicode(const char *ansi, size_t len)
		{
			wchar_t *ptr = m_ptr = new wchar_t[len + 1];
			size_t rlen = len;
			m_length = len;
            // ㅋㅋ
			while (rlen--)
				*ptr++ = (wchar_t)*ansi++;
			*ptr = 0;
			m_last = m_ptr + len - 1;
		}

		// perfect ansi여야한다.
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
        //  당신은 여기서부터 헤어날 수 없는 시공의 폭풍에 빠지게 됩니다.
        //  표준에 정의된 온갖 형변환 규칙과 연산 규칙이 당신을 혼돈의
        //  카오스로 빠지게 할 것 입니다.
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

		// xxxlen 반복 매크로
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
            // 제어경로 규칙을 변경해 else로 바꿀 수 있음
			if (!(trim[0] & (checker_type)( 0xffUL << (8 * 3)))) return ((size_t)trim - (size_t)ptr) + 3;
#ifdef _X64_MODE
			if (!(trim[0] & (checker_type)(0xffULL << (8 * 4)))) return ((size_t)trim - (size_t)ptr) + 4;
			if (!(trim[0] & (checker_type)(0xffULL << (8 * 5)))) return ((size_t)trim - (size_t)ptr) + 5;
			if (!(trim[0] & (checker_type)(0xffULL << (8 * 6)))) return ((size_t)trim - (size_t)ptr) + 6;
            // 제어경로 규칙을 변경해 else로 바꿀 수 있음
			if (!(trim[0] & (checker_type)(0xffULL << (8 * 7)))) return ((size_t)trim - (size_t)ptr) + 7;
#endif
            // 제어경로 반환조건을 임의로 설정하지 아니함
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
            // 제어경로 규칙을 변경해 else로 바꿀 수 있음
			if (!(trim[0] & (checker_type)( 0xffffUL << (16 * 1)))) return (((size_t)trim - (size_t)ptr) >> 1) + 1;
#ifdef _X64_MODE
			if (!(trim[0] & (checker_type)(0xffffULL << (16 * 2)))) return (((size_t)trim - (size_t)ptr) >> 1) + 2;
            // 제어경로 규칙을 변경해 else로 바꿀 수 있음
			if (!(trim[0] & (checker_type)(0xffffULL << (16 * 3)))) return (((size_t)trim - (size_t)ptr) >> 1) + 3;
#endif
            // 제어경로 반환조건을 임의로 설정하지 아니함
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