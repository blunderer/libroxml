/** \file roxml.h
 *  \brief header for libroxml.so
 *         
 * This is the header file used to develop some
 * softwares using the libroxml.so library.
 * \author blunderer <blunderer@blunderer.org>
 * \date 23 Dec 2008
 *
 * Copyright (C) 2009 blunderer
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

/** \mainpage libroxml homepage
 *
 * \section intro_sec Introduction
 * This library is minimum, easy-to-use, C implementation for xml file parsing
 * It includes a mini shell to navigate thru a xml file as a demo.
 *
 * \section why_sec Why libroxml?
 * Because XML parsing is always hard to reinvent, and because very often xml lib are 
 * too big to fit with very little application
 *
 * \section what_sec What can do libroxml?
 * It allow you to easily:
 * - load / unload document
 * - navigate thru an xml tree
 * - read attributes and attributes' values for nodes
 * - get content of nodes
 *
 * \section how_sec How does it work?
 * \ref roxml.h
 */

#ifndef ROXML_H
#define ROXML_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

/**
 * \def ROXML_API
 * 
 * part of the public API
 */
#define ROXML_API

#ifndef ROXML_INT
/** \struct node_t
 *
 * \brief node_t structure
 * 
 * This is the structure for a node. This struct is very
 * little as it only contains offset for node in file and
 * tree links
 */
typedef struct node node_t;
#endif

/**
 * \def ROXML_ATTR_NODE
 * 
 * constant for attribute nodes
 */
#define ROXML_ATTR_NODE	0x04

/**
 * \def ROXML_STD_NODE
 * 
 * constant for standard nodes
 */
#define ROXML_STD_NODE	0x08

/**
 * \def ROXML_TXT_NODE
 * 
 * constant for text nodes
 */
#define ROXML_TXT_NODE	0x10

/**
 * \def ROXML_CMT_NODE
 * 
 * constant for comment nodes
 */
#define ROXML_TXT_NODE	0x20

/**
 * \def ROXML_PROC_NODE
 * 
 * constant forprocessing_intruction nodes
 */
#define ROXML_TXT_NODE	0x40

/**
 * \def RELEASE_ALL
 * 
 * release all allocated memory
 */
#define RELEASE_ALL	(void*)-1

/**
 * \def RELEASE_LAST
 * 
 * release last allocated memory
 */
#define RELEASE_LAST	(void*)-2

/** \brief load function for buffers
 *
 * \fn node_t* ROXML_API roxml_load_buf(char *buffer);
 * This function load a document and all the corresponding nodes
 * \param buffer the XML buffer to load
 * \return the root node or NULL
 * \see roxml_close
 * \see roxml_load_doc
 * \see roxml_load
 */
node_t*	ROXML_API roxml_load_buf		(char *buffer);

/** \brief load function for files
 *
 * \fn node_t* ROXML_API roxml_load_doc(char *filename);
 * This function load a document and all the corresponding nodes
 * \param filename the XML document to load
 * \return the root node or NULL
 * \see roxml_close
 * \see roxml_load_buf
 * \see roxml_load
 */
node_t*	ROXML_API roxml_load_doc		(char *filename);

/** \brief unload function
 *
 * \fn void ROXML_API roxml_close(node_t *n);
 * This function clear a document and all the corresponding nodes
 * \param n is one node of the tree
 * \return void
 * \see roxml_load_doc
 */
void 	ROXML_API roxml_close			(node_t *n);

/** \brief parent getter function
 *
 * \fn node_t* ROXML_API roxml_get_parent(node_t *n);
 * This function returns the parent of a given node
 * \param n is one node of the tree
 * \return the node parent
 */
node_t*	ROXML_API roxml_get_parent		(node_t *n);

/** \brief chld getter function
 *
 * \fn node_t* ROXML_API roxml_get_chld(node_t *n, char * name, int nb);
 * This function returns a given chld of a node
 * \param n is one node of the tree
 * \param name is the name of the child to get
 * \param nb is the id of the chld to get
 * \return the chld corresponding to name or id (if both are set, name is used)
 * \see roxml_get_chld_nb
 */
node_t*	ROXML_API roxml_get_chld		(node_t *n, char * name, int nb);

/** \brief chlds number getter function
 *
 * \fn int ROXML_API roxml_get_chld_nb(node_t *n);
 * This function return the number of chlds for a given node
 * \param n is one node of the tree
 * \return  the number of chlds
 */
int 	ROXML_API roxml_get_chld_nb		(node_t *n);

/** \brief name getter function
 *
 * \fn char* ROXML_API roxml_get_name(node_t *n, char * name, int size);
 * This function return the name of the node or fill the current buffer with it if not NULL.
 * if name is NULL, the function will allocated a buffer that user should 
 * roxml_release when no longer needed.
 * Be carreful as if your buffer is too short for the returned string, it will be truncated
 * \param n is one node of the tree
 * \param name a buffer pointer or NULL if has to be auto allocated
 * \param size the size of buffer pointed by name if not NULL
 * \return the name of the node (return our buffer pointer if it wasn't NULL)
 * \nsee roxml_parse_node
 */
char*	ROXML_API roxml_get_name		(node_t *n, char * name, int size);

/** \brief content getter function
 *
 * \fn char * ROXML_API roxml_get_content(node_t *n, char * name, int buf_size, int * size);
 *
 * This function returns a pointer with text content of a node (chld are NOT included as text).;
 * if the returned pointer is NULL then the node was empty.
 * returned string should be roxml_release when not used anymore
 * \param n is one node of the tree
 * \param name is the buffer where result will be written or NULL for internal allocation
 * \param buf_size the size if the name buffer
 * \param size the actual size of result. if name was not NULL and size == buf_size, then maybe given buffer was too small
 * \return the content
 */
char *	ROXML_API roxml_get_content		(node_t *n, char * name, int buf_size, int * size);

/** \brief number of attribute getter function
 *
 * \fn int ROXML_API roxml_get_attr_nb(node_t *n);
 * 
 * This function returns the number of attributes for a given node
 * \param n is one node of the tree
 * \return the number of attributes in node
 */
int	ROXML_API roxml_get_attr_nb		(node_t *n);

/** \brief attribute getter function
 *
 * \fn char* ROXML_API roxml_get_attr(node_t *n, char * name, int nb);
 * This function get the nth attribute of a node.
 * User should roxml_release the returned buffer when no longer needed.
 * \param n is one node of the tree
 * \param name is the name of the child to get
 * \param nb the id of attribute to read
 * \return the attribute corresponding to name or id (if both are set, name is used)
 */
node_t*	ROXML_API roxml_get_attr		(node_t *n, char * name, int nb);

/** \brief exec path function
 *
 * \fn roxml_xpath(node_t *n, char * path, int *nb_ans);
 * This function return a node corresponding to a given path.
 * path syntax is : 
 * if path begin with  a "/" it is an absolute path relative to root
 * else it is a path relative to given node
 * handled XPath are : "/", "..", "@<attr name>", "<node name>[table idx]..."
 * resulting node table should be roxml_release when not used anymore
 * \param n is one node of the tree path can be relative to this or absolute
 * \param path the path to resolv
 * \param nb_ans the number of results
 * \return the node table or NULL 
 */
node_t ** ROXML_API roxml_xpath			(node_t *n, char * path, int *nb_ans);

/** \brief node type function
 *
 * \fn roxml_get_type(node_t *n);
 * This function tells if a node is a arg or real node.
 * \param n is the node to test
 * \return the node type
 */
int ROXML_API roxml_get_type			(node_t *n);

/** \brief node get index function
 *
 * \fn roxml_get_node_index(node_t *n, int * last);
 * This function tells the index of a node between all its homonyns.
 * \param n is the node to test
 * \param last is the index of last homonym
 * \return the idx or -1 if only one node
 */
int ROXML_API roxml_get_node_index		(node_t *n, int * last);

/** \brief memory cleanning function
 *
 * \fn roxml_release(void * data);
 * This function release the memory pointed by pointer
 * just like free would but for memory allocated with roxml_malloc. 
 * Freeing a NULL pointer won't do
 * anything. roxml_release also allow you to remove all 
 * previously allocated datas by using RELEASE_ALL as argument.
 * You can also safely use RELEASE_LAST argument that will release the 
 * previously allocated var within the current thread (making this
 * function thread safe).
 * if using roxml_release on a variable non roxml_mallocated, nothing will happen
 * \param data the pointer to delete or NULL or RELEASE_ALL or RELEASE_LAST
 * \return void
 */
void ROXML_API roxml_release			(void * data);

/** \brief node type getter function
 *
 * \fn roxml_get_type(node_t *n);
 * this function return the node type : ROXML_ARG ROXML_VAL
 * \param n the node to return type for
 * \return the node type
 */
int roxml_get_type				(node_t *n);

/** \brief add a node to the tree
 *
 * \fn roxml_add_node(node_t * parent, int type, char *name, char *value);
 * this function add a new node to the tree. This will not update de buffer or file,
 * only the RAM loaded tree
 * \param parent the parent node
 * \param type the type of node between ROXML_ATTR_NODE, ROXML_STD_NODE, ROXML_TXT_NODE
 * \param name the name of the node (for ROXML_ATTR_NODE and ROXML_STD_NODE only)
 * \param value the text content (for ROXML_STD_NODE, ROXML_TXT_NODE) or the attribute value (ROXML_ATTR_NODE)
 * \return the new node
 * \see roxml_commit_changes
 * \see roxml_del_node
 */
node_t * ROXML_API roxml_add_node		(node_t * parent, int type, char *name, char *value);

/** \brief text content getter function
 *
 * \fn roxml_get_text(node_t *n, int nb);
 * this function return the text content of a node as a TEXT node
 * \param n the node that contains text
 * \param nb the nth text node to retrieve
 * \return the text node or NULL
 * \see roxml_get_text_nb
 */
node_t * ROXML_API roxml_get_text		(node_t *n, int nb);

/** \brief text node number getter function
 *
 * \fn roxml_get_text_nb(node_t *n);
 * this function return the number of text nodes in 
 * a standard node
 * \param n the node to search into
 * \return the number of text node
 * \see roxml_get_text
 */
int ROXML_API roxml_get_text_nb			(node_t *n);

/** \brief node deletion function
 *
 * \fn roxml_del_node(node_t * n);
 * this function delete a node from the tree. The node is not really deleted 
 * from the file or buffer until the roxml_commit_changes is called.
 * \param n the node to delete
 * \return 
 */
void ROXML_API roxml_del_node			(node_t * n);

/** \brief sync function
 *
 * \fn roxml_commit_changes(node_t *n, char * dest, char ** buffer, int human);
 * this function sync changes to the given buffer or file in human or one-line format 
 * \param n the root node of the tree to write
 * \param dest the path to a file to write tree to
 * \param buffer the adress of a buffer where tre will be written
 * \param human 0 for one-line tree, or 1 for human format (using tabs, newlines...)
 * \return 
 */
void ROXML_API roxml_commit_changes		(node_t *n, char * dest, char ** buffer, int human);


#endif /* ROXML_H */

