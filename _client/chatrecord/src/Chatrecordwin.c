#include "../../include/include.h"

void show_chat_record(int fd,char * name,char * toname)
{

	int pos_cr[4][2] = {{1,67},{25,4},{25,50},{25,60}}; //四个位置
	int input_cr = 2;                                  //初始在上一页
	int leave_cr = 0;									//离开聊天记录界面
	int ch;

	strcpy(date,"  ");                                  //初始化date

	cr_order = 3;

	mainwin_cr = newwin(27,70,1,10);
	box(mainwin_cr,0,0);

	mvwhline(mainwin_cr,2,1,'-',68);
	mvwprintw(mainwin_cr,1,2,"%s 与 %s的聊天记录",name,toname);

	wattron(mainwin_cr,A_UNDERLINE|A_BOLD);
	mvwprintw(mainwin_cr,1,68,"X");
	//	wattroff(mainwin_cr,A_UNDERLINE|A_BOLD);

	mvwprintw(mainwin_cr,25,5,"文件");
	mvwprintw(mainwin_cr,25,51,"上一页");
	mvwprintw(mainwin_cr,25,61,"下一页");
	wattroff(mainwin_cr,A_UNDERLINE|A_BOLD);
	mvwprintw(mainwin_cr,pos_cr[input_cr][0],pos_cr[input_cr][1],"*");

	msg.action = GET_FIRST_CHAT_RECORD;
	strcpy(msg.name,name);
	strcpy(msg.toname[0],toname);
	write(fd,&msg,sizeof(msg));

	//可用到24行 从第3行开始
	touchwin(mainwin_cr);
	wrefresh(mainwin_cr);

	while(1)
	{
		ch = getch();
		mvwprintw(mainwin_cr,pos_cr[input_cr][0],pos_cr[input_cr][1]," ");
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
									mvwprintw(mainwin_cr,i,2,"%67s"," ");
								}
								wrefresh(mainwin_cr);

								cr_order = 3;                       //获取上一页 位置从第三行开始
								msg.action = GET_PRE_CHAT_RECORD;
								strcpy(msg.name,name);
								strcpy(msg.toname[0],toname);
								write(fd,&msg,sizeof(msg));
								break;
							}
						case 3:
							{
								int i;
								for(i = 3; i < 25; i++)
								{
									mvwprintw(mainwin_cr,i,2,"%67s"," ");
								}
								wrefresh(mainwin_cr);

								cr_order = 3;
								msg.action = GET_NEXT_CHAT_RECORD;
								strcpy(msg.name,name);
								strcpy(msg.toname[0],toname);
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

		mvwprintw(mainwin_cr,pos_cr[input_cr][0],pos_cr[input_cr][1],"*");
		wrefresh(mainwin_cr);

		if(leave_cr == 1)
		{
			break;
		}

	}
	touchwin(mainwin_chat);
	wrefresh(mainwin_chat);

}
