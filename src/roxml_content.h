#ifndef ROXML_CONTENT_H
#define ROXML_CONTENT_H

#include <roxml_internal.h>

/** \brief read xml doc function
 *
 * \fn roxml_read(int pos, int size, char * buffer, node_t *node)
 * this function read inside a xml doc (file or buffer) and fill the given buffer
 * \param pos the pos in the xml document
 * \param size the size of the data to read
 * \param buffer the destination buffer
 * \param node the node that belong to the tree we want to read to
 * \return the number of bytes read
 */
ROXML_INT int roxml_read(int pos, int size, char *buffer, node_t *node);

#endif /* ROXML_CONTENT_H */
