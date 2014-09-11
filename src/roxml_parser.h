#ifndef ROXML_PARSER_H
#define ROXML_PARSER_H

#include <roxml_internal.h>

/** \brief parser item creation function
 *
 * \fn roxml_append_parser_item(roxml_parser_item_t *head, char * key, roxml_parse_func func);
 * this function create a new parser item and append it to the parser list.
 * \param head the parser head list or NULL for first item
 * \param key the char to trigger callback for
 * \param func the function to call on match
 * \return the head item
 */
ROXML_INT roxml_parser_item_t *roxml_append_parser_item(roxml_parser_item_t * head, char *key, roxml_parse_func func);

/** \brief parser table deletion 
 *
 * \fn roxml_parser_free(roxml_parser_item_t *head);
 * this function delete a prepared parser object
 * \param head the parser object
 * \return
 */
ROXML_INT void roxml_parser_free(roxml_parser_item_t * head);

/** \brief parser preparation function
 *
 * \fn roxml_parser_prepare(roxml_parser_item_t *head);
 * this function compile a parser list into a table and calculate count variables for parsing optim
 * \param head the parser object
 * \return
 */
ROXML_INT roxml_parser_item_t *roxml_parser_prepare(roxml_parser_item_t * head);

/** \brief line parsing function
 *
 * \fn roxml_parse_line(roxml_parser_item_t * head, char *line, int len, void * ctx);
 * this function parse a line : it calls parsing functions when key matches
 * \param head the parser object
 * \param line the string to parse
 * \param len the len of string or 0 if auto calculate len (using \0)
 * \param ctx user data passed to the callbacks
 * \return the number of bytes processed
 */
ROXML_INT int roxml_parse_line(roxml_parser_item_t * head, char *line, int len, void *ctx);

#endif /* ROXML_PARSER_H */
