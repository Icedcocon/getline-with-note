#include "007getline.h"
#include <stdlib.h>
#include <errno.h>
#include <limits.h>

ssize_t getdelim(char** lineptr, size_t* n, int delim, FILE* stream){
	//cur_pos为指向字符串lineptr中当前位置的指针
	//new_lineptr为realloc后新内存块的首地址
	char* cur_pos, * new_lineptr;
	//new_lineptr_len用于记录新的内存快大小
	size_t new_lineptr_len;
	//c为getc返回的字符转化为int类型的值
	int c;

	//如果缓冲区地址为NULL、内存大小n的地址为NULL或文件流的指针为NULL，则返回-1
	if(lineptr == NULL || n == NULL || stream == NULL){
		errno = EINVAL;
		return -1;
	}

	//申请n个字节的内存，将内存快首地址给*lineptr
	if(*lineptr == NULL){
		*n = 128;//缓冲区初始长度，单位字节
		if((*lineptr = (char*)malloc(*n)) == NULL){
			errno = ENOMEM;
			return -1;
		}
	}

	//将当前位置初始化为缓冲区首字节
	cur_pos = *lineptr;
	for(;;){
		c = getc(stream);
		
		//如果从流中读取失败，返回-1
		//如果流中没有任何信息，则返回-1
		if(ferror(stream) || (c == EOF && cur_pos == *lineptr)){
			return -1;
		}

		if(c == EOF)
			break;
		
		//如果cur_pos指向倒数第二个位置（该位置一定被占用，最后一位留给'\0'）
		if((*lineptr + *n - cur_pos) < 2){
			//如果内存块大小大于ssize_t最大值的一半则报错
			if(SSIZE_MAX / 2 < *n){
#ifdef EOVERFLOW
				errno = EOVERFLOW;
#else
				errno = ERANGE;
#endif
				return -1;
			}
			//指定新的内存块大小
			new_lineptr_len = *n * 2;

			if((new_lineptr = (char*)realloc(*lineptr, new_lineptr_len)) == NULL){
				errno = ENOMEM;
				return -1;
			}
			//cur_pos指向新的内存块中的对应位置
			cur_pos = new_lineptr + (cur_pos - *lineptr);
			//*lineptr指向新的内存块首地址
			*lineptr = new_lineptr;
			//*n为新的内存块大小
			*n = new_lineptr_len;
		}
		
		//将读取到的字符放入缓冲区
		*cur_pos++ = (char)c;

		//遇到delimeter则停止
		if(c==delim)
			break;
	}

	*cur_pos = '\0';
	return (ssize_t)(cur_pos - *lineptr);
}

ssize_t getline(char** lineptr, size_t* n, FILE* stream){
	return getdelim(lineptr, n, '\n', stream);
}
