/***

   RollRat Software Project.
   Copyright (C) 2015. rollrat. All Rights Reserved.

File name:
   
   WStringSpecific.cpp

Purpose:

	RollRat Library

Author:

   10-25-2015:   HyunJun Jeong, Creation

***/

#include "WString.h"

namespace Utility {
	
	std::wostream& operator<<(std::wostream& os, const WString& refer)
	{
		os << refer.Reference();
		return os;
	}

}