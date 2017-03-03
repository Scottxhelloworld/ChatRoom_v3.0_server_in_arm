//#include "include.h"
#include "../../include/include.h"

void creategroupchatwin(int fd)
{
	char group_name[20];

	bzero(group_name,sizeof(group_name));
	head = (USER *)malloc(sizeof(USER));
	head->next = NULL;

	USER * head_insert;  //创建群插入的链表
	head_insert = (USER *)malloc(sizeof(USER));
	head_insert->next = NULL;

	USER * newuser = (USER *)malloc(sizeof(USER));
	strcpy(newuser->name,self_name);
	insert_user(head_insert,newuser);

	mainwin_create = newwin(27,60,1,20);  //25lines  60cols
	box(mainwin_create,0,0);
	mvwhline(mainwin_create,2,1,'-',58);
	mvwprintw(mainwin_create,1,2,"创建群");

	WINDOW * add_group = derwin(mainwin_create,3,25,3,3);
	mvwprintw(add_group,1,5,"请输入群名字");
	box(add_group,0,0);
	wrefresh(add_group);

	mvwprintw(mainwin_create,7,4,"最近联系人");
	mvwprintw(mainwin_create,13,4,"我的好友");
	mvwprintw(mainwin_create,4,40,"已选联系人");
	mvwprintw(mainwin_create,4,51,"1");
	mvwprintw(mainwin_create,5,42,"%s",self_name);
	mvwprintw(mainwin_create,24,40,"取消");
	mvwprintw(mainwin_create,24,50,"确定");
	mvwprintw(mainwin_create,4,2,"*");
	strcpy(msg.name,self_name);
	msg.action = GET_RECENT_CHAT;
	write(fd,&msg,sizeof(msg));

	usleep(1000);   //将最近联系人都获取到

	strcpy(msg.name,self_name);
	msg.action = GET_ALL_FRIENDS;
	write(fd,&msg,sizeof(msg));

	sleep(1);  //获取所有联系人到链表  有等待时间
	print_user(1);
	touchwin(mainwin_create);
	int pos[15][2] = {{4,2},{8,4},{9,4},{10,4},{11,4},{14,4},{15,4},{16,4},{17,4},{18,4},{19,4},{20,4},
						{21,4},{22,4},{23,4}}; 
	int select_create = 0;
	wrefresh(mainwin_create);
	int ch;
	int show_user    = 6;  
	int first_user   = 1; //第一个取的联系人位置  控制上下移动
	int goto_yesno   = 0;
	int leave_create = 0;
	while(1)
	{
		mvwprintw(mainwin_create,24,40,"取消");
		mvwprintw(mainwin_create,24,50,"确认");
		mvwprintw(mainwin_create,pos[select_create][0],pos[select_create][1]," ");
		ch = getch();
	
		switch(ch)
		{
			case KEY_UP:
			{
				select_create--;
				goto_yesno = 0;   //上下代表选择联系人
				if(select_create == 4 && first_user != 1)
				{
					select_create = 5;
					first_user--;
					print_user(first_user);
				}
				break;
			}
			case KEY_DOWN:
			{
				select_create++;  
				goto_yesno = 0;
				break;
			}
			case KEY_LEFT:
			{
				goto_yesno = 1;  //取消
				break;
			}
			case KEY_RIGHT:
			{
				goto_yesno = 2;  //确认
				break;
			}
			case '\n':
			{
				if(select_create == 0)
				{
					echo();
					curs_set(1);
					mvwprintw(add_group,1,2,"%20s"," ");
					wmove(add_group,1,2);
					wrefresh(add_group);
					wgetnstr(add_group,group_name,20);
					noecho();
					curs_set(0);
					strcpy(msg.toname[0],group_name);
					msg.action = CHECK_GROUP_NAME;
					write(fd,&msg,sizeof(msg));

				}
				else if(goto_yesno == 0)
				{
					int get;       //获取的人的名字在链表中的真正位置
					if(select_create <= 4)
					{
						get = select_create;
					}
					//获取选中的联系人
					else
					{
						get = select_create + first_user - 1;
					}
					mvwprintw(mainwin_create,show_user,42,"%s",get_user(head,get));
					mvwprintw(mainwin_create,4,51,"%d",show_user - 4);
					show_user++;
					USER * newuser = (USER *)malloc(sizeof(USER));
					strcpy(newuser->name,get_user(head,get));
					insert_user(head_insert,newuser);
				}
				if(goto_yesno == 1)
				{
					leave_create = 1;
				}
				if(goto_yesno == 2)
				{
					if(strlen(group_name) > 0)
					{
					USER * tmp = head_insert;
					//第一个人肯定是群主
					tmp = tmp->next;
					msg.action = CREATE_GROUP;
					strcpy(msg.name,tmp->name);
					strcpy(msg.toname[0],group_name);//群名放此
					write(fd,&msg,sizeof(msg)); 
					tmp = tmp->next;

					while(tmp != NULL)
					{
						msg.action = INSERT_GROUP;
						strcpy(msg.name,tmp->name);
						strcpy(msg.toname[0],group_name);
						write(fd,&msg,sizeof(msg));
						tmp = tmp->next;
						usleep(1000);
					}
					}

					wattron(mainwin_create,A_REVERSE);
					mvwprintw(mainwin_create,24,10,"建群成功!!!");
					wattroff(mainwin_create,A_REVERSE);
					wrefresh(mainwin_create);
					leave_create = 1;
					getch();
				}
				break;
			}
			default:
			{
				break;
			}
		}
		if(select_create < 0)
		{
			select_create = 0;
		}
		if(select_create > 14)
		{
			first_user++;
			if(first_user > get_user_count() - 10 + 1)
			{
				first_user = get_user_count() - 10 + 1;
				if(first_user < 1)
				{
					first_user = 1;
				}
			}
			print_user(first_user);
			select_create = 14;
		}
		if(goto_yesno == 1)
		{
			wattron(mainwin_create,A_REVERSE);
			mvwprintw(mainwin_create,24,40,"取消");
			wattroff(mainwin_create,A_REVERSE);
		}
		if(goto_yesno == 2)
		{
			wattron(mainwin_create,A_REVERSE);
			mvwprintw(mainwin_create,24,50,"确认");
			wattroff(mainwin_create,A_REVERSE);
		}
		if(leave_create == 1)
		{
			break;
		}
		mvwprintw(mainwin_create,pos[select_create][0],pos[select_create][1],"*");
		wrefresh(mainwin_create);

	}

	free_user(head);
	wclear(mainwin_create);
	delwin(mainwin_create);
	wrefresh(mainwin_create);

}
