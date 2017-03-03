//#include "include.h"
#include "../../include/include.h"

//获取所有链表中所有联系人个数  这个专门获取创建群时用
int get_user_count()
{
	USER * tmp = head;
	tmp = tmp->next;
	int i = 0;
	while(tmp != NULL)
	{
		i++;
		tmp = tmp->next;
	}

	return i - 4;
}


int get_list_count(USER * head1)
{
	USER * tmp = head1;
	tmp = tmp->next;
	int i = 0;
	while(tmp != NULL)
	{
		i++;
		tmp = tmp->next;
	}

	return i;
}
//根据下标找到对应的人姓名 i从1开始
char * get_user(USER * head1,int i)
{
	USER * tmp = head1;
	tmp = tmp->next;
	int j = 0;
	while(tmp != NULL)
	{
		j++;
		if(i == j)
		{
			return tmp->name;
		}
		tmp = tmp->next;
	}

	return "";
}

//返回链表节点
USER * get_user_node(USER * head1,int i)
{

	USER * tmp = head1;
	tmp = tmp->next;
	int j = 0;
	while(tmp != NULL)
	{
		j++;
		if(i == j)
		{
			return tmp;
		}
		tmp = tmp->next;
	}

	return NULL;
}

//i打印的是所有联系人的第i个 i从1开始 而链表是从最近联系人开始构造  
void print_user(int i)
{
	int j = 14;  //第一个联系人行书14
	for(i += 4; j < 24; i++,j++)
	{
		mvwprintw(mainwin_create,j,5,"%15s"," ");
		mvwprintw(mainwin_create,j,5,"%s",(get_user(head,i) == NULL)?"":get_user(head,i));
	}
}
