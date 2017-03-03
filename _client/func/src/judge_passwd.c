//#include <string.h>
//#include "../../include/include.h"
#include "../../include/Message.h"

int judge_passwd(char * passwd)
{
	int re = 0;
	int re_lowercase = 0;
	int re_uppercase = 0;
	int re_num = 0;
	int re_oth = 0;

	while(*passwd != '\0')
	{
		if(re_lowercase == 0 && *passwd >= 'a' && *passwd <= 'z')
		{
			re++;
			re_lowercase = 1;  //判断已经有小写字母
		}

		else if(re_uppercase == 0 && *passwd >= 'A' && *passwd <= 'Z')
		{
			re++;
			re_uppercase = 1;  //判定已有大写字母
		}

		else if(re_num == 0 && *passwd >= '0' && *passwd <= '9')
		{
			re++;
			re_num = 1;   
		}
		else if(re_oth == 0 && (*passwd < '0' || *passwd > '9') &&
			   	(*passwd < 'a' || *passwd > 'z') && (*passwd < 'A' || *passwd > 'Z'))
		{
			re++;
			re_oth = 1;
		}

		passwd++;
	}

	switch(re)
	{
		case 1:
			{
				return PASSWD_EASY;
				break;
			}
		case 2:
			{
				return PASSWD_NORMAL;
				break;
			}
		case 3:
			{
				return PASSWD_NORMAL;
				break;
			}
		case 4:			
			{
				return PASSWD_HARD;
				break;
			}
		default:
			{
				return -1;
				break;
			}
	}
}

#if 0
int main()
{
	char s[20] = "bywef";
	int ret = judge_passwd(s);
	printf("%d\n",ret);
}
#endif
