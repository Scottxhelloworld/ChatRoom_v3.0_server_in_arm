#include "../../include/include.h"

void show_chat_record_group(int fd,char * name,char * groupname)
{

	int pos_cr[4][2] = {{1,67},{25,4},{25,50},{25,60}}; //四个位置
	int input_cr = 2;                                  //初始在上一页
	int leave_cr = 0;									//离开聊天记录界面
	int ch;

	strcpy(date,"  ");                                  //初始化date

	cr_order = 3;

	mainwin_cr_group = newwin(27,70,1,10);
	box(mainwin_cr_group,0,0);

	mvwhline(mainwin_cr_group,2,1,'-',68);
	mvwprintw(mainwin_cr_group,1,2,"%s 聊天记录",groupname);

	wattron(mainwin_cr_group,A_UNDERLINE|A_BOLD);
	mvwprintw(mainwin_cr_group,1,68,"X");
	//	wattroff(mainwin_cr,A_UNDERLINE|A_BOLD);

	mvwprintw(mainwin_cr_group,25,5,"文件");
	mvwprintw(mainwin_cr_group,25,51,"上一页");
	mvwprintw(mainwin_cr_group,25,61,"下一页");
	wattroff(mainwin_cr_group,A_UNDERLINE|A_BOLD);
	mvwprintw(mainwin_cr_group,pos_cr[input_cr][0],pos_cr[input_cr][1],"*");

	msg.action = GET_FIRST_CHAT_RECORD_GROUP;
	strcpy(msg.name,name);
	strcpy(msg.toname[0],groupname);
	write(fd,&msg,sizeof(msg));

	//可用到24行 从第3行开始
	touchwin(mainwin_cr_group);
	wrefresh(mainwin_cr_group);

	while(1)
	{
		ch = getch();
		mvwprintw(mainwin_cr_group,pos_cr[input_cr][0],pos_cr[input_cr][1]," ");
		switch(ch)
		{
			case KEY_LEFT:
			case KEY_UP:
				{
					input_cr--;
					break;
				}
			case KEY_RIGHT:
			case KEY_DOWN:
				{
					input_cr++;
					break;
				}
			case '\n':
				{
					switch(input_cr)
					{
						case 0:
							{
								leave_cr = 1;
								break;
							}

	
						case 2:
							{
								int i;
								for(i = 3; i < 25; i++)
								{
									mvwprintw(mainwin_cr_group,i,2,"%67s"," ");
								}
								wrefresh(mainwin_cr_group);

								cr_order = 3;                       //获取上一页 位置从第三行开始
								msg.action = GET_PRE_CHAT_RECORD_GROUP;
								strcpy(msg.name,name);
								strcpy(msg.toname[0],groupname);
								write(fd,&msg,sizeof(msg));
								break;
							}
						case 3:
							{
								int i;
								for(i = 3; i < 25; i++)
								{
									mvwprintw(mainwin_cr_group,i,2,"%67s"," ");
								}
								wrefresh(mainwin_cr_group);

								cr_order = 3;
								msg.action = GET_NEXT_CHAT_RECORD_GROUP;
								strcpy(msg.name,name);
								strcpy(msg.toname[0],groupname);
								write(fd,&msg,sizeof(msg));
								break;

							}
						default:
							{
								break;
							}
					}
				}
			default:
				{
					break;
				}
		}

		if(input_cr < 0)
		{
			input_cr = 0;
		}
		else if(input_cr > 3)
		{
			input_cr = 3;
		}

		mvwprintw(mainwin_cr_group,pos_cr[input_cr][0],pos_cr[input_cr][1],"*");
		wrefresh(mainwin_cr_group);

		if(leave_cr == 1)
		{
			break;
		}

	}
	touchwin(mainwin_chat_group);
	wrefresh(mainwin_chat_group);

}
