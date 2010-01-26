
#ifdef UNITTEST

#ifndef UNITTEST_CPP
#define UNITTEST_CPP

#include "unittest.h"

unittest_func * unittest_head = NULL;
unittest_func unittest_pre = {NULL, {0}, NULL, 0};
unittest_func unittest_post = {NULL, {0}, NULL, 0}; 

void setpre(PROTO_FUNC func)
{
	unittest_pre.func = func;
}

void setpost(PROTO_FUNC func)
{
	unittest_post.func = func;
}

void setfunc(PROTO_FUNC func, char * name)
{
	unittest_func * unittest_list = unittest_head;
	if(unittest_list == NULL)	{
		unittest_head = (unittest_func*)calloc(1, sizeof(unittest_func));
		unittest_head->next = NULL;
		unittest_head->func = func;
		memset(unittest_head->name, 0, 256*sizeof(char));
		strncpy(unittest_head->name, name, 255);
		
	} else	{
		while(unittest_list->next != NULL)	unittest_list = unittest_list->next;
		unittest_list->next = (unittest_func*)calloc(1, sizeof(unittest_func));
		unittest_list = unittest_list->next;
		unittest_list->next = NULL;
		unittest_list->func = func;
		memset(unittest_list->name, 0, 256*sizeof(char));
		strncpy(unittest_list->name, name, 255);
	}
}

int unittest_utils(int argc, char ** argv)
{
	long unittest_ret = 0;
	if((strcmp(argv[1], "help") == 0) ||
		(strcmp(argv[1], "h") == 0) ||
		(strcmp(argv[1], "-h") == 0) ||
		(strcmp(argv[1], "-help") == 0) ||
		(strcmp(argv[1], "--help") == 0))
	{
		printf("unittest API by tristan LELONG\n"
			" <unitest> <options>\n"
			" with <unittest> the name of binary file and <options> one of the following:\n"
			"  --help / -h   : will print this help.\n"
			"  --list / -l   : will list available unittest in binary file. Return total number of tests.\n"
			"  --exec <num>  : will execute the <num> test (starting from 1). Return number of failed tests\n"
			"  --exec <name> : will execute the test named <name>. Return number of failed tests\n"
			"\n"
			" note: you can mix <num> and <name> by calling <unittest> --exec <num> <name> ...\n"
			"	using <unittest> with no arguments will execute the whole unittest serie\n\n");
		unittest_func * unittest_list = unittest_head;
		while(unittest_list != NULL)	{
			unittest_func * old_func = unittest_list;
			unittest_ret += unittest_list->return_value;
			unittest_list = unittest_list->next;
			free(old_func);
		}
		return 0;
	}
	if((strcmp(argv[1], "exec") == 0) ||
		(strcmp(argv[1], "e") == 0) ||
		(strcmp(argv[1], "-e") == 0) ||
		(strcmp(argv[1], "-exec") == 0) ||
		(strcmp(argv[1], "--exec") == 0))
	{
		int i = 0;
		for(i = 0; i < argc - 2; i++)	{
			unittest_func * unittest_list = unittest_head;
			int index = 0, count = 0;
			index = atoi(argv[i+2]);
			while(unittest_list != NULL)    {
				count++;
				if(index == 0)	{
					if(strcmp(unittest_list->name, argv[i+2]) == 0)	{
						if(unittest_pre.func != NULL)	unittest_pre.func();
						unittest_list->return_value = unittest_list->func();
						if(unittest_post.func != NULL)	unittest_post.func();
					}
				} else if(index == count)	{
					if(unittest_pre.func != NULL)	unittest_pre.func();
					unittest_list->return_value = unittest_list->func();
					if(unittest_post.func != NULL)	unittest_post.func();
				}
				unittest_list = unittest_list->next;
			}
		}
		unittest_func * unittest_list = unittest_head;
		while(unittest_list != NULL)	{
			unittest_func * old_func = unittest_list;
			unittest_ret += unittest_list->return_value;
			unittest_list = unittest_list->next;
			free(old_func);
		}
		return unittest_ret;	
	}
	if((strcmp(argv[1], "list") == 0) ||
		(strcmp(argv[1], "l") == 0) ||
		(strcmp(argv[1], "-l") == 0) ||
		(strcmp(argv[1], "-list") == 0) ||
		(strcmp(argv[1], "--list") == 0))
	{
		int count = 0;
		unittest_func * unittest_list = unittest_head;
		while(unittest_list != NULL)	{
			count++;
			printf("%s\n",unittest_list->name);
			unittest_list = unittest_list->next;
		}
		unittest_list = unittest_head;
		while(unittest_list != NULL)	{
			unittest_func * old_func = unittest_list;
			unittest_ret += unittest_list->return_value;
			unittest_list = unittest_list->next;
			free(old_func);
		}
		return count;
	}

	return 0;
}

int exectest(int argc, char ** argv)
{
	long unittest_ret = 0;

	if(argc > 1)	{
		return unittest_utils(argc, argv);
	}
	unittest_func * unittest_list = unittest_head;
	while(unittest_list != NULL)	{
		if(unittest_pre.func != NULL)	unittest_pre.func();
		unittest_list->return_value = unittest_list->func();
		if(unittest_post.func != NULL)	unittest_post.func();
		unittest_list = unittest_list->next;
	}
	
	unittest_list = unittest_head;
	while(unittest_list != NULL)	{
		unittest_func * old_func = unittest_list;
		unittest_ret += unittest_list->return_value;
		unittest_list = unittest_list->next;
		free(old_func);
	}
	return unittest_ret;	
}

#endif /* UNITTEST_CPP */

#endif /* UNITTEST */

