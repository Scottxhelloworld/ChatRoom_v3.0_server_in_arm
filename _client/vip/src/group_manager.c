//#include "include.h"
#include "../../include/include.h"

void group_manager(int fd,char * groupname)
{
	//获取禁言人
	msg.action = GET_BAN;
	strcpy(msg.toname[0],groupname);
	write(fd,&msg,sizeof(msg));

	int ban_talk[10];
	int ban_i = 0;     //禁言下标

	int can_talk[10];
	int can_i = 0;

	int remove_talk[10];  //踢人
	int remove_i = 0;

	char add_talk[10][20]; //拉人
	int add_i = 0;

	int first = 0;  //确保拉人链表只构造一次

	WINDOW * mainwin_manager = newwin(27,70,1,10);
	box(mainwin_manager,0,0);

	int half_count = get_list_count(head_group_members) / 2;
	USER * tmp = head_group_members;

	//群主本人不显示
	tmp = tmp->next->next;
	mvwvline(mainwin_manager,1,35,'|',20);


	int start_line = 1;
	int start_col  = 10;

	int i = 0;
#if 0
	usleep(1000);
	while(tmp != NULL)
	{
		i++;
		if(i <= half_count)
		{
			if(tmp->ban == 1)
			{
				mvwprintw(mainwin_manager,start_line + i,start_col - 2,"禁");
			}
			mvwprintw(mainwin_manager,start_line + i,start_col,"%d :%s",i,tmp->name);
		}
		else
		{
			if(tmp->ban == 1)
			{
				mvwprintw(mainwin_manager,start_line + (i - half_count),start_col + 33,"禁");
			}
			mvwprintw(mainwin_manager,start_line + (i - half_count),start_col + 35,"%d :%s",i,tmp->name);
		}

		tmp = tmp->next;
	}
#endif

	int start_item_line = 22;
	int start_item_col  = 7;
	char items[][20] = {"禁言","解禁","踢人","拉人","确定","取消"};

	for(i = 0; i < 6; i++)
	{
		if(0 == i)
		{
			wattron(mainwin_manager,A_REVERSE);	
		}
		mvwprintw(mainwin_manager,start_item_line,start_item_col + i * 10,"%s",items[i]);
		if(0 == i)
		{
			wattroff(mainwin_manager,A_REVERSE);
		}
	}
	wrefresh(mainwin_manager);

	i = 0;
	usleep(1000);
	while(tmp != NULL)
	{
		i++;
		if(i <= half_count)
		{
			if(tmp->ban == 1)
			{
				mvwprintw(mainwin_manager,start_line + i,start_col - 2,"禁");
			}
			mvwprintw(mainwin_manager,start_line + i,start_col,"%d :%s",i,tmp->name);
		}
		else
		{
			if(tmp->ban == 1)
			{
				mvwprintw(mainwin_manager,start_line + (i - half_count),start_col + 33,"禁");
			}
			mvwprintw(mainwin_manager,start_line + (i - half_count),start_col + 35,"%d :%s",i,tmp->name);
		}

		tmp = tmp->next;
	}
	int ch;
	int choice;
	int old_choice;

	choice = old_choice = 0;  // 当前选中和先前选中

	wrefresh(mainwin_manager);


	while(1)
	{
		ch = getch();
		switch(ch)
		{
			case KEY_LEFT:
			case KEY_UP:
				{
					choice = (choice == 0 ? 5: choice - 1);
					break;
				}
			case KEY_RIGHT:
			case KEY_DOWN:
				{
					choice = (choice == 5 ? 0 : choice + 1);
					break;
				}
			case '\n':
				{
					switch(choice)
					{
						case 0:
							{
								curs_set(1);
								echo();
								wmove(mainwin_manager,23,7);
								wrefresh(mainwin_manager);
								wscanw(mainwin_manager,"%d",&ban_talk[ban_i]);

								//对应名字前加禁 字
								if(ban_talk[ban_i] <= half_count)
								{
									mvwprintw(mainwin_manager,start_line + ban_talk[ban_i],start_col - 2,"禁");
								}
								else
								{
									mvwprintw(mainwin_manager,start_line + (ban_talk[ban_i] - half_count),start_col + 33,"禁");
								}

								ban_i++;
								noecho();
								curs_set(0);
								mvwprintw(mainwin_manager,23,7,"%3s"," ");
								wrefresh(mainwin_manager);
								break;

							}
						case 1:  //解禁
							{
								curs_set(1);
								echo();
								wmove(mainwin_manager,23,17);
								wrefresh(mainwin_manager);
								wscanw(mainwin_manager,"%d",&can_talk[can_i]);

								//对应名字前擦除禁字
								if(can_talk[can_i] <= half_count)
								{
									mvwprintw(mainwin_manager,start_line + can_talk[can_i],start_col - 2,"  ");
								}
								else
								{
									mvwprintw(mainwin_manager,start_line + (can_talk[can_i] - half_count),start_col + 33,"  ");
								}

								can_i++;
								noecho();
								curs_set(0);
								mvwprintw(mainwin_manager,23,17,"%3s"," ");
								wrefresh(mainwin_manager);
								break;
							}
						case 2:  //踢人
							{

								curs_set(1);
								echo();
								wmove(mainwin_manager,23,27);
								wrefresh(mainwin_manager);
								wscanw(mainwin_manager,"%d",&remove_talk[remove_i]);

								//对应名字前擦除禁字
								if(remove_talk[remove_i] <= half_count)
								{
									mvwprintw(mainwin_manager,start_line + remove_talk[remove_i],start_col - 5,"踢");
								}
								else
								{
									mvwprintw(mainwin_manager,start_line + (remove_talk[remove_i] - half_count),start_col + 30,"踢");
								}

								remove_i++;
								noecho();
								curs_set(0);
								mvwprintw(mainwin_manager,23,27,"%3s"," ");
								wrefresh(mainwin_manager);
								break;
							}
						case 3:             //拉人   这里都是默认拉人都会同意的
							{
#if 1

								char nametmp[20];
								if(first == 0)
								{
									//初始化头节点
									head = (USER *)malloc(sizeof(USER));
									head->next = NULL;
									msg.action = GET_NOT_IN_GROUP_USERS;
									strcpy(msg.toname[0],groupname);
									write(fd,&msg,sizeof(msg));
								}

								first = 1;
								echo();
								mvwprintw(mainwin_manager,23,37,"输入添加人名字:");
								wrefresh(mainwin_manager);
								move(24,63);
								refresh();
								curs_set(1);

								wscanw(mainwin_manager,"%s",nametmp);

								curs_set(0);
								noecho();

								//判断输入的姓名是否合法
								USER * tmp = head;
								USER * pre = tmp;
								if(tmp != NULL)
								{
									tmp = tmp->next;
								}
								while(tmp != NULL)
								{
									if(strcmp(tmp->name,nametmp) == 0)
									{
										mvwprintw(mainwin_manager,start_line + half_count + 1 + add_i,start_col,"%s",nametmp);
										wrefresh(mainwin_manager);
										strcpy(add_talk[add_i],nametmp);
										add_i++;
										pre->next = tmp->next;
										free(tmp);
										break;
									}

									tmp = tmp->next;
									pre = pre->next;
								}

								//说明名字不合法
								if(tmp == NULL)
								{
									mvwprintw(mainwin_manager,24,37,"名字不存在或者已在加入群组中!");
									wrefresh(mainwin_manager);
									getch();
									mvwprintw(mainwin_manager,24,37,"%30"," ");
									wrefresh(mainwin_manager);
								}
								mvwprintw(mainwin_manager,23,37,"%30s"," ");
								mvwprintw(mainwin_manager,24,37,"%30s"," ");
								break;
#endif
							}
						case 5:
							{
								goto end;
								break;
							}
						case 4:
							{
								int i;
								//有禁言的人
								if(ban_i > 0)
								{
									for(i = 0; i < ban_i; i++)	
									{
										msg.action = SET_BAN;
										strcpy(msg.toname[0],groupname);
										strcpy(msg.name,get_user(head_group_members,ban_talk[i] + 1)); //由于群主占了一个
										write(fd,&msg,sizeof(msg));
									}
								}

								//有解禁的人
								if(can_i > 0)
								{
									for(i = 0; i < can_i; i++)
									{
										USER * des = get_user_node(head_group_members,can_talk[i] + 1);
										msg.action = CANCEL_BAN;
										strcpy(msg.toname[0],groupname);
										strcpy(msg.name,des->name);
										des->ban = 0;
										write(fd,&msg,sizeof(msg));
									}
								}
								//有要踢走的人
								if(remove_i > 0)
								{


									for(i = 0; i < remove_i; i++)
									{
										//从数据库中删除
										msg.action = REMOVE_GROUP_MEMBER;
										strcpy(msg.toname[0],groupname);
										strcpy(msg.name,get_user(head_group_members,remove_talk[i] + 1));
										write(fd,&msg,sizeof(msg));
										//再更新链表
										delete_user(head_group_members,remove_talk[i] + 1);
									}

									//删除结束后发送一个空姓名 代表结束删除 服务器发送其他群成员通知群成员列表
									bzero(msg.name,sizeof(msg.name));
									msg.action = REMOVE_GROUP_MEMBER;
									write(fd,&msg,sizeof(msg));
									//再根据链表内容重新更新群组的好友联系人列表
									draw_group_member_list();	
								}

								//拉人 先从数据库添加  再重新获取群成员  因为要获取在线
								if(add_i > 0)
								{
									for(i = 0; i < add_i; i++)
									{
										msg.action = ADD_GROUP_MEMBERS;
										strcpy(msg.toname[0],groupname);
										strcpy(msg.name,add_talk[i]);

										write(fd,&msg,sizeof(msg));

									}
									free_user(head_group_members);

									head_group_members = (USER *)malloc(sizeof(USER));
									head_group_members->next = NULL;

									msg.action = GET_GROUP_MEMBERS;
									strcpy(msg.toname[0],groupname);
									write(fd,&msg,sizeof(msg));

									//拉人结束后 提醒在线群成员更新
									bzero(msg.name,sizeof(msg.name));
									msg.action = ADD_GROUP_MEMBERS;
									strcpy(msg.toname[0],groupname);
									write(fd,&msg,sizeof(msg));

								//	usleep(100000);

								//	draw_group_member_list();	
								}

								goto end;
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
		mvwprintw(mainwin_manager,start_item_line,start_item_col + old_choice * 10,"%s",items[old_choice]);//先将原来的文字重划
		wattron(mainwin_manager,A_REVERSE);
		mvwprintw(mainwin_manager,start_item_line,start_item_col + choice * 10,"%s",items[choice]);
		wattroff(mainwin_manager,A_REVERSE);
		old_choice = choice;
		wrefresh(mainwin_manager);
	}


end:
	wclear(mainwin_manager);
	wrefresh(mainwin_manager);
	delwin(mainwin_manager);

	touchwin(mainwin_chat_group);
	wrefresh(mainwin_chat_group);
}
