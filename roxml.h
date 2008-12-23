/** \file roxml.h
 *  \brief header for libroxml.so
 *         
 * This is the header file used to develop some
 * softwares using the libroxml.so library.
 * \author blunderer <blunderer@blunderer.org>
 * \date 23 Dec 2008
 *
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

#define ROXML_API
#define ROXML_INT

/** \struct node_t
 *
 * \brief node_t structure
 * 
 * This is the structure for a node. This struct is very
 * little as it only contains offset for node in file and
 * tree links
 */
typedef struct node {
	FILE *fil;			/*!< loaded document */
	unsigned long long pos;		/*!< offset of opening node in file */
	unsigned long long end;		/*!< offset of closing node in file */
	unsigned long long prv;		/*!< internal offset used to keep file position */
	struct node *bra;		/*!< ref to brother */
	struct node *son;		/*!< ref to son */
	struct node *fat;		/*!< ref to father */
} node_t;

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
#define PUSH(n)	n->prv = ftell(n->fil); fseek(n->fil, n->pos, SEEK_SET);

/**
 * \def POP(n)
 * 
 * restore old document position
 */
#define POP(n)	fseek(n->fil, n->prv, SEEK_SET);

/** \brief load function
 *
 * \fn node_t* ROXML_API roxml_load_doc(char *filename);
 * This function clear a document and all the corresponding nodes
 * param filename the XML document to load
 * return the root node or NULL
 * see roxml_close_doc
 */
node_t*	ROXML_API roxml_load_doc		(char *filename);

/** \brief unload function
 *
 * \fn void ROXML_API roxml_close_doc(node_t *n);
 * This function clear a document and all the corresponding nodes
 * param n is one node of the tree
 * return void
 * see roxml_load_doc
 */
void 	ROXML_API roxml_close_doc		(node_t *n);

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
 * \fn int ROXML_API roxml_get_content(node_t *n, char * content);
 * This function fill a pointer with the content of a node
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
 * return void
 */
node_t* ROXML_INT roxml_new_node		(int pos, FILE *file);

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
 * see roxml_close_doc
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

#endif /* ROXML_H */

