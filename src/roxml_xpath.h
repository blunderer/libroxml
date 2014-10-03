/**
 * \file roxml_xpath.h
 * \brief xpath execution module
 *
 * (C) Copyright 2014
 * Tristan Lelong <tristan.lelong@libroxml.net>
 *
 * SPDX-Licence-Identifier:     LGPL-2.1+
 * The author added a static linking exception, see License.txt.
 */

#ifndef ROXML_XPATH_H
#define ROXML_XPATH_H

#include "roxml_internal.h"

/** \brief xpath free function
 *
 * \fn roxml_free_xpath(xpath_node_t *xpath, int nb);
 * this function frees the parsed xpath structure
 * \param xpath the xpath to free
 * \param nb the number of xpath structures in the table
 * \return
 */
ROXML_INT void roxml_free_xpath(xpath_node_t *xpath, int nb);

/** \brief real xpath execution
 *
 * \fn roxml_exec_xpath(node_t *root, node_t *n, xpath_node_t *xpath, int index, int * count);
 * this function exec a decoded xpath strcuture
 * \param root the root of the tree
 * \param n the context node
 * \param xpath the xpath structure
 * \param index the number of xpath condition in string
 * \param count the pointer to a variable that is filled with the resulting node number
 * \return  the resulting node set that have to be freed with roxml_release
 */
ROXML_INT node_t **roxml_exec_xpath(node_t *root, node_t * n, xpath_node_t * xpath, int index, int *count);

ROXML_INT int _func_xpath_ignore(char *chunk, void *data);
ROXML_INT int _func_xpath_new_node(char *chunk, void *data);
ROXML_INT int _func_xpath_quote(char *chunk, void *data);
ROXML_INT int _func_xpath_dquote(char *chunk, void *data);
ROXML_INT int _func_xpath_open_parenthesys(char *chunk, void *data);
ROXML_INT int _func_xpath_close_parenthesys(char *chunk, void *data);
ROXML_INT int _func_xpath_open_brackets(char *chunk, void *data);
ROXML_INT int _func_xpath_close_brackets(char *chunk, void *data);
ROXML_INT int _func_xpath_condition_or(char *chunk, void *data);
ROXML_INT int _func_xpath_condition_and(char *chunk, void *data);
ROXML_INT int _func_xpath_path_or(char *chunk, void *data);
ROXML_INT int _func_xpath_operators(char *chunk, void *data, int operator, int operator_bis);
ROXML_INT int _func_xpath_operator_equal(char *chunk, void *data);
ROXML_INT int _func_xpath_operator_sup(char *chunk, void *data);
ROXML_INT int _func_xpath_operator_inf(char *chunk, void *data);
ROXML_INT int _func_xpath_operator_diff(char *chunk, void *data);
ROXML_INT int _func_xpath_number(char *chunk, void *data);
ROXML_INT int _func_xpath_funcs(char *chunk, void *data, int func, char *name);
ROXML_INT int _func_xpath_position(char *chunk, void *data);
ROXML_INT int _func_xpath_first(char *chunk, void *data);
ROXML_INT int _func_xpath_last(char *chunk, void *data);
ROXML_INT int _func_xpath_nsuri(char *chunk, void *data);
ROXML_INT int _func_xpath_lname(char *chunk, void *data);
ROXML_INT int _func_xpath_operator_add(char *chunk, void *data);
ROXML_INT int _func_xpath_operator_subs(char *chunk, void *data);
ROXML_INT int _func_xpath_default(char *chunk, void *data);

#endif /* ROXML_XPATH_H */
