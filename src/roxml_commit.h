#ifndef ROXML_COMMIT_H
#define ROXML_COMMIT_H

#include <roxml_internal.h>

/** \brief space printing function
 *
 * \fn roxml_print_space(FILE *f, char ** buf, int * offset, int * len, int lvl);
 * this function add some space to output when committing change in human format
 * \param f the file pointer if any
 * \param buf the pointer to string if any
 * \param offset the current offset in stream
 * \param len the total len of buffer if any
 * \param lvl the level in the tree
 * \return
 */
ROXML_INT void roxml_print_space(FILE * f, char **buf, int *offset, int *len, int lvl);

/** \brief string writter function
 *
 * \fn roxml_write_string(char ** buf, FILE * f, char * str, int *offset, int * len);
 * this function write a string to output when committing change
 * \param f the file pointer if any
 * \param buf the pointer to string if any
 * \param str the string to write
 * \param offset the current offset in stream
 * \param len the total len of buffer if any
 * \return
 */
ROXML_INT void roxml_write_string(char **buf, FILE * f, char *str, int *offset, int *len);

/** \brief tree write function
 *
 * \fn roxml_write_node(node_t *n, FILE *f, char ** buf, int human, int lvl, int *offset, int *len);
 * this function write each node of the tree to output
 * \param n the node to write
 * \param f the file pointer if any
 * \param buf the pointer to the buffer string if any
 * \param human 1 to use the human format else 0
 * \param lvl the current level in tree
 * \param offset the current offset in stream
 * \param len the total len of buffer if any
 * \return
 */
ROXML_INT void roxml_write_node(node_t *n, FILE * f, char **buf, int human, int lvl, int *offset, int *len);

#endif /* ROXML_COMMIT_H */
