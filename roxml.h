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

#define ROXML_API

#include "roxml-internal.h"


/** \brief load function for buffers
 *
 * \fn node_t* ROXML_API roxml_load_buf(char *buffer);
 * This function load a document and all the corresponding nodes
 * param buffer the XML buffer to load
 * return the root node or NULL
 * see roxml_close
 * see roxml_load_doc
 * see roxml_load
 */
node_t*	ROXML_API roxml_load_buf		(char *buffer);

/** \brief load function for files
 *
 * \fn node_t* ROXML_API roxml_load_doc(char *filename);
 * This function load a document and all the corresponding nodes
 * param filename the XML document to load
 * return the root node or NULL
 * see roxml_close
 * see roxml_load_buf
 * see roxml_load
 */
node_t*	ROXML_API roxml_load_doc		(char *filename);

/** \brief unload function
 *
 * \fn void ROXML_API roxml_close(node_t *n);
 * This function clear a document and all the corresponding nodes
 * param n is one node of the tree
 * return void
 * see roxml_load_doc
 */
void 	ROXML_API roxml_close		(node_t *n);

/** \brief parent getter function
 *
 * \fn node_t* ROXML_API roxml_get_parent(node_t *n);
 * This function returns the parent of a given node
 * param n is one node of the tree
 * return the node parent
 */
node_t*	ROXML_API roxml_get_parent		(node_t *n);

/** \brief son getter function
 *
 * \fn node_t* ROXML_API roxml_get_son_nth(node_t *n, int nb);
 * This function returns a given son of a node
 * param n is one node of the tree
 * param nb is the id of the son to get
 * return the nth son
 * see roxml_get_son_nb
 */
node_t*	ROXML_API roxml_get_son_nth		(node_t *n, int nb);

/** \brief sons number getter function
 *
 * \fn int ROXML_API roxml_get_son_nb(node_t *n);
 * This function return the number of sons for a given node
 * param n is one node of the tree
 * return  the number of sons
 * see roxml_get_son_nth
 */
int 	ROXML_API roxml_get_son_nb		(node_t *n);

/** \brief name getter function
 *
 * \fn char* ROXML_API roxml_get_name(node_t *n);
 * This function return the name of the node
 * User should free the returned buffer when no longer needed.
 * param n is one node of the tree
 * return the name of the node
 * see roxml_parse_node
 */
char*	ROXML_API roxml_get_name		(node_t *n);

/** \brief content getter function
 *
 * \fn int ROXML_API roxml_get_raw_content(node_t *n, char * content);
 * This function fill a pointer with the content of a node including sons as text;
 * if the pointer is NULL then the len is returned (without last \0). libroxml assume that
 * pointer is large enought to write the whole content string
 * param n is one node of the tree
 * param content is a pointer where content will be copied
 * return the len of content
 * see 
 */
int	ROXML_API roxml_get_raw_content		(node_t *n, char * content);

/** \brief content getter function
 *
 * \fn int ROXML_API roxml_get_content(node_t *n, char * content);
 * This function fill a pointer with only the text content of a node 
 * if the pointer is NULL then the len is returned (without last \0). libroxml assume that
 * pointer is large enought to write the whole content string
 * param n is one node of the tree
 * param content is a pointer where content will be copied
 * return the len of content
 * see 
 */
int	ROXML_API roxml_get_content		(node_t *n, char * content);

/** \brief number of attribute getter function
 *
 * \fn int ROXML_API roxml_get_nb_attr(node_t *n);
 * 
 * param n is one node of the tree
 * return the number of attributes in node
 * see roxml_get_attr_val_nth
 * see roxml_get_attr_nth
 * see roxml_parse_node
 */
int	ROXML_API roxml_get_nb_attr		(node_t *n);

/** \brief attribute getter function
 *
 * \fn char* ROXML_API roxml_get_attr_nth(node_t *n, int nb);
 * This function get the nth attribute of a node.
 * User should free the returned buffer when no longer needed.
 * param n is one node of the tree
 * param nb the id of attribute to read
 * return the attribute
 * see roxml_get_nb_attr
 * see roxml_get_attr_val_nth
 * see roxml_parse_node
 */
char*	ROXML_API roxml_get_attr_nth		(node_t *n, int nb);

/** \brief attribute value getter function
 *
 * \fn char* ROXML_API roxml_get_attr_val_nth(node_t *n, int nb);
 * This function get the nth attribute value of a node.
 * User should free the returned buffer when no longer needed.
 * param n is one node of the tree
 * param nb the id of attribute value to read
 * return the attribute value
 * see roxml_get_attr_nth
 * see roxml_get_nb_attr
 * see roxml_parse_node
 */
char*	ROXML_API roxml_get_attr_val_nth	(node_t *n, int nb);

/** \brief exec path function
 *
 * \fn roxml_exec_path(node_t *n, char * path);
 * This function return a node corresponding to a given path.
 * path syntax is : 
 * if path begin with  a "/" it is an absolute path relative to root
 * else it is a path relative to given node
 * param n is one node of the tree path can be relative to this or absolute
 * param path the path to resolv
 * param nb_ans the number of results
 * return the node table or NULL 
 */
node_t ** ROXML_API roxml_exec_path(node_t *n, char * path, int *nb_ans);

/** \brief node type function
 *
 * \fn roxml_is_arg(node_t *n);
 * This function tells if a node is a arg or real node.
 * param n is the node to test
 * return 1 if node is arg else return 0
 */
int ROXML_API roxml_is_arg(node_t *n);

/** \brief node get index function
 *
 * \fn roxml_get_node_index(node_t *n);
 * This function tells the index of a node between all its homonyns.
 * param n is the node to test
 * return the idx or -1 if only one node
 */
int ROXML_API roxml_get_node_index(node_t *n);

#endif /* ROXML_H */

