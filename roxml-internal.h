/** \file roxml.h
 *  \brief header for libroxml.so
 *         
 * This is the header file used to develop some
 * softwares using the libroxml.so library.
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

#ifndef ROXML_INT_H
#define ROXML_INT_H

#define ROXML_INT

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/** \struct node_t
 *
 * \brief node_t structure
 * 
 * This is the structure for a node. This struct is very
 * little as it only contains offset for node in file and
 * tree links
 */
typedef struct node {
	int type;			/*!< document or buffer */
	char *buf;			/*!< buffer address */
	unsigned int *idx;		/*!< index in buffer address */
	FILE *fil;			/*!< loaded document */
	unsigned long long pos;		/*!< offset of opening node in file */
	unsigned long long end;		/*!< offset of closing node in file */
	unsigned long long prv;		/*!< internal offset used to keep file position */
	struct node *bra;		/*!< ref to brother */
	struct node *son;		/*!< ref to son */
	struct node *fat;		/*!< ref to father */
} node_t;


/**
 * \def FILE_BUFF
 * 
 * constant for buffer document
 */
#define FILE_BUFF	-1

/**
 * \def FILE_FILE
 * 
 * constant for file document
 */
#define FILE_FILE	1

/**
 * \def STATE_NODE_NONE
 * 
 * state for the state machine for init
 */
#define STATE_NODE_NONE		0

/**
 * \def STATE_NODE_BEG
 * 
 * state for the state machine for begining of a node
 */
#define STATE_NODE_BEG		1

/**
 * \def STATE_NODE_NAME
 * 
 * state for the state machine for name read 
 */
#define STATE_NODE_NAME		2

/**
 * \def STATE_NODE_CONTENT
 * 
 * state for the state machine for content read
 */
#define STATE_NODE_CONTENT	3

/**
 * \def STATE_NODE_END
 * 
 * state for the state machine for end of node
 */
#define STATE_NODE_END		4

/**
 * \def STATE_NODE_SINGLE
 * 
 * state for the state machine for single nodes 
 */
#define STATE_NODE_SINGLE	5

/**
 * \def STATE_NODE_ATTR
 * 
 * state for the state machine for attribut reading 
 */
#define STATE_NODE_ATTR		6

/**
 * \def STATE_NODE_STRING
 * 
 * state for the state machine for string reading 
 */
#define STATE_NODE_STRING	7

/**
 * \def STATE_NODE_ARG
 * 
 * state for the state machine for attribute name reading 
 */
#define STATE_NODE_ARG		9

/**
 * \def STATE_NODE_ARGVAL
 * 
 * state for the state machine for attribute value reading 
 */
#define STATE_NODE_ARGVAL	10

/**
 * \def STATE_NODE_SEP	
 * 
 * state for the state machine for separator reading
 */
#define STATE_NODE_SEP		11

/**
 * \def MODE_COMMENT_NONE
 * 
 * mode init in state machine 
 */
#define MODE_COMMENT_NONE	0

/**
 * \def MODE_COMMENT_QUOTE
 * 
 * mode quoted in state machine
 */
#define MODE_COMMENT_QUOTE	1

/**
 * \def MODE_COMMENT_DQUOTE
 * 
 * mode double quoted in state machine
 */
#define MODE_COMMENT_DQUOTE	2

/**
 * \def PUSH(n)
 * 
 * save current document position and recall to node
 */
#define PUSH(n)	{if(n->type > 0){n->prv = ftell(n->fil); fseek(n->fil, n->pos, SEEK_SET);} else { n->prv = *(n->idx); *(n->idx) = n->pos;}}

/**
 * \def POP(n)
 * 
 * restore old document position
 */
#define POP(n)	{if(n->type > 0){fseek(n->fil, n->prv, SEEK_SET);} else { *(n->idx) = n->prv; }}

/**
 * \def ROXML_FGETC(n)
 * 
 * get next char
 */
#define ROXML_FGETC(n)	((n->type > 0)?fgetc(n->fil):n->buf[(*(n->idx))++])

/**
 * \def ROXML_FTELL(n)
 * 
 * get stream position
 */
#define ROXML_FTELL(n)	((n->type > 0)?ftell(n->fil):*((int*)n->idx))

/**
 * \def ROXML_FEOF(n)
 * 
 * get end of stream
 */
#define ROXML_FEOF(n)	((n->type > 0)?feof(n->fil):((*(n->idx))>=(strlen(n->buf)-1)))

/**
 * \def ROXML_FREAD(b, len, size, n)
 * 
 * get chunck of stream
 */
#define ROXML_FREAD(b, len, size, n)	{if(n->type > 0){fread(b, len, size, n->fil);} else { memcpy(b, n->buf+*(n->idx), (size)*(len)); }}

/** \brief internal function
 *
 * \fn void ROXML_INT roxml_free_node(node_t *n);
 * This function delete a node 
 * param n is one node of the tree
 * return void
 */
void	ROXML_INT roxml_free_node		(node_t *n);

/** \brief internal function
 *
 * \fn node_t* ROXML_INT roxml_new_node(int pos, FILE *file);
 * This function allocate a new node 
 * param pos is the beginning offset of the node in the file
 * param file is the pointer to the opened document
 * param buffer is the pointer to the buffer
 * param idx is the position pointer inside the buffer
 * return void
 */
node_t* ROXML_INT roxml_new_node		(int pos, FILE *file, char * buf, unsigned int * idx);

/** \brief internal function
 *
 * \fn node_t* ROXML_INT roxml_parent_node(node_t *parent, node_t *n);
 * This function give a node to its father and the father to the node
 * param parent is the father node
 * param n is one orphan node of the tree
 * return the parented node
 */
node_t*	ROXML_INT roxml_parent_node		(node_t *parent, node_t *n);

/** \brief internal function
 *
 * \fn void ROXML_INT roxml_parse_node(node_t *n, char *name, char * arg, char * value, int * num, int max);
 * This function delete a tree recursively
 * param n is one node of the tree
 * return void
 * see roxml_close
 */
void 	ROXML_INT roxml_del_tree		(node_t *n);

/** \brief internal function
 *
 * \fn void ROXML_INT roxml_parse_node(node_t *n, char *name, char * arg, char * value, int * num, int max);
 * This function read a node in the file and return all datas required
 * param n is one node of the tree
 * param name a pointer where name of node will be stored or NULL
 * param arg a pointer where nth arg of node will be stored or NULL
 * param value a pointer where nth value of node will be stored or NULL
 * param num a pointer where the number of attributes will be stored
 * param max the id of attribute or value we want to read
 * return void
 */
void 	ROXML_INT roxml_parse_node		(node_t *n, char *name, char * arg, char * value, int * num, int max);

/** \brief internal function
 *
 * \fn void ROXML_INT roxml_close_node(node_t *n, node_t *close);
 * This function close the node (add the end offset) and parent the node
 * param n is the node to close
 * param close is the node that close node n
 * return void
 */
void 	ROXML_INT roxml_close_node		(node_t *n, node_t *close);

/** \brief generic load function
 *
 * \fn node_t* ROXML_API roxml_load(node_t *current_node);
 * This function load a document and all the corresponding nodes
 * file and buffer params are exclusive. You usualy want to load
 * either a file OR a buffer
 * param current_node, the XML root
 * param file file descriptor of document
 * param buffer address of buffer that contains xml
 * return the root node or NULL
 * see roxml_close
 */
node_t*	ROXML_INT roxml_load			(node_t *current_node, FILE *file, char *buffer);

/** \brief recursiv resolv path function
 *
 * \fn roxml_resolv_path(node_t *n, char * path, int *idx, node_t **res);
 * this function resolv a chunk of path and call itself recursively
 * param current_node, the current node 
 * param path the path to resolv
 * param idx the actual number of results
 * param res the place where to store resulting nodes
 * see roxml_close
 */
void ROXML_INT roxml_resolv_path(node_t *n, char * path, int *idx, node_t ***res);

#endif /* ROXML_INT_H */

