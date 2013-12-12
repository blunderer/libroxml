/** \file roxml-tune.h
 *  \brief header for libroxml.so
 *         
 * This is the header file used to tune libroxml.
 * It contains the default options taht should fit
 * all standard systems.
 * Change this file only if you *really* know what you are doing.
 * \author blunderer <blunderer@blunderer.org>
 * \date 11 May 2012
 *
 * Copyright (C) 2012 blunderer
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * The author added a static linking exception, see License.txt.
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

/** \page roxml-tune tuning libroxml
 *
 * Being dedicated to embedded systems, libroxml include the ability to tune its behaviour.
 * files \file roxml-tune.h, \file roxml-tiny.h and \file site.in are used for that purpose.
 * 
 * \section tune <roxml-tune>
 * This file describe all options that can be used. Each consist of a #define CONFIG_XML_xxx set to
 * either 0 or 1. This file describes default values and should not be modified but to had options. 
 *
 * This file also implements some tuning options. We try not to use #defines all over the code except 
 * to activate or remove some portion. Instead, we do define macros.
 *
 * \section tune_end </roxml-tune>
 *
 * \section mk <site.mk>
 * This file is actually not handled in the source control system. The sample site.in is provided. Creating the site.mk
 * file in the libroxml root directory will automagically turn the USER DEFINED CONF behaviour.
 * Users should activate or remove options using this file.
 * All options are describer into file \file roxml-tune.h
 *
 * \section mk_end </roxml.mk>
 *
 */

#ifndef ROXML_TUNE_H
#define ROXML_TUNE_H

#ifdef ROXML_DEF_H
#error "roxml-defines.h must be included after roxml-tune.h to be tuned."
#endif

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

/*************** Define the default behaviour **********************/

/* Is the target library thread safe or not
 * This option should be activated.
 * If unsure, set to 1
 */
#ifndef CONFIG_XML_THREAD_SAFE
#if(CONFIG_XML_HEAP_BUFFERS==1)
#error "The CONFIG_XML_THREAD_SAFE option is not compatible with CONFIG_XML_HEAP_BUFFERS. Disable one of those."
#endif
#define CONFIG_XML_THREAD_SAFE	1
#endif /* CONFIG_XML_THREAD_SAFE */

/* Is the target library uses stack for internal
 * buffers or heap. Stack require some RAM while heap
 * can cut off performances.
 * This option should not be activated.
 * If unsure, set to 0
 */
#ifndef CONFIG_XML_HEAP_BUFFERS
#define CONFIG_XML_HEAP_BUFFERS	0
#endif /* CONFIG_XML_HEAP_BUFFERS */

/* Define the size of internal buffer.
 * For very small systems, large internal buffers
 * can cause the systeml the behave strangely.
 * This option should not be activated.
 * If unsure, set to 0
 */
#ifndef CONFIG_XML_SMALL_BUFFER
#define CONFIG_XML_SMALL_BUFFER	0
#endif /* CONFIG_XML_SMALL_BUFFER */

/* Limit the size of input XML libroxml can handle
 * to 64kb instead of 4Gb.
 * Setting this option will reduce the size of RAM loaded tree
 * This option should not be activated.
 * If unsure, set to 0
 */
#ifndef CONFIG_XML_SMALL_INPUT_FILE
#define CONFIG_XML_SMALL_INPUT_FILE	0
#endif /* CONFIG_XML_SMALL_INPUT_FILE */

/* Can the target library modify XML tree
 * When set to 0, this result in smaller binary.
 * (this saves about 3kb)
 * This option should be activated.
 * If unsure, set to 1
 */
#ifndef CONFIG_XML_READ_WRITE
#define CONFIG_XML_READ_WRITE		1
#endif /* CONFIG_XML_READ_WRITE */

/* Can the target library commit a tree
 * When set to 0, this result in smaller binary.
 * (this saves about 5kb)
 * This option should be activated.
 * If unsure, set to 1
 */
#ifndef CONFIG_XML_COMMIT_XML_TREE
#define CONFIG_XML_COMMIT_XML_TREE	1
#endif /* CONFIG_XML_COMMIT_XML_TREE */

/* Can the target library perform xpath queries
 * When set to 0, this result in smaller binary.
 * (this saves about 20kb)
 * This option should be activated.
 * If unsure, set to 1
 */
#ifndef CONFIG_XML_XPATH_ENGINE
#define CONFIG_XML_XPATH_ENGINE	1
#endif /* CONFIG_XML_XPATH_ENGINE */

/*************** Implement HEAP/STACK BUFFERS **********************/
#if(CONFIG_XML_HEAP_BUFFERS==1)
	#define ROXML_GET_BASE_BUFFER(name) char *name = malloc(ROXML_BASE_LEN)
	#define ROXML_PUT_BASE_BUFFER(name) free(name)
#else /* CONFIG_XML_HEAP_BUFFERS==0 */
	#define ROXML_GET_BASE_BUFFER(name) char name[ROXML_BASE_LEN]
	#define ROXML_PUT_BASE_BUFFER(name)
#endif /* CONFIG_XML_HEAP_BUFFERS */

/**************** Implement THREAD SAFETY **************************/
#if(CONFIG_XML_THREAD_SAFE==0)
	#define pthread_t			char
	#define pthread_mutex_t			char

	#define pthread_self()			0
	#define pthread_mutex_init(a, b)
	#define pthread_mutex_lock(a)		*(a) = 0
	#define pthread_mutex_unlock(a)		*(a) = 0
	#define pthread_mutex_destroy(a)
#else /* CONFIG_XML_THREAD_SAFE==1 */
	#if defined(_WIN32)
		#include "roxml-win32-native.h"
	#else
		#include <pthread.h>
	#endif
#endif /* CONFIG_XML_THREAD_SAFE */

#endif /* ROXML_TUNE_H */
