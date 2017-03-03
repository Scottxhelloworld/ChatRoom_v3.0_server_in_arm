//#include "include.h"
#include "../../include/include.h"

//尾插
void insert_user(USER * head1,USER * newuser)
{
	USER * tmp = head1;
	while(tmp->next != NULL)
	{
		tmp = tmp->next;
	}

	tmp->next = newuser;
	newuser->next = NULL;
}
