/***

   RollRat Software Project.
   Copyright (C) 2015. rollrat. All Rights Reserved.

File name:
   
   Array.h

Purpose:

	RollRat Library

Author:

   10-18-2015:   HyunJun Jeong, Creation

***/

#ifndef _ARRAY_9bf1541fdf7efd41b7b39543fd870ac4_
#define _ARRAY_9bf1541fdf7efd41b7b39543fd870ac4_

namespace Utility {
	
	//
	//	이 클래스는 라이브러리 내부에서만 사용되어야 합니다.
	//	외부에선 vector 등을 이용하시기 바랍니다.
	//
	template<typename type>
	class ArrayBase
	{
	public:

		ArrayBase()
			: m_size(0)
			, m_pointer(nullptr)
			, m_constptr(nullptr)
		{
		}

		ArrayBase(size_t size, type *pointer)
			: m_size(size)
			, m_pointer(pointer)
			, m_constptr(pointer)
		{
		}
		
		virtual ~ArrayBase()
		{
			if (!m_nodel)
			{
			_class_delete(*m_constptr, m_constptr);
			delete[] m_constptr;
			}
		}

		template<typename type>
		void _class_delete(type&, type*&)
		{
		}
		template<typename type>
		void _class_delete(type*&, type* const*& arr)
		{
			for (size_t i = 0; i < m_size; i++)
			{
				arr[i].~type();
				delete arr[i];
			}
		}

	protected:

		bool   m_nodel = false;
		size_t m_size;
		type  *m_pointer;
		type  *m_constptr;

	};

	// Read Only Array [ const array iterator ]
	template<typename type>
	class ReadOnlyArray
		: public ArrayBase<type>
	{
		typedef ReadOnlyArray<type> this_type;

	public:

		ReadOnlyArray(type *ptr, size_t size)
			: ArrayBase<type>(size, ptr)
		{
		}

		ReadOnlyArray()
			: ArrayBase<type>()
		{
		}

		~ReadOnlyArray()
		{
		}

		type operator*() const
		{
			return *this->m_pointer;
		}
		
		type* operator->() const
		{
			return **this;
		}

		type operator[](size_t off) const
		{
			return *(*this + off);
		}

		this_type& operator--()
		{
			--this->m_pointer;
			return *this;
		}

		this_type operator--(int)
		{
			this_type tmp = *this;
			tmp.m_nodel = true;
			--this->m_pointer;
			return tmp;
		}

		this_type& operator++()
		{
			++this->m_pointer;
			return *this;
		}

		this_type operator++(int)
		{
			this_type tmp = *this;
			tmp.m_nodel = true;
			++*this->m_pointer;
			return tmp;
		}

		this_type& operator+=(size_t size)
		{
			this->m_pointer += size;
			return *this;
		}

		this_type operator+(size_t size) const
		{
			this_type tmp = *this;
			tmp.m_nodel = true;
			return (tmp += size);
		}

		this_type& operator-=(size_t size)
		{
			this->m_pointer -= size;
			return *this;
		}

		this_type operator-(size_t size) const
		{
			this_type tmp = *this;
			tmp.m_nodel = true;
			return (tmp -= size);
		}

		bool operator==(const this_type& refer) const
		{
			return (this->m_pointer == refer.m_pointer);
		}

		bool operator!=(const this_type& refer) const
		{
			return (!(*this == refer));
		}

		bool operator<(const this_type& refer) const
		{
			return (this->m_pointer < refer.m_pointer);
		}

		bool operator>(const this_type& refer) const
		{
			return (refer < *this);
		}

		bool operator<=(const this_type& refer) const
		{
			return (!(refer < *this));
		}

		bool operator>=(const this_type& refer) const
		{
			return (!(*this < refer));
		}

		void Reset()
		{
			this->m_pointer = this->m_constptr;
		}

		// 배열 포인터 참조
		type* Array() const
		{
			return this->m_constptr;
		}

		size_t Size() const
		{
			return this->m_size;
		}

		// For Each <Item> In Type _ etc...
		template<typename func>
		void Each(func& function)
		{
			for ( size_t i = 0; i < this->m_size; i++ )
				function(this->m_constptr[i]);
		}

	};

}

#endif