/** \file roxml-win32-native.h
 *  \brief internal header for libroxml.so
 *         
 * This is the internal header file used by roxml-inernal.h
 * \author blunderer <blunderer@blunderer.org>
 * \date 23 Dec 2008
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef ROXML_WIN32_NATIVE_THREAD_H
#define ROXML_WIN32_NATIVE_THREAD_H

#if defined(_MSC_VER) && _MSC_VER >= 1400
#ifndef _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_DEPRECATE
#endif
#pragma warning(disable: 4996)
#endif

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

typedef HANDLE pthread_t;
typedef CRITICAL_SECTION pthread_mutex_t;

#define pthread_self()			GetCurrentThread()
#define pthread_mutex_init(a, b)	InitializeCriticalSection(a)
#define pthread_mutex_lock(a)		EnterCriticalSection(a)
#define pthread_mutex_unlock(a)		LeaveCriticalSection(a)
#define pthread_mutex_destroy(a)	DeleteCriticalSection(a)

#endif /* ROXML_WIN32_NATIVE_THREAD_H */

