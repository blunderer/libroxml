/** \file roxml-parse-engine.h
 *  \brief header for libroxml.so
 *         
 * This is the source file for lib libroxml.so internal functions
 * \author blunderer <blunderer@blunderer.org>
 * \date 20 Fev 2010
 *
 * Copyright (C) 2010 blunderer
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


#ifndef ROXML_PARSE_ENGINE_H
#define ROXML_PARSE_ENGINE_H

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

typedef int(*roxml_parse_func)(char *chunk, void * data);

typedef struct _roxml_parser_item
{
	char *chunk;
	int chunk_len;
	roxml_parse_func func;
	struct _roxml_parser_item *next;
} roxml_parser_item_t;

roxml_parser_item_t *roxml_append_parser_item(roxml_parser_item_t *head, char * key, roxml_parse_func func);
void roxml_parser_clear(roxml_parser_item_t *head);
int roxml_parse_line(roxml_parser_item_t * head, char *line, roxml_parse_func func, void * ctx);

int _func_ignore(char * chunk, void * data);
int _func_new_node(char * chunk, void * data);
int _func_quote(char * chunk, void * data);
int _func_dquote(char * chunk, void * data);
int _func_open_parenthesys(char * chunk, void * data);
int _func_close_parenthesys(char * chunk, void * data);
int _func_open_brackets(char * chunk, void * data);
int _func_close_brackets(char * chunk, void * data);
int _func_condition_or(char * chunk, void * data);
int _func_condition_and(char * chunk, void * data);
int _func_xpath_or(char * chunk, void * data);
int _func_operator_equal(char * chunk, void * data);
int _func_operator_sup(char * chunk, void * data);
int _func_operator_inf(char * chunk, void * data);
int _func_operator_diff(char * chunk, void * data);
int _func_number(char * chunk, void * data);
int _func_position(char * chunk, void * data);
int _func_first(char * chunk, void * data);
int _func_last(char * chunk, void * data);
int _func_operator_add(char * chunk, void * data);
int _func_operator_subs(char * chunk, void * data);
int _func_default_xpath(char * chunk, void * data);
int _func_xpath_all(char * chunk, void * data);


#endif /* ROXML_PARSE_ENGINE_H */

