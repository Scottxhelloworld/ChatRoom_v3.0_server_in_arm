//#include "include.h"

#include "../../include/include.h"
void free_user(USER * head1)
{

	USER * pre = head1;
	if(pre != NULL)
	{
		USER * tmp = pre->next;

		do
		{	
			free(pre);
			pre = NULL;
			pre = tmp;
			if(tmp != NULL)
			{
				tmp = tmp->next;
			}

		}while(tmp != NULL);
	}
}
