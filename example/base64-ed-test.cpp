/***

   RollRat Software Project.
   Copyright (C) 2015. rollrat. All Rights Reserved.

File name:
   
   base64-ed-test.cpp

Purpose:

	RollRat Software Grep Utility Burden

Author:

   11-14-2015:   HyunJun Jeong, Creation

***/

#include <locale>
#include <tchar.h>
#include <iostream>

#include "Base64.h"
#include "File.h"

using namespace Utility;

#define LIMITED_SOURCE_FILE_SIZE	1024*1024*25 // 25 MiB

int atq_tmain(int argc, char* argv[])
{
	std::locale::global(std::locale("kor"));
	std::wcout.imbue(std::locale("kor"));
	std::wcin.imbue(std::locale("kor"));
	
	std::wcout << L"RollRat Software File Base64 Encode/Decode Tool" << std::endl <<
		          L"Copyright (c) rollrat. 2015. All rights reserved." << std::endl << std::endl <<
				  L"            1. Encode    2. Decode        " << std::endl << std::endl;

RETRY:
	wchar_t ch;
	std::wcout << L"select> ";
	std::wcin >> ch;

	while (getchar() != '\n');

	if (ch == L'1' || ch == L'2')
	{
	READDR1:
		wchar_t tmp[256];
		wchar_t *context = nullptr;
		std::wcout << L"src-addr> ";
		fgetws(tmp, 255, stdin);
		wcstok_s(tmp, L"\n", &context);

		WString srcAddr(tmp, wcslen(tmp));

		if (!FileCheck::CheckFileExists(srcAddr))
		{
			std::cout << "\"" << tmp << "\"" << " FILE IS NOT FOUND !" << std::endl;
			goto READDR1;
		}

		size_t szSource = FileIO::GetFileSize(srcAddr);

		if (szSource > LIMITED_SOURCE_FILE_SIZE && ch == L'1')
		{
			wchar_t ch;
			std::wcout << L"File size is too long. Do you want to continue? \nPress 'y' key want to continue> ";
			std::wcin >> ch;

			while (getchar() != '\n');

			if (ch != L'y')
				goto READDR1;
		}

		switch ( ch )
		{
		case L'1':
			{
				std::wcout << L"Expected size : " << WString(size_t(((szSource / 3) * 4) / 1024)).InsertRight(3, L',') << L" KB" << std::endl
					<< L"Encoding..." << std::endl;

				WString bat = Base64::Base64Encode(FileIO::ReadByte(srcAddr));

				WString writtenIn = srcAddr + L".base64";

				std::wcout << L"\"" << writtenIn << L"\"" << " Writing to a file... " << std::endl;

				FileIO::WriteFilew(writtenIn, bat);
			}
			break;

		case L'2':
			{
				if (FileStringW::GetExtension(srcAddr) != L".base64")
				{
					std::wcout << L"Extension of the encoding file must .base64." << std::endl;
					goto READDR1;
				}

				std::wcout << L"Expected size : " << WString(size_t(((szSource / 4) * 3) / 1024)).InsertRight(3, L',') << L" KB" << std::endl
					<< L"Decoding..." << std::endl;
				
				Base64::Base64ArrayType bat = Base64::Base64Decode(FileIO::ReadFileA(srcAddr));
				
				WString writtenIn = srcAddr.Remove(srcAddr.Length() - WString(L".base64").Length());
				
				std::wcout << L"\"" << writtenIn << L"\"" << " Writing to a file... " << std::endl;

				FileIO::WriteByte(writtenIn, bat.Array(), bat.Size());
			}
			break;
		}

		std::wcout << L"Complete." << std::endl << std::endl;
	}
	goto RETRY;
}