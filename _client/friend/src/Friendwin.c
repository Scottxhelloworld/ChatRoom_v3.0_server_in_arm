//#include "include.h"

#include "../../include/include.h"

//int main()
int Friendwin(int fd)
{

	WINDOW * select_online_win;   //选择在线  隐身
	/*全局变量的初始化工作*/
	flag_open_chatwin[0] = flag_open_chatwin[1] = flag_open_chatwin[2] = flag_open_chatwin[3] = flag_open_chatwin[4] = 0;
	bzero(isrunning,sizeof(isrunning));
	flag_search = 0;




	strcpy(self_name,msg.name);  //保存自己名字
	//用户权限
	flag_vip = msg.hasquestion;
	int pos_c[7][2] = {{1,112},{5,97},{7,84},{9,84},{11,84},{13,84},{28,87}};
	int pos_name[][2] = {{11,12},{11,19},{11,26},{12,2},{14,2},{16,2},{18,2}};
	int input_c = 0;
	int leave_c = 0;

	char sig_c[128];

	setlocale(LC_ALL,"");
	initscr();
	noecho();
	curs_set(0);
	keypad(stdscr,TRUE);

	notify_file_win = newwin(5,25,25,90);
	box(notify_file_win,0,0);

	mainwin_c = newwin(30,30,0,85);//30lines 122cols
	box(mainwin_c,0,0);
	refresh();

	mvwprintw(mainwin_c,1,1,"Scott");
	wattron(mainwin_c,A_BOLD|A_UNDERLINE);
	mvwprintw(mainwin_c,1,28,"x");
	wattroff(mainwin_c,A_BOLD|A_UNDERLINE);

	facewin_c = derwin(mainwin_c,5,10,2,1);
	box(facewin_c,0,0);
	//mvwprintw(facewin_c,2,1,"o(//\\_//\\)o");
	wrefresh(facewin_c);

	if(flag_vip == 1)
	{
		wattron(mainwin_c,A_BOLD|A_UNDERLINE);
		mvwprintw(mainwin_c,2,13,"vip");
		wattroff(mainwin_c,A_BOLD|A_UNDERLINE);
	}
	mvwprintw(mainwin_c,3,13,"Welcome:%s",msg.name);
	mvwprintw(mainwin_c,5,13,"状态:");
	mvwprintw(mainwin_c,5,18,"在线");

	sigwin_c = derwin(mainwin_c,3,30,6,0);
	if(strlen(msg.msg) > 0)
	{
		mvwprintw(sigwin_c,1,3,"%s",msg.msg);   //个性签名
	}
	else
	{
		mvwprintw(sigwin_c,1,3,"设置个性姓名");
	}
	wrefresh(sigwin_c);

	searchwin_c = derwin(mainwin_c,3,30,8,0);
	mvwhline(searchwin_c,0,1,'-',28);
	mvwhline(searchwin_c,2,1,'-',28);
	mvwprintw(searchwin_c,1,25,"搜索");
	wrefresh(searchwin_c);

	mvwprintw(mainwin_c,11,2,"> 我的好友");
	//读取服务器端数据库  看是否有未接收消息

	msg.action = SEEK_UNREAD;
	strcpy(msg.name,self_name);
	write(fd,&msg,sizeof(msg));
	mvwprintw(mainwin_c,13,2,"> 我的群组");

	mvwhline(mainwin_c,27,1,'-',28);
	mvwprintw(mainwin_c,28,3,"设置");
	wrefresh(mainwin_c);

	select_online_win = newwin(4,10,2,97);
	mvwprintw(select_online_win,1,4,"在线");
	mvwprintw(select_online_win,2,4,"隐身");
	box(select_online_win,0,0);

	mvaddch(pos_c[input_c][0],pos_c[input_c][1],'*');
	refresh();

	int pos_search[2][2] = {{1,1},{1,22}};
	result_search_win = newwin(4,25,9,85);
	wattron(result_search_win,A_BOLD|A_UNDERLINE);
	mvwprintw(result_search_win,1,23,"Q");
	wattroff(result_search_win,A_BOLD|A_UNDERLINE);
	//	mvwprintw(result_search_win,2,4,"demo");
	box(result_search_win,0,0);


	int ch;
	while(1)
	{
		/*	while(flag_file == 1)
			{

			}*/
		ch = getch();
		mvaddch(pos_c[input_c][0],pos_c[input_c][1],' ');
		switch(ch)
		{
			case KEY_F(3):
				{
					FILE * fp = fopen("./screenshot","r");
					WINDOW * tmp = getwin(fp);
					wrefresh(tmp);
					getch();
					wclear(tmp);
					wrefresh(tmp);
					delwin(tmp);
					break;
				}
			case KEY_UP:
				{
					input_c--;
					if(input_c < 0)
					{
						input_c = 0;
					}
					break;
				}
			case KEY_DOWN:
				{
					input_c++;
					if(input_c > 6)
					{
						input_c = 6;
					}
					break;
				}
			case '\n':
				{
					switch(input_c)
					{
						case 0:                                              /*退出*/
							{
								leave_c = 1;
								msg.action = LEAVE_ONLINE;
								strcpy(msg.name,self_name);
								write(fd,&msg,sizeof(msg));

								//退出界面 将一些在运行的进程结束运行
								int i;
								for(i = 0; i < 5; i++)
								{
									if(isrunning[i] == 1)
									{
										pthread_cancel(pid[i]);
									}
								}
								break;
							}
						case 1:
							{
								touchwin(select_online_win);
								mvwprintw(select_online_win,1,3,"*");
								mvwprintw(select_online_win,2,3," ");
								wrefresh(select_online_win);
								int select_online = 1;                        /*1:在线  0:隐身*/
								int leave_select_online = 0;                       /*离开在线选择*/
								int pos_selectonline[2][2] = {{2,3},{1,3}};

								while(1)
								{
									mvwprintw(select_online_win,pos_selectonline[select_online][0],pos_selectonline[select_online][1]," ");
									int l_i = getch();                            /*选择在线或者隐身*/
									switch(l_i)
									{
										case KEY_LEFT:
										case KEY_UP:
										case KEY_RIGHT:
										case KEY_DOWN:
											{
												select_online = !select_online;
												break;
											}

										case '\n':
											{
												leave_select_online = 1;
												break;
											}
									}
									mvwprintw(select_online_win,pos_selectonline[select_online][0],pos_selectonline[select_online][1],"*");
									wrefresh(select_online_win);
									if(leave_select_online == 1)
									{
										if(select_online == 0)   //选择隐身
										{
											msg.action = HIDING;
											strcpy(msg.name,self_name);
											write(fd,&msg,sizeof(msg));
											mvwprintw(mainwin_c,5,18,"隐身");
											wrefresh(mainwin_c);
										}
										else
										{
											msg.action = BACK_ONLINE;
											strcpy(msg.name,self_name);
											write(fd,&msg,sizeof(msg));
											mvwprintw(mainwin_c,5,18,"在线");
											wrefresh(mainwin_c);

										}
										break;
									}

								}

								touchwin(mainwin_c);
								wrefresh(mainwin_c);
								break;
							}
						case 2:                                               /*个性签名*/
							{
								bzero(sig_c,sizeof(sig_c));
								mvwprintw(sigwin_c,1,3,"%26s"," ");

								wrefresh(sigwin_c);
								echo();
								curs_set(1);
								wmove(sigwin_c,1,3);
								wrefresh(sigwin_c);
								wgetnstr(sigwin_c,sig_c,28);
								curs_set(0);
								noecho();
								bzero(msg.msg,sizeof(msg.msg));
								if(strlen(sig_c) == 0)
								{
									mvwprintw(sigwin_c,1,3,"设置个性签名");	
									wrefresh(sigwin_c);
								}
								strcpy(msg.msg,sig_c);
								strcpy(msg.name,self_name);
								msg.action = UPDATE_SIG;  //获取个性签名
								write(fd,&msg,sizeof(msg));
								break;
							}
						case 3:                                              /*搜索*/
							{
								mvwprintw(searchwin_c,1,3,"%26s"," ");
								echo();
								curs_set(1);
								wmove(searchwin_c,1,3);
								wrefresh(searchwin_c);
								wgetnstr(searchwin_c,msg.toname[0],19);
								curs_set(0);
								noecho();
								mvwprintw(result_search_win,1,1,"*");
								msg.action = SEARCH_FRIENDS;
								int select_search = 0;                        /*0:选择  1:退出*/
								int leave_select_search = 0;                       /*离开选择*/

								write(fd,&msg,sizeof(msg));

								usleep(1000);	

								while(1)
								{
									mvwprintw(result_search_win,pos_search[select_search][0],pos_search[select_search][1]," ");
									int l_i = getch();                        
									switch(l_i)
									{
										case KEY_LEFT:
										case KEY_UP:
										case KEY_RIGHT:
										case KEY_DOWN:
											{
												select_search = !select_search;
												break;
											}

										case '\n':
											{
												if(select_search == 1)
												{	
													leave_select_search = 1;
												}
												else 
												{

													flag_open_chatwin[0] = 1;  //相当于下拉列表打开 下标为0的对应聊天窗口打开
													flag_open_chatwin[1] = 1;
													chatwin_order = 2;
													flag_search = 1;   
													chatwin(fd,online[0],selectchatname[0],sigs[0]);

													flag_open_chatwin[0] = 0;
													flag_open_chatwin[1] = 0;
													flag_search = 0;

												}
												break;
											}
									}
									mvwprintw(result_search_win,pos_search[select_search][0],pos_search[select_search][1],"*");
									wrefresh(result_search_win);

									if(leave_select_search == 1)
									{
										break;
									}
								}

								mvwprintw(searchwin_c,1,1,"%20s"," ");
								mvwprintw(searchwin_c,1,25,"搜索");
								wrefresh(searchwin_c);
								touchwin(mainwin_c);
								wrefresh(mainwin_c);

								break;
							}
						case 6:    //设置
							{
								optionswin(fd);
								break;
							}
						case 5:                //我的群组
							{

								//初始化群组链表
								head_group = (USER *)malloc(sizeof(USER));
								head_group->next = NULL;
								msg.action = GET_GROUP;
								strcpy(msg.name,self_name);
								write(fd,&msg,sizeof(msg));

								usleep(100000);
								mvwprintw(mainwin_c,14,4,"*");
								mvwprintw(mainwin_c,13,2,"V 我的群组");
								wattron(mainwin_c,A_BOLD);
								mvwprintw(mainwin_c,13,27,"q");
								wattroff(mainwin_c,A_BOLD);

								USER * tmp = head_group;
								int i = 0;
								if(tmp != NULL)
								{
									tmp = tmp->next;
							
									while(tmp != NULL)
									{
										mvwprintw(mainwin_c,14 + i,5,"%s",tmp->name);
										i++;
										tmp = tmp->next;
									}

									//i即是群的个数
									wrefresh(mainwin_c);

									int ch;
									int select_group = 1;
									int leave_group = 0;
									int goto_q = 0;

									while(1)
									{
										if(goto_q == 1)
										{
											mvwprintw(mainwin_c,13,26," ");
										}
										else mvwprintw(mainwin_c,13 + select_group,4," ");
										ch = getch();
										switch(ch)
										{
											case KEY_UP:
												{
													select_group--;
													break;
												}
											case KEY_DOWN:
												{
													select_group++;
													break;
												}
											case KEY_LEFT:
												{
													goto_q = 0;
													break;
												}
											case KEY_RIGHT:
												{
													goto_q = 1; //退出
													break;	
												}
											case '\n':
												{

													//退出时将链表清空 
													if(goto_q == 1)
													{
														free_user(head_group);
														leave_group = 1;
													}
													else
													{
														chatwin_order_group = 2;
														if(i > 0)
														{
															chatwin_group(fd,get_user(head_group,select_group));
														}
													}
												}
											default:
												{
													break;
												}
										}

										if(leave_group == 1)
										{
											break;
										}
										if(goto_q == 1)
										{
											mvwprintw(mainwin_c,13,26,"*");
										}
										if(select_group < 1)
										{
											select_group = 1;
										}
										if(select_group > i)
										{
											select_group = i;
										}

										if(goto_q != 1)
										{
											mvwprintw(mainwin_c,13 + select_group,4,"*");
										}
										wrefresh(mainwin_c);
									}
								}
								mvwprintw(mainwin_c,13,2,"> 我的群组");
								mvwprintw(mainwin_c,13,26,"  ");
								int j;
								for(j = 1; j <= i; j++)     
								{
									mvwprintw(mainwin_c,13 + j,5,"%15s"," ");
								}
								wrefresh(mainwin_c);
								break;
							}
						case 4:                                              /*我的好友  只查询前4个好友*/  
							{
								flag_open_chatwin[0] = 1;    //打开了下拉列表  再查询一次数据库 看是否有联系人需要闪烁

								if(isrunning[0] == 1)
								{
									pthread_cancel(pid[0]);    //将我的好友闪烁进程结束
									isrunning[0] = 0;
								}
								msg.action = GET_FIRST_FOUR_FRIENDS;
								strcpy(msg.name,self_name);
								write(fd,&msg,sizeof(msg));
								usleep(60000);
								mvwprintw(mainwin_c,11,2,"        ");
								mvwprintw(mainwin_c,11,2,"V 我的好友");
								mvwprintw(mainwin_c,11,13,"上一页");
								mvwprintw(mainwin_c,11,20,"下一页");
								wattron(mainwin_c,A_BOLD);
								mvwprintw(mainwin_c,11,27,"q");
								wattroff(mainwin_c,A_BOLD);
								wrefresh(mainwin_c);

								int select_name = 0;
								int i;
								int max_select = 0;
								for(i = 0; i < 4; i++)
								{
									if(strlen(selectchatname[i]) > 0)
									{
										max_select++;
									}
									else
									{
										break;
									}
								}         //使之到最后不会下去
								max_select = 4;
								max_select += 3;
								int get_input;
								mvwprintw(mainwin_c,pos_name[select_name][0],pos_name[select_name][1],"*");
								wrefresh(mainwin_c);
#if 1
								while(1)
								{
									get_input = getch();
									mvwprintw(mainwin_c,pos_name[select_name][0],pos_name[select_name][1]," ");
									int leave_select = 0;
									switch(get_input)
									{
										case KEY_LEFT:
											{

											}
										case KEY_UP:
											{
												select_name--;
												break;
											}
										case KEY_RIGHT:
											{

											}
										case KEY_DOWN:
											{
												select_name++;
												break;
											}
										case '\n':
											{

												switch(select_name)
												{
													case 0://上一页
														{	msg.action = GET_PRE_FOUR_FRIENDS;
															strcpy(msg.name,self_name);
															write(fd,&msg,sizeof(msg));

															break;
														}
													case 1: //下一页
														{	
															msg.action = GET_NEXT_FOUR_FRIENDS;
															strcpy(msg.name,self_name);
															write(fd,&msg,sizeof(msg));

															break;
														}
													case 2:          //退出选择好友
														{
															msg.action = SEEK_UNREAD;
															strcpy(msg.name,self_name);
															write(fd,&msg,sizeof(msg));

															leave_select = 1;
															mvwprintw(mainwin_c,pos_name[select_name][0],pos_name[select_name][1]," ");
															bzero(selectchatname,sizeof(selectchatname));
															flag_open_chatwin[0] = 0;   //下拉菜单收回
															mvwprintw(mainwin_c,11,2,"> 我的好友                ");
															int case_i;
															for(case_i = 0; case_i < 10; case_i++)
															{
																wmove(mainwin_c,12 + case_i,2);
																wprintw(mainwin_c,"%27s"," ");
															}
															mvwprintw(mainwin_c,13,2,"> 我的群组                 ");
															wrefresh(mainwin_c);
															break;
														}
													case 3:{} case 4:{} case 5:{} 
													case 6:{
															   flag_open_chatwin[select_name - 3 + 1] = 1;//控制闪烁是下标是select_name - 3 + 1
															   chatwin_order = 2;   //每调用一次新窗口  顺序置为第2行
															   chatwin(fd,online[select_name - 3],selectchatname[select_name - 3],sigs[select_name - 3]);  
															   flag_open_chatwin[select_name - 3 + 1] = 0;//执行完聊天窗口后  代表窗口未打开  置0
															   break;
														   }
												}

											}

										default:
											{
												break;
											}
									}
									if(select_name < 0)
									{
										select_name = 0;
									}
									else if(select_name >= max_select)
									{
										select_name = max_select - 1;
									}
									if(leave_select == 1)
									{
										break;
									}
									//	if(select_name < 3 || (strlen(selectchatname[select_name - 3]) > 0))
									//	{
									mvwprintw(mainwin_c,pos_name[select_name][0],pos_name[select_name][1],"*");
									//	}
									wrefresh(mainwin_c);

								}
#endif
								wrefresh(mainwin_c);
								break;
							}
					}
					break;
				}
			default:
				{
					break;
				}
		}

		mvaddch(pos_c[input_c][0],pos_c[input_c][1],'*');
		refresh();
		if(leave_c == 1)
		{
			break;
		}
	}

	endwin();
	return LEAVE_ONLINE;
}
