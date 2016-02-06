/***

   RollRat Software Project.
   Copyright (C) 2015. rollrat. All Rights Reserved.

File name:
   
   grep.cpp

Purpose:

	RollRat Software Grep Utility

Author:

   10-08-2015:   HyunJun Jeong, Creation

***/

#include <locale>
#include <tchar.h>
#include <iostream>
#include <ShlObj.h>
#include <vector>

#include "WStringBuilder.h"
#include "File.h"

#define DB_DEFAULT_DIRECTORY	L"C:\\rollrat\\db\\"
#define DB_DEFAULT_SAVE_DIRECTORY	L"C:\\rollrat\\result\\"
#define DB_DEFAULT_SEPERATOR	L"*"

using namespace Utility;

WString extensions = L".cpp|.h";

void fast_cls()
{
	COORD coordScreen = { 0, 0 };    /* here's where we'll home the
										cursor */
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	bool bSuccess;
	DWORD cCharsWritten;
	CONSOLE_SCREEN_BUFFER_INFO csbi; /* to get buffer info */
	DWORD dwConSize;                 /* number of character cells in
										the current buffer */

	bSuccess = GetConsoleScreenBufferInfo(hConsole, &csbi);
	dwConSize = csbi.dwSize.X * csbi.dwSize.Y;
	bSuccess = FillConsoleOutputCharacter(hConsole, (TCHAR) ' ',
		dwConSize, coordScreen, &cCharsWritten);
	bSuccess = GetConsoleScreenBufferInfo(hConsole, &csbi);
	bSuccess = FillConsoleOutputAttribute(hConsole, csbi.wAttributes,
		dwConSize, coordScreen, &cCharsWritten);
	bSuccess = SetConsoleCursorPosition(hConsole, coordScreen);
	return;
}

int gm_tmain(int argc, char **argv)
{
	std::locale::global(std::locale("kor"));
	std::wcout.imbue(std::locale("kor"));
	std::wcin.imbue(std::locale("kor"));

	std::wcout << L"RollRat Software Grep Utility 1.0" << std::endl <<
		          L"Copyright (c) rollrat. 2015. All rights reserved." << std::endl << std::endl;

ENTRY:
	if (argc == 1)
	{
		std::wcout << L"   1. Create db" << std::endl <<
			          L"   2. Load db" << std::endl << std::endl;

		std::wcout << L"com> ";

		int n;
		std::wcin >> n;

		if ( n == 1 )
		{
			wchar_t tmp[256];
		RETRY:
			std::wcout << L"addr> ";
			std::wcin >> tmp;

			size_t addr_len = wcslen(tmp);
			
			if ( tmp[addr_len - 1] == L'\\' && addr_len != 3 )
			{
				addr_len -= 1;
				tmp[addr_len - 1] = 0;
			}

			WString addr(tmp, addr_len);

			if ( !FileCheck::CheckDirectoryExists( addr ) )
			{
				std::wcout << L"Not founed directory. Please Try again." << std::endl;
				goto RETRY;
			}

			std::wcout << L"Scanning ..." << std::endl;
			
			WStringBuilder sb;
			FileEnumerateRecursionW ferw(addr_len == 3 ? addr.Substring(0,2) : addr, FileEnumerateOption::SkipDirectoryName);
			
			WString::SplitsArray ext_splits = extensions.Split(L"|");

			auto CheckExtension = [ext_splits](const WString& ext) -> bool 
				{
					for (size_t i = 0 ; i < ext_splits.Size(); i++)
					{
						if (ext == *ext_splits[i])
							return true;
					}
					return false;
				};

			ferw.start([&](const WString& ws) -> void 
			{
				if ( CheckExtension(FileStringW::GetExtension(ws).ToLower()) )
				{
					sb.Append(ws.Reference());
					sb.Append(DB_DEFAULT_SEPERATOR);
				}
			});
			
			if ( !FileCheck::CheckDirectoryExists( DB_DEFAULT_DIRECTORY ) )
			{
				 SHCreateDirectoryExW(0, DB_DEFAULT_DIRECTORY, 0);
			}
			if ( !FileCheck::CheckDirectoryExists( DB_DEFAULT_SAVE_DIRECTORY ) )
			{
				 SHCreateDirectoryExW(0, DB_DEFAULT_SAVE_DIRECTORY, 0);
			}
			
			FileIO::WriteFile( WString::Concat(DB_DEFAULT_DIRECTORY, addr_len != 3 ? FileStringW::GetFileName(addr) : WString::Concat(addr.Substring(0,1).ToUpper(), L" Drive"), L".txt"), sb.ToString());

			std::wcout << L"Complete." << std::endl << std::endl;
			goto ENTRY;
		}
		else if ( n == 2 )
		{
			fast_cls();

			InternalFileEnumerateW ifew(DB_DEFAULT_DIRECTORY, FileEnumerateOption::SkipDirectoryName);
			std::vector<WString> wss;

			do
			{
				wss.push_back(ifew.GetFullName());
				WString ref;
				std::wcout << WString::Concat((*new WString(wss.size())).PadLeft(5, L' '), L". ", ifew.GetName()).ToArray() << std::endl;
			} while (ifew.NextFile());

			int selected;

		SELECT_TRY:
			std::wcout << L"Select DB> ";
			std::wcin >> selected;

			if ( selected > wss.size() )
			{
				std::wcout << L"Please select vaild." << std::endl;
				goto SELECT_TRY;
			}
			std::wcout << L"Selected File is " << wss[selected-1].Reference() << std::endl << std::endl;

			WString::SplitsArray splits = FileIO::ReadFile(wss[selected - 1]).Split(L"*");

			std::wcout << L"Search What> ";

			wchar_t search_for[256];
			std::string gt;
			//std::wcin >> search_for;
			std::wcin.ignore();
			std::wcin.getline(search_for, sizeof(search_for));
			//std::getline(std::wcin, search_for);
			size_t search_for_len = wcslen(search_for);

			WStringBuilder result;
			size_t total_lines = 0;
			//result.Append(L"\xFEFFRollRat Software Grep Utility\nCopyright (c) rollrat. 2015. All rights reserved.\n\n");
			result.Append(L"RollRat Software Grep Utility\nCopyright (c) rollrat. 2015. All rights reserved.\n\n");
			
			std::wcout << L"Searching ..." << std::endl;
			
			for (size_t c = 0 ; c < splits.Size(); c++)
			{
				size_t matched_line = 0;
				WStringBuilder lines;

				FileIO::ReadFileCallback a{ [&](const WString& ws, size_t n) {
					if ( ws.Length() >= search_for_len )
					{
						if ( ws.Contains(search_for, search_for_len) )
						{
							lines.Append(WString::Concat(n, L": ").PadLeft(10, L' ').Reference());
							lines.Append(ws.Reference());
							lines.Append(L"\n");
							matched_line++;
						}
					}
				} };

				FileIO::ReadFileByLines(*splits[c], a);
				
				if (matched_line > 0)
				{
					result.Append(WString::Concat(*splits[c], L":").Reference());
					result.Append(L"\n");
					result.Append(lines.ToString().Reference());
					result.Append(WString::Concat(L"Lines: ", matched_line).Reference());
					result.Append(L"\n\n");
				}

				total_lines += matched_line;
			}

			result.Append(WString::Concat(L"Total Mached Lines: ", total_lines).Reference());
			FileIO::WriteFilew( WString::Concat(DB_DEFAULT_SAVE_DIRECTORY, FileStringW::GetFileNameWithoutExtension(wss[selected-1]), L".result.txt"), result.ToString());
			
			std::wcout << L"Complete." << std::endl << std::endl;
			goto ENTRY;

			result.Dispose();
		}
	}
}