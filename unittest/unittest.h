
#ifdef __cplusplus
extern "C" {
#endif

#ifdef UNITTEST

#ifndef UNITTEST_H
#define UNITTEST_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

// define asserts for unittests.

#define ASSERT_STRING_EQUAL(a,b)	if(strcmp(a, b) != 0)	{\
					func_is_on_error++;\
					printf("ERROR\tin file '%s' function '%s' at line %d in thread id 0x%x in process %d: \n\t- '%s' != '%s'\n",\
					__FILE__, __func__, __LINE__, (unsigned int)pthread_self(), getpid(), (a), (b)); \
				}
#define ASSERT_STRING_NOT_EQUAL(a,b)	if(strcmp(a, b) == 0)	{\
					func_is_on_error++;\
					printf("ERROR\tin file '%s' function '%s' at line %d in thread id 0x%x in process %d: \n\t- '%s' == '%s'\n",\
					__FILE__, __func__, __LINE__, (unsigned int)pthread_self(), getpid(), (a), (b)); \
				}
#define ASSERT_ESUP(a,b)	if((a) <= (b))	{\
					func_is_on_error++;\
					printf("ERROR\tin file '%s' function '%s' at line %d in thread id 0x%x in process %d: \n\t- %s !>= %s\n\t\t- %d !>= %d\n",\
					__FILE__, __func__, __LINE__, (unsigned int)pthread_self(), getpid(), #a, #b, (int)(a), (int)(b)); \
				}
#define ASSERT_EINF(a,b)	if((a) >= (b))	{\
					func_is_on_error++;\
					printf("ERROR\tin file '%s' function '%s' at line %d in thread id 0x%x in process %d: \n\t- %s !<= %s\n\t\t- %d !<= %d\n",\
					__FILE__, __func__, __LINE__, (unsigned int)pthread_self(), getpid(), #a, #b, (int)(a), (int)(b)); \
				}
#define ASSERT_SUP(a,b)		if((a) < (b))	{\
					func_is_on_error++;\
					printf("ERROR\tin file '%s' function '%s' at line %d in thread id 0x%x in process %d: \n\t- %s !> %s\n\t\t- %d !> %d\n",\
					__FILE__, __func__, __LINE__, (unsigned int)pthread_self(), getpid(), #a, #b, (int)(a), (int)(b)); \
				}
#define ASSERT_INF(a,b)		if((a) > (b))	{\
					func_is_on_error++;\
					printf("ERROR\tin file '%s' function '%s' at line %d in thread id 0x%x in process %d: \n\t- %s !< %s\n\t\t- %d !< %d\n",\
					__FILE__, __func__, __LINE__, (unsigned int)pthread_self(), getpid(), #a, #b, (int)(a), (int)(b)); \
				}
#define ASSERT_EQUAL(a,b)	if((a) != (b))	{\
					func_is_on_error++;\
					printf("ERROR\tin file '%s' function '%s' at line %d in thread id 0x%x in process %d: \n\t- %s != %s\n\t- %d != %d\n\t- 0x%x != 0x%x\n",\
					__FILE__, __func__, __LINE__, (unsigned int)pthread_self(), getpid(), #a, #b, (int)(a), (int)(b), (int)(a), (int)(b));\
				}
#define ASSERT_NOT_EQUAL(a,b)	if((a) == (b))	{\
					func_is_on_error++;\
					printf("ERROR\tin file '%s' function '%s' at line %d  in thread id 0x%x in process %d: \n\t- %s == %s\n\t- %d == %d\n\t- 0x%x != 0x%x\n",\
					__FILE__, __func__, __LINE__, (unsigned int)pthread_self(), getpid(), #a, #b, (int)(a), (int)b, (int)(a), (int)b);\
				}
#define ASSERT_TRUE(a)		if(!(a))	{\
					func_is_on_error++;\
					printf("ERROR\tin file '%s' function '%s' at line %d  in thread id 0x%x in process %d: \n\t- %s is FALSE \n",\
					__FILE__, __func__, __LINE__, (unsigned int)pthread_self(), getpid(), #a);\
				}
#define ASSERT_FALSE(a)		if((a))	{\
					func_is_on_error++;\
					printf("ERROR\tin file '%s' function '%s' at line %d  in thread id 0x%x in process %d: \n\t- %s is TRUE \n",\
					__FILE__, __func__, __LINE__, (unsigned int)pthread_self(), getpid(), #a);\
				}
#define ASSERT_NULL(a)		if((a) != NULL)	{\
					func_is_on_error++;\
					printf("ERROR\tin file '%s' function '%s' at line %d  in thread id 0x%x in process %d: \n\t- %s is not NULL: %p\n",\
					__FILE__, __func__, __LINE__, (unsigned int)pthread_self(), getpid(), #a, a);\
				}
#define ASSERT_NOT_NULL(a)	if((a) == NULL)	{\
					func_is_on_error++;\
					printf("ERROR\tin file '%s' function '%s' at line %d  in thread id 0x%x in process %d: \n\t- %s is NULL\n",\
					__FILE__, __func__, __LINE__, (unsigned int)pthread_self(), getpid(), #a);\
				}

#define EMPTY			printf("ERROR\tin file '%s' function '%s' at line %d  in thread id 0x%x in process %d: TO BE TESTED\n",\
                                                __FILE__, __func__, __LINE__, (unsigned int)pthread_self(), getpid());
#define RETURN			if(strcmp(__func__, "main")!=0)	{\
					if(func_is_on_error == 0) 	{\
						printf("OK:  \tfile '%s' function '%s' in thread id 0x%x in process %d: OK\n", __FILE__, __func__,\
							(unsigned int)pthread_self(), getpid());\
						printf("--------------------------------------------------------------------------------------\n");\
						return 0;\
					} else 	{\
						printf("FAILED:  file '%s' function '%s' in thread id 0x%x in process %d: %ld error(s)\n",\
							__FILE__, __func__, (unsigned int)pthread_self(), getpid(), func_is_on_error); \
						printf("--------------------------------------------------------------------------------------\n");\
						return 1;\
					}\
				} else	{\
					return func_is_on_error;\
				}
// define macro for unittest execution
#define INIT		long func_is_on_error = 0;
#define PRE_FUNC(a)	setpre(a);
#define POST_FUNC(a)	setpost(a);
#define TEST_FUNC(a)	setfunc(a, #a);
#define EXEC_UNITTEST	func_is_on_error = exectest(argc, argv);

typedef int(*PROTO_FUNC)(void);

typedef struct unittest_func {
	struct unittest_func * next;
	char name[256];
	PROTO_FUNC func;
	int return_value;
} unittest_func;

void setpre(PROTO_FUNC func);
void setpost(PROTO_FUNC func);
void setfunc(PROTO_FUNC func, char * name);
int exectest(int argc, char ** argv);

#endif //UNITTEST_H

#endif //UNITTEST

#ifdef __cplusplus
}
#endif

