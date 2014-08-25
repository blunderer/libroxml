#include <roxml_core.h>
#include <roxml_parser.h>
#include <roxml_internal.h>

ROXML_INT inline int roxml_read_buff(int pos, int size, char *buffer, node_t *node)
{
	int ret_len = 0;
	char *r1 = buffer;
	char const *r2 = node->src.buf + pos;

	while (size-- && (*r1++ = *r2++)) ;

	ret_len = r1 - buffer;

	return ret_len;
}

ROXML_INT inline void roxml_close_buff(node_t *n)
{
	return;
}

ROXML_INT int roxml_parse_buff(roxml_load_ctx_t *context, roxml_parser_item_t *parser, char *buffer)
{
	int ret = 0;                                                                                                                                           

	context->type = ROXML_BUFF;                                                                                                                             
	context->src = (void *)buffer;
	ret = roxml_parse_line(parser, buffer, 0, context);

	return (ret < 0) ? 1 : 0;
}

ROXML_API node_t *roxml_load_buf(char *buffer)
{
	node_t *current_node = NULL;
	if (buffer == NULL) {
		return NULL;
	}
	current_node = roxml_create_node(0, buffer, ROXML_ELM_NODE | ROXML_BUFF);
	current_node = roxml_append_node(NULL, current_node);
	return roxml_load(current_node, NULL, buffer);
}
