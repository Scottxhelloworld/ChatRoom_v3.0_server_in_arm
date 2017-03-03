#include "../../include/include.h"
//#include "include.h"


void chatwin(int fd,int online,char * toname,char * sig)
{
	//char self_name[20];
	//strcpy(self_name,msg.name);
	int input_chat = 1;

	char * errmsg;
	char ** result;
	char sql[128];

	FILE * fp;
	input_filename_win = newwin(4,30,19,20);

	int pos_chat[][2] = {{1,67},{19,2},{19,9},{19,23},{19,33},{19,44},{25,34},{25,44}};
	mainwin_chat = newwin(27,70,1,10);
	//box(mainwin_chat,0,0);
	wborder(mainwin_chat,'|','|','-','-','*','*','*','*');

	mvwprintw(mainwin_chat,1,2,"%s",toname);
	if(online == 0)
	{
		mvwprintw(mainwin_chat,1,15,"此人不在线,您可以发送离线消息");
	}
	if(strlen(sig) == 0)
	{
		mvwprintw(mainwin_chat,3,8,"这个人很懒,什么都没留下"); //此处写个性签名
	}
	else
	{
		mvwprintw(mainwin_chat,3,8,"%s",sig);
	}
	wattron(mainwin_chat,A_BOLD|A_UNDERLINE);
	mvwprintw(mainwin_chat,1,68,"x");
	wattroff(mainwin_chat,A_BOLD|A_UNDERLINE);
	mvwhline(mainwin_chat,4,1,'-',69);

	upwin_chat = derwin(mainwin_chat,5,70,0,0);
	wborder(upwin_chat,'|','|','-','-','*','*','*','*');
	wrefresh(upwin_chat);

	show_chat_win = derwin(mainwin_chat,15,55,4,0);
	wborder(show_chat_win,'|','|','-','-','*','*','*','*');
	scrollok(show_chat_win,true);


#if 0
	//下面函数将未打开界面时受到的信息打印到界面
	sprintf(sql,"select undo_message from undo_msg where self_name = '%s' and undo_name = '%s';",self_name,toname);
	ret = sqlite3_get_table(db,sql,&result,&nrows,&ncols,&errmsg);
	if(ret != SQLITE_OK)
	{
		printf("insert error");
	}
	else
	{
		int i;
		for(i = 1; i < (nrows + 1); i++)
		{
			mvwprintw(show_chat_win,chatwin_order,1,"%s:%s",toname,result[i]);
			chatwin_order++;
		}
		sprintf(sql,"delete from undo_msg where self_name = '%s' and undo_name = '%s';",self_name,toname);
		sqlite3_exec(db,sql,NULL,NULL,&errmsg);
	}
	wrefresh(show_chat_win);
#endif
	msg.action = MSG_READ;
	strcpy(msg.toname[0],toname);
	strcpy(msg.name,self_name);
	write(fd,&msg,sizeof(msg));

	middle_win_chat = derwin(mainwin_chat,3,55,18,0);
	wborder(middle_win_chat,'|','|','-','-','*','*','*','*');
	mvwprintw(middle_win_chat,1,3,"聊天");
	mvwprintw(middle_win_chat,1,10,"加入黑名单");
	mvwprintw(middle_win_chat,1,24,"文件");
	mvwprintw(middle_win_chat,1,34,"截图");
	mvwprintw(middle_win_chat,1,45,"聊天记录");
	wrefresh(middle_win_chat);

	sendwin_chat = derwin(mainwin_chat,7,55,20,0);
	wborder(sendwin_chat,'|','|','-','-','*','*','*','*');
	mvwprintw(sendwin_chat,5,35,"|发送|");
	mvwprintw(sendwin_chat,5,45,"|关闭|");
	wrefresh(sendwin_chat);

	WINDOW * notice_chat = newwin(4,30,17,12);
	box(notice_chat,0,0);
	mvwprintw(notice_chat,1,5,"确认加入黑名单?");
	wattron(notice_chat,A_REVERSE);
	mvwprintw(notice_chat,2,5,"确认");
	wattroff(notice_chat,A_REVERSE);
	mvwprintw(notice_chat,2,15,"取消");

	mvwprintw(mainwin_chat,pos_chat[input_chat][0],pos_chat[input_chat][1],"*");
	wrefresh(mainwin_chat);	

#if 0   //抖一下
	int i;
	for(i = 0; i < 3;i++)
	{	
		mvwin(mainwin_chat,1,10 - i);
		wrefresh(mainwin_chat);
		sleep(1);
	}
#endif
	int ch;
	int leave = 0;

	while(1)
	{
		ch = getch();
		mvwprintw(mainwin_chat,pos_chat[input_chat][0],pos_chat[input_chat][1]," ");
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
										   leave = 1;
										   break;
									   }
								   case 1:
									   {
										   strcpy(msg.toname[0],toname);
										   strcpy(msg.name,self_name);
										   curs_set(1);		
										   wmove(sendwin_chat,1,1);
										   wprintw(sendwin_chat,"%53s"," ");
										   wmove(sendwin_chat,1,1);
										   wrefresh(sendwin_chat);
										   echo();
										   wgetstr(sendwin_chat,msg.msg);
#if 0
										   if(chatwin_order < 14)
										   {
											   if(chatwin_order == 2)
											   {
												   mvwprintw(show_chat_win,1,20,"%s",get_current_time());										
											   }
											   if(strlen(msg.msg) >= 45)
											   {
												   mvwprintw(show_chat_win,chatwin_order,1,"%s   :me",msg.msg);
											   }
											   else
											   {
												   mvwprintw(show_chat_win,chatwin_order,45 - strlen(msg.msg),"%s   :me",msg.msg);
											   }
											   wrefresh(show_chat_win);
											   chatwin_order++;

										   }	
#endif
										   if(chatwin_order >= 14)
										   {
											   int i;
											   for(i = 1; i < 14; i++)
											   {
												   mvwprintw(show_chat_win,i,1,"%53s"," ");
											   }
											   wrefresh(show_chat_win);
											   chatwin_order = 2;
										   }
										   if(chatwin_order == 2)
										   {
											   mvwprintw(show_chat_win,1,20,"%s",get_current_time());
										   }
										   if(strlen(msg.msg) >= 45)
										   {
											   mvwprintw(show_chat_win,chatwin_order,1,"%s   :me",msg.msg);
										   }
										   else
										   {
											   mvwprintw(show_chat_win,chatwin_order,45 - strlen(msg.msg),"%s   :me",msg.msg);
										   }
										   wrefresh(show_chat_win);
										   chatwin_order++;

										   msg.action = CHAT_PRIVATE;
										   write(fd,&msg,sizeof(msg));
										   mvwprintw(sendwin_chat,1,1,"%53s"," ");	
										   wrefresh(sendwin_chat);						 //每输入一次消息后清空发送窗口
										   noecho();
										   curs_set(0);
										   break;
									   }
								   case 2:                 //黑名单
									   {
										   touchwin(notice_chat);
										   wrefresh(notice_chat);

										   int ch_l;
										   int leave_l = 0;
										   int select_l = 0;  //0确定 1取消

										   while(1)
										   {
											   ch_l = getch();
											   switch(ch_l)
											   {
												   case KEY_RIGHT:
												   case KEY_DOWN:
												   case KEY_LEFT:
												   case KEY_UP:
													   {
														   select_l = !select_l;
														   break;
													   }
												   case '\n':
													   {
														   leave_l = 1;
														   if(select_l == 0)
														   {
															   msg.action = ADD_TO_BLACKLIST;
															   strcpy(msg.name,self_name);
															   strcpy(msg.toname[0],toname);
															   write(fd,&msg,sizeof(msg));
														   }
														   break;
													   }
											   }
											   if(select_l == 0)
											   {
												   wattron(notice_chat,A_REVERSE);
												   mvwprintw(notice_chat,2,5,"确认");
												   wattroff(notice_chat,A_REVERSE);
												   mvwprintw(notice_chat,2,15,"取消");

											   }
											   else
											   {
												   mvwprintw(notice_chat,2,5,"确认");
												   wattron(notice_chat,A_REVERSE);
												   mvwprintw(notice_chat,2,15,"取消");
												   wattroff(notice_chat,A_REVERSE);

											   }
											   wrefresh(notice_chat);
											   if(leave_l == 1)
											   {
												   break;
											   }

										   }
										   touchwin(mainwin_chat);
										   wrefresh(mainwin_chat); 
										   break;
									   }
								   case 3:            //文件
									   {

										   box(input_filename_win,0,0);
										   mvwprintw(input_filename_win,1,2,"请输入要发送的文件名");
										   mvwprintw(input_filename_win,2,2,"%18s"," ");
										   touchwin(input_filename_win);
										   wmove(input_filename_win,2,2);	
										   echo();
										   curs_set(1);
										   wrefresh(input_filename_win);
										   wgetnstr(input_filename_win,msg.msg,20);
										   curs_set(0);

										   //文件不存在 返回非0
										   if(access(msg.msg,0) != 0)
										   {
											   mvwprintw(input_filename_win,1,2,"%20s"," ");
											   mvwprintw(input_filename_win,2,2,"%18s"," ");
											   mvwprintw(input_filename_win,1,2,"文件不存在!");
											   mvwprintw(input_filename_win,2,2,"请重输");
											   wrefresh(input_filename_win);
											   getch();
											   touchwin(mainwin_chat);
										       wrefresh(mainwin_chat);
											   break;
										   }
										   else
										   {
											   mvwprintw(input_filename_win,1,2,"%20s"," ");
											   mvwprintw(input_filename_win,2,2,"%20s"," ");
											   mvwprintw(input_filename_win,1,2,"文件已发送!");
											   mvwprintw(input_filename_win,2,2,"等待对方接收");
											   wrefresh(input_filename_win);
											   getch();
										   }
										   touchwin(mainwin_chat);
										   wrefresh(mainwin_chat);
										   noecho();

										   // int file_fd = open("",O_RDONLY);

										   msg.action = BEGIN_TRANSFER_FILE;
										   strcpy(msg.name,self_name);
										   strcpy(msg.toname[0],toname);
										   write(fd,&msg,sizeof(msg));
										   break;
									   }
								   case 4:
									   {
										   fp = fopen("./screenshot","w");
										   putwin(mainwin_chat,fp);
										   fclose(fp);
										   mvwprintw(show_chat_win,chatwin_order,1,"保存截图成功");
										   chatwin_order++;
										   wrefresh(show_chat_win);
										   break;
									   }
								   case 5:            //聊天记录
									   {
										   show_chat_record(fd,self_name,toname);
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
		else if(input_chat > 7)
		{
			input_chat = 7;
		}

		mvwprintw(mainwin_chat,pos_chat[input_chat][0],pos_chat[input_chat][1],"*");
		wrefresh(mainwin_chat);
		if(leave == 1)
		{
			break;
		}
	}

	wclear(mainwin_chat);
	wrefresh(mainwin_chat);
	strcpy(msg.name,self_name);

	sleep(1);

}
