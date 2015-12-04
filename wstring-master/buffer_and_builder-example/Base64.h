/***

   RollRat Software Project.
   Copyright (C) 2015. rollrat. All Rights Reserved.

File name:
   
   Base64.h

Purpose:

	RollRat Library

Author:

   11-06-2015:   HyunJun Jeong, Creation

***/

#ifndef _BASE64_9bf1541fdf7efd41b7b39543fd870ac4_
#define _BASE64_9bf1541fdf7efd41b7b39543fd870ac4_

#include "WStringBuffer.h"
#include "Array.h"

namespace Utility {

	//
	//	wchar_t형식은 2byte씩, char은 1byte씩 변환됨
	//
	class Base64
	{
	public:

		typedef ReadOnlyArray<unsigned char> Base64ArrayType;
		
		//
		//	base64인코딩은 6개의 비트를 ANSICS로 변환한다.
		//	즉, (size * 6) % 5 == 0 인 경우엔 정확하게 칸을 채울 수 있으나
		//	1~4인 경우엔 각각 ==, =, ==, =라는 패딩을 문자열 마지막에 삽입한다.
		//	전체 크기는 약 size * 8 / 6으로 size * 4 / 3이다.
		//
		static WString Base64Encode(unsigned char *bytes, size_t size)
		{
			// ANSI Table
			const wchar_t *table =
				L"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
				L"abcdefghijklmnopqrstuvwxyz"
				L"0123456789+/";

			// 패딩계산할 바에 2 더하는 것이 낫다.
			WStringBuffer wb(size * 4 / 3 + 2);
			size_t i = 0;

			if ( size > 1 )
			{
				// 3글자당 4개의 base64 unit이 생성되므로
				// 조건삽입이 3번 필요하다
				for ( ; i < size - 2; i += 3 )
				{
					//      (bytes[i] & 0xfc) >> 2;
					wb.Append(table[(bytes[i] & 0xfc) >> 2]);

					//      (bytes[i] & 0x03) << 4;
					//    | (*(bytes + i + 1) & 0xf0) >> 4;
					wb.Append(table[((bytes[i] & 0x03) << 4) | ((*(bytes + i + 1) & 0xf0) >> 4)]);

					//      (*(bytes + i + 1) & 0x0f) << 2;
					//    | (*(bytes + i + 2) & 0xc0) >> 6;
					wb.Append(table[((*(bytes + i + 1) & 0x0f) << 2) | ((*(bytes + i + 2) & 0xc0) >> 6)]);

					//      *(bytes + i + 2) & 0x3f;
					wb.Append(table[*(bytes + i + 2) & 0x3f]);
				}
			}

			if ( i < size )
			{
				//      (bytes[i] & 0xfc) >> 2;
				wb.Append(table[(bytes[i] & 0xfc) >> 2]);

				if ( i + 1 == size )
				{
					//      (bytes[i] & 0x03) << 4;
					wb.Append(table[(bytes[i] & 0x03) << 4]);
					wb.Append(L"==");
				}
				else
				{
					//      (bytes[i] & 0x03) << 4;
					//   |= (*(bytes + i + 1) & 0xf0) >> 4;
					wb.Append(table[((bytes[i] & 0x03) << 4) | (*(bytes + i + 1) & 0xf0) >> 4]);
					//      (*(bytes + i + 1) & 0x0f) << 2;
					wb.Append(table[(*(bytes + i + 1) & 0x0f) << 2]);
					wb.Append(L"=");
				}
			}

			return wb.ToString();
		}

		static WString Base64Encode(const Base64ArrayType& bat)
		{
			return Base64Encode(bat.Array(), bat.Size());
		}

		static WString Base64Encode(const WString& refer)
		{
			return Base64Encode((unsigned char *)refer.Reference(), refer.Length() << 1);
		}

		static WString Base64Encode(const wchar_t *wchs)
		{
			return Base64Encode((unsigned char *)wchs, wcslen(wchs) << 1);
		}

		static WString Base64Encode(const char *chs)
		{
			return Base64Encode((unsigned char *)chs, strlen(chs));
		}

		static Base64ArrayType Base64Decode(const wchar_t *wchs, size_t size)
		{
			// error
			if (size % 4 != 0)
				return Base64ArrayType(nullptr, 0);

			const unsigned char reversetable[] = {
				// skip ~ *
				-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
				-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
				-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,

				// +
				62,

				// , ~ .
				-1,-1,-1,

				// /
				63,

				// 0~9
				52,53,54,55,56,57,58,59,60,61,

				// : ~ @
				-1,-1,-1,-1,-1,-1,-1,

				// A ~ Z
				0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,

				// [ ~ `
				-1,-1,-1,-1,-1,-1,

				// a ~ z
				26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,
			};

			size_t totalsz;

			unsigned char *ret = new unsigned char[
				totalsz = (size * 3) / 4 -
				((wchs[size - 1] == L'=') + (wchs[size - 2] == L'='))
			];
			unsigned char *ptr = ret;

			for (size_t i = 0; i < size; i += 4)
			{
				*ptr++ = (unsigned char)((reversetable[wchs[i]]     << 2) | (reversetable[wchs[i + 1]] >> 4));
				*ptr++ = (unsigned char)((reversetable[wchs[i + 1]] << 4) | (reversetable[wchs[i + 2]] >> 2));
				*ptr++ = (unsigned char)((reversetable[wchs[i + 2]] << 6) | (reversetable[wchs[i + 3]]     ));
			}

			return Base64ArrayType(ret, totalsz);
		}
		
		static Base64ArrayType Base64Decode(const wchar_t *wchs)
		{
			return Base64Decode(wchs, wcslen(wchs));
		}
		
		static Base64ArrayType Base64Decode(const WString& refer)
		{
			return Base64Decode(refer.Reference(), refer.Length());
		}

		static Base64ArrayType Base64Decode(const char *chs, size_t size)
		{
			// error
			if (size % 4 != 0)
				return Base64ArrayType(nullptr, 0);

			const unsigned char reversetable[] = {
				// skip ~ *
				-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
				-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
				-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,

				// +
				62,

				// , ~ .
				-1,-1,-1,

				// /
				63,

				// 0~9
				52,53,54,55,56,57,58,59,60,61,

				// : ~ @
				-1,-1,-1,-1,-1,-1,-1,

				// A ~ Z
				0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,

				// [ ~ `
				-1,-1,-1,-1,-1,-1,

				// a ~ z
				26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,
			};

			size_t totalsz;

			unsigned char *ret = new unsigned char[
				totalsz = (size * 3) / 4 -
				((chs[size - 1] == L'=') + (chs[size - 2] == L'='))
			];
			unsigned char *ptr = ret;

			for (size_t i = 0; i < size; i += 4)
			{
				*ptr++ = (unsigned char)((reversetable[chs[i]]     << 2) | (reversetable[chs[i + 1]] >> 4));
				*ptr++ = (unsigned char)((reversetable[chs[i + 1]] << 4) | (reversetable[chs[i + 2]] >> 2));
				*ptr++ = (unsigned char)((reversetable[chs[i + 2]] << 6) | (reversetable[chs[i + 3]]     ));
			}

			return Base64ArrayType(ret, totalsz);
		}
		
		static Base64ArrayType Base64Decode(const char *wchs)
		{
			return Base64Decode(wchs, strlen(wchs));
		}
		

	};

}

#endif