/***

   RollRat Software Project.
   Copyright (C) 2015. rollrat. All Rights Reserved.

   wstring-test.cpp

***/

#include <locale.h>
#include <tchar.h>
#include <iostream>

#include "WString.h"

using namespace Utility;

int test_tmain()
{
	std::locale::global(std::locale("kor"));
	std::wcout.imbue(std::locale("kor"));

	WString wstr(L"TEST String 1234567890");

	// Concat: 문자열 병합
	std::wcout << WString::Concat(L"123", L"456", L"789") << std::endl;
	// -> "123456789"


	// Append: 문자열 첫 요소에 삽입
	std::wcout << wstr.Append(L"test!") << std::endl;
	// -> "test!TEST String 1234567890"


	// Conatins: 포함여부 확인
	std::wcout << wstr.Contains(L"String") << std::endl;
	// -> "1"


	// FindFirst: 첫 번째 포함위치 탐색
	std::wcout << wstr.FindFirst(L"String") << std::endl;
	// -> "5"


	// FindLast: 마지막 포함위치 탐색
	WString wstr0(L"TEST String 1234567890 String 1234556");
	std::wcout << wstr0.FindLast(L"String") << std::endl;
	// -> "23"


	// Substring: 문자열 자름
	std::wcout << wstr.Substring(5) << std::endl;
	// -> "String 1234567890"
	std::wcout << wstr.Substring(5,6) << std::endl;
	// -> "String"


	// SubstringReverse: 뒤에서 부터 문자열 자름
	std::wcout << wstr.SubstringReverse(10) << std::endl;
	// -> "1234567890"
	std::wcout << wstr.SubstringReverse(10,5) << std::endl;
	// -> "67890"


	// Remove: 문자열 삭제
	std::wcout << wstr.Remove(4) << std::endl;
	// -> "TEST"
	std::wcout << wstr.Remove(5,6) << std::endl;
	// -> "TEST  1234567890"

	
	// Insert: 문자열 삽입
	std::wcout << wstr.Insert(4, L"Test") << std::endl;
	// -> "TESTTest String 1234567890 String 1234556"


	// TrimStart: 선행공백제거
	// TrimEnd: 후행공백제거
	// Trim: 선/후행공백제거
	WString wstr1(L"   trim   ");
	std::wcout << wstr1.TrimStart() << std::endl;
	// -> "trim   "
	std::wcout << wstr1.TrimEnd() << std::endl;
	// -> "   trim"
	std::wcout << wstr1.Trim() << std::endl;
	// -> "trim"


	// Len: 포함 문자열 개수 가져오기
	WString wstr2(L"string 123 string 456 s tri ng 789 string");
	std::wcout << wstr2.Count(L"string") << std::endl;
	// -> "3"


	// Split: 문자열을 기준으로 문자열 자름
	WString wstr3(L"s|plitstes|t123s|plitstes|t456s|plitstes|t789");
	WString::SplitsArray splits = wstr3.Split(L"|plitstes|");
	splits.Each([] (const WString* refer) { std::wcout << *refer <<  L' '; });
	std::wcout << std::endl;
	// -> s t123s t456s 789


	// ToLower: 소문자로 변환
	// ToUpper: 대문자로 변환
	WString wstr4(L"StRinG");
	std::wcout << wstr4.ToLower() << std::endl;
	// -> "string"
	std::wcout << wstr4.ToUpper() << std::endl;
	// -> "STRING"


	// StartsWith: 처음부분이 일치하는 여부확인
	// EndsWith: 마지막부분이 일치하는 여부확인
	WString wstr5(L"startsTestends");
	std::wcout << wstr5.StartsWith(L"starts") << std::endl;
	std::wcout << wstr5.EndsWith(L"ends") << std::endl;
	// -> "1"
	std::wcout << wstr5.StartsWith(L"tarts") << std::endl;
	std::wcout << wstr5.EndsWith(L"end") << std::endl;
	// -> "0"


	// PadLeft: 문자열을 왼쪽으로 정렬하고 남은 공간을 문자로 채움
	// PadRight: 문자열을 오른쪽으로 정렬하고 남은 공간을 문자로 채움
	WString wstr6(L"PadTest");
	std::wcout << wstr6.PadLeft(wstr6.Length() + 3, L'#') << std::endl;
	// -> "###PadTest"
	std::wcout << wstr6.PadRight(wstr6.Length() + 3, L'#') << std::endl;
	// -> "PadTest###"


	// InsertLeft: 왼쪽부터 일정구간씩 띄어 문자(열) 삽입
	// InsertRight: 오른쪽부터 일정구간씩 띄어 문자(열) 삽입
	WString wstr7(L"123456789");
	std::wcout << wstr7.InsertLeft(3, L',') << std::endl;
	// -> "123,456,789"
	std::wcout << wstr7.InsertRight(3, L"sss") << std::endl;
	// -> "123sss456sss789"


	// Replace: 특정 문자열을 특정 문자열로 치환
	WString wstr8(L"123##456##789");
	std::wcout << wstr8.Replace(L"##", L"^^^") << std::endl;
	// -> "123^^^456^^^789"


	// Repeat: 반복
	WString wstr9(L"iter");
	std::wcout << wstr9.Repeat(3) << std::endl;
	// -> "iteriteriter"


	// Reverse: 뒤집음
	WString wstr10(L"reverse");
	std::wcout << wstr10.Reverse() << std::endl;
	// -> "esrever"



	return 0;
}