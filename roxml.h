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

#define ROXML_API

#ifndef ROXML_PRIVATE
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
void 	ROXML_API roxml_close		(node_t *n);

/** \brief parent getter function
 *
 * \fn node_t* ROXML_API roxml_get_parent(node_t *n);
 * This function returns the parent of a given node
 * \param n is one node of the tree
 * \return the node parent
 */
node_t*	ROXML_API roxml_get_parent		(node_t *n);

/** \brief son getter function
 *
 * \fn node_t* ROXML_API roxml_get_son_nth(node_t *n, int nb);
 * This function returns a given son of a node
 * \param n is one node of the tree
 * \param nb is the id of the son to get
 * \return the nth son
 * \see roxml_get_son_nb
 */
node_t*	ROXML_API roxml_get_son_nth		(node_t *n, int nb);

/** \brief sons number getter function
 *
 * \fn int ROXML_API roxml_get_son_nb(node_t *n);
 * This function return the number of sons for a given node
 * \param n is one node of the tree
 * \return  the number of sons
 * \see roxml_get_son_nth
 */
int 	ROXML_API roxml_get_son_nb		(node_t *n);

/** \brief name getter function
 *
 * \fn char* ROXML_API roxml_get_name(node_t *n);
 * This function return the name of the node
 * User should roxml_release the returned buffer when no longer needed.
 * \param n is one node of the tree
 * \return the name of the node
 * \nsee roxml_parse_node
 */
char*	ROXML_API roxml_get_name		(node_t *n);

/** \brief content getter function
 *
 * \fn char * ROXML_API roxml_get_raw_content(node_t *n);
 *
 * This function returns a pointer with raw content of a node (son are included as text).;
 * if the returned pointer is NULL then the node was empty.
 * returned string should be roxml_release when not used anymore
 * \param n is one node of the tree
 * \return the content
 */
char *	ROXML_API roxml_get_raw_content		(node_t *n);

/** \brief content getter function
 *
 * \fn char * ROXML_API roxml_get_content(node_t *n);
 *
 * This function returns a pointer with text content of a node (son are NOT included as text).;
 * if the returned pointer is NULL then the node was empty.
 * returned string should be roxml_release when not used anymore
 * \param n is one node of the tree
 * \return the content
 */
char *	ROXML_API roxml_get_content		(node_t *n);

/** \brief number of attribute getter function
 *
 * \fn int ROXML_API roxml_get_attr_nb(node_t *n);
 * 
 * \param n is one node of the tree
 * \return the number of attributes in node
 * \see roxml_get_attr_val_nth
 * \see roxml_get_attr_nth
 * \see roxml_parse_node
 */
int	ROXML_API roxml_get_attr_nb		(node_t *n);

/** \brief attribute getter function
 *
 * \fn char* ROXML_API roxml_get_attr_nth(node_t *n, int nb);
 * This function get the nth attribute of a node.
 * User should roxml_release the returned buffer when no longer needed.
 * \param n is one node of the tree
 * \param nb the id of attribute to read
 * \return the attribute
 * \see roxml_get_nb_attr
 * \see roxml_get_attr_val_nth
 * \see roxml_parse_node
 */
char*	ROXML_API roxml_get_attr_nth		(node_t *n, int nb);

/** \brief attribute value getter function
 *
 * \fn char* ROXML_API roxml_get_attr_val_nth(node_t *n, int nb);
 * This function get the nth attribute value of a node.
 * User should roxml_release the returned buffer when no longer needed.
 * \param n is one node of the tree
 * \param nb the id of attribute value to read
 * \return the attribute value
 * \see roxml_get_attr_nth
 * \see roxml_get_nb_attr
 * \see roxml_parse_node
 */
char*	ROXML_API roxml_get_attr_val_nth	(node_t *n, int nb);

/** \brief exec path function
 *
 * \fn roxml_exec_path(node_t *n, char * path, int *nb_ans);
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
node_t ** ROXML_API roxml_exec_path(node_t *n, char * path, int *nb_ans);

/** \brief node type function
 *
 * \fn roxml_is_arg(node_t *n);
 * This function tells if a node is a arg or real node.
 * \param n is the node to test
 * \return 1 if node is arg else return 0
 */
int ROXML_API roxml_is_arg(node_t *n);

/** \brief node get index function
 *
 * \fn roxml_get_node_index(node_t *n, int * last);
 * This function tells the index of a node between all its homonyns.
 * \param n is the node to test
 * \param last is the index of last homonym
 * \return the idx or -1 if only one node
 */
int ROXML_API roxml_get_node_index(node_t *n, int * last);

/** \brief memory cleanning function
 *
 * \fn roxml_release(void * data);
 * This function release the memory pointed by pointer
 * just like free would. Freeing a NULL pointer won't do
 * anything. roxml_release also allow you to remove all 
 * previously allocated datas by using RELEASE_ALL as argument.
 * You can also safely use RELEASE_LAST argument that will release the 
 * previously allocated var within the current thread (making this
 * function thread safe).
 * \param data the pointer to delete or NULL or RELEASE_ALL or RELEASE_LAST
 * \return void
 */
void ROXML_API roxml_release(void * data);

void roxml_commit_changes(node_t *n);

#endif /* ROXML_H */

