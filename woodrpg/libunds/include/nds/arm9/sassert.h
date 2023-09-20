/*---------------------------------------------------------------------------------

	sassert.h -- definitons for DS assertions

	Copyright (C) 2007
		Dave Murphy (WinterMute)
		Jason Rogers (Dovoto)

	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any
	damages arising from the use of this software.

	Permission is granted to anyone to use this software for any
	purpose, including commercial applications, and to alter it and
	redistribute it freely, subject to the following restrictions:

	1.	The origin of this software must not be misrepresented; you
	must not claim that you wrote the original software. If you use
	this software in a product, an acknowledgment in the product
	documentation would be appreciated but is not required.

	2.	Altered source versions must be plainly marked as such, and
	must not be misrepresented as being the original software.

	3.	This notice may not be removed or altered from any source
	distribution.

---------------------------------------------------------------------------------*/
/*! \file sassert.h
	\brief Simple assertion with a message conplies to nop if NDEBUG is defined
*/

#ifndef _sassert_h_
#define _sassert_h_

#ifdef __cplusplus
extern "C" {
#endif

#define sassert(e,s)  	((void)0)

#ifdef __cplusplus
}
#endif

#endif // _sassert_h_
