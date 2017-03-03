//#include "include.h"
#include "../../include/include.h"

//删除第i个节点 从1开始
void delete_user(USER * head1,int i)
{
	int j = 0;
	USER * tmp = head1;
	USER * pre = tmp;   //tmp保存前一个值
	tmp = tmp->next;
	while(tmp != NULL)
	{
		j++;
		if(i == j)
		{
			pre->next = tmp->next;
			free(tmp);
			tmp = NULL;
			break;
		}

		pre = pre->next;
		tmp = tmp->next;
	}
}
