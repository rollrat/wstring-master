/***

   RollRat Software Project.
   Copyright (C) 2015. rollrat. All Rights Reserved.

File name:
   
   URLEncoding.h

Purpose:

	RollRat Library

Author:

   11-07-2015:   HyunJun Jeong, Creation

***/

#ifndef _URLENCODING_9bf1541fdf7efd41b7b39543fd870ac4_
#define _URLENCODING_9bf1541fdf7efd41b7b39543fd870ac4_

#include "WStringBuilder.h"
#include "Array.h"
#include "Encoding.h"

namespace Utility {
	
	/*
	Generated by
	for (int i = 0; i <= 0xff; i++)
	{
		std::cout << "L\"%" << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << i << "\",";
	}
	*/
	const wchar_t *percent_encoding_table[256] =
	{
		L"%00",L"%01",L"%02",L"%03",L"%04",L"%05",L"%06",L"%07",L"%08",L"%09",L"%0A",L"%0B",L"%0C",L"%0D",L"%0E",L"%0F",
		L"%10",L"%11",L"%12",L"%13",L"%14",L"%15",L"%16",L"%17",L"%18",L"%19",L"%1A",L"%1B",L"%1C",L"%1D",L"%1E",L"%1F",
		L"%20",L"%21",L"%22",L"%23",L"%24",L"%25",L"%26",L"%27",L"%28",L"%29",L"%2A",L"%2B",L"%2C",L"%2D",L"%2E",L"%2F",
		L"%30",L"%31",L"%32",L"%33",L"%34",L"%35",L"%36",L"%37",L"%38",L"%39",L"%3A",L"%3B",L"%3C",L"%3D",L"%3E",L"%3F",
		L"%40",L"%41",L"%42",L"%43",L"%44",L"%45",L"%46",L"%47",L"%48",L"%49",L"%4A",L"%4B",L"%4C",L"%4D",L"%4E",L"%4F",
		L"%50",L"%51",L"%52",L"%53",L"%54",L"%55",L"%56",L"%57",L"%58",L"%59",L"%5A",L"%5B",L"%5C",L"%5D",L"%5E",L"%5F",
		L"%60",L"%61",L"%62",L"%63",L"%64",L"%65",L"%66",L"%67",L"%68",L"%69",L"%6A",L"%6B",L"%6C",L"%6D",L"%6E",L"%6F",
		L"%70",L"%71",L"%72",L"%73",L"%74",L"%75",L"%76",L"%77",L"%78",L"%79",L"%7A",L"%7B",L"%7C",L"%7D",L"%7E",L"%7F",
		L"%80",L"%81",L"%82",L"%83",L"%84",L"%85",L"%86",L"%87",L"%88",L"%89",L"%8A",L"%8B",L"%8C",L"%8D",L"%8E",L"%8F",
		L"%90",L"%91",L"%92",L"%93",L"%94",L"%95",L"%96",L"%97",L"%98",L"%99",L"%9A",L"%9B",L"%9C",L"%9D",L"%9E",L"%9F",
		L"%A0",L"%A1",L"%A2",L"%A3",L"%A4",L"%A5",L"%A6",L"%A7",L"%A8",L"%A9",L"%AA",L"%AB",L"%AC",L"%AD",L"%AE",L"%AF",
		L"%B0",L"%B1",L"%B2",L"%B3",L"%B4",L"%B5",L"%B6",L"%B7",L"%B8",L"%B9",L"%BA",L"%BB",L"%BC",L"%BD",L"%BE",L"%BF",
		L"%C0",L"%C1",L"%C2",L"%C3",L"%C4",L"%C5",L"%C6",L"%C7",L"%C8",L"%C9",L"%CA",L"%CB",L"%CC",L"%CD",L"%CE",L"%CF",
		L"%D0",L"%D1",L"%D2",L"%D3",L"%D4",L"%D5",L"%D6",L"%D7",L"%D8",L"%D9",L"%DA",L"%DB",L"%DC",L"%DD",L"%DE",L"%DF",
		L"%E0",L"%E1",L"%E2",L"%E3",L"%E4",L"%E5",L"%E6",L"%E7",L"%E8",L"%E9",L"%EA",L"%EB",L"%EC",L"%ED",L"%EE",L"%EF",
		L"%F0",L"%F1",L"%F2",L"%F3",L"%F4",L"%F5",L"%F6",L"%F7",L"%F8",L"%F9",L"%FA",L"%FB",L"%FC",L"%FD",L"%FE",L"%FF"
	};

	class URLEncoding
	{
	public:
		
		typedef ReadOnlyArray<unsigned char> URLEncodingArrayType;
		
        static WString Encode(unsigned char *bytes, size_t size)
		{
            WStringBuilder wb;


			for (size_t i = 0; i < size; i++)
			{
				wb.Append(percent_encoding_table[bytes[i]]);
			}

            return wb.ToString();
		}
		
		static WString Encode(const URLEncodingArrayType& bat)
		{
			return Encode(bat.Array(), bat.Size());
		}

		static WString Encode(const wchar_t *wchs, size_t len)
		{
			WStringBuilder wb;

			for (size_t i = 0; i < len; i++)
			{
				// iswalnum은 한글을 alpabet으로 취급함
				if (wchs[i] <= CHAR_MAX && (iswalnum(wchs[i]) || wcschr(L"*-._", wchs[i])))
				{
					wb.Append(wchs[i]);
				}
				else if (wchs[i] == L' ')
				{
					wb.Append(L"+");
				}
				else
				{
					unsigned long ul = Encoding::ToUtf8Ch(wchs[i]);
					
					if (ul >= 0x1000000)
					{
						wb.Append(percent_encoding_table[(ul & 0xff000000) >> 24], 3);
					}
					if (ul >= 0x10000)
					{
						wb.Append(percent_encoding_table[(ul & 0xff0000) >> 16], 3);
					}
					if (ul >= 0x100)
					{
						wb.Append(percent_encoding_table[(ul & 0xff00) >> 8], 3);
					}
					wb.Append(percent_encoding_table[ul & 0xff], 3);
				}
			}

            return wb.ToString();
		}

		static WString Encode(const WString& refer)
		{
			return Encode(refer.Reference(), refer.Length());
		}

		static WString Encode(const wchar_t *wchs)
		{
			return Encode(wchs, wcslen(wchs));
		}

		static WString Encode(const char *chs)
		{
			return WString(chs);
		}

		static WString Decode(const wchar_t *wchs, size_t len)
		{
			WStringBuilder wb;

			auto read_ch = [] (wchar_t ch) {
				return ch > L'9' ? ch < L'a' ?
					ch - L'A' + 10 : ch - L'a' + 10 : ch & 0xf;
			};

			for (size_t i = 0; i < len; )
			{
				if (wchs[i] == L'%')
				{
					unsigned long long buf;
					unsigned long mask = (read_ch(wchs[i + 1]) << 4) | (read_ch(wchs[i + 2]));

					i += 3;

					if ((mask & 0xfc) == 0xfc)
					{
						buf  = (read_ch(wchs[i + 1]))  << 36;
						buf |= (read_ch(wchs[i + 2]))  << 32;
						buf |= (read_ch(wchs[i + 4]))  << 28;
						buf |= (read_ch(wchs[i + 5]))  << 24;
						buf |= (read_ch(wchs[i + 7]))  << 20;
						buf |= (read_ch(wchs[i + 8]))  << 16;
						buf |= (read_ch(wchs[i + 10])) << 12;
						buf |= (read_ch(wchs[i + 11])) << 8;
						buf |= (read_ch(wchs[i + 13])) << 4;
						buf |= (read_ch(wchs[i + 14]));

						i += 15;
					}
					else if ((mask & 0xf8) == 0xf8)
					{
						buf  = (read_ch(wchs[i + 1]))  << 28;
						buf |= (read_ch(wchs[i + 2]))  << 24;
						buf |= (read_ch(wchs[i + 4]))  << 20;
						buf |= (read_ch(wchs[i + 5]))  << 16;
						buf |= (read_ch(wchs[i + 7]))  << 12;
						buf |= (read_ch(wchs[i + 8]))  << 8;
						buf |= (read_ch(wchs[i + 10])) << 4;
						buf |= (read_ch(wchs[i + 11]));

						i += 12;
					}
					else if ((mask & 0xf0) == 0xf0)
					{
						buf  = (read_ch(wchs[i + 1])) << 20;
						buf |= (read_ch(wchs[i + 2])) << 16;
						buf |= (read_ch(wchs[i + 4])) << 12;
						buf |= (read_ch(wchs[i + 5])) << 8;
						buf |= (read_ch(wchs[i + 7])) << 4;
						buf |= (read_ch(wchs[i + 8]));

						i += 9;
					}
					else if ((mask & 0xe0) == 0xe0) // long
					{
						buf  = (read_ch(wchs[i + 1])) << 12;
						buf |= (read_ch(wchs[i + 2])) << 8;
						buf |= (read_ch(wchs[i + 4])) << 4;
						buf |= (read_ch(wchs[i + 5]));
						
						i += 6;
					}
					else if ((mask & 0xc0) == 0xc0) // mb
					{
						buf  = (read_ch(wchs[i + 1])) << 4;
						buf |= (read_ch(wchs[i + 2]));
						
						i += 3;
					}
					else	// ansi
					{
						buf = mask;
					}

					wb.Append(Encoding::FromUtf8Ch_nomask(buf, mask));
				}
				else
				{
					if (wchs[i] == L'+')
						wb.Append(L' ');
					else
						wb.Append(wchs[i]);
					i++;
				}
			}

			return wb.ToString();
		}
		
		static WString Decode(const WString& refer)
		{
			return Decode(refer.Reference(), refer.Length());
		}

		static WString Decode(const wchar_t *wchs)
		{
			return Decode(wchs, wcslen(wchs));
		}

		static WString Decode(const char *chs)
		{
			return Decode(WString(chs));
		}
		
	};

}

#endif