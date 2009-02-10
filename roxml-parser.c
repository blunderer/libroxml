
#include "roxml.h"

int main(int argc, char ** argv)
{
	int j = 0;
	node_t *root;
	node_t *cur;
	
	if(argc < 2)	{
		return -1;
	}
	root = roxml_load_doc(argv[1]);
	cur = root;
	
	for(j = 2; j < argc; j++)	{
		int found = 0;
		int i, nb = roxml_get_son_nb(cur);
		for(i = 0; i < nb && !found; i++)	{
			if(strcmp(roxml_get_name(roxml_get_son_nth(cur, i)), argv[j])==0)	{
				if(roxml_get_son_nb(roxml_get_son_nth(cur, i)) > 0)	{
					found = 1;
					cur = roxml_get_son_nth(cur, i);
				} else	{
					int len = 0;
					char *content;
					len = roxml_get_content(roxml_get_son_nth(cur, i), NULL);
					content = (char*)malloc(sizeof(char)*(len+1));
					roxml_get_content(roxml_get_son_nth(cur, i), content);
					fprintf(stdout,"%s\n",content);
					free(content);
				}
			}
		}
		if(found == 0)	{
			break;
		}
	}

	roxml_close_doc(root);
	return 0;
}

