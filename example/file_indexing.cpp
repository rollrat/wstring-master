/***

   RollRat Software Project.
   Copyright (C) 2015. rollrat. All Rights Reserved.

File name:
   
   file_indexing.cpp

Purpose:

	RollRat Software Grep Utility Burden

Author:

   10-18-2015:   HyunJun Jeong, Creation

***/

#include <locale>
#include <tchar.h>
#include <iostream>
#include <ShlObj.h>

#include "WStringBuilder.h"
#include "File.h"
#include "InfoZip\zip.h"

#define DB_DEFAULT_DIRECTORY	L"C:\\rollrat\\db\\"

using namespace Utility;

extern void fast_cls();

int a_tmain(int argc, char **argv)
{
	std::locale::global(std::locale("kor"));
	std::wcout.imbue(std::locale("kor"));
	std::wcin.imbue(std::locale("kor"));

	std::wcout << L"RollRat Software File Indexing Tool" << std::endl <<
		          L"Copyright (c) rollrat. 2015. All rights reserved." << std::endl << std::endl;

	WString ref;
	wchar_t tmp[256];
	wchar_t *context = nullptr;
NEW:
RETRY:
	std::wcout << L"addr> ";
	fgetws(tmp, 255, stdin);
	wcstok_s(tmp, L"\n", &context);

	size_t addr_len = wcslen(tmp);

	if (tmp[addr_len - 1] == L'\\' && addr_len != 3)
	{
		addr_len -= 1;
		tmp[addr_len - 1] = 0;
	}

	WString addr(tmp, addr_len);

	if (!FileCheck::CheckDirectoryExists(addr))
	{
		std::wcout << L"Not founed directory. Please Try again." << std::endl;
		goto RETRY;
	}

	std::wcout << L"Scanning ..." << std::endl;

	WStringBuilder sb(1024 * 1024 * 8);
	FileEnumerateRecursionW ferw(addr_len == 3 ? addr.Substring(0, 2) : addr);
	sb.Append(L"\xFEFF"); // Little Endian Unicode magic
	sb.Append(addr);
	if (!addr.EndsWith(L"\\"))
		sb.Append(L"\\");
	sb.Append(L'|');
	sb.Append(((uint64_t)ferw.getdata().ftCreationTime.dwHighDateTime << 32) | (ferw.getdata().ftCreationTime.dwLowDateTime));
	sb.Append(L'|');
	sb.Append(((uint64_t)ferw.getdata().ftLastAccessTime.dwHighDateTime << 32) | (ferw.getdata().ftLastAccessTime.dwLowDateTime));
	sb.Append(L'|');
	sb.Append(((uint64_t)ferw.getdata().ftLastWriteTime.dwHighDateTime << 32) | (ferw.getdata().ftLastWriteTime.dwLowDateTime));
	sb.Append(L'*');

	ferw.start([&](const WString& ws) -> void
	{
		sb.Append(ws.Reference(), ws.Length());
		if (ferw.isfolder())
			sb.Append(L'\\');
		else
		{
			sb.Append(L'|');
			sb.Append(ferw.filesize());
		}	
		sb.Append(L'|');
		sb.Append(((uint64_t)ferw.getdata().ftCreationTime.dwHighDateTime << 32) | (ferw.getdata().ftCreationTime.dwLowDateTime));
		sb.Append(L'|');
		sb.Append(((uint64_t)ferw.getdata().ftLastAccessTime.dwHighDateTime << 32) | (ferw.getdata().ftLastAccessTime.dwLowDateTime));
		sb.Append(L'|');
		sb.Append(((uint64_t)ferw.getdata().ftLastWriteTime.dwHighDateTime << 32) | (ferw.getdata().ftLastWriteTime.dwLowDateTime));
		sb.Append(L'*');
	});

	if (!FileCheck::CheckDirectoryExists(DB_DEFAULT_DIRECTORY))
	{
		SHCreateDirectoryExW(0, DB_DEFAULT_DIRECTORY, 0);
	}

	WString saveFileName = WString::Concat(
		DB_DEFAULT_DIRECTORY, L"file.tmp");
	FileIO::WriteFile(saveFileName, sb.ToString());

	std::wcout << L"Compressing ..." << std::endl;

	HZIP hz = CreateZip(WString::Concat(
		DB_DEFAULT_DIRECTORY, addr_len > 3 ? FileStringW::GetFileName(addr) 
		: WString::Concat(addr.Substring(0, 1).ToUpper(), L" Drive"), L".chksdb").Reference(), 0);
	ZipAdd(hz, L"file.tmp", saveFileName.Reference());
	CloseZip(hz);

	DeleteFileW(saveFileName.Reference());

	std::wcout << L"Complete." << std::endl << std::endl;
	sb.Dispose();
	goto NEW;
}
