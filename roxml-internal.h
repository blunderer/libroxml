/** \file roxml-internal.h
 *  \brief internal header for libroxml.so
 *         
 * This is the header file used by roxml.c
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

#define IGNORE_EMPTY_TEXT_NODES

/**
 * \def ROXML_INT
 *
 * internal function: not part of the API
 */
#define ROXML_INT

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

/** \struct memory_cell_t
 *
 * \brief memory cell structure
 * 
 * This is the structure for a memory cell. It contains the
 * pointer info and type. It also contains the caller id so that
 * it can free without reference to a specific pointer
 */
typedef struct memory_cell {
	int type;			/*!< pointer type from PTR_NODE, PTR_CHAR... */
	int occ;			/*!< number of element */
	void *ptr;			/*!< pointer */
	pthread_t id;			/*!< thread id of allocator */
	struct memory_cell *next;	/*!< next memory cell */
	struct memory_cell *prev;	/*!< prev memory cell */
} memory_cell_t;

/** \struct xpath_cond_t
 *
 * \brief xpath cond structure
 * 
 * This is the structure for a xpath cond. It contains the
 * node condition
 */
typedef struct _xpath_cond {
	char rel;			/*!< relation with previous */
	char op;			/*!< operator used */
	char func;			/*!< function to process */
	char *arg1;			/*!< condition arg1 as string */
	char *arg2;			/*!< condition arg2 as string */
	struct _xpath_cond*	next;	/*!< next xpath condition pointer */
} xpath_cond_t;

/** \struct xpath_node_t
 *
 * \brief xpath node structure
 * 
 * This is the structure for a xpath node. It contains the
 * node axis and conditions
 */
typedef struct _xpath_node {
	char rel;			/*!< relation with previous */
	char *axis;			/*!< axis string */
	struct _xpath_cond *cond;	/*!< condition list */
	struct _xpath_node*	next;	/*!< next xpath pointer */
} xpath_node_t;

/** \struct node_t
 *
 * \brief node_t structure
 * 
 * This is the structure for a node. This struct is very
 * little as it only contains offset for node in file and
 * tree links
 */
typedef struct node {
	char type;			/*!< document or buffer / attribute or value */
	unsigned int *idx;		/*!< index in buffer address */
	union {
		char *buf;		/*!< buffer address */
		FILE *fil;		/*!< loaded document */
	} src;
	unsigned long pos;		/*!< offset of begining of opening node in file */
	unsigned long end;		/*!< offset of begining of closing node in file */
	struct node *sibl;		/*!< ref to brother */
	struct node *chld;		/*!< ref to chld */
	struct node *prnt;		/*!< ref to parent */
	struct node *attr;		/*!< ref to attributes */
	struct node *text;		/*!< ref to content */
	struct node *next;		/*!< ref to next internal use */
} node_t;

#define ROXML_PRIVATE
#include "roxml.h"

#define ROXML_PATH_OR	"||"
#define ROXML_PATH_AND	"&&"
#define ROXML_COND_OR	"or"
#define ROXML_COND_AND	"and"

#define ROXML_OPERATOR_OR	1
#define ROXML_OPERATOR_AND	2
#define ROXML_OPERATOR_INF	3
#define ROXML_OPERATOR_SUP	4
#define ROXML_OPERATOR_EINF	5
#define ROXML_OPERATOR_ESUP	6
#define ROXML_OPERATOR_DIFF	7
#define ROXML_OPERATOR_EQU	8

#define ROXML_FUNC_INTCOMP	0
#define ROXML_FUNC_STRCOMP	1
#define ROXML_FUNC_POS		2
#define ROXML_FUNC_FIRST	3
#define ROXML_FUNC_LAST		4

#define ROXML_FUNC_POS_STR	"position()"
#define ROXML_FUNC_FIRST_STR	"first()"
#define ROXML_FUNC_LAST_STR	"last()"

#define ROXML_BULK_READ		4096
#define ROXML_LONG_LEN		512
#define ROXML_BASE_LEN		128
#define ROXML_BULK_CTX		8

#define ROXML_L_CHILD		"child::"
#define ROXML_L_DESC_O_SELF	"descendant-or-self::"
#define ROXML_L_SELF		"self::"
#define ROXML_L_PARENT		"parent::"
#define ROXML_L_ATTR		"attribute::"
#define ROXML_L_DESC		"descendant::"
#define ROXML_L_ANC		"ancestor::"
#define ROXML_L_NEXT_SIBL	"following-sibling::"
#define ROXML_L_PREV_SIBL	"preceding-sibling::"
#define ROXML_L_NEXT		"following::"
#define ROXML_L_PREV		"preceding::"
#define ROXML_L_NS		"namespace::"
#define ROXML_L_ANC_O_SELF	"ancestor-or-self::"

#define ROXML_S_CHILD
#define ROXML_S_DESC_O_SELF	""
#define ROXML_S_SELF		"."
#define ROXML_S_PARENT		".."
#define ROXML_S_ATTR		"@"

#define ROXML_DIRECT		0
#define ROXML_DESC_ONLY		1
#define ROXML_SELF_AND_DESC	2


/**
 * \def INTERNAL_BUF_SIZE
 * 
 * constant for internal buffer size
 */
#define INTERNAL_BUF_SIZE	512

/**
 * \def PTR_NONE
 * 
 * constant for void pointers
 */
#define PTR_NONE	-1

/**
 * \def PTR_VOID
 * 
 * constant for void pointers
 */
#define PTR_VOID	0

/**
 * \def PTR_CHAR
 * 
 * constant for char pointers
 */
#define PTR_CHAR	2

/**
 * \def PTR_CHAR_STAR
 * 
 * constant for char table pointers
 */
#define PTR_CHAR_START	3

/**
 * \def PTR_NODE
 * 
 * constant for node pointers
 */
#define PTR_NODE	4

/**
 * \def PTR_NODE_STAR
 * 
 * constant for node table pointers
 */
#define PTR_NODE_STAR	5

/**
 * \def PTR_INT
 * 
 * constant for int pointer
 */
#define PTR_INT	6

/**
 * \def PTR_INT_STAR
 * 
 * constant for int table pointers
 */
#define PTR_INT_STAR	7

/**
 * \def PTR_RESULT
 * 
 * constant for node table pointers where node are not to delete
 */
#define PTR_NODE_RESULT	8

/**
 * \def PTR_IS_STAR(a)
 * 
 * macro returning if a memory_cell is a star cell
 */
#define PTR_IS_STAR(a)	((a)->type % 2)

/**
 * \def ROXML_FILE
 * 
 * constant for argument node
 */
#define ROXML_FILE	0x01

/**
 * \def ROXML_BUFF
 * 
 * constant for buffer document
 */
#define ROXML_BUFF	0x02

/**
 * \def ROXML_PENDING
 * 
 * constant for pending node
 */
#define ROXML_PENDING	0x20


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
 * \def STATE_INSIDE_ARG_BEG
 * 
 * inside node state begining (attribute declaration)
 */
#define STATE_INSIDE_ARG_BEG	0

/**
 * \def STATE_INSIDE_ARG
 * 
 * inside node state arg name
 */
#define STATE_INSIDE_ARG	1

/**
 * \def STATE_INSIDE_VAL_BEG
 * 
 * inside node state arg value
 */
#define STATE_INSIDE_VAL_BEG	2

/**
 * \def STATE_INSIDE_VAL
 * 
 * inside node state arg value
 */
#define STATE_INSIDE_VAL	3

/**
 * \def ROXML_WHITE(n)
 * 
 * save current document position and recall to node
 */
#define ROXML_WHITE(n) ((n==' ')||(n=='\t')||(n=='\n')||(n=='\r'))

/** \brief internal function
 *
 * \fn void ROXML_INT roxml_free_node(node_t *n);
 * This function delete a node 
 * \param n is one node of the tree
 * \return void
 */
void	ROXML_INT roxml_free_node		(node_t *n);

/** \brief internal function
 *
 * \fn node_t* ROXML_INT roxml_create_node(int pos, FILE *file, char * buf, unsigned int * idx);
 * This function allocate a new node 
 * \param pos is the beginning offset of the node in the file
 * \param file is the pointer to the opened document
 * \param buffer is the pointer to the buffer
 * \param idx is the position pointer inside the buffer
 * \param type is the type of node between arg and val
 * \return the new node
 */
node_t* ROXML_INT roxml_create_node		(int pos, FILE *file, char * buf, unsigned int * idx, int type);

/** \brief internal function
 *
 * \fn void ROXML_INT roxml_del_tree(node_t *n);
 * This function delete a tree recursively
 * \param n is one node of the tree
 * \return void
 * see roxml_close
 */
void 	ROXML_INT roxml_del_tree		(node_t *n);

/** \brief internal function
 *
 * \fn void ROXML_INT roxml_parse_node(node_t *n, char *name, char * arg, char * value, int * num, int max);
 * This function read a node in the file and return all datas required
 * \param n is one node of the tree
 * \param name a pointer where name of node will be stored or NULL
 * \param arg a pointer where nth arg of node will be stored or NULL
 * \param value a pointer where nth value of node will be stored or NULL
 * \param num a pointer where the number of attributes will be stored
 * \param max the id of attribute or value we want to read
 * \return void
 */
void 	ROXML_INT roxml_parse_node		(node_t *n, char *name, char * arg, char * value, int * num, int max);

/** \brief internal function
 *
 * \fn void ROXML_INT roxml_close_node(node_t *n, node_t *close);
 * This function close the node (add the end offset) and parent the node
 * \param n is the node to close
 * \param close is the node that close node n
 * \return void
 */
void 	ROXML_INT roxml_close_node		(node_t *n, node_t *close);

/** \brief generic load function
 *
 * \fn node_t* ROXML_API roxml_load(node_t *current_node, FILE *file, char *buffer);
 * This function load a document and all the corresponding nodes
 * file and buffer \params are exclusive. You usualy want to load
 * either a file OR a buffer
 * \param current_node, the XML root
 * \param file file descriptor of document
 * \param buffer address of buffer that contains xml
 * \return the root node or NULL
 * see roxml_close
 */
node_t*	ROXML_INT roxml_load			(node_t *current_node, FILE *file, char *buffer);

/** \brief recursiv resolv path function
 *
 * \fn void ROXML_INT roxml_resolv_path(node_t *n, char * path, int *idx, node_t ***res);
 * this function resolv a chunk of path and call itself recursively
 * \param current_node, the current node 
 * \param path the path to resolv
 * \param idx the actual number of results
 * \param res the place where to store resulting nodes
 * see roxml_close
 */
void ROXML_INT roxml_resolv_path		(node_t *n, char * path, int *idx, node_t ***res);

/** \brief recursiv resolv path function
 *
 * \fn int ROXML_INT roxml_xpath_conditionnal(node_t *n, char *condition);
 * this function resolv an xpath condition
 * \param n, the current node 
 * \param condition the condition in brackets
 * see roxml_resolv_path
 */
int ROXML_INT roxml_xpath_conditionnal		(node_t *n, char *condition);

/** \brief alloc memory function function
 *
 * \fn roxml_malloc(int size, int num, int type)
 * this function allocate some memory that will be reachable at
 * any time by libroxml memory manager
 * \param size the size of memory to allocate for each elem
 * \param num the number of element
 * \param type the kind of pointer
 */
void * ROXML_INT roxml_malloc(int size, int num, int type);

int roxml_parse_xpath(char *path, xpath_node_t ** xpath);


#ifdef __DEBUG
extern unsigned int _nb_node;
extern unsigned int _nb_attr;
extern unsigned int _nb_text;
extern memory_cell_t head_cell;
#endif
#endif /* ROXML_INT_H */

