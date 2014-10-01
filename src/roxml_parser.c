/**
 * \file roxml_parser.c
 * \brief Parsing engine
 *
 * (C) Copyright 2014
 * Tristan Lelong <tristan.lelong@libroxml.net>
 *
 * SPDX-Licence-Identifier:	LGPL-2.1+
 * The author added a static linking exception, see License.txt.
 */

#include <stdlib.h>
#include <string.h>
#include <roxml_parser.h>

/* #define DEBUG_PARSING */

ROXML_INT roxml_parser_item_t *roxml_append_parser_item(roxml_parser_item_t *head, char *key, roxml_parse_func func)
{
	roxml_parser_item_t *item = head;

	if (head == NULL) {
		item = (roxml_parser_item_t *)calloc(1, sizeof(roxml_parser_item_t));
		head = item;
	} else {
		item = head;
		while (item->next)
			item = item->next;
		item->next = (roxml_parser_item_t *)calloc(1, sizeof(roxml_parser_item_t));
		item = item->next;
	}
	item->chunk = key ? key[0] : 0;
	item->func = func;

	return head;
}

ROXML_INT void roxml_parser_free(roxml_parser_item_t *head)
{
	free(head);
}

/** \brief parser list deletion
 *
 * \fn roxml_parser_clear(roxml_parser_item_t *head);
 * this function delete a parser list (when not yet prepared)
 * \param head the parser object
 * \return
 */
ROXML_STATIC ROXML_INT void roxml_parser_clear(roxml_parser_item_t *head)
{
	roxml_parser_item_t *item = head;

	while (item) {
		roxml_parser_item_t *to_delete = item;
		item = item->next;
		free(to_delete);
	}

	return;
}

ROXML_INT roxml_parser_item_t *roxml_parser_prepare(roxml_parser_item_t *head)
{
	roxml_parser_item_t *item = head;
	roxml_parser_item_t *table = NULL;
	int count = 0;

	head->count = 0;
	head->def_count = 0;
	while (item) {
		if (item->chunk != 0) {
			head->count++;
		}
		head->def_count++;
		item = item->next;
	}

	table = malloc(sizeof(roxml_parser_item_t) * (head->def_count));

	item = head;

	while (item) {
		memcpy(&table[count], item, sizeof(roxml_parser_item_t));
		item = item->next;
		count++;
	}
	roxml_parser_clear(head);

	return table;
}

ROXML_INT int roxml_parse_line(roxml_parser_item_t *head, char *line, int len, void *ctx)
{
	int count = head->count;
	int def_count = head->def_count;
	char *line_end = line;
	char *chunk = line;

	if (len > 0) {
		line_end = line + len;
	} else {
		line_end = line + strlen(line);
	}

	while (chunk < line_end) {
		int i = 0;
		for (; i < count; i++) {
			if (chunk[0] == head[i].chunk) {
				int ret = head[i].func(chunk, ctx);
				if (ret > 0) {
					chunk += ret;
					break;
				} else if (ret < 0) {
					return -1;
				}
			}
		}
		for (; i >= count && i < def_count; i++) {
			int ret = head[i].func(chunk, ctx);
			if (ret > 0) {
				chunk += ret;
				break;
			} else if (ret < 0) {
				return -1;
			}
		}
	}

	return (chunk - line);
}
