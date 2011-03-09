
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include "roxml-internal.h"

int main(int argc, char ** argv)
{
	int nb, len, ram;
	char input[128] = "test-1200K.xml";

	if(argc < 2)	{
		printf("no input file use '%s'\n", input);
	} else	{
		strncpy(input, argv[1], 128);
	}

	FILE * f = fopen(input, "r");
	if(f)	{
		fseek(f, 0, SEEK_END);
		len = ftell(f);
		fclose(f);
	} else	{
		printf("input file doesnt exist\n");
		return 1;
	}

	node_t *root = roxml_load_doc(input);

	int pgsize = 0;
	int size = 0;
	int resident = 0;
	int share = 0;
	int text = 0;
	int lib = 0;
	int data = 0;
	int dt = 0;
	char statfile[512];
	sprintf(statfile, "/proc/%d/statm", getpid());
	FILE * fstatfile = fopen(statfile, "r");
	if(fstatfile)   {
		fscanf(fstatfile, "%d %d %d %d %d %d %d", &size, &resident, &share, &text, &lib, &data, &dt);
		fclose(fstatfile);
	}
	pgsize = getpagesize();
	ram = pgsize*resident;

	node_t ** set = roxml_xpath(root, "//*", &nb);

	roxml_release(set);
	roxml_close(root);

	printf("stats for '%s':\n", input);
	printf("\tload_doc: %d bytes\n", len);
	printf("\txpath processed %d nodes\n", nb);
	printf("\ttotal node:\t\t\t %u\n",_nb_node);
	printf("\ttotal attr:\t\t\t %u\n",_nb_attr);
	printf("\ttotal text:\t\t\t %u\n",_nb_text);
	printf("\tram tree:\t\t\t %lu bytes\n",sizeof(node_t)*(_nb_node+_nb_attr+_nb_text));
	printf("\tram total:\t\t\t %d bytes\n",ram);
	printf("\tram ratio:\t\t\t %f\n",(float)(sizeof(node_t)*(_nb_node+_nb_attr+_nb_text))/(float)len);

	return 0;
}

