
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/time.h>
#include "roxml-internal.h"

int main(int argc, char ** argv)
{
	int nb, len;
	//char *name = NULL;
	struct timeval dt_load;
	struct timeval dt_close;
	struct timeval dt_xpath;
	struct timeval dt_name1;
	struct timeval dt_name2;
	struct timeval dt_release;
	struct timeval tv1, tv2;
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

	gettimeofday(&tv1, NULL);
	node_t *root = roxml_load_doc(input);
	gettimeofday(&tv2, NULL);
	dt_load.tv_sec = (tv2.tv_sec - tv1.tv_sec);
	dt_load.tv_usec = (tv2.tv_usec - tv1.tv_usec);

	gettimeofday(&tv1, NULL);
	node_t ** set = roxml_xpath(root, "//*", &nb);
	gettimeofday(&tv2, NULL);
	dt_xpath.tv_sec = (tv2.tv_sec - tv1.tv_sec);
	dt_xpath.tv_usec = (tv2.tv_usec - tv1.tv_usec);

	gettimeofday(&tv1, NULL);
//	name = roxml_get_name(set[0], NULL, 0);
	gettimeofday(&tv2, NULL);
//	roxml_release(name);
	dt_name1.tv_sec = (tv2.tv_sec - tv1.tv_sec);
	dt_name1.tv_usec = (tv2.tv_usec - tv1.tv_usec);

	gettimeofday(&tv1, NULL);
//	name = roxml_get_name(set[nb-1], NULL, 0);
	gettimeofday(&tv2, NULL);
//	roxml_release(name);
	dt_name2.tv_sec = (tv2.tv_sec - tv1.tv_sec);
	dt_name2.tv_usec = (tv2.tv_usec - tv1.tv_usec);

	gettimeofday(&tv1, NULL);
	roxml_release(set);
	gettimeofday(&tv2, NULL);
	dt_release.tv_sec = (tv2.tv_sec - tv1.tv_sec);
	dt_release.tv_usec = (tv2.tv_usec - tv1.tv_usec);

	gettimeofday(&tv1, NULL);
	roxml_close(root);
	gettimeofday(&tv2, NULL);
	dt_close.tv_sec = (tv2.tv_sec - tv1.tv_sec);
	dt_close.tv_usec = (tv2.tv_usec - tv1.tv_usec);

	if(dt_load.tv_usec < 0) { dt_load.tv_sec++; dt_load.tv_usec = 1000000-dt_load.tv_usec; }
	if(dt_xpath.tv_usec < 0) { dt_xpath.tv_sec++; dt_xpath.tv_usec = 1000000-dt_xpath.tv_usec; }
	if(dt_name1.tv_usec < 0) { dt_name1.tv_sec++; dt_name1.tv_usec = 1000000-dt_name1.tv_usec; }
	if(dt_name2.tv_usec < 0) { dt_name2.tv_sec++; dt_name2.tv_usec = 1000000-dt_name2.tv_usec; }
	if(dt_release.tv_usec < 0) { dt_release.tv_sec++; dt_release.tv_usec = 1000000-dt_release.tv_usec; }
	if(dt_close.tv_usec < 0) { dt_close.tv_sec++; dt_close.tv_usec = 1000000-dt_close.tv_usec; }

	printf("stats for '%s':\n", input);
	printf("\tload_doc (%d bytes):\t %d.%06d sec\n", len, (int)dt_load.tv_sec, (int)dt_load.tv_usec);
	printf("\txpath processed %d nodes:\t %d.%06d sec\n", nb, (int)dt_xpath.tv_sec, (int)dt_xpath.tv_usec);
	//printf("\tread first node:\t\t %d.%06d sec\n", (int)dt_name1.tv_sec, (int)dt_name1.tv_usec);
	//printf("\tread last node:\t\t\t %d.%06d sec\n", (int)dt_name2.tv_sec, (int)dt_name2.tv_usec);
	printf("\trelease result:\t\t\t %d.%06d sec\n", (int)dt_release.tv_sec, (int)dt_release.tv_usec);
	printf("\tclose_doc:\t\t\t %d.%06d sec\n", (int)dt_close.tv_sec, (int)dt_close.tv_usec);

	return 0;
}

