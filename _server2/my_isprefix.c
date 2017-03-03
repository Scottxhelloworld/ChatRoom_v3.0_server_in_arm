#include <string.h>

//判断前一个字符串是否是后一个字符串的前缀
int my_isprefix(char * src1,char * src2)
{
	int len = strlen(src1);
	int len2 = strlen(src2);
	int re = 0;

	//若前一个长度长  则返回0
	if(len > len2)
	{
		return 0;
	}

	while(*src1 != '\0')
	{
		if(*src1 != *src2)
		{
			break;
		}
		src1++;
		src2++;
		re++;
	}

	if(re == len)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}
