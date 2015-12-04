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

	// Concat: ���ڿ� ����
	std::wcout << WString::Concat(L"123", L"456", L"789") << std::endl;
	// -> "123456789"


	// Append: ���ڿ� ù ��ҿ� ����
	std::wcout << wstr.Append(L"test!") << std::endl;
	// -> "test!TEST String 1234567890"


	// Conatins: ���Կ��� Ȯ��
	std::wcout << wstr.Contains(L"String") << std::endl;
	// -> "1"


	// FindFirst: ù ��° ������ġ Ž��
	std::wcout << wstr.FindFirst(L"String") << std::endl;
	// -> "5"


	// FindLast: ������ ������ġ Ž��
	WString wstr0(L"TEST String 1234567890 String 1234556");
	std::wcout << wstr0.FindLast(L"String") << std::endl;
	// -> "23"


	// Substring: ���ڿ� �ڸ�
	std::wcout << wstr.Substring(5) << std::endl;
	// -> "String 1234567890"
	std::wcout << wstr.Substring(5,6) << std::endl;
	// -> "String"


	// SubstringReverse: �ڿ��� ���� ���ڿ� �ڸ�
	std::wcout << wstr.SubstringReverse(10) << std::endl;
	// -> "1234567890"
	std::wcout << wstr.SubstringReverse(10,5) << std::endl;
	// -> "67890"


	// Remove: ���ڿ� ����
	std::wcout << wstr.Remove(4) << std::endl;
	// -> "TEST"
	std::wcout << wstr.Remove(5,6) << std::endl;
	// -> "TEST  1234567890"

	
	// Insert: ���ڿ� ����
	std::wcout << wstr.Insert(4, L"Test") << std::endl;
	// -> "TESTTest String 1234567890 String 1234556"


	// TrimStart: �����������
	// TrimEnd: �����������
	// Trim: ��/�����������
	WString wstr1(L"   trim   ");
	std::wcout << wstr1.TrimStart() << std::endl;
	// -> "trim   "
	std::wcout << wstr1.TrimEnd() << std::endl;
	// -> "   trim"
	std::wcout << wstr1.Trim() << std::endl;
	// -> "trim"


	// Len: ���� ���ڿ� ���� ��������
	WString wstr2(L"string 123 string 456 s tri ng 789 string");
	std::wcout << wstr2.Count(L"string") << std::endl;
	// -> "3"


	// Split: ���ڿ��� �������� ���ڿ� �ڸ�
	WString wstr3(L"s|plitstes|t123s|plitstes|t456s|plitstes|t789");
	WString::SplitsArray splits = wstr3.Split(L"|plitstes|");
	splits.Each([] (const WString* refer) { std::wcout << *refer <<  L' '; });
	std::wcout << std::endl;
	// -> s t123s t456s 789


	// ToLower: �ҹ��ڷ� ��ȯ
	// ToUpper: �빮�ڷ� ��ȯ
	WString wstr4(L"StRinG");
	std::wcout << wstr4.ToLower() << std::endl;
	// -> "string"
	std::wcout << wstr4.ToUpper() << std::endl;
	// -> "STRING"


	// StartsWith: ó���κ��� ��ġ�ϴ� ����Ȯ��
	// EndsWith: �������κ��� ��ġ�ϴ� ����Ȯ��
	WString wstr5(L"startsTestends");
	std::wcout << wstr5.StartsWith(L"starts") << std::endl;
	std::wcout << wstr5.EndsWith(L"ends") << std::endl;
	// -> "1"
	std::wcout << wstr5.StartsWith(L"tarts") << std::endl;
	std::wcout << wstr5.EndsWith(L"end") << std::endl;
	// -> "0"


	// PadLeft: ���ڿ��� �������� �����ϰ� ���� ������ ���ڷ� ä��
	// PadRight: ���ڿ��� ���������� �����ϰ� ���� ������ ���ڷ� ä��
	WString wstr6(L"PadTest");
	std::wcout << wstr6.PadLeft(wstr6.Length() + 3, L'#') << std::endl;
	// -> "###PadTest"
	std::wcout << wstr6.PadRight(wstr6.Length() + 3, L'#') << std::endl;
	// -> "PadTest###"


	// InsertLeft: ���ʺ��� ���������� ��� ����(��) ����
	// InsertRight: �����ʺ��� ���������� ��� ����(��) ����
	WString wstr7(L"123456789");
	std::wcout << wstr7.InsertLeft(3, L',') << std::endl;
	// -> "123,456,789"
	std::wcout << wstr7.InsertRight(3, L"sss") << std::endl;
	// -> "123sss456sss789"


	// Replace: Ư�� ���ڿ��� Ư�� ���ڿ��� ġȯ
	WString wstr8(L"123##456##789");
	std::wcout << wstr8.Replace(L"##", L"^^^") << std::endl;
	// -> "123^^^456^^^789"


	// Repeat: �ݺ�
	WString wstr9(L"iter");
	std::wcout << wstr9.Repeat(3) << std::endl;
	// -> "iteriteriter"


	// Reverse: ������
	WString wstr10(L"reverse");
	std::wcout << wstr10.Reverse() << std::endl;
	// -> "esrever"



	return 0;
}