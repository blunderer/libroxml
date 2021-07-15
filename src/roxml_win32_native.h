/**
 * \file roxml_win32_native.h
 * \brief special definitions for windows native build
 *
 * (C) Copyright 2014
 * Tristan Lelong <tristan.lelong@libroxml.net>
 *
 * SPDX-Licence-Identifier:     LGPL-2.1+
 * The author added a static linking exception, see License.txt.
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

#define pthread_self()                 ((unsigned long int) GetCurrentThread())
#define pthread_mutex_init(a, b)       ({int ret = 0; InitializeCriticalSection(a); ret;})
#define pthread_mutex_lock(a)          ({int ret = 0; EnterCriticalSection(a); ret;})
#define pthread_mutex_unlock(a)        ({int ret = 0; LeaveCriticalSection(a); ret;})
#define pthread_mutex_destroy(a)       ({int ret = 0; DeleteCriticalSection(a); ret; })

#ifndef ENODATA
#define ENODATA 61
#endif

#endif /* ROXML_WIN32_NATIVE_THREAD_H */
