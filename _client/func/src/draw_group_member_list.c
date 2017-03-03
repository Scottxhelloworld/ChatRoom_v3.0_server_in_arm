//#include "include.h"
#include "../../include/include.h"

void draw_group_member_list()
{
	int lines = 1;   
	int i;
	USER * tmp = head_group_members;
	if(tmp != NULL)
	{
		tmp = tmp->next;
	}
	while(tmp != NULL)
	{
		mvwprintw(mainwin_chat_group,4 + lines,48,"%20s"," ");
		mvwprintw(mainwin_chat_group,4 + lines,48,"%s",tmp->name);
		if(lines == 1)
		{
			wprintw(mainwin_chat_group,"(群主)");
		}
		mvwprintw(mainwin_chat_group,4 + lines,63,"%s",(tmp->online == 0?"离线":"在线"));
		tmp = tmp->next;
		lines++;
	}

	for(;lines < 19; lines++)
	{
		mvwprintw(mainwin_chat_group,4 + lines,48,"%20s"," ");
	}
	wrefresh(mainwin_chat_group);

}
