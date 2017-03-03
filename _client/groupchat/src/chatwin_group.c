//#include "include.h"
#include "../../include/include.h"


void chatwin_group(int fd,char * groupname)
{
	strcpy(current_groupname,groupname);
	int input_chat = 1;
	int max_select = 5;

	int pos_chat[][2] = {{1,67},{19,2},{19,17},{19,34},{25,24},{25,34}};   //最后一个为群管理 只有管理员可见
	mainwin_chat_group = newwin(27,70,1,10);
	wborder(mainwin_chat_group,'|','|','-','-','*','*','*','*');

	mvwprintw(mainwin_chat_group,1,2,"%s",groupname);

	touchwin(mainwin_chat_group);
	//填入XX创建于XXXX
	/*	if(strlen(sig) == 0)
		{
		mvwprintw(mainwin_chat_group,3,8,"这个人很懒,什么都没留下"); //此处写个性签名
		}*/
	msg.action = GET_GROUP_CREATE_TIME;
	strcpy(msg.name,self_name);
	strcpy(msg.toname[0],groupname);
	write(fd,&msg,sizeof(msg));


	usleep(100);
	//初始化头节点
	head_group_members = (USER *)malloc(sizeof(USER));
	head_group_members->next = NULL;

	msg.action = GET_GROUP_MEMBERS;
	strcpy(msg.toname[0],groupname);
	write(fd,&msg,sizeof(msg));

	wattron(mainwin_chat_group,A_BOLD|A_UNDERLINE);
	mvwprintw(mainwin_chat_group,1,68,"x");
	wattroff(mainwin_chat_group,A_BOLD|A_UNDERLINE);
	mvwhline(mainwin_chat_group,4,1,'-',69);

	upwin_chat = derwin(mainwin_chat_group,5,70,0,0);
	wborder(upwin_chat,'|','|','-','-','*','*','*','*');
	wrefresh(upwin_chat);

	show_chat_win_group = derwin(mainwin_chat_group,15,45,4,0);
	wborder(show_chat_win_group,'|','|','-','-','*','*','*','*');
	scrollok(show_chat_win_group,true);
	wrefresh(show_chat_win_group);


	WINDOW * middle_win_chat_group = derwin(mainwin_chat_group,3,45,18,0);
	wborder(middle_win_chat_group,'|','|','-','-','*','*','*','*');
	mvwprintw(middle_win_chat_group,1,3,"聊天");
	mvwprintw(middle_win_chat_group,1,35,"消息记录");
	wrefresh(middle_win_chat_group);

	WINDOW * sendwin_chat_group = derwin(mainwin_chat_group,7,45,20,0);
	wborder(sendwin_chat_group,'|','|','-','-','*','*','*','*');
	mvwprintw(sendwin_chat_group,5,25,"|发送|");
	mvwprintw(sendwin_chat_group,5,35,"|关闭|");
	wrefresh(sendwin_chat_group);

	//等链表写好
	usleep(200000);
#if 0
	int lines = 1;   
	int i;
	USER * tmp = head_group_members;
	tmp = tmp->next;
	//	start_color();
	//	init_pair(1,COLOR_RED,COLOR_BLACK);
	while(tmp != NULL)
	{
		mvwprintw(mainwin_chat_group,4 + lines,48,"%s",tmp->name);
		if(lines == 1)
		{
			wprintw(mainwin_chat_group,"(群主)");
		}
		mvwprintw(mainwin_chat_group,4 + lines,63,"%s",(tmp->online == 0?"离线":"在线"));
		tmp = tmp->next;
		lines++;
	}
#endif
//	draw_group_member_list();

	//如果是群主添加群管理
	if(strcmp(head_group_members->next->name,self_name) == 0)
	{
		mvwprintw(middle_win_chat_group,1,18,"群管理");
		wrefresh(middle_win_chat_group);
	}
	//普通人增加退群
	else
	{
		mvwprintw(middle_win_chat_group,1,18,"退群");
		wrefresh(middle_win_chat_group);
	}
	mvwprintw(mainwin_chat_group,pos_chat[input_chat][0],pos_chat[input_chat][1],"*");
	wrefresh(mainwin_chat_group);	

	msg.action = GET_RECENT_GROUP_CHAT_RECORD;
	strcpy(msg.name,self_name);
	strcpy(msg.toname[0],groupname);
	write(fd,&msg,sizeof(msg));

	int ch;
	int leave = 0;

	while(1)
	{
		ch = getch();
		mvwprintw(mainwin_chat_group,pos_chat[input_chat][0],pos_chat[input_chat][1]," ");
		switch(ch)
		{
			case KEY_LEFT:{}
			case KEY_UP:
						  {
							  input_chat--;
							  break;
						  }
			case KEY_RIGHT:{}
			case KEY_DOWN:
						   {
							   input_chat++;
							   break;
						   }
			case '\n':
						   {
							   switch(input_chat)
							   {

								   case 0:
									   {
										   //	   flag_open_chatwin[1] = 0;
										   free_user(head_group_members);  
										   head_group_members = NULL;
										   leave = 1;
										   break;
									   }
								   case 1:
									   {
										   curs_set(1);		
										   wmove(sendwin_chat_group,1,1);
										   wprintw(sendwin_chat_group,"%43s"," ");
										   wmove(sendwin_chat_group,1,1);
										   wrefresh(sendwin_chat_group);
										   echo();
										   wgetstr(sendwin_chat_group,msg.msg);

										   if(chatwin_order_group >= 14)
										   {
											   int i;
											   for(i = 1; i < 14; i++)
											   {
												   mvwprintw(show_chat_win_group,i,1,"%43s"," ");
											   }
											   wrefresh(show_chat_win_group);
											   chatwin_order_group = 2;
										   }
										   if(chatwin_order_group == 2)
										   {
											   mvwprintw(show_chat_win_group,1,10,"%s",get_current_time());
										   }

										   if(strlen(msg.msg) >= 35)
										   {
											   mvwprintw(show_chat_win_group,chatwin_order_group,1,"%s   :me",msg.msg);
										   }
										   else
										   {
											   mvwprintw(show_chat_win_group,chatwin_order_group,35 - strlen(msg.msg),"%s   :me",msg.msg);
										   }
										   wrefresh(show_chat_win_group);
										   chatwin_order_group++;

										   strcpy(msg.name,self_name);
										   msg.action = CHAT_GROUP;
										   strcpy(msg.toname[0],groupname);
										   write(fd,&msg,sizeof(msg));

										   mvwprintw(sendwin_chat_group,1,1,"%43s"," ");	
										   wrefresh(sendwin_chat_group);						 //每输入一次消息后清空发送窗口
										   noecho();
										   curs_set(0);
										   break;
									   }						     	
								   case 2:
									   {
										   //群主
										   if(strcmp(head_group_members->next->name,self_name) == 0)
										   {
											   group_manager(fd,groupname);
										   }

										   //普通成员 可以退群
										   else
										   {
											    echo();
												mvwprintw(mainwin_chat_group,21,17,"是否确认退出?(Y/N):");
												wrefresh(mainwin_chat_group);
												int ch_get = wgetch(mainwin_chat_group);
												noecho();
												getch();
												if(ch_get == 'y' || ch_get == 'Y')
												{
													msg.action = LEAVE_GROUP;
													strcpy(msg.toname[0],groupname);
													strcpy(msg.name,self_name);
													write(fd,&msg,sizeof(msg));
												}

												mvwprintw(mainwin_chat_group,21,17,"%22s"," ");
												wrefresh(mainwin_chat_group);
										   }
										   break;
									   }
								   case 3:
									   {
										   show_chat_record_group(fd,self_name,groupname);
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
		if(input_chat < 0)
		{
			input_chat = 0;
		}
		else if(input_chat > max_select)
		{
			input_chat = max_select;
		}

		mvwprintw(mainwin_chat_group,pos_chat[input_chat][0],pos_chat[input_chat][1],"*");
		wrefresh(mainwin_chat_group);
		if(leave == 1)
		{
			break;
		}
	}
	wclear(mainwin_chat_group);
	wrefresh(mainwin_chat_group);
	strcpy(msg.name,self_name);
	bzero(current_groupname,sizeof(current_groupname));
}
