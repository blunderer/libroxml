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

roxml_parser_item_t *roxml_append_parser_item(roxml_parser_item_t *head, char * key, roxml_parse_func func);
void roxml_parser_free(roxml_parser_item_t *head);
void roxml_parser_clear(roxml_parser_item_t *head);
int roxml_parse_line(roxml_parser_item_t * head, char *line, int len, void * ctx);
roxml_parser_item_t * roxml_parser_prepare(roxml_parser_item_t *head);

// xpath parser functions
int _func_xpath_ignore(char * chunk, void * data);
int _func_xpath_new_node(char * chunk, void * data);
int _func_xpath_quote(char * chunk, void * data);
int _func_xpath_dquote(char * chunk, void * data);
int _func_xpath_open_parenthesys(char * chunk, void * data);
int _func_xpath_close_parenthesys(char * chunk, void * data);
int _func_xpath_open_brackets(char * chunk, void * data);
int _func_xpath_close_brackets(char * chunk, void * data);
int _func_xpath_condition_or(char * chunk, void * data);
int _func_xpath_condition_and(char * chunk, void * data);
int _func_xpath_path_or(char * chunk, void * data);
int _func_xpath_operator_equal(char * chunk, void * data);
int _func_xpath_operator_sup(char * chunk, void * data);
int _func_xpath_operator_inf(char * chunk, void * data);
int _func_xpath_operator_diff(char * chunk, void * data);
int _func_xpath_number(char * chunk, void * data);
int _func_xpath_position(char * chunk, void * data);
int _func_xpath_first(char * chunk, void * data);
int _func_xpath_last(char * chunk, void * data);
int _func_xpath_operator_add(char * chunk, void * data);
int _func_xpath_operator_subs(char * chunk, void * data);
int _func_xpath_default(char * chunk, void * data);
int _func_xpath_all(char * chunk, void * data);

// load parser functions
int _func_load_proc_state(char * chunk, void * data);
int _func_load_quoted(char * chunk, void * data);
int _func_load_open_cdata(char * chunk, void * data);
int _func_load_open_comment(char * chunk, void * data);
int _func_load_open_pi(char * chunk, void * data);
int _func_load_open_spec_node(char * chunk, void * data);
int _func_load_close_cdata(char * chunk, void * data);
int _func_load_close_comment(char * chunk, void * data);
int _func_load_close_pi(char * chunk, void * data);
int _func_load_open_node(char * chunk, void * data);
int _func_load_close_node(char * chunk, void * data);
int _func_load_end_node(char * chunk, void * data);
int _func_load_white(char * chunk, void * data);
int _func_load_white(char * chunk, void * data);
int _func_load_white(char * chunk, void * data);
int _func_load_white(char * chunk, void * data);
int _func_load_default(char * chunk, void * data);

#endif /* ROXML_PARSE_ENGINE_H */

