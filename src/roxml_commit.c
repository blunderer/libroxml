#include <stdlib.h>
#include <string.h>
#include <roxml_core.h>
#include <roxml_commit.h>

ROXML_INT void roxml_print_space(FILE * f, char **buf, int *offset, int *len, int lvl)
{
	if (lvl > 0) {
		int i = 0;

		if (buf && *buf) {
			int pos = *offset + lvl;
			if (pos >= *len) {
				*buf = realloc(*buf, *len + ROXML_BASE_LEN);
				memset(*buf + *len, 0, ROXML_BASE_LEN);
				*len += ROXML_BASE_LEN;
			}
			for (; i < lvl; i++) {
				strcat(*buf, " ");
			}
			*offset = pos;
		}
		if (f) {
			for (; i < lvl; i++) {
				fwrite(" ", 1, 1, f);
			}
		}
	}
}

ROXML_INT void roxml_write_string(char **buf, FILE * f, char *str, int *offset, int *len)
{
	int min_len = strlen(str);
	int pos = *offset + min_len;
	int appended_space = ROXML_BASE_LEN * ((int)(min_len / ROXML_BASE_LEN) + 1);

	if ((pos >= *len) && (buf) && (*buf)) {
		*buf = realloc(*buf, *len + appended_space);
		memset(*buf + *len, 0, appended_space);
		*len += appended_space;
	}
	if (f) {
		fprintf(f, "%s", str);
	}
	if (buf && *buf) {
		strcat(*buf + (*offset), str);
	}
	*offset = pos;
}

ROXML_INT void roxml_write_node(node_t *n, FILE * f, char **buf, int human, int lvl, int *offset, int *len)
{
	ROXML_GET_BASE_BUFFER(name);

	roxml_get_name(n, name, ROXML_BASE_LEN);
	if (human) {
		roxml_print_space(f, buf, offset, len, lvl);
	}
	if ((n->type & ROXML_NODE_TYPES) == ROXML_ELM_NODE) {
		ROXML_GET_BASE_BUFFER(ns);
		node_t *attr = n->attr;
		if (n->prnt) {
			roxml_write_string(buf, f, "<", offset, len);
			if (n->ns) {
				roxml_get_name(n->ns, ns, ROXML_BASE_LEN);
				if (ns[0] != '\0') {
					roxml_write_string(buf, f, ns, offset, len);
					roxml_write_string(buf, f, ":", offset, len);
				}
			}
			roxml_write_string(buf, f, name, offset, len);
		}
		while (attr) {
			int status = 0;
			char *value;
			char arg[ROXML_BASE_LEN];
			char val[ROXML_BASE_LEN];

			roxml_get_name(attr, arg, ROXML_BASE_LEN);
			value = roxml_get_content(attr, val, ROXML_BASE_LEN, &status);
			if (status >= ROXML_BASE_LEN) {
				value = roxml_get_content(attr, NULL, 0, &status);
			}
			roxml_write_string(buf, f, " ", offset, len);
			if (attr->type & ROXML_NS_NODE) {
				roxml_write_string(buf, f, "xmlns", offset, len);
				if (arg[0] != '\0') {
					roxml_write_string(buf, f, ":", offset, len);
				}
			}
			if (attr->ns) {
				ROXML_GET_BASE_BUFFER(arg_ns);
				roxml_get_name(attr->ns, arg_ns, ROXML_BASE_LEN);
				if (arg_ns[0] != '\0') {
					roxml_write_string(buf, f, arg_ns, offset, len);
					roxml_write_string(buf, f, ":", offset, len);
				}
				ROXML_PUT_BASE_BUFFER(arg_ns);
			}
			roxml_write_string(buf, f, arg, offset, len);
			roxml_write_string(buf, f, "=\"", offset, len);
			roxml_write_string(buf, f, value, offset, len);
			roxml_write_string(buf, f, "\"", offset, len);
			attr = attr->sibl;
			roxml_release(value);
		}
		if (n->chld) {
			node_t *chld = n->chld;
			if (n->prnt) {
				roxml_write_string(buf, f, ">", offset, len);
				if (human) {
					roxml_write_string(buf, f, "\n", offset, len);
				}
			}
			while (chld) {
				if (chld->type & ROXML_TXT_NODE) {
					ROXML_GET_BASE_BUFFER(val);
					char *value;
					int status;
					if (human) {
						roxml_print_space(f, buf, offset, len, lvl + 1);
					}
					value = roxml_get_content(chld, val, ROXML_BASE_LEN, &status);
					if (status >= ROXML_BASE_LEN) {
						value = roxml_get_content(chld, NULL, 0, &status);
					}
					if ((chld->type & ROXML_CDATA_NODE) == ROXML_CDATA_NODE) {
						roxml_write_string(buf, f, "<![CDATA[", offset, len);
						roxml_write_string(buf, f, value, offset, len);
						roxml_write_string(buf, f, "]]>", offset, len);
					} else {
						char *end = value + status - 2;
						char *strip = value;
						while (roxml_is_separator(end[0])) {
							end[0] = '\0';
							end--;
						}
						while (roxml_is_separator(strip[0])) {
							strip++;
						}

						roxml_write_string(buf, f, strip, offset, len);
					}
					if (human) {
						roxml_write_string(buf, f, "\n", offset, len);
					}
					chld = chld->sibl;
					roxml_release(value);
					ROXML_PUT_BASE_BUFFER(val);
				} else {
					roxml_write_node(chld, f, buf, human, lvl + 1, offset, len);
					chld = chld->sibl;
				}
			}
			if (n->prnt) {
				if (human) {
					roxml_print_space(f, buf, offset, len, lvl);
				}
				roxml_write_string(buf, f, "</", offset, len);
				if (n->ns) {
					if (ns[0] != '\0') {
						roxml_write_string(buf, f, ns, offset, len);
						roxml_write_string(buf, f, ":", offset, len);
					}
				}
				roxml_write_string(buf, f, name, offset, len);
				roxml_write_string(buf, f, ">", offset, len);
				if (human) {
					roxml_write_string(buf, f, "\n", offset, len);
				}
			}
		} else {
			roxml_write_string(buf, f, "/>", offset, len);
			if (human) {
				roxml_write_string(buf, f, "\n", offset, len);
			}
		}
		ROXML_PUT_BASE_BUFFER(ns);
	} else {
		char *value;
		int status;
		char head[8];
		char tail[8];

		ROXML_GET_BASE_BUFFER(val);

		if ((n->type & ROXML_NODE_TYPES) == ROXML_CMT_NODE) {
			strcpy(head, "<!--");
			strcpy(tail, "-->");
		} else if ((n->type & ROXML_NODE_TYPES) == ROXML_DOCTYPE_NODE) {
			strcpy(head, "<");
			strcpy(tail, ">");
		} else if ((n->type & ROXML_NODE_TYPES) == ROXML_PI_NODE) {
			strcpy(head, "<?");
			strcpy(tail, "?>");
		} else {
			head[0] = '\0';
			tail[0] = '\0';
		}

		roxml_write_string(buf, f, head, offset, len);

		if (name[0]) {
			roxml_write_string(buf, f, name, offset, len);
		}

		value = roxml_get_content(n, val, ROXML_BASE_LEN, &status);
		if (status >= ROXML_BASE_LEN) {
			value = roxml_get_content(n, NULL, 0, &status);
		}
		if (name[0] && value && value[0]) {
			roxml_write_string(buf, f, " ", offset, len);
		}
		roxml_write_string(buf, f, value, offset, len);
		roxml_release(value);

		roxml_write_string(buf, f, tail, offset, len);

		if (human) {
			roxml_write_string(buf, f, "\n", offset, len);
		}
		ROXML_PUT_BASE_BUFFER(val);
	}
	ROXML_PUT_BASE_BUFFER(name);
}

ROXML_API int roxml_commit_changes(node_t *n, char *dest, char **buffer, int human)
{
	int len = 0;
	int size = 0;

	if (n) {
		FILE *fout = NULL;

		if (dest) {
			fout = fopen(dest, "w");
		} else if (buffer) {
			*buffer = (char *)malloc(ROXML_BASE_LEN);
			memset(*buffer, 0, ROXML_BASE_LEN);
		}

		if (fout || buffer) {
			len = ROXML_BASE_LEN;

			if ((n->prnt == NULL) || (n->prnt && n->prnt->prnt == NULL)) {
				if (n->prnt) {
					n = n->prnt->chld;
				} else {
					n = n->chld;
				}
				while (n) {
					roxml_write_node(n, fout, buffer, human, 0, &size, &len);
					n = n->sibl;
				}
			} else {
				roxml_write_node(n, fout, buffer, human, 0, &size, &len);
			}
			if (buffer) {
				char *ptr = NULL;
				len -= ROXML_BASE_LEN;
				ptr = *buffer + len;
				len += strlen(ptr);
			} else if (fout) {
				len = ftell(fout);
				fclose(fout);
			}
		}
	}

	return len;
}
