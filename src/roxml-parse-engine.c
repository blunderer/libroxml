/** \file roxml-parse-engine.c
 *  \brief source for libroxml.so
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


#include "roxml-parse-engine.h"
#include "roxml-internal.h"

roxml_parser_item_t *roxml_append_parser_item(roxml_parser_item_t *head, char * key, roxml_parse_func func)
{
	roxml_parser_item_t *item = head;

	if(head == NULL) {
		item = (roxml_parser_item_t*)calloc(1, sizeof(roxml_parser_item_t));
		head = item;
	} else {
		item = head;
		while(item->next) item = item->next;
		item->next = (roxml_parser_item_t*)calloc(1, sizeof(roxml_parser_item_t));
		item = item->next;
	}
	item->chunk = key?strdup(key):key;
	item->chunk_len = key?strlen(key):0;
	item->func = func;

	return head;
}

void roxml_parser_clear(roxml_parser_item_t *head)
{
	roxml_parser_item_t *item = head;

	while(item) {
		roxml_parser_item_t * to_delete = item;
		item = item->next;
		free(item->chunk);
		free(to_delete);
	}

	return;
}

int roxml_parse_line(roxml_parser_item_t * head, char *line, roxml_parse_func func, void * ctx)
{
	int pos = 0;
	int line_size = strlen(line);

	while(pos < line_size) {
		int ret = 0;
		roxml_parser_item_t *item = head;
		while(item) {
			int exec = 1;
			if(item->chunk) {
				exec = 0;
				if(strncmp(line+pos, item->chunk, item->chunk_len) == 0) { exec = 1; }
			}
			if(exec) { ret = item->func(line+pos, ctx); }
			if(ret > 0) {
				pos += ret;
				break;
			} else if(ret < 0) {
				return pos;
			}
			item = item->next;
		}
		if(func) func(line+pos, ctx);
		if(ret == 0) { pos++; }
	}
	return pos;
}

int _func_ignore(char * chunk, void * data)
{
	return 1;
}

int _func_new_node(char * chunk, void * data)
{
	int cur = 0;
	roxml_xpath_ctx_t *ctx = (roxml_xpath_ctx_t*)data;
	if(!ctx->quoted && !ctx->dquoted && !ctx->parenthesys && !ctx->bracket)     {
		int offset = 0;
		xpath_node_t * tmp_node = (xpath_node_t*)calloc(1, sizeof(xpath_node_t));
		if((chunk[cur] == '/')&&(ctx->is_first_node)) { 
			free(tmp_node);
			ctx->new_node = ctx->first_node;
			ctx->first_node->abs = 1;
		} else if((chunk[cur] == '/')&&(ctx->wait_first_node)) { 
			free(tmp_node);
			ctx->first_node->abs = 1;
		} else if((ctx->is_first_node)||(ctx->wait_first_node)) {
			free(tmp_node);
		} else {
			if(ctx->new_node)    { ctx->new_node->next = tmp_node; }
			ctx->new_node = tmp_node;
		}
		ctx->is_first_node = 0;
		ctx->wait_first_node = 0;
		ctx->new_node = roxml_set_axes(ctx->new_node, chunk+cur, &offset);
		cur = offset + 1;
	}
	return cur;
}

int _func_quote(char * chunk, void * data)
{
	roxml_xpath_ctx_t *ctx = (roxml_xpath_ctx_t*)data;
	if(!ctx->dquoted) {
		ctx->quoted = (ctx->quoted+1)%2;
	}
	return 1;
}

int _func_dquote(char * chunk, void * data)
{
	roxml_xpath_ctx_t *ctx = (roxml_xpath_ctx_t*)data;
	if(!ctx->quoted) {
		ctx->dquoted = (ctx->dquoted+1)%2;
	}
	return 1;
}

int _func_open_parenthesys(char * chunk, void * data)
{
	roxml_xpath_ctx_t *ctx = (roxml_xpath_ctx_t*)data;
	if(!ctx->quoted && !ctx->dquoted) {
		ctx->parenthesys = (ctx->parenthesys+1)%2;
	}
	return 1;
}

int _func_close_parenthesys(char * chunk, void * data)
{
	roxml_xpath_ctx_t *ctx = (roxml_xpath_ctx_t*)data;
	if(!ctx->quoted && !ctx->dquoted) {
		ctx->parenthesys = (ctx->parenthesys+1)%2;
	}
	return 1;
}

int _func_open_brackets(char * chunk, void * data)
{
	int cur = 0;
	roxml_xpath_ctx_t *ctx = (roxml_xpath_ctx_t*)data;
	if(!ctx->quoted && !ctx->dquoted) {
		ctx->bracket = (ctx->bracket+1)%2;
		chunk[0] = '\0';

		ctx->shorten_cond = 2;
		xpath_cond_t * tmp_cond = (xpath_cond_t*)calloc(1, sizeof(xpath_cond_t));
		ctx->new_node->cond = tmp_cond;
		ctx->new_cond = tmp_cond;
		ctx->new_cond->arg1 = chunk+cur+1;
	}
	cur++;
	return 1;
}

int _func_close_brackets(char * chunk, void * data)
{
	int cur = 0;
	roxml_xpath_ctx_t *ctx = (roxml_xpath_ctx_t*)data;
	if(!ctx->quoted && !ctx->dquoted) {
		ctx->bracket = (ctx->bracket+1)%2;
		chunk[0] = '\0';

		if(ctx->new_cond->func == ROXML_FUNC_XPATH) {
			xpath_node_t *xp;
			ctx->new_cond->func2 = roxml_parse_xpath(ctx->new_cond->arg1, &xp, 1);
			ctx->new_cond->xp = xp;
		}
	}
	cur++;
	return 1;
}

int _func_condition_or(char * chunk, void * data)
{
	roxml_xpath_ctx_t *ctx = (roxml_xpath_ctx_t*)data;
	int cur = 0;
	if(!ctx->bracket && !ctx->quoted && !ctx->dquoted) {
		if(ctx->context != 1) { return 0; }
		chunk[-1] = '\0';
		cur += strlen(ROXML_COND_OR);
		xpath_node_t * tmp_node = (xpath_node_t*)calloc(ctx->nbpath+1, sizeof(xpath_node_t));
		memcpy(tmp_node, ctx->first_node, ctx->nbpath*sizeof(xpath_node_t));
		free(ctx->first_node);
		ctx->first_node = tmp_node;
		ctx->wait_first_node = 1;
		ctx->new_node = tmp_node+ctx->nbpath;
		ctx->new_node->rel = ROXML_OPERATOR_OR;
		ctx->nbpath++;
	} else if(ctx->bracket && !ctx->quoted && !ctx->dquoted) {
		if(ctx->new_cond->func != ROXML_FUNC_XPATH) {
			chunk[-1] = '\0';
			cur += strlen(ROXML_COND_OR);
			xpath_cond_t * tmp_cond = (xpath_cond_t*)calloc(1, sizeof(xpath_cond_t));
			if(ctx->new_cond) { ctx->new_cond->next = tmp_cond; }
			ctx->new_cond = tmp_cond;
			ctx->new_cond->rel = ROXML_OPERATOR_OR;
			ctx->new_cond->arg1 = chunk+cur+1;
		}
	}
	return cur;
}

int _func_condition_and(char * chunk, void * data)
{
	roxml_xpath_ctx_t *ctx = (roxml_xpath_ctx_t*)data;
	int cur = 0;
	if(!ctx->bracket && !ctx->quoted && !ctx->dquoted) {
		if(ctx->context != 1) { return 0; }
		chunk[-1] = '\0';
		cur += strlen(ROXML_COND_AND);
		xpath_node_t * tmp_node = (xpath_node_t*)calloc(ctx->nbpath+1, sizeof(xpath_node_t));
		memcpy(tmp_node, ctx->first_node, ctx->nbpath*sizeof(xpath_node_t));
		free(ctx->first_node);
		ctx->first_node = tmp_node;
		ctx->wait_first_node = 1;
		ctx->new_node = tmp_node+ctx->nbpath;
		ctx->new_node->rel = ROXML_OPERATOR_AND;
		ctx->nbpath++;
	} else if(ctx->bracket && !ctx->quoted && !ctx->dquoted) {
		if(ctx->new_cond->func != ROXML_FUNC_XPATH) {
			chunk[-1] = '\0';
			cur += strlen(ROXML_COND_AND);
			xpath_cond_t * tmp_cond = (xpath_cond_t*)calloc(1, sizeof(xpath_cond_t));
			if(ctx->new_cond) { ctx->new_cond->next = tmp_cond; }
			ctx->new_cond = tmp_cond;
			ctx->new_cond->rel = ROXML_OPERATOR_AND;
			ctx->new_cond->arg1 = chunk+cur+1;
		}
	}
	return cur;
}

int _func_xpath_or(char * chunk, void * data)
{
	roxml_xpath_ctx_t *ctx = (roxml_xpath_ctx_t*)data;
	int cur = 0;
	if(!ctx->bracket && !ctx->quoted && !ctx->dquoted) {
		chunk[-1] = '\0';
		cur += strlen(ROXML_PATH_OR);
		xpath_node_t * tmp_node = (xpath_node_t*)calloc(ctx->nbpath+1, sizeof(xpath_node_t));
		memcpy(tmp_node, ctx->first_node, ctx->nbpath*sizeof(xpath_node_t));
		free(ctx->first_node);
		ctx->first_node = tmp_node;
		ctx->wait_first_node = 1;
		ctx->new_node = tmp_node+ctx->nbpath;
		ctx->new_node->rel = ROXML_OPERATOR_OR;
		ctx->nbpath++;
	}
	return cur;
}

int _func_operator_equal(char * chunk, void * data)
{
	int cur = 0;
	roxml_xpath_ctx_t *ctx = (roxml_xpath_ctx_t*)data;
	if(!ctx->bracket && !ctx->quoted && !ctx->dquoted) {
		xpath_node_t *xp_root = ctx->new_node;
		xpath_cond_t * xp_cond = (xpath_cond_t*)calloc(1, sizeof(xpath_cond_t));
		xp_root->xp_cond = xp_cond;
		xp_cond->op = ROXML_OPERATOR_EQU;
		chunk[cur] = '\0';
		if(ROXML_WHITE(chunk[cur-1])) {
			chunk[cur-1] = '\0';
		}
		if(ROXML_WHITE(chunk[cur+1])) {
			cur++;
			chunk[cur] = '\0';
		}
		xp_cond->arg2 = chunk+cur+1;
		if((xp_cond->arg2[0] > '9')||(xp_cond->arg2[0] < '0')) {
			xp_cond->func = ROXML_FUNC_STRCOMP;
		}
		cur++;
	} else if(ctx->bracket && !ctx->quoted && !ctx->dquoted) {
		if(ctx->new_cond->func != ROXML_FUNC_XPATH) {
			ctx->new_cond->op = ROXML_OPERATOR_EQU;
			chunk[cur] = '\0';
			if(ROXML_WHITE(chunk[cur-1])) {
				chunk[cur-1] = '\0';
			}
			if(ROXML_WHITE(chunk[cur+1])) {
				cur++;
				chunk[cur] = '\0';
			}
			ctx->new_cond->arg2 = chunk+cur+1;
			if((ctx->new_cond->arg2[0] > '9')||(ctx->new_cond->arg2[0] < '0')) {
				ctx->new_cond->func = ROXML_FUNC_STRCOMP;
			}
			cur++;
		}
	}
	return cur;
}

int _func_operator_sup(char * chunk, void * data)
{
	roxml_xpath_ctx_t *ctx = (roxml_xpath_ctx_t*)data;
	int cur = 0;
	if(!ctx->bracket && !ctx->quoted && !ctx->dquoted) {
		xpath_node_t *xp_root = ctx->new_node;
		xpath_cond_t * xp_cond = (xpath_cond_t*)calloc(1, sizeof(xpath_cond_t));
		xp_root->xp_cond = xp_cond;
		chunk[cur] = '\0';
		xp_cond->op = ROXML_OPERATOR_SUP;
		if(ROXML_WHITE(chunk[cur-1])) {
			chunk[cur-1] = '\0';
		}
		if(chunk[cur+1] == '=') {
			cur++;
			chunk[cur] = '\0';
			xp_cond->op = ROXML_OPERATOR_ESUP;
		}
		if(ROXML_WHITE(chunk[cur+1])) {
			cur++;
			chunk[cur] = '\0';
		}
		xp_cond->arg2 = chunk+cur+1;
		cur++;
	} else if(ctx->bracket && !ctx->quoted && !ctx->dquoted) {
		if(ctx->new_cond->func != ROXML_FUNC_XPATH) {
			chunk[cur] = '\0';
			ctx->new_cond->op = ROXML_OPERATOR_SUP;
			if(ROXML_WHITE(chunk[cur-1])) {
				chunk[cur-1] = '\0';
			}
			if(chunk[cur+1] == '=') {
				cur++;
				chunk[cur] = '\0';
				ctx->new_cond->op = ROXML_OPERATOR_ESUP;
			}
			if(ROXML_WHITE(chunk[cur+1])) {
				cur++;
				chunk[cur] = '\0';
			}
			ctx->new_cond->arg2 = chunk+cur+1;
			cur++;
		}
	}
	return cur;
}

int _func_operator_inf(char * chunk, void * data)
{
	roxml_xpath_ctx_t *ctx = (roxml_xpath_ctx_t*)data;
	int cur = 0;
	if(!ctx->bracket && !ctx->quoted && !ctx->dquoted) {
		xpath_node_t *xp_root = ctx->new_node;
		xpath_cond_t * xp_cond = (xpath_cond_t*)calloc(1, sizeof(xpath_cond_t));
		xp_root->xp_cond = xp_cond;
		chunk[cur] = '\0';
		xp_cond->op = ROXML_OPERATOR_INF;
		if(ROXML_WHITE(chunk[cur-1])) {
			chunk[cur-1] = '\0';
		}
		if(chunk[cur+1] == '=') {
			cur++;
			chunk[cur] = '\0';
			xp_cond->op = ROXML_OPERATOR_EINF;
		}
		if(ROXML_WHITE(chunk[cur+1])) {
			cur++;
			chunk[cur] = '\0';
		}
		xp_cond->arg2 = chunk+cur+1;
		cur++;
	} else if(ctx->bracket && !ctx->quoted && !ctx->dquoted) {
		if(ctx->new_cond->func != ROXML_FUNC_XPATH) {
			chunk[cur] = '\0';
			ctx->new_cond->op = ROXML_OPERATOR_INF;
			if(ROXML_WHITE(chunk[cur-1])) {
				chunk[cur-1] = '\0';
			}
			if(chunk[cur+1] == '=') {
				cur++;
				chunk[cur] = '\0';
				ctx->new_cond->op = ROXML_OPERATOR_EINF;
			}
			if(ROXML_WHITE(chunk[cur+1])) {
				cur++;
				chunk[cur] = '\0';
			}
			ctx->new_cond->arg2 = chunk+cur+1;
			cur++;
		}
	}
	return cur;
}

int _func_operator_diff(char * chunk, void * data)
{
	roxml_xpath_ctx_t *ctx = (roxml_xpath_ctx_t*)data;
	int cur = 0;
	if(!ctx->bracket && !ctx->quoted && !ctx->dquoted) {
		xpath_node_t *xp_root = ctx->new_node;
		xpath_cond_t * xp_cond = (xpath_cond_t*)calloc(1, sizeof(xpath_cond_t));
		xp_root->xp_cond = xp_cond;
		chunk[cur] = '\0';
		chunk[cur+1] = '\0';
		xp_cond->op = ROXML_OPERATOR_DIFF;
		if(ROXML_WHITE(chunk[cur-1])) {
			chunk[cur-1] = '\0';
		}
		cur++;
		if(ROXML_WHITE(chunk[cur+1])) {
			cur++;
			chunk[cur] = '\0';
		}
		xp_cond->arg2 = chunk+cur+1;
		if((xp_cond->arg2[0] > '9')||(xp_cond->arg2[0] < '0')) {
			xp_cond->func = ROXML_FUNC_STRCOMP;
		}
		cur++;
	} else if(ctx->bracket && !ctx->quoted && !ctx->dquoted) {
		if(ctx->new_cond->func != ROXML_FUNC_XPATH) {
			chunk[cur] = '\0';
			chunk[cur+1] = '\0';
			ctx->new_cond->op = ROXML_OPERATOR_DIFF;
			if(ROXML_WHITE(chunk[cur-1])) {
				chunk[cur-1] = '\0';
			}
			cur++;
			if(ROXML_WHITE(chunk[cur+1])) {
				cur++;
				chunk[cur] = '\0';
			}
			ctx->new_cond->arg2 = chunk+cur+1;
			if((ctx->new_cond->arg2[0] > '9')||(ctx->new_cond->arg2[0] < '0')) {
				ctx->new_cond->func = ROXML_FUNC_STRCOMP;
			}
			cur++;
		}
	}
	return cur;
}

int _func_number(char * chunk, void * data)
{
	roxml_xpath_ctx_t *ctx = (roxml_xpath_ctx_t*)data;
	int cur = 0;
	if(ctx->bracket && !ctx->quoted && !ctx->dquoted) {
		if((ctx->new_cond->func != ROXML_FUNC_XPATH) && (ctx->shorten_cond)){
			cur = 1;
			ctx->new_cond->func = ROXML_FUNC_POS;
			ctx->new_cond->op = ROXML_OPERATOR_EQU;
			ctx->new_cond->arg2 = chunk;
			while((chunk[cur+1] >= '0') && (chunk[cur+1] <= '9')) { cur++; }
		}
	}
	return cur;
}

int _func_position(char * chunk, void * data)
{
	roxml_xpath_ctx_t *ctx = (roxml_xpath_ctx_t*)data;
	int cur = 0;
	if(ctx->new_cond->func != ROXML_FUNC_XPATH) {
		int func = ROXML_FUNC_POS;
		cur += strlen(ROXML_FUNC_POS_STR)-1;
		ctx->new_cond->func = func;
	}
	return cur;
}

int _func_first(char * chunk, void * data)
{
	roxml_xpath_ctx_t *ctx = (roxml_xpath_ctx_t*)data;
	int cur = 0;
	if(ctx->new_cond->func != ROXML_FUNC_XPATH) {
		int func = ROXML_FUNC_FIRST;
		cur += strlen(ROXML_FUNC_FIRST_STR)-1;
		if(ctx->new_cond->op > 0) { ctx->new_cond->func2 = func; }
		else { ctx->new_cond->func = func; }
	}
	return cur;
}

int _func_last(char * chunk, void * data)
{
	roxml_xpath_ctx_t *ctx = (roxml_xpath_ctx_t*)data;
	int cur = 0;
	if(ctx->new_cond->func != ROXML_FUNC_XPATH) {
		int func = ROXML_FUNC_LAST;
		cur += strlen(ROXML_FUNC_LAST_STR)-1;
		if(ctx->new_cond->op > 0) { ctx->new_cond->func2 = func; }
		else { ctx->new_cond->func = func; }
	}
	return cur;
}

int _func_operator_add(char * chunk, void * data)
{
	roxml_xpath_ctx_t *ctx = (roxml_xpath_ctx_t*)data;
	int cur = 0;
	if(ctx->bracket && !ctx->quoted && !ctx->dquoted) {
		if(ctx->new_cond->func != ROXML_FUNC_XPATH) {
			if((ctx->new_cond->func == ROXML_FUNC_LAST)||(ctx->new_cond->func == ROXML_FUNC_FIRST)) {
				ctx->new_cond->op = ROXML_OPERATOR_ADD;
			}
			chunk[cur] = '\0';
			if(ROXML_WHITE(chunk[cur+1])) {
				cur++;
				chunk[cur] = '\0';
			}
			ctx->new_cond->arg2 = chunk+cur+1;
		}
	}
	return cur;
}

int _func_operator_subs(char * chunk, void * data)
{
	roxml_xpath_ctx_t *ctx = (roxml_xpath_ctx_t*)data;
	int cur = 0;
	if(ctx->bracket && !ctx->quoted && !ctx->dquoted) {
		if(ctx->new_cond->func != ROXML_FUNC_XPATH) {
			if((ctx->new_cond->func == ROXML_FUNC_LAST)||(ctx->new_cond->func == ROXML_FUNC_FIRST)) {
				ctx->new_cond->op = ROXML_OPERATOR_SUB;
			}
			chunk[cur] = '\0';
			if(ROXML_WHITE(chunk[cur+1])) {
				cur++;
				chunk[cur] = '\0';
			}
			ctx->new_cond->arg2 = chunk+cur+1;
		}
	}
	return cur;
}

int _func_default_xpath(char * chunk, void * data)
{
	int cur = 0;
	roxml_xpath_ctx_t *ctx = (roxml_xpath_ctx_t*)data;
	
	if((ctx->is_first_node)||(ctx->wait_first_node)) {
		if(!ctx->quoted && !ctx->dquoted && !ctx->parenthesys && !ctx->bracket)     {
			int offset = 0;
			xpath_node_t * tmp_node = (xpath_node_t*)calloc(1, sizeof(xpath_node_t));
			if((chunk[cur] == '/')&&(ctx->is_first_node)) { 
				free(tmp_node);
				ctx->new_node = ctx->first_node;
				ctx->first_node->abs = 1;
			} else if((chunk[cur] == '/')&&(ctx->wait_first_node)) { 
				free(tmp_node);
				ctx->first_node->abs = 1;
			} else if((ctx->is_first_node)||(ctx->wait_first_node)) {
				free(tmp_node);
			} else {
				if(ctx->new_node)    { ctx->new_node->next = tmp_node; }
				ctx->new_node = tmp_node;
			}
			ctx->is_first_node = 0;
			ctx->wait_first_node = 0;
			ctx->new_node = roxml_set_axes(ctx->new_node, chunk+cur, &offset);
			cur += offset;
		}
	} else if(ctx->bracket && !ctx->quoted && !ctx->dquoted) {
		if(ctx->new_cond->func != ROXML_FUNC_XPATH) {
			if(ctx->shorten_cond) {
				int bracket_lvl = 1;
				ctx->new_cond->func = ROXML_FUNC_XPATH;
				ctx->new_cond->arg1 = chunk+cur;
				while(bracket_lvl > 0) {
					if(chunk[cur] == '[') { bracket_lvl++; }
					else if(chunk[cur] == ']') { bracket_lvl--; }
					cur++;
				}
				cur--;
			}
		}
	}
	return cur>0?cur:0;
}

int _func_xpath_all(char * chunk, void * data)
{
	roxml_xpath_ctx_t *ctx = (roxml_xpath_ctx_t*)data;
	if(ctx->bracket && !ctx->quoted && !ctx->dquoted) {
		if(ctx->new_cond->func != ROXML_FUNC_XPATH) {
			if(ctx->shorten_cond) {
				ctx->shorten_cond--;
			}
		}
	}
	return 0;
}

