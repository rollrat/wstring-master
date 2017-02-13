/***

   RollRat Software Project.
   Copyright (C) 2015-2017. rollrat. All Rights Reserved.

File name:
   
   WStringBuffer.h

Purpose:

   RollRat Framework

Author:

   11-13-2015:   HyunJun Jeong, Creation

***/

#ifndef _WSTRING_BUFFER_9bf1541fdf7efd41b7b39543fd870ac4_
#define _WSTRING_BUFFER_9bf1541fdf7efd41b7b39543fd870ac4_

#include <memory.h>

#include "WString.h"

namespace Utility {
	
	class WStringBuffer
	{
		wchar_t* m_ptr;
		size_t   m_length;
		size_t   m_capacity;

	public:

		WStringBuffer()
			: m_ptr(nullptr)
			, m_length(0)
			, m_capacity(0)
		{
		}

		WStringBuffer(size_t capacity)
			: m_ptr(nullptr)
			, m_length(0)
			, m_capacity(capacity)
		{
			Ensure(m_capacity);
		}

		~WStringBuffer()
		{
			m_ptr != nullptr ? delete[] m_ptr : 0;
		}

		void Append(const WString& refer)
		{
			Append(refer.Reference(), refer.Length());
		}

		void Append(const wchar_t *wstr)
		{
			Append(wstr, wcslen(wstr));
		}
		
		void Append(wchar_t ch)
		{
			if ( m_length == m_capacity )
				EnsureCopy(m_length + 1);

			 m_ptr[ m_length++ ] = ch;
		}

		template<typename wt_over>
		void Append(wt_over over)
		{
			Append(WString(over));
		}

		size_t Length()
		{
			return m_length;
		}
		
		WString ToString()
		{
			return WString((const wchar_t *)m_ptr, m_length);
		}

		void EnsureMore(size_t size)
		{
			EnsureCopy(size + m_capacity);
		}

	private:
		
		void Append(const wchar_t *wstr, size_t len)
		{
			if ( m_length + len > m_capacity )
			{
				EnsureCopy(m_length + len);
			}
			
			memcpy(m_ptr + m_length, wstr, len * sizeof(wchar_t));
			m_length += len;
		}

		void Ensure(size_t size)
		{
			m_ptr = new wchar_t[size];
			m_capacity = size;
		}

		void EnsureCopy(size_t size)
		{
			if (size > m_capacity)
			{
				// realloc?
				wchar_t *tmp = m_ptr;
				Ensure(size);
				memcpy(m_ptr, tmp, m_length * sizeof(wchar_t));
				delete[] tmp;
				//realloc(m_ptr, size * sizeof(wchar_t));
			}
		}

	};
}

#endif