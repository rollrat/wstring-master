/***

   RollRat Software Project.
   Copyright (C) 2015. rollrat. All Rights Reserved.

File name:
   
   Encoding.h

Purpose:

	RollRat Library

Author:

   10-10-2015:   HyunJun Jeong, Creation

***/

#ifndef _ENCODING_9bf1541fdf7efd41b7b39543fd870ac4_
#define _ENCODING_9bf1541fdf7efd41b7b39543fd870ac4_

#include <codecvt>
#include <algorithm>

#include "WString.h"

// ���Ͽ�
#define utf8_bom "\xef\xbb\xbf"
#define unicode_bom "\xff\xfe"

namespace Utility {

	//
	//	unicode�� utf-8�̳� ansi���� multibyte�� 
	//	�ٲ� �� ����� �� �ִ�, API�� �����Դϴ�.
	//
	class Encoding
	{
	public:

		// ��ȯ ��Ģ
		// ANSI : 0xxx xxxx
		// 2ch  : 110x xxxx 10xx xxxx 
		// 3ch  : 1110 xxxx 10xx xxxx * 2
		// 4ch  : 1111 0xxx 10xx xxxx * 3
		// ��������
		// 5ch  : 1111 10xx 10xx xxxx * 4 (wchar_t ���� x, 3 bytes)
		// 6ch  : 1111 110x 10xx xxxx * 5 (wchar_t ���� x, 3 bytes)

		// ANSI�� 7bit
		// 2ch   11bit
		// 3ch   16bit
		// 4ch   21bit
		// 5ch   26bit
		// 6ch   31bit

		// wchar_t�� utf8�������� ��ȯ��
		// ��, �ִ� 4 byte���ڸ� ������
		static unsigned long ToUtf8Ch(wchar_t ch)
		{
			if (ch > 0x7F)
			{
				int ret = (ch & 0x3F) |
					((ch << 2) & 0x00003F00);
				if (ch < 0x800)
					return ret | 0x0000C080;
				ret |= ((ch << 4) & 0x003F0000);
				if (ch < 0x10000)
					return ret | 0x00E08080;
				return ret | ((ch << 6) & 0x3F000000) | 0xF0808080;
			}
			return ch;
		}

		// �ִ� 4 bytes ���ڿ��� ��ȯ
		// mask�� ������ �ֻ��� 1 byte
		// low�� mask�� ������ ������ ����Ʈ, �Ǵ� ��ü ����Ʈ
		static wchar_t FromUtf8Ch_nomask(unsigned long long low, unsigned long mask)
		{
			wchar_t wt;

			if ((mask & 0xfc) == 0xfc)
			{
				// 6bytes not exist
				// in wchar_t
			}
			else if ((mask & 0xf8) == 0xf8)
			{
				// 5bytes not exist
				// in wchar_t
			}
			else if ((mask & 0xf0) == 0xf0)
			{
				wt = ((mask & 0xf) << 18) |
					(low & 0x3f) | ((low & 0x3f00) >> 2) | ((low & 0x3f0000) >> 10);
			}
			else if ((mask & 0xe0) == 0xe0)
			{
				wt = ((mask & 0x1f) << 12) |
					(low & 0x3f) | ((low & 0x3f00) >> 2); // >> 8 + << 6 = >> 2
			}
			else if ((mask & 0xc0) == 0xc0)
			{
				wt = (mask & 0x3f >> 6) | (low & 0x3f);
			}
			else
			{
				wt = (wchar_t)low;
			}

			return wt;
		}

		static WString AnsiToUnicode(const char *ansi, size_t len)
		{
			wchar_t *ret = new wchar_t[len + 1];
			wchar_t *ptr = ret;
			size_t rlen = len;
			while (rlen--)
				*ptr++ = (wchar_t)*ansi++;
			*ptr = 0;
			StringReferencePutWith with;
			return WString(ret, len, with);
		}

	};

}

#endif