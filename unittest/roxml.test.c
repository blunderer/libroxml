
#include "unittest.h"		
#include "roxml-internal.h"

int test_load_doc(void)
{
	INIT /* init context macro */

	node_t *root = roxml_load_doc("roxml.test.xml");
	ASSERT_NOT_NULL(root);
	
	roxml_close(root);

	root = roxml_load_doc("empty.test.xml");
	ASSERT_NULL(root);
	
	RETURN /* close context macro */
}

int test_load_fd(void)
{
	INIT /* init context macro */

	int fd = open("roxml.test.xml", O_RDONLY);
	node_t *root = roxml_load_fd(fd);
	ASSERT_NOT_NULL(root);
	
	roxml_close(root);

	fd = open("empty.test.xml", O_RDONLY);
	root = roxml_load_fd(fd);
	ASSERT_NULL(root);
	
	RETURN /* close context macro */
}

int test_tree_on_doc(void)
{
	INIT /* init context macro */

	node_t *root = roxml_load_doc("roxml.test.xml");
	
	ASSERT_NOT_NULL(root);

	ASSERT_NOT_NULL(root->chld); // node0
	ASSERT_NOT_NULL(root->chld->chld);	// node1
	ASSERT_NOT_NULL(root->chld->chld->sibl);	// node2
	ASSERT_NOT_NULL(root->chld->chld->sibl->sibl);	// node7
	ASSERT_NOT_NULL(root->chld->chld->sibl->chld);	//node3
	ASSERT_NOT_NULL(root->chld->chld->sibl->chld->sibl);	//node4
	ASSERT_NOT_NULL(root->chld->chld->sibl->chld->sibl->sibl);	//node5
	ASSERT_NOT_NULL(root->chld->chld->sibl->chld->sibl->sibl->chld);	//node6
	ASSERT_NOT_NULL(root->chld->chld->sibl->sibl->chld);	//node8
	ASSERT_NULL(root->chld->chld->sibl->sibl->chld->chld);
	ASSERT_NULL(root->chld->chld->sibl->sibl->chld->sibl);
	ASSERT_NULL(root->chld->chld->sibl->chld->sibl->sibl->chld->sibl);
	ASSERT_NULL(root->chld->chld->sibl->chld->sibl->sibl->chld->chld);
	ASSERT_NULL(root->chld->chld->sibl->chld->sibl->chld);
	ASSERT_NULL(root->chld->chld->sibl->chld->chld);
	ASSERT_NULL(root->chld->chld->sibl->chld->sibl->sibl->sibl);
	ASSERT_NULL(root->chld->chld->sibl->sibl->sibl);
	ASSERT_NULL(root->chld->chld->chld);
	ASSERT_NULL(root->sibl);

	// node0
	ASSERT_NULL(root->chld->attr)

	// node1
	ASSERT_NOT_NULL(root->chld->chld->attr)
	ASSERT_NOT_NULL(root->chld->chld->attr->sibl)
	ASSERT_NULL(root->chld->chld->attr->sibl->sibl)

	// node2
	ASSERT_NOT_NULL(root->chld->chld->sibl->attr)
	ASSERT_NOT_NULL(root->chld->chld->sibl->attr->sibl)
	ASSERT_NULL(root->chld->chld->sibl->attr->sibl->sibl)

	// node3
	ASSERT_NULL(root->chld->chld->sibl->chld->attr)

	// node4
	ASSERT_NOT_NULL(root->chld->chld->sibl->chld->sibl->attr)
	ASSERT_NOT_NULL(root->chld->chld->sibl->chld->sibl->attr->sibl)
	ASSERT_NULL(root->chld->chld->sibl->chld->sibl->attr->sibl->sibl)

	// node5
	ASSERT_NULL(root->chld->chld->sibl->chld->sibl->sibl->attr)

	// node6
	ASSERT_NULL(root->chld->chld->sibl->chld->sibl->sibl->chld->attr)

	// node7
	ASSERT_NULL(root->chld->chld->sibl->sibl->attr)

#ifdef IGNORE_EMPTY_TEXT_NODES
	// node0
	ASSERT_NULL(root->chld->text)

	// node1
	ASSERT_NOT_NULL(root->chld->chld->text)
	ASSERT_NULL(root->chld->chld->text->sibl)

	// node2
	ASSERT_NULL(root->chld->chld->sibl->text)

	// node3
	ASSERT_NOT_NULL(root->chld->chld->sibl->chld->text)
	ASSERT_NULL(root->chld->chld->sibl->chld->text->sibl)

	// node4
	ASSERT_NULL(root->chld->chld->sibl->chld->sibl->text)

	// node5
	ASSERT_NOT_NULL(root->chld->chld->sibl->chld->sibl->sibl->text)
	ASSERT_NOT_NULL(root->chld->chld->sibl->chld->sibl->sibl->text->sibl)
	ASSERT_NULL(root->chld->chld->sibl->chld->sibl->sibl->text->sibl->sibl)

	// node6
	ASSERT_NOT_NULL(root->chld->chld->sibl->chld->sibl->sibl->chld->text)
	ASSERT_NULL(root->chld->chld->sibl->chld->sibl->sibl->chld->text->sibl)

	// node7
	ASSERT_NOT_NULL(root->chld->chld->sibl->sibl->text)
	ASSERT_NOT_NULL(root->chld->chld->sibl->sibl->text->sibl)
	ASSERT_NULL(root->chld->chld->sibl->sibl->text->sibl->sibl)
#else /* !IGNORE_EMPTY_TEXT_NODES */
	ASSERT_NOT_NULL(root->chld->text)
	ASSERT_NOT_NULL(root->chld->text->sibl)
	ASSERT_NOT_NULL(root->chld->text->sibl->sibl)
	ASSERT_NOT_NULL(root->chld->text->sibl->sibl->sibl)
	ASSERT_NULL(root->chld->text->sibl->sibl->sibl->sibl)

	// node1
	ASSERT_NOT_NULL(root->chld->chld->text)
	ASSERT_NULL(root->chld->chld->text->sibl)

	// node2
	ASSERT_NOT_NULL(root->chld->chld->sibl->text)
	ASSERT_NOT_NULL(root->chld->chld->sibl->text->sibl)
	ASSERT_NOT_NULL(root->chld->chld->sibl->text->sibl->sibl)
	ASSERT_NOT_NULL(root->chld->chld->sibl->text->sibl->sibl->sibl)
	ASSERT_NULL(root->chld->chld->sibl->text->sibl->sibl->sibl->sibl)

	// node3
	ASSERT_NOT_NULL(root->chld->chld->sibl->chld->text)
	ASSERT_NULL(root->chld->chld->sibl->chld->text->sibl)

	// node4
	ASSERT_NULL(root->chld->chld->sibl->chld->sibl->text)

	// node5
	ASSERT_NOT_NULL(root->chld->chld->sibl->chld->sibl->sibl->text)
	ASSERT_NOT_NULL(root->chld->chld->sibl->chld->sibl->sibl->text->sibl)
	ASSERT_NULL(root->chld->chld->sibl->chld->sibl->sibl->text->sibl->sibl)

	// node6
	ASSERT_NOT_NULL(root->chld->chld->sibl->chld->sibl->sibl->chld->text)
	ASSERT_NULL(root->chld->chld->sibl->chld->sibl->sibl->chld->text->sibl)

	// node7
	ASSERT_NOT_NULL(root->chld->chld->sibl->sibl->text)
	ASSERT_NULL(root->chld->chld->sibl->sibl->text->sibl)
#endif /* !IGNORE_EMPTY_TEXT_NODES */

	roxml_close(root);

	RETURN /* close context macro */
}

int test_names_on_doc(void)
{
	INIT /* init context macro */

	char string[128];
	node_t *root = roxml_load_doc("roxml.test.xml");
	
	ASSERT_STRING_EQUAL(roxml_get_name(root->chld, NULL, 0), "node0"); // node0
	ASSERT_STRING_EQUAL(roxml_get_name(root->chld->chld, NULL, 0), "node1");	// node1
	ASSERT_STRING_EQUAL(roxml_get_name(root->chld->chld->sibl, NULL, 0), "node2");	// node2
	ASSERT_STRING_EQUAL(roxml_get_name(root->chld->chld->sibl->sibl, NULL, 0), "node7");	// node7
	ASSERT_STRING_EQUAL(roxml_get_name(root->chld->chld->sibl->chld, NULL, 0), "node3");	//node3
	ASSERT_STRING_EQUAL(roxml_get_name(root->chld->chld->sibl->chld->sibl, NULL, 0), "node4");	//node4
	ASSERT_STRING_EQUAL(roxml_get_name(root->chld->chld->sibl->chld->sibl->sibl, NULL, 0), "node5");	//node5
	ASSERT_STRING_EQUAL(roxml_get_name(root->chld->chld->sibl->chld->sibl->sibl->chld, NULL, 0), "node6");	//node6

	ASSERT_NULL(roxml_get_name(root->chld->attr, NULL, 0))

	ASSERT_STRING_EQUAL(roxml_get_name(root->chld->chld->attr, NULL, 0), "name")
	ASSERT_STRING_EQUAL(roxml_get_name(root->chld->chld->attr->sibl, NULL, 0), "value")
	ASSERT_STRING_EQUAL(roxml_get_name(root->chld->chld->sibl->attr, NULL, 0), "name")
	ASSERT_STRING_EQUAL(roxml_get_name(root->chld->chld->sibl->attr->sibl, NULL, 0), "value")
	ASSERT_STRING_EQUAL(roxml_get_name(root->chld->chld->sibl->chld->sibl->attr, NULL, 0), "name")
	ASSERT_STRING_EQUAL(roxml_get_name(root->chld->chld->sibl->chld->sibl->attr->sibl, NULL, 0), "value")

	ASSERT_EQUAL(roxml_get_name(root->chld->chld->sibl->chld->sibl->attr->sibl, string, 128), string)
	ASSERT_STRING_EQUAL(string, "value")

	roxml_release(RELEASE_ALL);
	roxml_close(root);

	RETURN /* close context macro */
}

int test_load_buf(void)
{
	INIT /* init context macro */

	FILE * doc = NULL;
	char buf[2048];

	memset(buf, 0, sizeof(char)*2048);
	doc = fopen("roxml.test.xml", "r");
	
	ASSERT_NOT_NULL(doc)
	if(doc)	{
		fread(buf, 2048, 1, doc);
		fclose(doc);
	}

	node_t *root = roxml_load_buf(buf);
	ASSERT_NOT_NULL(root);
	
	roxml_close(root);

	root = roxml_load_buf(NULL);
	ASSERT_NULL(root);
	
	RETURN /* close context macro */
}

int test_tree_on_buf(void)
{
	INIT /* init context macro */

	FILE * doc = NULL;
	char buf[2048];

	memset(buf, 0, sizeof(char)*2048);
	doc = fopen("roxml.test.xml", "r");
	
	ASSERT_NOT_NULL(doc)
	if(doc)	{
		fread(buf, 2048, 1, doc);
		fclose(doc);
	} else	{
		ASSERT_NOT_NULL(NULL);
		RETURN
	}

	node_t *root = roxml_load_buf(buf);
	
	ASSERT_NOT_NULL(root);

	ASSERT_NOT_NULL(root->chld); // node0
	ASSERT_NOT_NULL(root->chld->chld);	// node1
	ASSERT_NOT_NULL(root->chld->chld->sibl);	// node2
	ASSERT_NOT_NULL(root->chld->chld->sibl->sibl);	// node7
	ASSERT_NOT_NULL(root->chld->chld->sibl->chld);	//node3
	ASSERT_NOT_NULL(root->chld->chld->sibl->chld->sibl);	//node4
	ASSERT_NOT_NULL(root->chld->chld->sibl->chld->sibl->sibl);	//node5
	ASSERT_NOT_NULL(root->chld->chld->sibl->chld->sibl->sibl->chld);	//node6
	ASSERT_NOT_NULL(root->chld->chld->sibl->sibl->chld);
	ASSERT_NULL(root->chld->chld->sibl->sibl->chld->chld);
	ASSERT_NULL(root->chld->chld->sibl->sibl->chld->sibl);
	ASSERT_NULL(root->chld->chld->sibl->chld->sibl->sibl->chld->sibl);
	ASSERT_NULL(root->chld->chld->sibl->chld->sibl->sibl->chld->chld);
	ASSERT_NULL(root->chld->chld->sibl->chld->sibl->chld);
	ASSERT_NULL(root->chld->chld->sibl->chld->chld);
	ASSERT_NULL(root->chld->chld->sibl->chld->sibl->sibl->sibl);
	ASSERT_NULL(root->chld->chld->sibl->sibl->sibl);
	ASSERT_NULL(root->chld->chld->chld);
	ASSERT_NULL(root->sibl);

	// node0
	ASSERT_NULL(root->chld->attr)

	// node1
	ASSERT_NOT_NULL(root->chld->chld->attr)
	ASSERT_NOT_NULL(root->chld->chld->attr->sibl)
	ASSERT_NULL(root->chld->chld->attr->sibl->sibl)

	// node2
	ASSERT_NOT_NULL(root->chld->chld->sibl->attr)
	ASSERT_NOT_NULL(root->chld->chld->sibl->attr->sibl)
	ASSERT_NULL(root->chld->chld->sibl->attr->sibl->sibl)

	// node3
	ASSERT_NULL(root->chld->chld->sibl->chld->attr)

	// node4
	ASSERT_NOT_NULL(root->chld->chld->sibl->chld->sibl->attr)
	ASSERT_NOT_NULL(root->chld->chld->sibl->chld->sibl->attr->sibl)
	ASSERT_NULL(root->chld->chld->sibl->chld->sibl->attr->sibl->sibl)

	// node5
	ASSERT_NULL(root->chld->chld->sibl->chld->sibl->sibl->attr)

	// node6
	ASSERT_NULL(root->chld->chld->sibl->chld->sibl->sibl->chld->attr)

	// node7
	ASSERT_NULL(root->chld->chld->sibl->sibl->attr)

	roxml_close(root);

	RETURN /* close context macro */
}

int test_names_on_buf(void)
{
	INIT /* init context macro */

	FILE * doc = NULL;
	char buf[2048];
	char string[128];

	memset(buf, 0, sizeof(char)*2048);
	doc = fopen("roxml.test.xml", "r");
	
	ASSERT_NOT_NULL(doc)
	if(doc)	{
		fread(buf, 2048, 1, doc);
		fclose(doc);
	}

	node_t *root = roxml_load_buf(buf);
	
	ASSERT_STRING_EQUAL(roxml_get_name(root->chld, NULL, 0), "node0"); // node0
	ASSERT_STRING_EQUAL(roxml_get_name(root->chld->chld, NULL, 0), "node1");	// node1
	ASSERT_STRING_EQUAL(roxml_get_name(root->chld->chld->sibl, NULL, 0), "node2");	// node2
	ASSERT_STRING_EQUAL(roxml_get_name(root->chld->chld->sibl->sibl, NULL, 0), "node7");	// node7
	ASSERT_STRING_EQUAL(roxml_get_name(root->chld->chld->sibl->chld, NULL, 0), "node3");	//node3
	ASSERT_STRING_EQUAL(roxml_get_name(root->chld->chld->sibl->chld->sibl, NULL, 0), "node4");	//node4
	ASSERT_STRING_EQUAL(roxml_get_name(root->chld->chld->sibl->chld->sibl->sibl, NULL, 0), "node5");	//node5
	ASSERT_STRING_EQUAL(roxml_get_name(root->chld->chld->sibl->chld->sibl->sibl->chld, NULL, 0), "node6");	//node6

	ASSERT_NULL(roxml_get_name(root->chld->attr, NULL, 0))

	ASSERT_STRING_EQUAL(roxml_get_name(root->chld->chld->attr, NULL, 0), "name")
	ASSERT_STRING_EQUAL(roxml_get_name(root->chld->chld->attr->sibl, NULL, 0), "value")
	ASSERT_STRING_EQUAL(roxml_get_name(root->chld->chld->sibl->attr, NULL, 0), "name")
	ASSERT_STRING_EQUAL(roxml_get_name(root->chld->chld->sibl->attr->sibl, NULL, 0), "value")
	ASSERT_STRING_EQUAL(roxml_get_name(root->chld->chld->sibl->chld->sibl->attr, NULL, 0), "name")
	ASSERT_STRING_EQUAL(roxml_get_name(root->chld->chld->sibl->chld->sibl->attr->sibl, NULL, 0), "value")

	ASSERT_EQUAL(roxml_get_name(root->chld->chld->sibl->chld->sibl->attr->sibl, string, 128), string)
	ASSERT_STRING_EQUAL(string, "value")

	roxml_release(RELEASE_ALL);
	roxml_close(root);

	RETURN /* close context macro */
}

int test_load_human_doc(void)
{
	INIT /* init context macro */

	node_t *root = roxml_load_doc("roxml.test.xml.human");
	ASSERT_NOT_NULL(root);
	
	roxml_close(root);

	root = roxml_load_doc("empty.test.xml");
	ASSERT_NULL(root);
	
	RETURN /* close context macro */
}

int test_tree_on_human_doc(void)
{
	INIT /* init context macro */

	node_t *root = roxml_load_doc("roxml.test.xml.human");
	
	ASSERT_NOT_NULL(root);

	ASSERT_NOT_NULL(root->chld); // node0
	ASSERT_NOT_NULL(root->chld->chld);	// node1
	ASSERT_NOT_NULL(root->chld->chld->sibl);	// node2
	ASSERT_NOT_NULL(root->chld->chld->sibl->sibl);	// node7
	ASSERT_NOT_NULL(root->chld->chld->sibl->chld);	//node3
	ASSERT_NOT_NULL(root->chld->chld->sibl->chld->sibl);	//node4
	ASSERT_NOT_NULL(root->chld->chld->sibl->chld->sibl->sibl);	//node5
	ASSERT_NOT_NULL(root->chld->chld->sibl->chld->sibl->sibl->chld);	//node6
	ASSERT_NOT_NULL(root->chld->chld->sibl->sibl->chld);
	ASSERT_NULL(root->chld->chld->sibl->sibl->chld->chld);
	ASSERT_NULL(root->chld->chld->sibl->sibl->chld->sibl);
	ASSERT_NULL(root->chld->chld->sibl->chld->sibl->sibl->chld->sibl);
	ASSERT_NULL(root->chld->chld->sibl->chld->sibl->sibl->chld->chld);
	ASSERT_NULL(root->chld->chld->sibl->chld->sibl->chld);
	ASSERT_NULL(root->chld->chld->sibl->chld->chld);
	ASSERT_NULL(root->chld->chld->sibl->chld->sibl->sibl->sibl);
	ASSERT_NULL(root->chld->chld->sibl->sibl->sibl);
	ASSERT_NULL(root->chld->chld->chld);
	ASSERT_NULL(root->sibl);

	// node0
	ASSERT_NULL(root->chld->attr)

	// node1
	ASSERT_NOT_NULL(root->chld->chld->attr)
	ASSERT_NOT_NULL(root->chld->chld->attr->sibl)
	ASSERT_NULL(root->chld->chld->attr->sibl->sibl)

	// node2
	ASSERT_NOT_NULL(root->chld->chld->sibl->attr)
	ASSERT_NOT_NULL(root->chld->chld->sibl->attr->sibl)
	ASSERT_NULL(root->chld->chld->sibl->attr->sibl->sibl)

	// node3
	ASSERT_NULL(root->chld->chld->sibl->chld->attr)

	// node4
	ASSERT_NOT_NULL(root->chld->chld->sibl->chld->sibl->attr)
	ASSERT_NOT_NULL(root->chld->chld->sibl->chld->sibl->attr->sibl)
	ASSERT_NULL(root->chld->chld->sibl->chld->sibl->attr->sibl->sibl)

	// node5
	ASSERT_NULL(root->chld->chld->sibl->chld->sibl->sibl->attr)

	// node6
	ASSERT_NULL(root->chld->chld->sibl->chld->sibl->sibl->chld->attr)

	// node7
	ASSERT_NULL(root->chld->chld->sibl->sibl->attr)

	roxml_close(root);

	RETURN /* close context macro */
}

int test_names_on_human_doc(void)
{
	INIT /* init context macro */

	char string[128];
	node_t *root = roxml_load_doc("roxml.test.xml.human");
	
	ASSERT_STRING_EQUAL(roxml_get_name(root->chld, NULL, 0), "node0"); // node0
	ASSERT_STRING_EQUAL(roxml_get_name(root->chld->chld, NULL, 0), "node1");	// node1
	ASSERT_STRING_EQUAL(roxml_get_name(root->chld->chld->sibl, NULL, 0), "node2");	// node2
	ASSERT_STRING_EQUAL(roxml_get_name(root->chld->chld->sibl->sibl, NULL, 0), "node7");	// node7
	ASSERT_STRING_EQUAL(roxml_get_name(root->chld->chld->sibl->chld, NULL, 0), "node3");	//node3
	ASSERT_STRING_EQUAL(roxml_get_name(root->chld->chld->sibl->chld->sibl, NULL, 0), "node4");	//node4
	ASSERT_STRING_EQUAL(roxml_get_name(root->chld->chld->sibl->chld->sibl->sibl, NULL, 0), "node5");	//node5
	ASSERT_STRING_EQUAL(roxml_get_name(root->chld->chld->sibl->chld->sibl->sibl->chld, NULL, 0), "node6");	//node6

	ASSERT_NULL(roxml_get_name(root->chld->attr, NULL, 0))

	ASSERT_STRING_EQUAL(roxml_get_name(root->chld->chld->attr, NULL, 0), "name")
	ASSERT_STRING_EQUAL(roxml_get_name(root->chld->chld->attr->sibl, NULL, 0), "value")
	ASSERT_STRING_EQUAL(roxml_get_name(root->chld->chld->sibl->attr, NULL, 0), "name")
	ASSERT_STRING_EQUAL(roxml_get_name(root->chld->chld->sibl->attr->sibl, NULL, 0), "value")
	ASSERT_STRING_EQUAL(roxml_get_name(root->chld->chld->sibl->chld->sibl->attr, NULL, 0), "name")
	ASSERT_STRING_EQUAL(roxml_get_name(root->chld->chld->sibl->chld->sibl->attr->sibl, NULL, 0), "value")

	ASSERT_EQUAL(roxml_get_name(root->chld->chld->sibl->chld->sibl->attr->sibl, string, 128), string)
	ASSERT_STRING_EQUAL(string, "value")

	roxml_release(RELEASE_ALL);
	roxml_close(root);

	RETURN /* close context macro */
}

int test_load_human_buf(void)
{
	INIT /* init context macro */

	FILE * doc = NULL;
	char buf[2048];

	memset(buf, 0, sizeof(char)*2048);
	doc = fopen("roxml.test.xml.human", "r");
	
	ASSERT_NOT_NULL(doc)
	if(doc)	{
		fread(buf, 2048, 1, doc);
		fclose(doc);
	}

	node_t *root = roxml_load_buf(buf);
	ASSERT_NOT_NULL(root);
	
	roxml_close(root);

	root = roxml_load_buf(NULL);
	ASSERT_NULL(root);
	
	RETURN /* close context macro */
}

int test_tree_on_human_buf(void)
{
	INIT /* init context macro */

	FILE * doc = NULL;
	char buf[2048];

	memset(buf, 0, sizeof(char)*2048);
	doc = fopen("roxml.test.xml.human", "r");
	
	ASSERT_NOT_NULL(doc)
	if(doc)	{
		fread(buf, 2048, 1, doc);
		fclose(doc);
	}

	node_t *root = roxml_load_buf(buf);
	
	ASSERT_NOT_NULL(root);

	ASSERT_NOT_NULL(root->chld); // node0
	ASSERT_NOT_NULL(root->chld->chld);	// node1
	ASSERT_NOT_NULL(root->chld->chld->sibl);	// node2
	ASSERT_NOT_NULL(root->chld->chld->sibl->sibl);	// node7
	ASSERT_NOT_NULL(root->chld->chld->sibl->chld);	//node3
	ASSERT_NOT_NULL(root->chld->chld->sibl->chld->sibl);	//node4
	ASSERT_NOT_NULL(root->chld->chld->sibl->chld->sibl->sibl);	//node5
	ASSERT_NOT_NULL(root->chld->chld->sibl->chld->sibl->sibl->chld);	//node6
	ASSERT_NOT_NULL(root->chld->chld->sibl->sibl->chld);
	ASSERT_NULL(root->chld->chld->sibl->sibl->chld->chld);
	ASSERT_NULL(root->chld->chld->sibl->sibl->chld->sibl);
	ASSERT_NULL(root->chld->chld->sibl->chld->sibl->sibl->chld->sibl);
	ASSERT_NULL(root->chld->chld->sibl->chld->sibl->sibl->chld->chld);
	ASSERT_NULL(root->chld->chld->sibl->chld->sibl->chld);
	ASSERT_NULL(root->chld->chld->sibl->chld->chld);
	ASSERT_NULL(root->chld->chld->sibl->chld->sibl->sibl->sibl);
	ASSERT_NULL(root->chld->chld->sibl->sibl->sibl);
	ASSERT_NULL(root->chld->chld->chld);
	ASSERT_NULL(root->sibl);

	// node0
	ASSERT_NULL(root->chld->attr)

	// node1
	ASSERT_NOT_NULL(root->chld->chld->attr)
	ASSERT_NOT_NULL(root->chld->chld->attr->sibl)
	ASSERT_NULL(root->chld->chld->attr->sibl->sibl)

	// node2
	ASSERT_NOT_NULL(root->chld->chld->sibl->attr)
	ASSERT_NOT_NULL(root->chld->chld->sibl->attr->sibl)
	ASSERT_NULL(root->chld->chld->sibl->attr->sibl->sibl)

	// node3
	ASSERT_NULL(root->chld->chld->sibl->chld->attr)

	// node4
	ASSERT_NOT_NULL(root->chld->chld->sibl->chld->sibl->attr)
	ASSERT_NOT_NULL(root->chld->chld->sibl->chld->sibl->attr->sibl)
	ASSERT_NULL(root->chld->chld->sibl->chld->sibl->attr->sibl->sibl)

	// node5
	ASSERT_NULL(root->chld->chld->sibl->chld->sibl->sibl->attr)

	// node6
	ASSERT_NULL(root->chld->chld->sibl->chld->sibl->sibl->chld->attr)

	// node7
	ASSERT_NULL(root->chld->chld->sibl->sibl->attr)

	roxml_close(root);

	RETURN /* close context macro */
}

int test_names_on_human_buf(void)
{
	INIT /* init context macro */

	FILE * doc = NULL;
	char buf[2048];
	char string[128];

	memset(buf, 0, sizeof(char)*2048);
	doc = fopen("roxml.test.xml.human", "r");
	
	ASSERT_NOT_NULL(doc)
	if(doc)	{
		fread(buf, 2048, 1, doc);
		fclose(doc);
	}

	node_t *root = roxml_load_buf(buf);
	
	ASSERT_STRING_EQUAL(roxml_get_name(root->chld, NULL, 0), "node0"); // node0
	ASSERT_STRING_EQUAL(roxml_get_name(root->chld->chld, NULL, 0), "node1");	// node1
	ASSERT_STRING_EQUAL(roxml_get_name(root->chld->chld->sibl, NULL, 0), "node2");	// node2
	ASSERT_STRING_EQUAL(roxml_get_name(root->chld->chld->sibl->sibl, NULL, 0), "node7");	// node7
	ASSERT_STRING_EQUAL(roxml_get_name(root->chld->chld->sibl->chld, NULL, 0), "node3");	//node3
	ASSERT_STRING_EQUAL(roxml_get_name(root->chld->chld->sibl->chld->sibl, NULL, 0), "node4");	//node4
	ASSERT_STRING_EQUAL(roxml_get_name(root->chld->chld->sibl->chld->sibl->sibl, NULL, 0), "node5");	//node5
	ASSERT_STRING_EQUAL(roxml_get_name(root->chld->chld->sibl->chld->sibl->sibl->chld, NULL, 0), "node6");	//node6

	ASSERT_NULL(roxml_get_name(root->chld->attr, NULL, 0))

	ASSERT_STRING_EQUAL(roxml_get_name(root->chld->chld->attr, NULL, 0), "name")
	ASSERT_STRING_EQUAL(roxml_get_name(root->chld->chld->attr->sibl, NULL, 0), "value")
	ASSERT_STRING_EQUAL(roxml_get_name(root->chld->chld->sibl->attr, NULL, 0), "name")
	ASSERT_STRING_EQUAL(roxml_get_name(root->chld->chld->sibl->attr->sibl, NULL, 0), "value")
	ASSERT_STRING_EQUAL(roxml_get_name(root->chld->chld->sibl->chld->sibl->attr, NULL, 0), "name")
	ASSERT_STRING_EQUAL(roxml_get_name(root->chld->chld->sibl->chld->sibl->attr->sibl, NULL, 0), "value")

	ASSERT_EQUAL(roxml_get_name(root->chld->chld->sibl->chld->sibl->attr->sibl, string, 128), string)
	ASSERT_STRING_EQUAL(string, "value")

	roxml_release(RELEASE_ALL);
	roxml_close(root);

	RETURN /* close context macro */
}

int test_malloc_release(void)
{
	INIT /* init context macro */

	roxml_release(RELEASE_ALL);
	ASSERT_NULL(head_cell.next);

	memory_cell_t *ptr = &head_cell;
	char * ptr1 = roxml_malloc(1, 10, PTR_CHAR);
	char * ptr2 = roxml_malloc(1, 10, PTR_CHAR);
	char * ptr3 = roxml_malloc(1, 10, PTR_CHAR);
	char * ptr4 = roxml_malloc(1, 10, PTR_CHAR);

	ASSERT_EQUAL(ptr->next->ptr, ptr1);
	ASSERT_EQUAL(ptr->next->next->ptr, ptr2);
	ASSERT_EQUAL(ptr->next->next->next->ptr, ptr3);
	ASSERT_EQUAL(ptr->next->next->next->next->ptr, ptr4);
	ASSERT_NULL(ptr->next->next->next->next->next);

	roxml_release(ptr4);
	ptr = head_cell.next;
	while(ptr)	{
		ASSERT_NOT_EQUAL(ptr->ptr, ptr4);
		ptr = ptr->next;
	}

	roxml_release(RELEASE_LAST);
	ptr = head_cell.next;
	while(ptr)	{
		ASSERT_NOT_EQUAL(ptr->ptr, ptr3);
		ptr = ptr->next;
	}

	roxml_release(ptr1);
	ptr = head_cell.next;
	while(ptr)	{
		ASSERT_NOT_EQUAL(ptr->ptr, ptr1);
		ptr = ptr->next;
	}

	roxml_release(RELEASE_ALL);
	ASSERT_NULL(head_cell.next);
	roxml_release(RELEASE_ALL);
	ASSERT_NULL(head_cell.next);
	roxml_release(RELEASE_LAST);
	ASSERT_NULL(head_cell.next);

	RETURN /* close context macro */
}

int test_create_node(void)
{
	INIT

	FILE * doc = (FILE *)0x42;
	char * buf = (char *)0x44;

	node_t * node = roxml_create_node(1, buf, ROXML_FILE | ROXML_ELM_NODE);
	node_t * cnode = roxml_create_node(1, doc, ROXML_FILE | ROXML_ELM_NODE);
	node_t * anode = roxml_create_node(1, doc, ROXML_FILE | ROXML_ATTR_NODE);
	node_t * close = roxml_create_node(10, doc, ROXML_FILE | ROXML_ELM_NODE);
	roxml_close_node(node, close);
	ASSERT_EQUAL(node->type, ROXML_FILE | ROXML_ELM_NODE)
	ASSERT_EQUAL(node->src.buf, buf)
	ASSERT_EQUAL(node->pos, 1)
	ASSERT_EQUAL(node->end, 10)
	ASSERT_NULL(node->prnt)
	ASSERT_NULL(node->sibl)
	ASSERT_NULL(node->chld)
	ASSERT_NULL(node->attr)

	roxml_parent_node(cnode, node);
	roxml_parent_node(node, anode);
	ASSERT_EQUAL(node->prnt, cnode)
	ASSERT_EQUAL(anode->prnt, node)
	ASSERT_EQUAL(cnode->chld, node)
	ASSERT_EQUAL(node->attr, anode)

	roxml_parent_node(cnode, node);
	roxml_parent_node(node, anode);
	ASSERT_EQUAL(node->sibl, node)
	ASSERT_EQUAL(anode->sibl, anode)

	roxml_free_node(node);
	roxml_free_node(cnode);
	roxml_free_node(anode);

	RETURN
}

int test_get_root(void)
{
	INIT
	node_t *root = roxml_load_doc("roxml.test.xml");

	ASSERT_STRING_EQUAL(roxml_get_name(root, NULL, 0), "documentRoot")

	node_t * node0 = roxml_get_chld(root, NULL, 0);
	ASSERT_STRING_EQUAL(roxml_get_name(node0, NULL, 0), "node0")
	node_t * node1 = roxml_get_chld(node0, NULL, 0);

	roxml_release(RELEASE_ALL);
	roxml_close(root);

	root = roxml_load_doc("roxml.test.xml.invalid");

	ASSERT_STRING_EQUAL(roxml_get_name(root, NULL, 0), "documentRoot")

	node0 = roxml_get_chld(root, NULL, 0);
	ASSERT_STRING_EQUAL(roxml_get_content(node0, NULL, 0, NULL), "xml version=\"1.0\"? encoding=\"fr_FR\"")
	node1 = roxml_get_chld(node0, NULL, 0);

	roxml_release(RELEASE_ALL);
	roxml_close(root);

	root = roxml_load_doc("roxml.test.xml.valid");

	ASSERT_STRING_EQUAL(roxml_get_name(root, NULL, 0), "node0")

	roxml_release(RELEASE_ALL);
	roxml_close(root);

	root = roxml_load_doc("roxml.test.xml.valid.comment");

	ASSERT_STRING_EQUAL(roxml_get_name(root, NULL, 0), "node0")

	roxml_release(RELEASE_ALL);
	roxml_close(root);

	RETURN
}

int test_get_chld(void)
{
	INIT
	node_t *root = roxml_load_doc("roxml.test.xml");

	ASSERT_STRING_EQUAL(roxml_get_name(root, NULL, 0), "documentRoot")

	node_t * node0 = roxml_get_chld(root, NULL, 0);
	ASSERT_STRING_EQUAL(roxml_get_name(node0, NULL, 0), "node0")
	node_t * node1 = roxml_get_chld(node0, NULL, 0);
	ASSERT_STRING_EQUAL(roxml_get_name(node1, NULL, 0), "node1")
	node_t * node2 = roxml_get_chld(node0, NULL, 1);
	ASSERT_STRING_EQUAL(roxml_get_name(node2, NULL, 0), "node2")
	node_t * node3 = roxml_get_chld(node2, NULL, 0);
	ASSERT_STRING_EQUAL(roxml_get_name(node3, NULL, 0), "node3")
	node_t * node4 = roxml_get_chld(node2, NULL, 1);
	ASSERT_STRING_EQUAL(roxml_get_name(node4, NULL, 0), "node4")
	node_t * node5 = roxml_get_chld(node2, NULL, 2);
	ASSERT_STRING_EQUAL(roxml_get_name(node5, NULL, 0), "node5")
	node_t * node6 = roxml_get_chld(node5, NULL, 0);
	ASSERT_STRING_EQUAL(roxml_get_name(node6, NULL, 0), "node6")
	node_t * node7 = roxml_get_chld(node0, NULL, 2);
	ASSERT_STRING_EQUAL(roxml_get_name(node7, NULL, 0), "node7")

	node0 = roxml_get_chld(root, "node0", 0);
	ASSERT_STRING_EQUAL(roxml_get_name(node0, NULL, 0), "node0")
	node1 = roxml_get_chld(node0,"node1", 0);
	ASSERT_STRING_EQUAL(roxml_get_name(node1, NULL, 0), "node1")
	node2 = roxml_get_chld(node0, "node2", 0);
	ASSERT_STRING_EQUAL(roxml_get_name(node2, NULL, 0), "node2")
	node3 = roxml_get_chld(node2, "node3", 0);
	ASSERT_STRING_EQUAL(roxml_get_name(node3, NULL, 0), "node3")
	node4 = roxml_get_chld(node2, "node4", 0);
	ASSERT_STRING_EQUAL(roxml_get_name(node4, NULL, 0), "node4")
	node5 = roxml_get_chld(node2, "node5", 0);
	ASSERT_STRING_EQUAL(roxml_get_name(node5, NULL, 0), "node5")
	node6 = roxml_get_chld(node5, "node6", 0);
	ASSERT_STRING_EQUAL(roxml_get_name(node6, NULL, 0), "node6")
	node7 = roxml_get_chld(node0, "node7", 0);
	ASSERT_STRING_EQUAL(roxml_get_name(node7, NULL, 0), "node7")

	roxml_release(RELEASE_ALL);
	roxml_close(root);

	RETURN
}

int test_get_sibl(void)
{
	INIT
	node_t *root = roxml_load_doc("roxml.test.xml");

	ASSERT_STRING_EQUAL(roxml_get_name(root, NULL, 0), "documentRoot")

	node_t * node0 = roxml_get_chld(root, NULL, 0);
	node_t * node1 = roxml_get_chld(node0, NULL, 0);
	node_t * node2 = roxml_get_chld(node0, NULL, 1);
	node_t * node3 = roxml_get_chld(node2, NULL, 0);
	node_t * node4 = roxml_get_chld(node2, NULL, 1);
	node_t * node5 = roxml_get_chld(node2, NULL, 2);
	node_t * node6 = roxml_get_chld(node5, NULL, 0);
	node_t * node7 = roxml_get_chld(node0, NULL, 2);

	ASSERT_NULL(roxml_get_prev_sibling(node0));
	ASSERT_NULL(roxml_get_next_sibling(node0));

	ASSERT_NULL(roxml_get_prev_sibling(node1));
	ASSERT_NOT_NULL(roxml_get_next_sibling(node1));
	ASSERT_EQUAL(node2, roxml_get_next_sibling(node1));

	ASSERT_NOT_NULL(roxml_get_prev_sibling(node2));
	ASSERT_NOT_NULL(roxml_get_next_sibling(node2));
	ASSERT_EQUAL(node1, roxml_get_prev_sibling(node2));
	ASSERT_EQUAL(node7, roxml_get_next_sibling(node2));

	ASSERT_NULL(roxml_get_prev_sibling(node3));
	ASSERT_NOT_NULL(roxml_get_next_sibling(node3));
	ASSERT_EQUAL(node4, roxml_get_next_sibling(node3));

	ASSERT_NOT_NULL(roxml_get_prev_sibling(node4));
	ASSERT_NOT_NULL(roxml_get_next_sibling(node4));
	ASSERT_EQUAL(node3, roxml_get_prev_sibling(node4));
	ASSERT_EQUAL(node5, roxml_get_next_sibling(node4));

	ASSERT_NOT_NULL(roxml_get_prev_sibling(node5));
	ASSERT_NULL(roxml_get_next_sibling(node5));
	ASSERT_EQUAL(node4, roxml_get_prev_sibling(node5));

	ASSERT_NULL(roxml_get_prev_sibling(node6));
	ASSERT_NULL(roxml_get_next_sibling(node6));

	ASSERT_NOT_NULL(roxml_get_prev_sibling(node7));
	ASSERT_NULL(roxml_get_next_sibling(node7));
	ASSERT_EQUAL(node2, roxml_get_prev_sibling(node7));

	roxml_release(RELEASE_ALL);
	roxml_close(root);

	RETURN
}

int test_get_chld_nb(void)
{
	INIT

	node_t *root = roxml_load_doc("roxml.test.xml");

	int nb = roxml_get_chld_nb(root);	// root
	ASSERT_EQUAL(nb, 1);
	nb = roxml_get_chld_nb(root->chld);	// node0
	ASSERT_EQUAL(nb, 3);
	nb = roxml_get_chld_nb(root->chld->chld);	// node1
	ASSERT_EQUAL(nb, 0);
	nb = roxml_get_chld_nb(root->chld->chld->sibl);	// node2
	ASSERT_EQUAL(nb, 3);
	nb = roxml_get_chld_nb(root->chld->chld->sibl->sibl);	// node7
	ASSERT_EQUAL(nb, 1);
	nb = roxml_get_chld_nb(root->chld->chld->sibl->sibl->chld);	// node8
	ASSERT_EQUAL(nb, 0);
	nb = roxml_get_chld_nb(root->chld->chld->sibl->chld); // node3
	ASSERT_EQUAL(nb, 0);
	nb = roxml_get_chld_nb(root->chld->chld->sibl->chld->sibl);	// node4
	ASSERT_EQUAL(nb, 0);
	nb = roxml_get_chld_nb(root->chld->chld->sibl->chld->sibl->sibl);	// node5
	ASSERT_EQUAL(nb, 1);
	nb = roxml_get_chld_nb(root->chld->chld->sibl->chld->sibl->sibl->chld);	// node6
	ASSERT_EQUAL(nb, 0);

	roxml_close(root);

	RETURN
}

int test_get_content(void)
{
	INIT
	int len = 0;
	char buf[4];
	char buf2[6];
	char buf3[7];
	char buf4[16];
	node_t *root = roxml_load_doc("roxml.test.xml");
	
	ASSERT_STRING_EQUAL(roxml_get_content(root->chld, NULL, 0, NULL), ""); // node0
	ASSERT_STRING_EQUAL(roxml_get_content(root->chld->chld, NULL, 0, NULL), "text1");	// node1
	ASSERT_STRING_EQUAL(roxml_get_content(root->chld->chld->sibl, NULL, 0, NULL), "");	// node2
	ASSERT_STRING_EQUAL(roxml_get_content(root->chld->chld->sibl->sibl, NULL, 0, NULL), "\"\"");	// node7
	ASSERT_STRING_EQUAL(roxml_get_content(root->chld->chld->sibl->chld, NULL, 0, NULL), "text2");	//node3
	ASSERT_STRING_EQUAL(roxml_get_content(root->chld->chld->sibl->chld->sibl, NULL, 0, NULL), "");	//node4
	ASSERT_STRING_EQUAL(roxml_get_content(root->chld->chld->sibl->chld->sibl->sibl, NULL, 0, NULL), "text3text4");	//node5
	ASSERT_STRING_EQUAL(roxml_get_content(root->chld->chld->sibl->chld->sibl->sibl->chld, NULL, 0, NULL), "text5");	//node6

	ASSERT_NULL(roxml_get_content(root->chld->attr, NULL, 0, NULL))

	ASSERT_STRING_EQUAL(roxml_get_content(root->chld->chld->attr, NULL, 0, NULL), "name1")
	ASSERT_STRING_EQUAL(roxml_get_content(root->chld->chld->attr->sibl, NULL, 0, NULL), "value1")
	ASSERT_STRING_EQUAL(roxml_get_content(root->chld->chld->sibl->attr, NULL, 0, NULL), "name2")
	ASSERT_STRING_EQUAL(roxml_get_content(root->chld->chld->sibl->attr->sibl, NULL, 0, NULL), "value2")
	ASSERT_STRING_EQUAL(roxml_get_content(root->chld->chld->sibl->chld->sibl->attr, NULL, 0, NULL), "name4")
	ASSERT_STRING_EQUAL(roxml_get_content(root->chld->chld->sibl->chld->sibl->attr->sibl, NULL, 0, NULL), "value4")

	char * value = roxml_get_content(root->chld->chld->attr->sibl, NULL, 0, &len);
	ASSERT_EQUAL(len, 7)
	ASSERT_STRING_EQUAL(value, "value1")
	value = roxml_get_content(root->chld->chld->attr->sibl, buf, 4, &len);
	ASSERT_EQUAL(len, 4)
	ASSERT_STRING_EQUAL(value, "val")
	value = roxml_get_content(root->chld->chld->attr->sibl, buf2, 6, &len);
	ASSERT_EQUAL(len, 6)
	ASSERT_STRING_EQUAL(value, "value")
	value = roxml_get_content(root->chld->chld->attr->sibl, buf3, 7, &len);
	ASSERT_EQUAL(len, 7)
	ASSERT_STRING_EQUAL(value, "value1")
	value = roxml_get_content(root->chld->chld->attr->sibl, buf4, 16, &len);
	ASSERT_EQUAL(len, 7)
	ASSERT_STRING_EQUAL(value, "value1")

	roxml_release(RELEASE_ALL);
	roxml_close(root);

	RETURN
}

int test_get_attr_nb(void)
{
	INIT
	node_t *root = roxml_load_doc("roxml.test.xml");

	int nb = roxml_get_attr_nb(root);	// root
	ASSERT_EQUAL(nb, 0);
	nb = roxml_get_attr_nb(root->chld);	// node0
	ASSERT_EQUAL(nb, 0);
	nb = roxml_get_attr_nb(root->chld->chld);	// node1
	ASSERT_EQUAL(nb, 2);
	nb = roxml_get_attr_nb(root->chld->chld->sibl);	// node2
	ASSERT_EQUAL(nb, 2);
	nb = roxml_get_attr_nb(root->chld->chld->sibl->sibl);	// node7
	ASSERT_EQUAL(nb, 0);
	nb = roxml_get_attr_nb(root->chld->chld->sibl->chld); // node3
	ASSERT_EQUAL(nb, 0);
	nb = roxml_get_attr_nb(root->chld->chld->sibl->chld->sibl);	// node4
	ASSERT_EQUAL(nb, 2);
	nb = roxml_get_attr_nb(root->chld->chld->sibl->chld->sibl->sibl);	// node5
	ASSERT_EQUAL(nb, 0);
	nb = roxml_get_attr_nb(root->chld->chld->sibl->chld->sibl->sibl->chld);	// node6
	ASSERT_EQUAL(nb, 0);

	roxml_close(root);
	RETURN
}

int test_get_attr(void)
{
	INIT

	node_t *root = roxml_load_doc("roxml.test.xml");

	node_t * node0 = roxml_get_chld(root, NULL, 0);
	node_t * node1 = roxml_get_chld(node0, NULL, 0);
	node_t * node2 = roxml_get_chld(node0, NULL, 1);
	node_t * node4 = roxml_get_chld(node2, NULL, 1);

	node_t * attr0 = roxml_get_attr(root, NULL, 0);
	node_t * attr1 = roxml_get_attr(node1, NULL, 0);
	node_t * attr2 = roxml_get_attr(node2, NULL, 0);
	node_t * attr4 = roxml_get_attr(node4, NULL, 0);

	ASSERT_NULL(attr0)
	ASSERT_STRING_EQUAL(roxml_get_name(attr1, NULL, 0), "name")
	ASSERT_STRING_EQUAL(roxml_get_name(attr2, NULL, 0), "name")
	ASSERT_STRING_EQUAL(roxml_get_name(attr4, NULL, 0), "name")

	attr0 = roxml_get_attr(root, NULL, 1);
	attr1 = roxml_get_attr(node1, NULL, 1);
	attr2 = roxml_get_attr(node2, NULL, 1);
	attr4 = roxml_get_attr(node4, NULL, 1);

	ASSERT_NULL(attr0)
	ASSERT_STRING_EQUAL(roxml_get_name(attr1, NULL, 0), "value")
	ASSERT_STRING_EQUAL(roxml_get_name(attr2, NULL, 0), "value")
	ASSERT_STRING_EQUAL(roxml_get_name(attr4, NULL, 0), "value")

	attr0 = roxml_get_attr(root, "value", 0);
	attr1 = roxml_get_attr(node1, "value", 0);
	attr2 = roxml_get_attr(node2, "value", 0);
	attr4 = roxml_get_attr(node4, "value", 0);

	ASSERT_NULL(attr0)
	ASSERT_STRING_EQUAL(roxml_get_name(attr1, NULL, 0), "value")
	ASSERT_STRING_EQUAL(roxml_get_name(attr2, NULL, 0), "value")
	ASSERT_STRING_EQUAL(roxml_get_name(attr4, NULL, 0), "value")

	attr0 = roxml_get_attr(root, NULL, 2);
	attr1 = roxml_get_attr(node1, NULL, 2);
	attr2 = roxml_get_attr(node2, NULL, 2);
	attr4 = roxml_get_attr(node4, NULL, 2);

	ASSERT_NULL(attr0)
	ASSERT_NULL(attr1)
	ASSERT_NULL(attr2)
	ASSERT_NULL(attr4)

	attr1 = roxml_get_attr(node1, "toto", 0);
	ASSERT_NULL(attr1)
	roxml_close(root);

	root = roxml_load_doc("roxml.test.xml.wrong");

	char buf[256];
	node0 = roxml_get_chld(root, NULL, 0);
	node1 = roxml_get_chld(node0, NULL, 0);
	attr0 = roxml_get_attr(node1, NULL, 0);
	ASSERT_STRING_EQUAL(roxml_get_name(attr0, buf, 256), "name")
	attr1 = roxml_get_attr(node1, NULL, 1);
	ASSERT_STRING_EQUAL(roxml_get_name(attr1, buf, 256), "value")

	roxml_close(root);

	roxml_release(RELEASE_ALL);

	RETURN
}

int test_parse_xpath(void)
{
	INIT

	xpath_node_t *ptr;
	char mypath[256] = "/node/item[12]/title";
	int ret = roxml_parse_xpath(mypath, &ptr, 0);	

	ASSERT_EQUAL(ret, 1)
	ASSERT_EQUAL(ptr[0].axes, ROXML_ID_CHILD);
	ASSERT_NULL(ptr[0].cond);
	ASSERT_STRING_EQUAL(ptr[0].name, "node")
	ASSERT_NOT_NULL(ptr[0].next);
	ASSERT_STRING_EQUAL(ptr[0].next->name, "item")
	ASSERT_EQUAL(ptr[0].next->axes, ROXML_ID_CHILD)
	ASSERT_NOT_NULL(ptr[0].next->cond);
	ASSERT_EQUAL(ptr[0].next->cond->func, ROXML_FUNC_POS)
	ASSERT_STRING_EQUAL(ptr[0].next->cond->arg2, "12")
	ASSERT_STRING_EQUAL(ptr[0].next->cond->arg1, "12")
	ASSERT_EQUAL(ptr[0].next->next->axes, ROXML_ID_CHILD)
	ASSERT_NOT_NULL(ptr[0].next->next);
	ASSERT_NULL(ptr[0].next->next->next);
	ASSERT_STRING_EQUAL(ptr[0].next->next->name, "title")
	ASSERT_NULL(ptr[0].next->next->cond)

	roxml_free_xpath(ptr, ret);

	strcpy(mypath, "/node/item[child::title = \"toto\"]/title");
	ret = roxml_parse_xpath(mypath, &ptr, 0);	
	ASSERT_EQUAL(ret, 1)
	ASSERT_STRING_EQUAL(ptr[0].name, "node");
	ASSERT_NOT_NULL(ptr[0].next);
	ASSERT_STRING_EQUAL(ptr[0].next->name, "item");
	ASSERT_NOT_NULL(ptr[0].next->cond);
	ASSERT_EQUAL(ptr[0].next->cond->func, ROXML_FUNC_XPATH);
	ASSERT_NOT_NULL(ptr[0].next->cond->xp);
	ASSERT_EQUAL(ptr[0].next->cond->func2, 1);
	ASSERT_EQUAL(ptr[0].next->cond->xp[0].axes, ROXML_ID_CHILD);
	ASSERT_STRING_EQUAL(ptr[0].next->cond->xp[0].name, "title");
	ASSERT_EQUAL(ptr[0].next->cond->xp[0].xp_cond->func, ROXML_FUNC_STRCOMP);
	ASSERT_STRING_EQUAL(ptr[0].next->cond->xp[0].xp_cond->arg2, "toto");
	ASSERT_NOT_NULL(ptr[0].next->next);
	ASSERT_STRING_EQUAL(ptr[0].next->next->name, "title");
	ASSERT_NULL(ptr[0].next->next->next);
	roxml_free_xpath(ptr, ret);

	strcpy(mypath, "/node/item[child::title = \"toto\" and price < 12]/title");
	ret = roxml_parse_xpath(mypath, &ptr, 0);	
	ASSERT_EQUAL(ret, 1)
	ASSERT_STRING_EQUAL(ptr[0].name, "node");
	ASSERT_NOT_NULL(ptr[0].next);
	ASSERT_STRING_EQUAL(ptr[0].next->name, "item");
	ASSERT_NOT_NULL(ptr[0].next->cond);
	ASSERT_EQUAL(ptr[0].next->cond->func, ROXML_FUNC_XPATH);
	ASSERT_NOT_NULL(ptr[0].next->cond->xp);
	ASSERT_EQUAL(ptr[0].next->cond->func2, 2);
	ASSERT_EQUAL(ptr[0].next->cond->xp[0].axes, ROXML_ID_CHILD);
	ASSERT_STRING_EQUAL(ptr[0].next->cond->xp[0].name, "title");
	ASSERT_EQUAL(ptr[0].next->cond->xp[0].xp_cond->func, ROXML_FUNC_STRCOMP);
	ASSERT_STRING_EQUAL(ptr[0].next->cond->xp[0].xp_cond->arg2, "toto");
	ASSERT_EQUAL(ptr[0].next->cond->xp[1].axes, ROXML_ID_CHILD);
	ASSERT_EQUAL(ptr[0].next->cond->xp[1].rel, ROXML_OPERATOR_AND);
	ASSERT_STRING_EQUAL(ptr[0].next->cond->xp[1].name, "price");
	ASSERT_EQUAL(ptr[0].next->cond->xp[1].xp_cond->func, ROXML_FUNC_INTCOMP);
	ASSERT_STRING_EQUAL(ptr[0].next->cond->xp[1].xp_cond->arg2, "12");

	ASSERT_NOT_NULL(ptr[0].next->next);
	ASSERT_STRING_EQUAL(ptr[0].next->next->name, "title");
	ASSERT_NULL(ptr[0].next->next->next);
	roxml_free_xpath(ptr, ret);

	strcpy(mypath, "/node/item[child::title = \"toto\" and @price < 12]/title");
	ret = roxml_parse_xpath(mypath, &ptr, 0);	
	ASSERT_EQUAL(ret, 1)
	ASSERT_STRING_EQUAL(ptr[0].name, "node");
	ASSERT_NOT_NULL(ptr[0].next);
	ASSERT_STRING_EQUAL(ptr[0].next->name, "item");
	ASSERT_NOT_NULL(ptr[0].next->cond);
	ASSERT_EQUAL(ptr[0].next->cond->func, ROXML_FUNC_XPATH);
	ASSERT_NOT_NULL(ptr[0].next->cond->xp);
	ASSERT_EQUAL(ptr[0].next->cond->func2, 2);
	ASSERT_EQUAL(ptr[0].next->cond->xp[0].axes, ROXML_ID_CHILD);
	ASSERT_STRING_EQUAL(ptr[0].next->cond->xp[0].name, "title");
	ASSERT_EQUAL(ptr[0].next->cond->xp[0].xp_cond->func, ROXML_FUNC_STRCOMP);
	ASSERT_STRING_EQUAL(ptr[0].next->cond->xp[0].xp_cond->arg2, "toto");
	ASSERT_EQUAL(ptr[0].next->cond->xp[1].axes, ROXML_ID_ATTR);
	ASSERT_STRING_EQUAL(ptr[0].next->cond->xp[1].name, "price");
	ASSERT_EQUAL(ptr[0].next->cond->xp[1].xp_cond->func, ROXML_FUNC_INTCOMP);
	ASSERT_STRING_EQUAL(ptr[0].next->cond->xp[1].xp_cond->arg2, "12");
	ASSERT_NOT_NULL(ptr[0].next->next);
	ASSERT_STRING_EQUAL(ptr[0].next->next->name, "title");
	ASSERT_NULL(ptr[0].next->next->next);
	roxml_free_xpath(ptr, ret);

	strcpy(mypath, "/node/item[child::bande = \"toto\" and @oral < 12]/title");
	ret = roxml_parse_xpath(mypath, &ptr, 0);	
	ASSERT_EQUAL(ret, 1)
	ASSERT_STRING_EQUAL(ptr[0].name, "node");
	ASSERT_NOT_NULL(ptr[0].next);
	ASSERT_STRING_EQUAL(ptr[0].next->name, "item");
	ASSERT_NOT_NULL(ptr[0].next->cond);
	ASSERT_EQUAL(ptr[0].next->cond->func, ROXML_FUNC_XPATH);
	ASSERT_NOT_NULL(ptr[0].next->cond->xp);
	ASSERT_EQUAL(ptr[0].next->cond->func2, 2);
	ASSERT_EQUAL(ptr[0].next->cond->xp[0].axes, ROXML_ID_CHILD);
	ASSERT_STRING_EQUAL(ptr[0].next->cond->xp[0].name, "bande");
	ASSERT_EQUAL(ptr[0].next->cond->xp[0].xp_cond->func, ROXML_FUNC_STRCOMP);
	ASSERT_STRING_EQUAL(ptr[0].next->cond->xp[0].xp_cond->arg2, "toto");
	ASSERT_EQUAL(ptr[0].next->cond->xp[1].axes, ROXML_ID_ATTR);
	ASSERT_STRING_EQUAL(ptr[0].next->cond->xp[1].name, "oral");
	ASSERT_EQUAL(ptr[0].next->cond->xp[1].xp_cond->func, ROXML_FUNC_INTCOMP);
	ASSERT_STRING_EQUAL(ptr[0].next->cond->xp[1].xp_cond->arg2, "12");
	ASSERT_NOT_NULL(ptr[0].next->next);
	ASSERT_STRING_EQUAL(ptr[0].next->next->name, "title");
	ASSERT_NULL(ptr[0].next->next->next);
	roxml_free_xpath(ptr, ret);

	strcpy(mypath, "/node/item[child::and = \"toto\" and @or < 12]/title");
	ret = roxml_parse_xpath(mypath, &ptr, 0);	
	ASSERT_EQUAL(ret, 1)
	ASSERT_STRING_EQUAL(ptr[0].name, "node");
	ASSERT_NOT_NULL(ptr[0].next);
	ASSERT_STRING_EQUAL(ptr[0].next->name, "item");
	ASSERT_NOT_NULL(ptr[0].next->cond);
	ASSERT_EQUAL(ptr[0].next->cond->func, ROXML_FUNC_XPATH);
	ASSERT_NOT_NULL(ptr[0].next->cond->xp);
	ASSERT_EQUAL(ptr[0].next->cond->func2, 2);
	ASSERT_EQUAL(ptr[0].next->cond->xp[0].axes, ROXML_ID_CHILD);
	ASSERT_STRING_EQUAL(ptr[0].next->cond->xp[0].name, "and");
	ASSERT_EQUAL(ptr[0].next->cond->xp[0].xp_cond->func, ROXML_FUNC_STRCOMP);
	ASSERT_STRING_EQUAL(ptr[0].next->cond->xp[0].xp_cond->arg2, "toto");
	ASSERT_EQUAL(ptr[0].next->cond->xp[1].axes, ROXML_ID_ATTR);
	ASSERT_STRING_EQUAL(ptr[0].next->cond->xp[1].name, "or");
	ASSERT_EQUAL(ptr[0].next->cond->xp[1].xp_cond->func, ROXML_FUNC_INTCOMP);
	ASSERT_STRING_EQUAL(ptr[0].next->cond->xp[1].xp_cond->arg2, "12");
	ASSERT_NOT_NULL(ptr[0].next->next);
	ASSERT_STRING_EQUAL(ptr[0].next->next->name, "title");
	ASSERT_NULL(ptr[0].next->next->next);
	roxml_free_xpath(ptr, ret);

	strcpy(mypath, "/node/item[child::and = \"toto or titi\" and @or < 12]/title");
	ret = roxml_parse_xpath(mypath, &ptr, 0);	
	ASSERT_EQUAL(ret, 1)
	ASSERT_STRING_EQUAL(ptr[0].name, "node");
	ASSERT_NOT_NULL(ptr[0].next);
	ASSERT_STRING_EQUAL(ptr[0].next->name, "item");
	ASSERT_NOT_NULL(ptr[0].next->cond);
	ASSERT_EQUAL(ptr[0].next->cond->func, ROXML_FUNC_XPATH);
	ASSERT_NOT_NULL(ptr[0].next->cond->xp);
	ASSERT_EQUAL(ptr[0].next->cond->func2, 2);
	ASSERT_EQUAL(ptr[0].next->cond->xp[0].axes, ROXML_ID_CHILD);
	ASSERT_STRING_EQUAL(ptr[0].next->cond->xp[0].name, "and");
	ASSERT_EQUAL(ptr[0].next->cond->xp[0].xp_cond->func, ROXML_FUNC_STRCOMP);
	ASSERT_STRING_EQUAL(ptr[0].next->cond->xp[0].xp_cond->arg2, "toto or titi");
	ASSERT_EQUAL(ptr[0].next->cond->xp[1].axes, ROXML_ID_ATTR);
	ASSERT_STRING_EQUAL(ptr[0].next->cond->xp[1].name, "or");
	ASSERT_EQUAL(ptr[0].next->cond->xp[1].xp_cond->func, ROXML_FUNC_INTCOMP);
	ASSERT_STRING_EQUAL(ptr[0].next->cond->xp[1].xp_cond->arg2, "12");
	ASSERT_NOT_NULL(ptr[0].next->next);
	ASSERT_STRING_EQUAL(ptr[0].next->next->name, "title");
	ASSERT_NULL(ptr[0].next->next->next);
	roxml_free_xpath(ptr, ret);

	strcpy(mypath, "/node/item[child::desc[@price < 12]]/title[text() = \"toto\"]");
	ret = roxml_parse_xpath(mypath, &ptr, 0);	
	ASSERT_EQUAL(ret, 1)
	ASSERT_STRING_EQUAL(ptr[0].name, "node");
	ASSERT_NOT_NULL(ptr[0].next);
	ASSERT_STRING_EQUAL(ptr[0].next->name, "item");
	ASSERT_NOT_NULL(ptr[0].next->cond);
	ASSERT_EQUAL(ptr[0].next->cond->func, ROXML_FUNC_XPATH);
	ASSERT_NOT_NULL(ptr[0].next->cond->xp);
	ASSERT_EQUAL(ptr[0].next->cond->xp[0].axes, ROXML_ID_CHILD);
	ASSERT_STRING_EQUAL(ptr[0].next->cond->xp[0].name, "desc");
	ASSERT_NOT_NULL(ptr[0].next->cond->xp[0].cond);
	ASSERT_EQUAL(ptr[0].next->cond->xp[0].cond->func, ROXML_FUNC_XPATH);
	ASSERT_NOT_NULL(ptr[0].next->cond->xp[0].cond->xp);
	ASSERT_EQUAL(ptr[0].next->cond->xp[0].cond->xp[0].axes, ROXML_ID_ATTR);
	ASSERT_STRING_EQUAL(ptr[0].next->cond->xp[0].cond->xp[0].name, "price");
	ASSERT_EQUAL(ptr[0].next->cond->xp[0].cond->xp[0].xp_cond->func, ROXML_FUNC_INTCOMP);
	ASSERT_STRING_EQUAL(ptr[0].next->cond->xp[0].cond->xp[0].xp_cond->arg2, "12");
	ASSERT_NOT_NULL(ptr[0].next->next);
	ASSERT_STRING_EQUAL(ptr[0].next->next->name, "title");
	ASSERT_NOT_NULL(ptr[0].next->next->cond);
	ASSERT_EQUAL(ptr[0].next->next->cond->func, ROXML_FUNC_XPATH);
	ASSERT_NOT_NULL(ptr[0].next->next->cond->xp);
	ASSERT_EQUAL(ptr[0].next->next->cond->xp[0].axes, ROXML_ID_CHILD);
	ASSERT_STRING_EQUAL(ptr[0].next->next->cond->xp[0].name, "text()");
	ASSERT_EQUAL(ptr[0].next->next->cond->xp[0].xp_cond->func, ROXML_FUNC_STRCOMP);
	ASSERT_STRING_EQUAL(ptr[0].next->next->cond->xp[0].xp_cond->arg2, "toto");
	ASSERT_NULL(ptr[0].next->next->next);
	roxml_free_xpath(ptr, ret);

	strcpy(mypath, "/node[first() or last()]/item/title/@version | /node/item/title[@version < 3] | /node/item/title[@version = v1]");
	ret = roxml_parse_xpath(mypath, &ptr, 0);	

	ASSERT_EQUAL(ret, 3)
	ASSERT_NOT_NULL(ptr[0].next);
	ASSERT_NOT_NULL(ptr[0].next->next);
	ASSERT_NOT_NULL(ptr[0].next->next->next);
	ASSERT_NULL(ptr[0].next->next->next->next);
	ASSERT_EQUAL(ptr[0].rel, ROXML_OPERATOR_OR)
	ASSERT_EQUAL(ptr[0].axes, ROXML_ID_CHILD)
	ASSERT_STRING_EQUAL(ptr[0].name, "node")
	ASSERT_NOT_NULL(ptr[0].cond)
	ASSERT_NOT_NULL(ptr[0].cond->next)
	ASSERT_NULL(ptr[0].cond->next->next)
	ASSERT_STRING_EQUAL(ptr[0].cond->arg1, "first()")
	ASSERT_STRING_EQUAL(ptr[0].cond->next->arg1, "last()")
	ASSERT_EQUAL(ptr[0].cond->next->func, ROXML_FUNC_LAST)
	ASSERT_EQUAL(ptr[0].cond->func, ROXML_FUNC_FIRST)
	ASSERT_EQUAL(ptr[0].cond->next->rel, ROXML_OPERATOR_OR)
	ASSERT_EQUAL(ptr[0].next->axes, ROXML_ID_CHILD)
	ASSERT_STRING_EQUAL(ptr[0].next->name, "item")
	ASSERT_NULL(ptr[0].next->cond)
	ASSERT_EQUAL(ptr[0].next->next->axes, ROXML_ID_CHILD)
	ASSERT_STRING_EQUAL(ptr[0].next->next->name, "title")
	ASSERT_NULL(ptr[0].next->next->cond)
	ASSERT_EQUAL(ptr[0].next->next->next->axes, ROXML_ID_ATTR)
	ASSERT_STRING_EQUAL(ptr[0].next->next->next->name, "version")
	ASSERT_NULL(ptr[0].next->next->next->cond)

	ASSERT_EQUAL(ptr[1].axes, ROXML_ID_CHILD);
	ASSERT_NULL(ptr[1].cond);
	ASSERT_NOT_NULL(ptr[1].next);
	ASSERT_EQUAL(ptr[1].rel, ROXML_OPERATOR_OR)
	ASSERT_EQUAL(ptr[1].axes, ROXML_ID_CHILD)
	ASSERT_STRING_EQUAL(ptr[1].name, "node")
	ASSERT_STRING_EQUAL(ptr[1].next->name, "item")
	ASSERT_EQUAL(ptr[1].next->axes, ROXML_ID_CHILD)
	ASSERT_NOT_NULL(ptr[1].next->next);
	ASSERT_NULL(ptr[1].next->cond)
	ASSERT_NULL(ptr[1].next->next->next);
	ASSERT_EQUAL(ptr[1].next->next->axes, ROXML_ID_CHILD)
	ASSERT_STRING_EQUAL(ptr[1].next->next->name, "title")
	ASSERT_NOT_NULL(ptr[1].next->next->cond)
	ASSERT_NULL(ptr[1].next->next->cond->next)
	ASSERT_STRING_EQUAL(ptr[1].next->next->cond->xp[0].name, "version")
	ASSERT_STRING_EQUAL(ptr[1].next->next->cond->xp[0].xp_cond->arg2, "3")
	ASSERT_EQUAL(ptr[1].next->next->cond->xp[0].xp_cond->func, ROXML_FUNC_INTCOMP)
	ASSERT_EQUAL(ptr[1].next->next->cond->xp[0].xp_cond->op, ROXML_OPERATOR_INF)

	ASSERT_EQUAL(ptr[2].axes, ROXML_ID_CHILD);
	ASSERT_NULL(ptr[2].cond);
	ASSERT_EQUAL(ptr[2].rel, ROXML_OPERATOR_OR)
	ASSERT_NOT_NULL(ptr[2].next);
	ASSERT_NOT_NULL(ptr[2].next->next);
	ASSERT_NULL(ptr[2].next->next->next);
	ASSERT_EQUAL(ptr[2].axes, ROXML_ID_CHILD)
	ASSERT_STRING_EQUAL(ptr[2].name, "node")
	ASSERT_EQUAL(ptr[2].next->axes, ROXML_ID_CHILD)
	ASSERT_STRING_EQUAL(ptr[2].next->name, "item")
	ASSERT_NULL(ptr[2].next->cond)
	ASSERT_EQUAL(ptr[2].next->next->axes, ROXML_ID_CHILD)
	ASSERT_STRING_EQUAL(ptr[2].next->next->name, "title")
	ASSERT_NOT_NULL(ptr[2].next->next->cond)
	ASSERT_NULL(ptr[2].next->next->cond->next)
	ASSERT_EQUAL(ptr[2].next->next->cond->xp[0].axes, ROXML_ID_ATTR)
	ASSERT_STRING_EQUAL(ptr[2].next->next->cond->xp[0].name, "version")
	ASSERT_STRING_EQUAL(ptr[2].next->next->cond->xp[0].xp_cond->arg2, "v1")
	ASSERT_EQUAL(ptr[2].next->next->cond->xp[0].xp_cond->func, ROXML_FUNC_STRCOMP)
	ASSERT_EQUAL(ptr[2].next->next->cond->xp[0].xp_cond->op, ROXML_OPERATOR_EQU)

	roxml_free_xpath(ptr, ret);
	RETURN
}

int test_xpath(void)
{
	INIT

	int nbans;
	node_t *node0;
	node_t *node2;
	node_t *root = roxml_load_doc("roxml.test.xml");

	node_t **node_set = roxml_xpath(root, "/node0/node2/node3", &nbans);
	ASSERT_EQUAL(nbans, 1)
	ASSERT_STRING_EQUAL(roxml_get_name(node_set[0], NULL, 0), "node3")

	node_set = roxml_xpath(root, "/node0/node2/node", &nbans);
	ASSERT_EQUAL(nbans, 0)

	node_set = roxml_xpath(root, "/node0/node2/node31", &nbans);
	ASSERT_EQUAL(nbans, 0)

	node_set = roxml_xpath(root, "/node0/node2/@name", &nbans);
	ASSERT_EQUAL(nbans, 1)
	ASSERT_STRING_EQUAL(roxml_get_name(node_set[0], NULL, 0), "name")
	ASSERT_STRING_EQUAL(roxml_get_content(node_set[0], NULL, 0, NULL), "name2")

	node_set = roxml_xpath(root, "/node0[node1 = text1]", &nbans);
	ASSERT_EQUAL(nbans, 1)
	ASSERT_STRING_EQUAL(roxml_get_name(node_set[0], NULL, 0), "node0")

	node_set = roxml_xpath(root, "/node0/node2/attribute::name", &nbans);
	ASSERT_EQUAL(nbans, 1)
	ASSERT_STRING_EQUAL(roxml_get_name(node_set[0], NULL, 0), "name")
	ASSERT_STRING_EQUAL(roxml_get_content(node_set[0], NULL, 0, NULL), "name2")

	node_set = roxml_xpath(root, "node0", &nbans);
	ASSERT_EQUAL(nbans, 1)
	ASSERT_STRING_EQUAL(roxml_get_name(node_set[0], NULL, 0), "node0")

	node_set = roxml_xpath(root, "/node0//node5", &nbans);
	ASSERT_EQUAL(nbans, 1)
	ASSERT_STRING_EQUAL(roxml_get_name(node_set[0], NULL, 0), "node5")

	node_set = roxml_xpath(root, "/node0//node5/node6", &nbans);
	ASSERT_EQUAL(nbans, 1)
	ASSERT_STRING_EQUAL(roxml_get_name(node_set[0], NULL, 0), "node6")

	node_set = roxml_xpath(root, "/node0/node1[@name=\"name1\"]", &nbans);
	ASSERT_EQUAL(nbans, 1)
	ASSERT_STRING_EQUAL(roxml_get_name(node_set[0], NULL, 0), "node1")

	node_set = roxml_xpath(root, "/node0/node2", &nbans);
	ASSERT_EQUAL(nbans, 1)
	ASSERT_STRING_EQUAL(roxml_get_name(node_set[0], NULL, 0), "node2")
	node2 = node_set[0];

	node_set = roxml_xpath(root, "/child::node0/child::node2/parent::", &nbans);
	ASSERT_EQUAL(nbans, 1)
	ASSERT_STRING_EQUAL(roxml_get_name(node_set[0], NULL, 0), "node0")

	node_set = roxml_xpath(root, "/child::node0/child::node2", &nbans);
	ASSERT_EQUAL(nbans, 1)
	ASSERT_STRING_EQUAL(roxml_get_name(node_set[0], NULL, 0), "node2")
	node2 = node_set[0];

	node_set = roxml_xpath(node2, ".", &nbans);
	ASSERT_EQUAL(nbans, 1)
	ASSERT_STRING_EQUAL(roxml_get_name(node_set[0], NULL, 0), "node2")

	node_set = roxml_xpath(node2, "..", &nbans);
	ASSERT_EQUAL(nbans, 1)
	ASSERT_STRING_EQUAL(roxml_get_name(node_set[0], NULL, 0), "node0")

	node_set = roxml_xpath(node2, "node2", &nbans);
	ASSERT_EQUAL(nbans, 0)

	node_set = roxml_xpath(root, "/", &nbans);
	ASSERT_EQUAL(nbans, 1)
	ASSERT_STRING_EQUAL(roxml_get_name(node_set[0], NULL, 0), "documentRoot")

	node_set = roxml_xpath(node2, "/", &nbans);
	ASSERT_EQUAL(nbans, 1)
	ASSERT_STRING_EQUAL(roxml_get_name(node_set[0], NULL, 0), "documentRoot")

	node_set = roxml_xpath(node2, "node3", &nbans);
	ASSERT_EQUAL(nbans, 1)
	ASSERT_STRING_EQUAL(roxml_get_name(node_set[0], NULL, 0), "node3")

	node_set = roxml_xpath(node2, "//node4", &nbans);
	ASSERT_EQUAL(nbans, 1)
	ASSERT_STRING_EQUAL(roxml_get_name(node_set[0], NULL, 0), "node4")

	node_set = roxml_xpath(node2, "//node7", &nbans);
	ASSERT_EQUAL(nbans, 1)
	node_set = roxml_xpath(node2, "descendant-or-self::node7", &nbans);
	ASSERT_EQUAL(nbans, 0)

	node_set = roxml_xpath(node2, "/node3", &nbans);
	ASSERT_EQUAL(nbans, 0)

	node_set = roxml_xpath(node2, "preceding::node1", &nbans);
	ASSERT_EQUAL(nbans, 1)
	ASSERT_STRING_EQUAL(roxml_get_name(node_set[0], NULL, 0), "node1")

	node_set = roxml_xpath(root, "/node0/node7/preceding::node4", &nbans);
	ASSERT_EQUAL(nbans, 1)
	ASSERT_STRING_EQUAL(roxml_get_name(node_set[0], NULL, 0), "node4")

	node_set = roxml_xpath(root, "/node0/node2/following::node8", &nbans);
	ASSERT_EQUAL(nbans, 1)
	ASSERT_STRING_EQUAL(roxml_get_name(node_set[0], NULL, 0), "node8")

	node_set = roxml_xpath(node2, "preceding::node7", &nbans);
	ASSERT_EQUAL(nbans, 0)

	node_set = roxml_xpath(node2, "following::node1", &nbans);
	ASSERT_EQUAL(nbans, 0)

	node_set = roxml_xpath(node2, "following::node4", &nbans);
	ASSERT_EQUAL(nbans, 0)

	node_set = roxml_xpath(node2, "ancestor-or-self::node2", &nbans);
	ASSERT_EQUAL(nbans, 1)
	ASSERT_STRING_EQUAL(roxml_get_name(node_set[0], NULL, 0), "node2")

	node_set = roxml_xpath(node2, "ancestor::node2", &nbans);
	ASSERT_EQUAL(nbans, 0)

	node_set = roxml_xpath(node2, "ancestor::node6", &nbans);
	ASSERT_EQUAL(nbans, 0)

	node_set = roxml_xpath(node2, "ancestor::node1", &nbans);
	ASSERT_EQUAL(nbans, 0)

	node_set = roxml_xpath(node2, "ancestor::node0", &nbans);
	ASSERT_EQUAL(nbans, 1)
	ASSERT_STRING_EQUAL(roxml_get_name(node_set[0], NULL, 0), "node0")

	node_set = roxml_xpath(node2, "following::node7", &nbans);
	ASSERT_EQUAL(nbans, 1)
	ASSERT_STRING_EQUAL(roxml_get_name(node_set[0], NULL, 0), "node7")

	node_set = roxml_xpath(root, "/node4", &nbans);
	ASSERT_EQUAL(nbans, 0)

	node_set = roxml_xpath(root, "root/node0", &nbans);
	ASSERT_EQUAL(nbans, 0)
	
	node_set = roxml_xpath(root, "node0", &nbans);
	ASSERT_EQUAL(nbans, 1)
	ASSERT_STRING_EQUAL(roxml_get_name(node_set[0], NULL, 0), "node0")
	node0 = node_set[0];

	node_set = roxml_xpath(root, "/descendant-or-self::node1/attribute::name", &nbans);
	ASSERT_EQUAL(nbans, 1)
	ASSERT_STRING_EQUAL(roxml_get_name(node_set[0], NULL, 0), "name")

	node_set = roxml_xpath(root, "/descendant-or-self::/attribute::name", &nbans);
	ASSERT_EQUAL(nbans, 3)
	ASSERT_STRING_EQUAL(roxml_get_name(node_set[0], NULL, 0), "name")
	ASSERT_STRING_EQUAL(roxml_get_name(node_set[1], NULL, 0), "name")
	ASSERT_STRING_EQUAL(roxml_get_name(node_set[2], NULL, 0), "name")

	node_set = roxml_xpath(node0, ".//node0", &nbans);
	ASSERT_EQUAL(nbans, 0)

	node_set = roxml_xpath(root, ".//node0", &nbans);
	ASSERT_EQUAL(nbans, 1)
	ASSERT_STRING_EQUAL(roxml_get_name(node_set[0], NULL, 0), "node0")

	node_set = roxml_xpath(root, "//node0", &nbans);
	ASSERT_EQUAL(nbans, 1)
	ASSERT_STRING_EQUAL(roxml_get_name(node_set[0], NULL, 0), "node0")

	node_set = roxml_xpath(root, "/descendant-or-self::node()/node0", &nbans);
	ASSERT_EQUAL(nbans, 1)
	ASSERT_STRING_EQUAL(roxml_get_name(node_set[0], NULL, 0), "node0")

	node_set = roxml_xpath(node0, "descendant-or-self::node1", &nbans);
	ASSERT_EQUAL(nbans, 1)
	ASSERT_STRING_EQUAL(roxml_get_name(node_set[0], NULL, 0), "node1")

	node_set = roxml_xpath(node0, "descendant-or-self::node1/@*", &nbans);
	ASSERT_EQUAL(nbans, 2)
	ASSERT_STRING_EQUAL(roxml_get_content(node_set[0], NULL, 0, NULL), "name1")
	ASSERT_STRING_EQUAL(roxml_get_content(node_set[1], NULL, 0, NULL), "value1")
		
	node_set = roxml_xpath(root, "//*[@name=\"name1\"]", &nbans);
	ASSERT_EQUAL(nbans, 1)
	ASSERT_STRING_EQUAL(roxml_get_name(node_set[0], NULL, 0), "node1")

	node_set = roxml_xpath(root, "//*", &nbans);
	ASSERT_EQUAL(nbans, 9)

	node_set = roxml_xpath(root, "//text()", &nbans);
	ASSERT_EQUAL(nbans, 7)

	node_set = roxml_xpath(root, "/descendant-or-self::node()/node()", &nbans);
	ASSERT_EQUAL(nbans, 22)

	node_set = roxml_xpath(root, "//node()", &nbans);
	ASSERT_EQUAL(nbans, 22)

	node_set = roxml_xpath(root, "//", &nbans);
	ASSERT_EQUAL(nbans, 23)

	node_set = roxml_xpath(root, "*", &nbans);
	ASSERT_EQUAL(nbans, 1)

	node_set = roxml_xpath(node0, "*", &nbans);
	ASSERT_EQUAL(nbans, 3)

	node_set = roxml_xpath(root, "//@name", &nbans);
	ASSERT_EQUAL(nbans, 3)
	ASSERT_STRING_EQUAL(roxml_get_content(node_set[0], NULL, 0, NULL), "name4")
	ASSERT_STRING_EQUAL(roxml_get_content(node_set[1], NULL, 0, NULL), "name1")
	ASSERT_STRING_EQUAL(roxml_get_content(node_set[2], NULL, 0, NULL), "name2")

	node_set = roxml_xpath(node0, "node1/text()", &nbans);
	ASSERT_EQUAL(nbans, 1)
	ASSERT_STRING_EQUAL(roxml_get_content(node_set[0], NULL, 0, NULL), "text1")
	
	roxml_close(root);

	roxml_release(RELEASE_ALL);

	root = roxml_load_doc("roxml.test.xml.attributes");
	ASSERT_NOT_NULL(root)

	node_set = roxml_xpath(root, "/node0[item[@val > 2]]", &nbans);
	ASSERT_EQUAL(nbans, 1)
	ASSERT_STRING_EQUAL(roxml_get_name(node_set[0], NULL, 0), "node0")

	node_set = roxml_xpath(root, "/node0[item/@val > 2]", &nbans);
	ASSERT_EQUAL(nbans, 1)
	ASSERT_STRING_EQUAL(roxml_get_name(node_set[0], NULL, 0), "node0")

	node_set = roxml_xpath(root, "/node0[item/@val > 2]/item", &nbans);
	ASSERT_EQUAL(nbans, 6)
	node_t *attr = roxml_get_attr(node_set[0], "id", 0);
	ASSERT_STRING_EQUAL(roxml_get_content(attr, NULL, 0, NULL), "0")
	attr = roxml_get_attr(node_set[1], "id", 0);
	ASSERT_STRING_EQUAL(roxml_get_content(attr, NULL, 0, NULL), "1")
	attr = roxml_get_attr(node_set[2], "id", 0);
	ASSERT_STRING_EQUAL(roxml_get_content(attr, NULL, 0, NULL), "2")
	attr = roxml_get_attr(node_set[3], "id", 0);
	ASSERT_STRING_EQUAL(roxml_get_content(attr, NULL, 0, NULL), "3")
	attr = roxml_get_attr(node_set[4], "id", 0);
	ASSERT_STRING_EQUAL(roxml_get_content(attr, NULL, 0, NULL), "4")
	attr = roxml_get_attr(node_set[5], "id", 0);
	ASSERT_STRING_EQUAL(roxml_get_content(attr, NULL, 0, NULL), "5")

	node_set = roxml_xpath(root, "/node0/item[@val > 3 or @val < 3]", &nbans);
	ASSERT_EQUAL(nbans, 5)
	attr = roxml_get_attr(node_set[0], "id", 0);
	ASSERT_STRING_EQUAL(roxml_get_content(attr, NULL, 0, NULL), "0")
	attr = roxml_get_attr(node_set[1], "id", 0);
	ASSERT_STRING_EQUAL(roxml_get_content(attr, NULL, 0, NULL), "1")
	attr = roxml_get_attr(node_set[2], "id", 0);
	ASSERT_STRING_EQUAL(roxml_get_content(attr, NULL, 0, NULL), "2")
	attr = roxml_get_attr(node_set[3], "id", 0);
	ASSERT_STRING_EQUAL(roxml_get_content(attr, NULL, 0, NULL), "3")
	attr = roxml_get_attr(node_set[4], "id", 0);
	ASSERT_STRING_EQUAL(roxml_get_content(attr, NULL, 0, NULL), "5")

	node_set = roxml_xpath(root, "/node0/item[@val > 2 and @val < 4]", &nbans);
	ASSERT_EQUAL(nbans, 1)
	attr = roxml_get_attr(node_set[0], "id", 0);
	ASSERT_STRING_EQUAL(roxml_get_content(attr, NULL, 0, NULL), "4")

	node_set = roxml_xpath(root, "/node0/item[@val > 2.1]", &nbans);
	ASSERT_EQUAL(nbans, 2)
	attr = roxml_get_attr(node_set[0], "id", 0);
	ASSERT_STRING_EQUAL(roxml_get_content(attr, NULL, 0, NULL), "4")
	attr = roxml_get_attr(node_set[1], "id", 0);
	ASSERT_STRING_EQUAL(roxml_get_content(attr, NULL, 0, NULL), "5")

	node_set = roxml_xpath(root, "/node0/item[@val <= 2]", &nbans);
	ASSERT_EQUAL(nbans, 4)
	attr = roxml_get_attr(node_set[0], "id", 0);
	ASSERT_STRING_EQUAL(roxml_get_content(attr, NULL, 0, NULL), "0")
	attr = roxml_get_attr(node_set[1], "id", 0);
	ASSERT_STRING_EQUAL(roxml_get_content(attr, NULL, 0, NULL), "1")
	attr = roxml_get_attr(node_set[2], "id", 0);
	ASSERT_STRING_EQUAL(roxml_get_content(attr, NULL, 0, NULL), "2")
	attr = roxml_get_attr(node_set[3], "id", 0);
	ASSERT_STRING_EQUAL(roxml_get_content(attr, NULL, 0, NULL), "3")

	node_set = roxml_xpath(root, "/node0/item[@val >= 2]", &nbans);
	ASSERT_EQUAL(nbans, 5)
	attr = roxml_get_attr(node_set[0], "id", 0);
	ASSERT_STRING_EQUAL(roxml_get_content(attr, NULL, 0, NULL), "1")
	attr = roxml_get_attr(node_set[1], "id", 0);
	ASSERT_STRING_EQUAL(roxml_get_content(attr, NULL, 0, NULL), "2")
	attr = roxml_get_attr(node_set[2], "id", 0);
	ASSERT_STRING_EQUAL(roxml_get_content(attr, NULL, 0, NULL), "3")
	attr = roxml_get_attr(node_set[3], "id", 0);
	ASSERT_STRING_EQUAL(roxml_get_content(attr, NULL, 0, NULL), "4")
	attr = roxml_get_attr(node_set[4], "id", 0);
	ASSERT_STRING_EQUAL(roxml_get_content(attr, NULL, 0, NULL), "5")

	node_set = roxml_xpath(root, "/node0/item[@val < 2]", &nbans);
	ASSERT_EQUAL(nbans, 1)
	attr = roxml_get_attr(node_set[0], "id", 0);
	ASSERT_STRING_EQUAL(roxml_get_content(attr, NULL, 0, NULL), "0")

	node_set = roxml_xpath(root, "/node0/item[@val = 3]", &nbans);
	ASSERT_EQUAL(nbans, 1)
	attr = roxml_get_attr(node_set[0], "id", 0);
	ASSERT_STRING_EQUAL(roxml_get_content(attr, NULL, 0, NULL), "4")

	node_set = roxml_xpath(root, "/node0/item[@val != 3]", &nbans);
	ASSERT_EQUAL(nbans, 5)
	attr = roxml_get_attr(node_set[0], "id", 0);
	ASSERT_STRING_EQUAL(roxml_get_content(attr, NULL, 0, NULL), "0")
	attr = roxml_get_attr(node_set[1], "id", 0);
	ASSERT_STRING_EQUAL(roxml_get_content(attr, NULL, 0, NULL), "1")
	attr = roxml_get_attr(node_set[2], "id", 0);
	ASSERT_STRING_EQUAL(roxml_get_content(attr, NULL, 0, NULL), "2")
	attr = roxml_get_attr(node_set[3], "id", 0);
	ASSERT_STRING_EQUAL(roxml_get_content(attr, NULL, 0, NULL), "3")
	attr = roxml_get_attr(node_set[4], "id", 0);
	ASSERT_STRING_EQUAL(roxml_get_content(attr, NULL, 0, NULL), "5")

	node_set = roxml_xpath(root, "/node0/item[0]", &nbans);
	ASSERT_EQUAL(nbans, 0)
	
	node_set = roxml_xpath(root, "/node0/item[1]", &nbans);
	ASSERT_EQUAL(nbans, 1)
	attr = roxml_get_attr(node_set[0], "id", 0);
	ASSERT_STRING_EQUAL(roxml_get_content(attr, NULL, 0, NULL), "0")
	
	node_set = roxml_xpath(root, "/node0/item[first()]", &nbans);
	ASSERT_EQUAL(nbans, 1)
	attr = roxml_get_attr(node_set[0], "id", 0);
	ASSERT_STRING_EQUAL(roxml_get_content(attr, NULL, 0, NULL), "0")
	
	node_set = roxml_xpath(root, "/node0/item[last()]", &nbans);
	ASSERT_EQUAL(nbans, 1)
	attr = roxml_get_attr(node_set[0], "id", 0);
	ASSERT_STRING_EQUAL(roxml_get_content(attr, NULL, 0, NULL), "5")
	
	node_set = roxml_xpath(root, "/node0/item[last() - 1]", &nbans);
	ASSERT_EQUAL(nbans, 1)
	attr = roxml_get_attr(node_set[0], "id", 0);
	ASSERT_STRING_EQUAL(roxml_get_content(attr, NULL, 0, NULL), "4")
	
	node_set = roxml_xpath(root, "/node0/item[first() + 1]", &nbans);
	ASSERT_EQUAL(nbans, 1)
	attr = roxml_get_attr(node_set[0], "id", 0);
	ASSERT_STRING_EQUAL(roxml_get_content(attr, NULL, 0, NULL), "1")
	
	node_set = roxml_xpath(root, "/node0/item[position() = 3]", &nbans);
	ASSERT_EQUAL(nbans, 1)
	attr = roxml_get_attr(node_set[0], "id", 0);
	ASSERT_STRING_EQUAL(roxml_get_content(attr, NULL, 0, NULL), "2")
	
	node_set = roxml_xpath(root, "/node0/item[3]", &nbans);
	ASSERT_EQUAL(nbans, 1)
	attr = roxml_get_attr(node_set[0], "id", 0);
	ASSERT_STRING_EQUAL(roxml_get_content(attr, NULL, 0, NULL), "2")
	
	node_set = roxml_xpath(root, "/node0/item[position() != 2]", &nbans);
	ASSERT_EQUAL(nbans, 5)
	attr = roxml_get_attr(node_set[0], "id", 0);
	ASSERT_STRING_EQUAL(roxml_get_content(attr, NULL, 0, NULL), "0")
	attr = roxml_get_attr(node_set[1], "id", 0);
	ASSERT_STRING_EQUAL(roxml_get_content(attr, NULL, 0, NULL), "2")
	attr = roxml_get_attr(node_set[2], "id", 0);
	ASSERT_STRING_EQUAL(roxml_get_content(attr, NULL, 0, NULL), "3")
	attr = roxml_get_attr(node_set[3], "id", 0);
	ASSERT_STRING_EQUAL(roxml_get_content(attr, NULL, 0, NULL), "4")
	attr = roxml_get_attr(node_set[4], "id", 0);
	ASSERT_STRING_EQUAL(roxml_get_content(attr, NULL, 0, NULL), "5")
	
	node_set = roxml_xpath(root, "/node0/item[position() < 3]", &nbans);
	ASSERT_EQUAL(nbans, 2)
	attr = roxml_get_attr(node_set[0], "id", 0);
	ASSERT_STRING_EQUAL(roxml_get_content(attr, NULL, 0, NULL), "0")
	attr = roxml_get_attr(node_set[1], "id", 0);
	ASSERT_STRING_EQUAL(roxml_get_content(attr, NULL, 0, NULL), "1")
	
	node_set = roxml_xpath(root, "/node0/item[position() > 3]", &nbans);
	ASSERT_EQUAL(nbans, 3)
	attr = roxml_get_attr(node_set[0], "id", 0);
	ASSERT_STRING_EQUAL(roxml_get_content(attr, NULL, 0, NULL), "3")
	attr = roxml_get_attr(node_set[1], "id", 0);
	ASSERT_STRING_EQUAL(roxml_get_content(attr, NULL, 0, NULL), "4")
	attr = roxml_get_attr(node_set[2], "id", 0);
	ASSERT_STRING_EQUAL(roxml_get_content(attr, NULL, 0, NULL), "5")
	
	node_set = roxml_xpath(root, "/node0/item[position() <= 3]", &nbans);
	ASSERT_EQUAL(nbans, 3)
	attr = roxml_get_attr(node_set[0], "id", 0);
	ASSERT_STRING_EQUAL(roxml_get_content(attr, NULL, 0, NULL), "0")
	attr = roxml_get_attr(node_set[1], "id", 0);
	ASSERT_STRING_EQUAL(roxml_get_content(attr, NULL, 0, NULL), "1")
	attr = roxml_get_attr(node_set[2], "id", 0);
	ASSERT_STRING_EQUAL(roxml_get_content(attr, NULL, 0, NULL), "2")
	
	node_set = roxml_xpath(root, "/node0/item[position() >= 3]", &nbans);
	ASSERT_EQUAL(nbans, 4)
	attr = roxml_get_attr(node_set[0], "id", 0);
	ASSERT_STRING_EQUAL(roxml_get_content(attr, NULL, 0, NULL), "2")
	attr = roxml_get_attr(node_set[1], "id", 0);
	ASSERT_STRING_EQUAL(roxml_get_content(attr, NULL, 0, NULL), "3")
	attr = roxml_get_attr(node_set[2], "id", 0);
	ASSERT_STRING_EQUAL(roxml_get_content(attr, NULL, 0, NULL), "4")
	attr = roxml_get_attr(node_set[3], "id", 0);
	ASSERT_STRING_EQUAL(roxml_get_content(attr, NULL, 0, NULL), "5")

	roxml_close(root);

	root = roxml_load_doc("roxml.test.xml.multiple");
	ASSERT_NOT_NULL(root)
	
	node_set = roxml_xpath(root, "node", &nbans);
	ASSERT_EQUAL(nbans, 0)

	node_set = roxml_xpath(root, "//node", &nbans);
	ASSERT_EQUAL(nbans, 5)
	ASSERT_STRING_EQUAL(roxml_get_name(node_set[0], NULL, 0), "node")
	ASSERT_STRING_EQUAL(roxml_get_content(node_set[0], NULL, 0, NULL), "1")
	ASSERT_STRING_EQUAL(roxml_get_name(node_set[1], NULL, 0), "node")
	ASSERT_STRING_EQUAL(roxml_get_content(node_set[1], NULL, 0, NULL), "2")
	ASSERT_STRING_EQUAL(roxml_get_name(node_set[2], NULL, 0), "node")
	ASSERT_STRING_EQUAL(roxml_get_content(node_set[2], NULL, 0, NULL), "3")
	ASSERT_STRING_EQUAL(roxml_get_name(node_set[3], NULL, 0), "node")
	ASSERT_STRING_EQUAL(roxml_get_content(node_set[3], NULL, 0, NULL), "4")
	ASSERT_STRING_EQUAL(roxml_get_name(node_set[4], NULL, 0), "node")
	ASSERT_STRING_EQUAL(roxml_get_content(node_set[4], NULL, 0, NULL), "5")

	node_set = roxml_xpath(root, "/node0/node[1]", &nbans);
	ASSERT_EQUAL(nbans, 1)
	ASSERT_STRING_EQUAL(roxml_get_content(node_set[0], NULL, 0, NULL), "1")

	node_set = roxml_xpath(root, "/node0/*[1]", &nbans);
	ASSERT_EQUAL(nbans, 1)
	ASSERT_STRING_EQUAL(roxml_get_name(node_set[0], NULL, 0), "item")

	node_set = roxml_xpath(root, "/node0//node2/node3//node5", &nbans);
	ASSERT_EQUAL(nbans, 3)
	ASSERT_STRING_EQUAL(roxml_get_name(node_set[0], NULL, 0), "node5")
	ASSERT_STRING_EQUAL(roxml_get_name(node_set[1], NULL, 0), "node5")
	ASSERT_STRING_EQUAL(roxml_get_name(node_set[2], NULL, 0), "node5")

	roxml_close(root);

	roxml_release(RELEASE_ALL);

	RETURN
}

int test_get_node_type(void)
{
	INIT

	int type;

	node_t * cnode = roxml_create_node(1, NULL, ROXML_FILE | ROXML_ELM_NODE);
	node_t * anode = roxml_create_node(1, NULL, ROXML_FILE | ROXML_ATTR_NODE);

	type = roxml_get_type(cnode);
	ASSERT_EQUAL(type & ROXML_ELM_NODE, ROXML_ELM_NODE)
	ASSERT_NOT_EQUAL(type & ROXML_ATTR_NODE, ROXML_ATTR_NODE)

	type = roxml_get_type(anode);
	ASSERT_NOT_EQUAL(type & ROXML_ELM_NODE, ROXML_ELM_NODE)
	ASSERT_EQUAL(type & ROXML_ATTR_NODE, ROXML_ATTR_NODE)

	roxml_free_node(cnode);
	roxml_free_node(anode);

	RETURN
}

int test_get_node_index(void)
{
	INIT

	node_t *root = roxml_load_doc("roxml.test.xml.multiple");

	node_t * node0 = roxml_get_chld(root, NULL, 0);
	node_t * node = roxml_get_chld(node0, NULL, 0);
	ASSERT_EQUAL(roxml_get_node_position(node), 1)
	node = roxml_get_chld(node0, NULL, 1);
	ASSERT_EQUAL(roxml_get_node_position(node), 1)
	node = roxml_get_chld(node0, NULL, 2);
	ASSERT_EQUAL(roxml_get_node_position(node), 2)
	node = roxml_get_chld(node0, NULL, 3);
	ASSERT_EQUAL(roxml_get_node_position(node), 3)
	node = roxml_get_chld(node0, NULL, 4);
	ASSERT_EQUAL(roxml_get_node_position(node), 4)
	
	roxml_close(root);

	RETURN
}

int test_spec_nodes(void)
{
	INIT /* init context macro */

	int nbans;
	node_t *root = roxml_load_doc("roxml.test.xml.specnodes");

	// test internal struct
	ASSERT_NOT_NULL(root)
	ASSERT_NOT_NULL(root->chld) // DOCTYPE
	ASSERT_NOT_NULL(root->chld->sibl) // node
	ASSERT_NOT_NULL(root->chld->sibl->chld) // comment
	ASSERT_NOT_NULL(root->chld->sibl->chld->sibl) // pi
	ASSERT_NOT_NULL(root->chld->sibl->chld->sibl->sibl) // node1
	ASSERT_NOT_NULL(root->chld->sibl->chld->sibl->sibl->text) // text
	ASSERT_NULL(root->chld->sibl->chld->sibl->sibl->chld) // node2
	ASSERT_EQUAL(root->chld->sibl->chld->type, ROXML_CMT_NODE | ROXML_FILE) // comment
	ASSERT_EQUAL(root->chld->sibl->chld->sibl->type, ROXML_PI_NODE | ROXML_FILE) // pi

	// test content
	char * content = roxml_get_content(root->chld->sibl->chld, NULL, 0, NULL);
	ASSERT_STRING_EQUAL(content, "this is a comment")
	content = roxml_get_content(root->chld->sibl->chld->sibl, NULL, 0, NULL);
	ASSERT_STRING_EQUAL(content, " value=\"2\" ")
	content = roxml_get_content(root->chld->sibl->chld->sibl->sibl, NULL, 0, NULL);
	ASSERT_STRING_EQUAL(content, " <![CDATA[ <node2></node2> ]]> ")

	// test xpath
	node_t **node_set = roxml_xpath(root, "//comment()", &nbans);
	ASSERT_EQUAL(nbans, 2)
	ASSERT_STRING_EQUAL(roxml_get_content(node_set[0], NULL, 0, NULL), "this is a comment")
	ASSERT_STRING_EQUAL(roxml_get_content(node_set[1], NULL, 0, NULL), " <toto/> ")
	node_set = roxml_xpath(root, "//processing-instruction()", &nbans);
	ASSERT_EQUAL(nbans, 1)
	ASSERT_STRING_EQUAL(roxml_get_content(node_set[0], NULL, 0, NULL), " value=\"2\" ")
	node_set = roxml_xpath(root, "//*", &nbans);
	ASSERT_EQUAL(nbans, 3)
	ASSERT_STRING_EQUAL(roxml_get_name(node_set[0], NULL, 0), "!DOCTYPE")
	ASSERT_STRING_EQUAL(roxml_get_name(node_set[1], NULL, 0), "node")
	ASSERT_STRING_EQUAL(roxml_get_name(node_set[2], NULL, 0), "node1")
	node_set = roxml_xpath(root, "//node()", &nbans);
	ASSERT_EQUAL(nbans, 7)
	ASSERT_EQUAL(roxml_get_type(node_set[0]), ROXML_ELM_NODE)
	ASSERT_EQUAL(roxml_get_type(node_set[1]), ROXML_ELM_NODE)
	ASSERT_EQUAL(roxml_get_type(node_set[2]), ROXML_TXT_NODE)
	ASSERT_EQUAL(roxml_get_type(node_set[3]), ROXML_CMT_NODE)
	ASSERT_EQUAL(roxml_get_type(node_set[4]), ROXML_PI_NODE)
	ASSERT_EQUAL(roxml_get_type(node_set[5]), ROXML_ELM_NODE)
	ASSERT_EQUAL(roxml_get_type(node_set[6]), ROXML_CMT_NODE)
	ASSERT_STRING_EQUAL(roxml_get_name(node_set[0], NULL, 0), "!DOCTYPE")
	ASSERT_STRING_EQUAL(roxml_get_name(node_set[1], NULL, 0), "node")
	ASSERT_STRING_EQUAL(roxml_get_name(node_set[5], NULL, 0), "node1")
	ASSERT_STRING_EQUAL(roxml_get_content(node_set[2], NULL, 0, NULL), " <![CDATA[ <node2></node2> ]]> ")
	ASSERT_STRING_EQUAL(roxml_get_content(node_set[3], NULL, 0, NULL), "this is a comment")
	ASSERT_STRING_EQUAL(roxml_get_content(node_set[4], NULL, 0, NULL), " value=\"2\" ")
	ASSERT_STRING_EQUAL(roxml_get_content(node_set[5], NULL, 0, NULL), " <![CDATA[ <node2></node2> ]]> ")
	ASSERT_STRING_EQUAL(roxml_get_content(node_set[6], NULL, 0, NULL), " <toto/> ")

	roxml_release(RELEASE_ALL);
	roxml_close(root);
	
	RETURN /* close context macro */
}

int test_whitespaces_in_attr(void)
{
	INIT /* init context macro */

	int nb_ans;
	node_t *root = roxml_load_doc("roxml.test.xml.whitespaces");
	node_t *node1 = roxml_get_chld(root, "node1", 0);
	node_t *node2 = roxml_get_chld(root, "node2", 0);
	node_t *node3 = roxml_get_chld(root, "node3", 0);

	// test content
	nb_ans = roxml_get_attr_nb(node1);
	ASSERT_EQUAL(nb_ans, 1)
	nb_ans = roxml_get_attr_nb(node2);
	ASSERT_EQUAL(nb_ans, 1)
	nb_ans = roxml_get_attr_nb(node3);
	ASSERT_EQUAL(nb_ans, 1)

	node_t *attr1 = roxml_get_attr(node1, NULL, 0);
	node_t *attr2 = roxml_get_attr(node2, NULL, 0);
	node_t *attr3 = roxml_get_attr(node3, NULL, 0);

	char * content = roxml_get_content(attr1, NULL, 0, NULL);
	ASSERT_STRING_EQUAL(content, "toto et titi");
	content = roxml_get_content(attr2, NULL, 0, NULL);
	ASSERT_STRING_EQUAL(content, "toto et titi");
	content = roxml_get_content(attr3, NULL, 0, NULL);
	ASSERT_NOT_NULL(content);
	ASSERT_STRING_EQUAL(content, "toto et titi");

	roxml_release(RELEASE_ALL);
	roxml_close(root);
	
	RETURN /* close context macro */
}

int test_write_tree(void)
{
	char * buffer;
	INIT /* init context macro */

	node_t *root = roxml_load_doc("roxml.test.xml");
	node_t *node = roxml_add_node(root, 1, ROXML_CMT_NODE, NULL, "this is a test XML file");
	node = roxml_add_node(root, 0, ROXML_CMT_NODE, NULL, "this was a test XML file");
	node = roxml_add_node(root, 2, ROXML_PI_NODE, NULL, "value=\"2\"");
	roxml_commit_changes(root, "out.xml.copy", NULL, 1);
	roxml_close(root);

	root = roxml_add_node(NULL, 0, ROXML_ELM_NODE, "xml", NULL);
	node = roxml_add_node(root, 0, ROXML_CMT_NODE, NULL, "this is a test XML file");
	node = roxml_add_node(root, 0, ROXML_PI_NODE, NULL, "value=\"2\"");
	node = roxml_add_node(root, 0, ROXML_ELM_NODE, "node1", "content1");
	roxml_add_node(root, 0, ROXML_ATTR_NODE, "attr1", "value1");
	node_t *node2 = roxml_add_node(node, 0, ROXML_ELM_NODE, "node2", "content2");
	roxml_add_node(node, 0, ROXML_TXT_NODE, NULL, "content1bis");
	roxml_add_node(node2, 0, ROXML_TXT_NODE, NULL, "content2bis");
	roxml_add_node(node, 0, ROXML_ELM_NODE, "node3", "content3");
	
	ASSERT_NOT_NULL(root)	// xml
	ASSERT_NOT_NULL(root->attr) // attr1
	ASSERT_NOT_NULL(root->chld) // cmt
	ASSERT_NOT_NULL(root->chld->sibl) // pi
	ASSERT_NOT_NULL(root->chld->sibl->sibl) // node1
	ASSERT_NOT_NULL(root->chld->sibl->sibl->text) // content1
	ASSERT_NOT_NULL(root->chld->sibl->sibl->text->sibl) // content1bis
	ASSERT_NOT_NULL(root->chld->sibl->sibl->chld) // node2
	ASSERT_NOT_NULL(root->chld->sibl->sibl->chld->text) // content2
	ASSERT_NOT_NULL(root->chld->sibl->sibl->chld->text->sibl) // content2bis
	ASSERT_NOT_NULL(root->chld->sibl->sibl->chld->sibl) // node3
	ASSERT_NOT_NULL(root->chld->sibl->sibl->chld->sibl->text) // content3

	roxml_commit_changes(root, "out.xml", NULL, 1);
	roxml_commit_changes(root, "out.xml.human", NULL, 0);
	int len = roxml_commit_changes(root, NULL, &buffer, 0);

	FILE * fout = fopen("out.buf.xml", "w");
	fwrite(buffer, 1, len, fout);
	fclose(fout);
	free(buffer);

	printf("==================================\n");

	len = roxml_commit_changes(root, NULL, &buffer, 1);
	fout = fopen("out.buf.xml.human", "w");
	fwrite(buffer, 1, len, fout);
	fclose(fout);
	free(buffer);

	roxml_close(root);
	
	RETURN /* close context macro */
}


int main(int argc, char ** argv)	{
	INIT /* init context macro */

	TEST_FUNC(test_load_doc) 
	TEST_FUNC(test_load_fd)
	TEST_FUNC(test_tree_on_doc) 
	TEST_FUNC(test_names_on_doc) 
	TEST_FUNC(test_load_buf) 
	TEST_FUNC(test_tree_on_buf) 
	TEST_FUNC(test_names_on_buf) 
	TEST_FUNC(test_load_human_doc) 
	TEST_FUNC(test_tree_on_human_doc) 
	TEST_FUNC(test_names_on_human_doc) 
	TEST_FUNC(test_load_human_buf) 
	TEST_FUNC(test_tree_on_human_buf) 
	TEST_FUNC(test_names_on_human_buf) 
	TEST_FUNC(test_malloc_release) 
	TEST_FUNC(test_create_node)
	TEST_FUNC(test_get_root)
	TEST_FUNC(test_get_chld)
	TEST_FUNC(test_get_sibl)
	TEST_FUNC(test_get_chld_nb)
	TEST_FUNC(test_get_content)
	TEST_FUNC(test_get_attr_nb)
	TEST_FUNC(test_get_attr)
	TEST_FUNC(test_parse_xpath)
	TEST_FUNC(test_xpath)
	TEST_FUNC(test_get_node_type)
	TEST_FUNC(test_get_node_index)
	TEST_FUNC(test_write_tree) 
	TEST_FUNC(test_spec_nodes) 
	TEST_FUNC(test_whitespaces_in_attr)

	EXEC_UNITTEST /* exec tests depending on command line option see available options with --help */

	RETURN /* close context macro */
}
