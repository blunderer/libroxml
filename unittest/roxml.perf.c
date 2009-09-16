
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/time.h>
#include "../roxml-internal.h"

int main(int argc, char ** argv)
{
	int nb, len, ram;
	char *name = NULL;
	struct timeval dt_load;
	struct timeval dt_close;
	struct timeval dt_xpath;
	struct timeval dt_name1;
	struct timeval dt_name2;
	struct timeval dt_release;
	struct timeval tv1, tv2;

	if(argc < 2)	{
		printf("no input file\n");
		return 1;
	}

	FILE * f = fopen(argv[1], "r");
	if(f)	{
		fseek(f, 0, SEEK_END);
		len = ftell(f);
		fclose(f);
	} else	{
		printf("input file doesnt exist\n");
		return 1;
	}

	gettimeofday(&tv1, NULL);
	node_t *root = roxml_load_doc(argv[1]);
	gettimeofday(&tv2, NULL);
	dt_load.tv_sec = (tv2.tv_sec - tv1.tv_sec);
	dt_load.tv_usec = (tv2.tv_usec - tv1.tv_usec);

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

	gettimeofday(&tv1, NULL);
	node_t ** set = roxml_xpath(root, "//*", &nb);
	gettimeofday(&tv2, NULL);
	dt_xpath.tv_sec = (tv2.tv_sec - tv1.tv_sec);
	dt_xpath.tv_usec = (tv2.tv_usec - tv1.tv_usec);

	gettimeofday(&tv1, NULL);
	name = roxml_get_name(set[0], NULL, 0);
	gettimeofday(&tv2, NULL);
	roxml_release(name);
	dt_name1.tv_sec = (tv2.tv_sec - tv1.tv_sec);
	dt_name1.tv_usec = (tv2.tv_usec - tv1.tv_usec);

	gettimeofday(&tv1, NULL);
	name = roxml_get_name(set[nb-1], NULL, 0);
	roxml_release(name);
	gettimeofday(&tv2, NULL);
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


	printf("stats for '%s':\n", argv[1]);
	printf("\tload_doc (%d bytes):\t\t %d.%06d sec\n", len, (int)dt_load.tv_sec, (int)dt_load.tv_usec);
	printf("\txpath processed %d nodes:\t %d.%06d sec\n", nb, (int)dt_xpath.tv_sec, (int)dt_xpath.tv_usec);
	printf("\tread first node:\t\t %d.%06d sec\n", (int)dt_name1.tv_sec, (int)dt_name1.tv_usec);
	printf("\tread last node:\t\t\t %d.%06d sec\n", (int)dt_name2.tv_sec, (int)dt_name2.tv_usec);
	printf("\trelease result:\t\t\t %d.%06d sec\n", (int)dt_release.tv_sec, (int)dt_release.tv_usec);
	printf("\tclose_doc:\t\t\t %d.%06d sec\n", (int)dt_close.tv_sec, (int)dt_close.tv_usec);
	printf("\tram tree:\t\t\t %lu bytes\n",sizeof(node_t)*nb);
	printf("\tram total:\t\t\t %d bytes\n",ram);
	printf("\tram ratio:\t\t\t %f\n",(float)(sizeof(node_t)*nb)/(float)len);

	return 0;
}

