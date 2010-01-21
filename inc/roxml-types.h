/** \file roxml-types.h
 *  \brief internal header for libroxml.so
 *         
 * This is the internal header file used by roxml.c
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

#ifndef ROXML_TYPES_H
#define ROXML_TYPES_H

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
	char axes;			/*!< axes for operator */
	char op;			/*!< operator used */
	char op2;			/*!< operator used on arg2 */
	char func;			/*!< function to process */
	char func2;			/*!< function to process in arg2 */
	char *arg1;			/*!< condition arg1 as string */
	char *arg2;			/*!< condition arg2 as string */
	struct _xpath_cond *next;	/*!< next xpath condition pointer */
} xpath_cond_t;

/** \struct xpath_node_t
 *
 * \brief xpath node structure
 * 
 * This is the structure for a xpath node. It contains the
 * node axes and conditions
 */
typedef struct _xpath_node {
	char abs;			/*!< for first node: is path absolute */
	char rel;			/*!< relation with previous */
	char axes;			/*!< axes type */
	char *name;			/*!< axes string */
	struct _xpath_cond *cond;	/*!< condition list */
	struct _xpath_node *next;	/*!< next xpath pointer */
} xpath_node_t;

/** \struct xpath_tok_t
 *
 * \brief xpath token structure
 * 
 * This is the structure for a xpath token. It contains the
 * xpath id
 */
typedef struct _xpath_tok_table {
	unsigned char id;		/*!< token id == ROXML_REQTABLE_ID */
	unsigned char ids[256];		/*!< token id table */
	pthread_mutex_t mut;		/*!< token table allocation mutex */
	struct _xpath_tok *next;	/*!< next xpath token */
} xpath_tok_table_t;

/** \struct xpath_tok_t
 *
 * \brief xpath token structure
 * 
 * This is the structure for a xpath token. It contains the
 * xpath id
 */
typedef struct _xpath_tok {
	unsigned char id;		/*!< token id */
	struct _xpath_tok *next;	/*!< next xpath token */
} xpath_tok_t;

/** \struct node_t
 *
 * \brief node_t structure
 * 
 * This is the structure for a node. This struct is very
 * little as it only contains offset for node in file and
 * tree links
 */
typedef struct node {
	unsigned char type;		/*!< document or buffer / attribute or value */
	union {
		char *buf;		/*!< buffer address */
		FILE *fil;		/*!< loaded document */
		void *src;		/*!< xml src address */
	} src;
	unsigned long pos;		/*!< offset of begining of opening node in file */
	unsigned long end;		/*!< offset of begining of closing node in file */
	struct node *sibl;		/*!< ref to brother */
	struct node *chld;		/*!< ref to chld */
	struct node *prnt;		/*!< ref to parent */
	struct node *attr;		/*!< ref to attributes */
	struct node *text;		/*!< ref to content */
	struct node *next;		/*!< ref to next (internal use) */
	void *priv;			/*!< ref to xpath tok (internal use) */
} node_t;

typedef struct {
	int pos;
	int empty_text_node;
	int state;
	int previous_state;
	int mode;
	int inside_node_state;
	int type;
	void * src;
	node_t *candidat_node;
	node_t *candidat_txt;
	node_t *candidat_arg;
	node_t *candidat_val;
	node_t *current_node;
} roxml_parse_ctx_t;

#endif /* ROXML_TYPES_H */

