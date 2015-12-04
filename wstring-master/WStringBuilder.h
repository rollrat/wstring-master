/***

   RollRat Software Project.
   Copyright (C) 2015. rollrat. All Rights Reserved.

File name:
   
   WStringBuilder.h

Purpose:

	RollRat Library

Author:

   10-07-2015:   HyunJun Jeong, Creation

***/

#ifndef _WSTRING_BUILDER_9bf1541fdf7efd41b7b39543fd870ac4_
#define _WSTRING_BUILDER_9bf1541fdf7efd41b7b39543fd870ac4_

#include <memory.h>

#include "WString.h"

namespace Utility {
	
	//
	//	���̿� ������� �� �Ϸ��� ���ڿ� ������ �ſ� ������ �����ϴ� �˰����Դϴ�.
	//
	class WStringBuilder
	{
		size_t          capacity = 1024;

		typedef struct _WStringBuilderNode
		{
			size_t                      m_offset;
			size_t                      m_length;
			wchar_t                    *m_ptr;
			struct _WStringBuilderNode *m_next;
		} WStringBuilderNode;

		WStringBuilderNode *m_last;
		WStringBuilderNode *m_head;

	public:

		WStringBuilder()
		{
			___Init();
		}

		WStringBuilder(size_t capacity)
			: WStringBuilder()
		{
			this->capacity = capacity;
		}

		WStringBuilder(WString& refer)
			: WStringBuilder()
		{
			Append(refer);
		}

		WStringBuilder(WString& refer, size_t capacity)
			: WStringBuilder(capacity)
		{
			Append(refer);
		}

		~WStringBuilder()
		{
			___Dispose();
		}

		void Append(const WString& refer)
		{
			if (!refer.Empty())
			{
				if (capacity > refer.Length() + m_last->m_length)
				{
					___Ensure();

					memcpy( m_last->m_ptr +  m_last->m_length, refer.Reference(), refer.Length() * sizeof(wchar_t) );

					 m_last->m_length += refer.Length();
				}
				else
				{
					___Expand();

					if (capacity <= refer.Length())
					{
						 m_last->m_ptr = (wchar_t *)refer.ToArray();
						 m_last->m_length = refer.Length();
					}
					else
					{
						Append(refer);
						return;
					}

					___LinkTo();
				}
			}
		}

		void Append(wchar_t ch)
		{
			___Ensure();

			if ( m_last->m_length == capacity - 1)
				___Expand();

			 m_last->m_ptr[ m_last->m_length++] = ch;
		}

		template<typename wt_over>
		void Append(wt_over over)
		{
			Append(WString(over));
		}
		
		void Append(const wchar_t *str, size_t len)
		{
			if (len > 0)
			{
				if (capacity > len +  m_last->m_length)
				{
					___Ensure();

					memcpy( m_last->m_ptr +  m_last->m_length, str, len * sizeof(wchar_t) );

					m_last->m_length += len;
				}
				else
				{
					___Expand();

					m_last->m_length = len;
					m_last->m_ptr = new wchar_t[len];
					memcpy( m_last->m_ptr, str, len * sizeof(wchar_t) );

					___LinkTo();
				}
			}
		}

		void Append(const wchar_t *str)
		{
			size_t len = wcslen(str);
			Append(str, len);
		}
		
		size_t Length() const
		{
			return m_last->m_offset + m_last->m_length;
		}

		WString ToString()
		{
			size_t len = m_last->m_offset + m_last->m_length;
			wchar_t *newString = new wchar_t[len + 1];
			
			WStringBuilderNode *iter = m_head;
			for ( ; iter != nullptr; iter = iter->m_next )
			{
				memcpy(newString + iter->m_offset, iter->m_ptr, iter->m_length * sizeof(wchar_t));
			}
			newString[len] = 0;

			StringReferencePutWith with;
			return WString(newString, len, with);
		}

		void Dispose()
		{
			___Dispose();
			___Init();
		}

	private:

		WStringBuilderNode *___Create()
		{
			WStringBuilderNode *wsbn = new WStringBuilderNode;
			wsbn->m_length = 0;
			wsbn->m_ptr = nullptr;
			wsbn->m_next = nullptr;
			return wsbn;
		}

		void ___Dispose()
		{
			WStringBuilderNode *iter = m_head;
			WStringBuilderNode *prev = nullptr;

			for ( ; iter != nullptr;  )
			{
				delete[] iter->m_ptr;
				prev = iter;
				iter = iter->m_next;
				delete prev;
			}

			m_head = m_last = nullptr;
		}

		void ___Init()
		{
			m_last = m_head = ___Create();
			m_last->m_offset = 0;
		}

		void ___Ensure()
		{
			if (m_last->m_ptr == nullptr)
			{
				m_last->m_ptr = new wchar_t[capacity];
			}
		}

		bool ___Expand()
		{
			if (m_last->m_length > 0)
			{
				___LinkTo();
				return true;
			}
			return false;
		}

		void ___LinkTo()
		{
			WStringBuilderNode *twsbn = ___Create();
			twsbn->m_offset = m_last->m_length + m_last->m_offset;
			m_last->m_next = twsbn;
			m_last = twsbn;
		}

	};
}

#endif