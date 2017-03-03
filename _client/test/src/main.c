//#include "include.h"
#include "../../include/include.h"


int tcp_client(char * arg);

void * show_blink(void * arg)
{
	char name[20];
	strcpy(name,(char *)arg);
	int i;
	for(i = 0; i < 4; i++)
	{
		if(strcmp(name,selectchatname[i]) == 0)
		{
			break;
		}
	}

	if(i < 4)               //已经找到了对应的name
	{
		while(flag_open_chatwin[0] == 1 && flag_open_chatwin[i + 1] == 0)   //判断对应窗口是否已经打开
		{
			mvwprintw(mainwin_c,12 + 2 * i,3,"%s(%s)",name,online[i] == 1?"在线":"离线");
			wrefresh(mainwin_c);
			usleep(800000);
			mvwprintw(mainwin_c,12 + 2 * i,3,"%20s"," ");
			wrefresh(mainwin_c);
			usleep(800000);	

		}
	}

	if(flag_open_chatwin[0] == 1)
	{
		mvwprintw(mainwin_c,12 + 2 * i,3,"%s(%s)",name,online[i] == 1?"在线":"离线");
		wrefresh(mainwin_c);
	}
	pthread_exit(NULL);
}

void * show_blink_toplist(void * arg)
{
	//while(flag_open_chatwin[0] == 0)
	while(1)
	{
		mvwprintw(mainwin_c,11,2,"%10s"," ");
		wrefresh(mainwin_c);
		usleep(800000);
		mvwprintw(mainwin_c,11,2,"> 我的好友");
		wrefresh(mainwin_c);
		usleep(800000);	

	}

	mvwprintw(mainwin_c,11,2,"%10s"," ");
	mvwprintw(mainwin_c,11,2,"V 我的好友");
	wrefresh(mainwin_c);
	pthread_exit(NULL);

}


void * read_msg(void * arg)
{
	int fd = *(int *)arg;
	int n_read;

	int ret;

	//	sqlite3 * db;
	char ** result;
	char * errmsg;
	int nrows;
	int ncols; 

	if((ret = sqlite3_open("./database.db",&db)) != SQLITE_OK)
	{
		printf("sqlite_open:\n");
	}

	while(1)
	{


		if((n_read == read(fd,&msg,sizeof(MSG))) == -1)
		{
			perror("read");
			exit(-1);
		}


		else
		{
			switch(msg.action)
			{
				case LOGIN_SUCCESS:
					{
						g_flag = LOGIN_SUCCESS;
						break;
					}
				case LOGIN_FAILURE:
					{
						g_flag = LOGIN_FAILURE;
						break;
					}
				case LOGIN_ONLINE:
					{
						g_flag = LOGIN_ONLINE;
						break;
					}
				case NAME_ERR:
					{
						mvwprintw(mainwin,4,51,"            ");
						mvwprintw(mainwin,4,51,"昵称已被注册");
						mvwprintw(mainwin,4,20,"                    ");
						wrefresh(mainwin);
						break;
					}
				case GET_QUESTION:
					{
						mvwprintw(mainwin_gpwd,22,25,"%17s"," ");
						wrefresh(mainwin_gpwd);
						mvwprintw(questionwin,1,1,"%s",msg.question);
						wrefresh(questionwin);
						break;
					}
				case NO_QUESTION:
					{
						mvwprintw(mainwin_gpwd,22,25,"获取密保问题失败");
						wrefresh(mainwin_gpwd);
						break;
					}
				case WRONG_ANSWER:
					{
						mvwprintw(mainwin_gpwd,22,25,"问题回答错误!");
						wrefresh(mainwin_gpwd);
						break;
					}
				case MODIFY_PASSWD_SUCCESS:
					{	
						mvwprintw(mainwin_gpwd,22,25,"%17s"," ");
						mvwprintw(mainwin_gpwd,22,25,"修改密码成功!");
						wrefresh(mainwin_gpwd);
						break;
					}
				case NAME_SUCCESS:
					{
						mvwprintw(mainwin,4,51,"            ");
						mvwprintw(mainwin,4,51,"昵称有效");
						wrefresh(mainwin);
						break;
					}
				case GROUP_NAME_OK:
					{
						mvwprintw(mainwin_create,4,29,"%5s"," ");
						mvwprintw(mainwin_create,4,29,"OK");
						wrefresh(mainwin_create);
						break;
					}
				case GROUP_NAME_ERROR:
					{
						mvwprintw(mainwin_create,4,29,"%5s"," ");
						mvwprintw(mainwin_create,4,29,"USED");
						wrefresh(mainwin_create);
						break;
					}
				case GET_NOT_IN_GROUP_USERS:
					{
						USER * newuser = (USER *)malloc(sizeof(USER));
						strcpy(newuser->name,msg.name);
						insert_user(head,newuser);					
						break;
					}
				case GET_GROUP:
					{
						USER * newuser = (USER *)malloc(sizeof(USER));
						strcpy(newuser->name,msg.toname[0]);
						insert_user(head_group,newuser);
						break;
					}
				case REG_FAILURE:
					{
						mvwprintw(noticewin,2,5,"                ");
						mvwprintw(noticewin,3,5,"                         ");
						mvwprintw(noticewin,2,5,"注册失败");
						mvwprintw(noticewin,3,5,"请重试!           ");
						wrefresh(noticewin);
						break;
					}
				case REG_SUCCESS:
					{
						mvwprintw(noticewin,2,5,"                    ");
						mvwprintw(noticewin,3,5,"                          ");
						mvwprintw(noticewin,2,5,"注册成功!");
						mvwprintw(noticewin,3,5,"按任意键返回登录界面!");
						wrefresh(noticewin);
						reg = 1;
						break;

					}
				case GET_PRE_FOUR_FRIENDS:
				case GET_NEXT_FOUR_FRIENDS:
				case GET_FIRST_FOUR_FRIENDS:
					{
						int i = 0;	
						mvwprintw(mainwin_c,13,2,"            ");

						for(i = 0; i < 4; i++)
						{
							mvwprintw(mainwin_c,12 + 2 * i + 1,3,"%20s"," ");  //个性签名晴空
							mvwprintw(mainwin_c,12 + 2 * i,3,"%20s"," ");   //避免名字覆盖
							if(isrunning[i + 1] == 1)
							{
								pthread_cancel(pid[i + 1]);
								isrunning[i + 1] = 0;       //取消闪烁对应位闪烁进程为0
							}
						}
						i = 0;
						bzero(online,sizeof(online));               //每刷新一次页面 置0这个页面4个联系人的在线信息 
						flag_open_chatwin[1] = flag_open_chatwin[2] = flag_open_chatwin[3] = flag_open_chatwin[4] = 0;

						//			while(strlen(msg.toname[i]) > 0)
						while(i < 4)
						{
							//	mvwprintw(mainwin_c,12 + 2 * i,3,"%s",msg.toname[i]);//将联系人显示到窗口
							bzero(selectchatname[i],sizeof(selectchatname[i]));
							bzero(sigs[i],sizeof(sigs[i]));
							if(strlen(msg.toname[i]) > 0)
							{
								strcpy(selectchatname[i],msg.toname[i]);
								strcpy(sigs[i],msg.sigs[i]);
								online[i] = msg.online[i];
								mvwprintw(mainwin_c,12 + 2 * i,3,"%s(%s)",selectchatname[i],msg.online[i] == 1?"在线":"离线");//将联系人信息显示到窗口

								mvwprintw(mainwin_c,12 + 2 * i + 1,5,"%s",msg.sigs[i]);
								if(msg.has_undomsg[i] == 1)//有未接收消息 创建线程使之闪烁
								{
									if(isrunning[i + 1] == 0)  //并且没有运行闪烁线程
									{
										//pthread_t pid;
										pthread_create(&pid[i + 1],NULL,show_blink,(void *)selectchatname[i]);  //获取联系人之后发现有为接受消息  创建线程使其闪烁
										isrunning[i + 1] = 1;
									}

								}
							}
							i++;
						}

						mvwprintw(mainwin_c,21,2,"> 我的群组");
						wrefresh(mainwin_c);

						break;
					}
				case GET_ALL_FRIENDS:
					{
						if(strlen(msg.toname[0]) > 0)	
						{
							USER * newuser = (USER *)malloc(sizeof(USER));
							strcpy(newuser->name,msg.toname[0]);
							insert_user(head,newuser);
						}
						break;
					}

				case GET_RECENT_CHAT:
					{
						int i;
						for(i = 0; i < 4; i++)
						{
							//	if(strlen(msg.toname[i]) > 0)
							//	{
							mvwprintw(mainwin_create,8 + i,5,"%s",msg.toname[i]);
							USER * newuser = (USER *)malloc(sizeof(USER));
							strcpy(newuser->name,msg.toname[i]);
							insert_user(head,newuser);
							//	}
						}
						wrefresh(mainwin_create);
						break;
					}
				case SEEK_UNREAD:
					{
						if(isrunning[0] == 0)       //这时下拉列表没有闪 使之闪烁 并且置ifunning[0] 为 1
						{
							pthread_create(&pid[0],NULL,show_blink_toplist,NULL);  //调用显示闪烁函数
							isrunning[0] = 1;
						}

						break;
					}
				case MSG_READ:
					{
						mvwprintw(show_chat_win,chatwin_order - 1,20,"%s",msg.chat_time);
						mvwprintw(show_chat_win,chatwin_order,1,"未读 %s:%s",msg.toname[0],msg.msg);  //未读消息
						chatwin_order += 2;
						wrefresh(show_chat_win);
						break;
					}
				case NOTIFY_OTHER:    //当前页面有用户更新状态
					{
						int i;
						for(i = 0; i < 4; i++)
						{
							if(strcmp(selectchatname[i],msg.name) == 0)
							{
								if(flag_search == 0)
								{
									online[i] = 1;
									mvwprintw(mainwin_c,12 + 2 * i,3,"%s(%s)",selectchatname[i],online[i] == 1?"在线":"离线");//将联系人信息显示到窗口
									wrefresh(mainwin_c);
								}
								else
								{
									online[0] = 1;
									mvwprintw(result_search_win,1,2,"%s(%s)",selectchatname[0],online[0] == 1?"在线":"离线");
									wrefresh(result_search_win);
								}

								if(flag_open_chatwin[i + 1] == 1)
								{
									mvwprintw(mainwin_chat,1,15,"%30s"," ");
									wrefresh(mainwin_chat);

								}
								break;
							}
						}

						//以下更新群
						if(head_group_members != NULL)
						{
							USER * tmp = head_group_members;
							tmp = tmp->next;

							int i = 1;
							while(tmp != NULL)
							{
								if(strcmp(tmp->name,msg.name) == 0)
								{
									tmp->online = 1;
									mvwprintw(mainwin_chat_group,4 + i,63,"在线");	
									break;
								}
								tmp = tmp->next;
								i++;
							}

							wrefresh(mainwin_chat_group);
						}
						break;
					}
				case NOTIFY_LEAVE_ONLINE:
					{
						int i;
						for(i = 0; i < 4; i++)
						{
							if(strcmp(selectchatname[i],msg.name) == 0)
							{
								if(flag_search == 0)
								{
									online[i] = 0;   //离线
									mvwprintw(mainwin_c,12 + 2 * i,3,"%s(%s)",selectchatname[i],online[i] == 1?"在线":"离线");//将联系人信息显示到窗口
									wrefresh(mainwin_c);
								}
								else
								{
									online[0] = 0;
									mvwprintw(result_search_win,1,2,"%s(%s)",selectchatname[0],online[0] == 1?"在线":"离线");
									wrefresh(result_search_win);
								}

								if(flag_open_chatwin[i + 1] == 1)
								{
									mvwprintw(mainwin_chat,1,15,"%s","此人不在线,您可以发送离线消息");
									wrefresh(mainwin_chat);

								}
								break;
							}
						}
						//以下更新群
						if(head_group_members != NULL)
						{
							USER * tmp = head_group_members;
							tmp = tmp->next;

							int i = 1;
							while(tmp != NULL)
							{
								if(strcmp(tmp->name,msg.name) == 0)
								{
									tmp->online = 0;
									mvwprintw(mainwin_chat_group,4 + i,63,"离线");	
									break;
								}
								tmp = tmp->next;
								i++;
							}

							wrefresh(mainwin_chat_group);
						}

						break;
					}
				case NOTIFY_UPDATE_SIG:
					{
						int i;
						for(i = 0; i < 4; i++)
						{
							if(strcmp(selectchatname[i],msg.name) == 0)   //更新好友签名
							{
								if(flag_search == 0)   //如果是由于搜索打开的窗口 则不需要更新列表下的联系人
								{
									strcpy(sigs[i],msg.msg);
									mvwprintw(mainwin_c,12 + 2 * i + 1,2,"%20s"," ");
									mvwprintw(mainwin_c,12 + 2 * i + 1,5,"%s",msg.msg);
									wrefresh(mainwin_c);
								}

								else
								{
									strcpy(sigs[0],msg.msg);	
									mvwprintw(result_search_win,2,4,"%20s"," ");
									mvwprintw(result_search_win,2,4,"%s",sigs[0]);
									wrefresh(result_search_win);

								}
								if(flag_open_chatwin[i + 1] == 1)
								{
									if(strlen(msg.msg) > 0)
									{
										mvwprintw(mainwin_chat,3,8,"%50s"," ");
										mvwprintw(mainwin_chat,3,8,"%s",msg.msg);
									}
									else
									{
										mvwprintw(mainwin_chat,3,8,"%50s"," ");
										mvwprintw(mainwin_chat,3,8,"这个人很懒,什么都没留下");
									}
									wrefresh(mainwin_chat);
								}
								break;        
							}

						}
						break;

					}
				case SEARCH_FRIENDS:
					{
						mvwprintw(result_search_win,1,2,"%20s"," ");
						mvwprintw(result_search_win,1,2,"%s(%s)",msg.toname[0],msg.online[0] == 1?"在线":"离线");
						mvwprintw(result_search_win,2,4,"%s",msg.sigs[0]);
						touchwin(result_search_win);
						wrefresh(result_search_win);
						bzero(selectchatname[0],sizeof(selectchatname[0]));
						bzero(sigs[0],sizeof(sigs[0]));
						strcpy(selectchatname[0],msg.toname[0]);
						online[0] = msg.online[0];
						strcpy(sigs[0],msg.sigs[0]);
						break;
					}
				case SEARCH_ERROR:
					{

						mvwprintw(result_search_win,1,22," ");
						//	mvwprintw(result_search_win,1,1,"%20s","*");
						mvwprintw(result_search_win,1,2,"%20s"," ");
						mvwprintw(result_search_win,2,4,"%20s"," ");
						mvwprintw(result_search_win,1,2,"not found");
						touchwin(result_search_win);
						wrefresh(result_search_win);

						break;
					}
				case GET_NEXT_CHAT_RECORD:
				case GET_PRE_CHAT_RECORD:
				case GET_FIRST_CHAT_RECORD:
					{
						//将发送记录的时间分解开
						char * date_tmp;
						char * time_tmp;

						date_tmp = strtok(msg.chat_time," ");  //将日期分割开
						time_tmp = strtok(NULL," ");           //时间

						if(strcmp(date,date_tmp) != 0)
						{
							mvwprintw(mainwin_cr,cr_order,3,"----------------------------%s----------------------------",date_tmp);
							strcpy(date,date_tmp);
							cr_order++;
						}

						mvwprintw(mainwin_cr,cr_order,2,"%s %s",msg.name,time_tmp);	
						cr_order++;
						mvwprintw(mainwin_cr,cr_order,4,"%s",msg.msg);
						cr_order++;
						wrefresh(mainwin_cr);
						break;
					}
				case GET_NEXT_CHAT_RECORD_GROUP:
				case GET_PRE_CHAT_RECORD_GROUP:
				case GET_FIRST_CHAT_RECORD_GROUP:
					{
						//将发送记录的时间分解开
						char * date_tmp;
						char * time_tmp;

						date_tmp = strtok(msg.chat_time," ");  //将日期分割开
						time_tmp = strtok(NULL," ");           //时间

						if(strcmp(date,date_tmp) != 0)
						{
							mvwprintw(mainwin_cr_group,cr_order,3,"----------------------------%s----------------------------",date_tmp);
							strcpy(date,date_tmp);
							cr_order++;
						}

						mvwprintw(mainwin_cr_group,cr_order,2,"%s %s",msg.name,time_tmp);	
						cr_order++;
						mvwprintw(mainwin_cr_group,cr_order,4,"%s",msg.msg);
						cr_order++;
						wrefresh(mainwin_cr_group);
						break;
					}

				case BEGIN_TRANSFER_FILE:
					{
						//					int save_fd = open("./demosave.c",O_WRONLY|O_APPEND);
						//					write(save_fd,&msg.msg,sizeof(msg.msg));

						//flag_file = 1;	
						break;
					}
#if 1
				case START_SEND_FILE:
					{
						int srcfile_fd = open(msg.msg,O_RDONLY);
						int nread;

						char nametmp[20];
						char tonametmp[20];
						//toname[0] 是自己的名字 name是对方的名字
						strcpy(nametmp,msg.toname[0]);
						strcpy(tonametmp,msg.name);
						strcpy(msg.toname[1],msg.msg);   //toname[1]  为发送的文件名
						bzero(msg.msg,sizeof(msg.msg));
						while((nread = read(srcfile_fd,&msg.msg,sizeof(msg.msg))) > 0)
						{
							msg.action = SENDING_FILE;
							strcpy(msg.name,nametmp);
							strcpy(msg.toname[0],tonametmp);
							msg.hasquestion = nread;   //存储读了多少字节
							write(fd,&msg,sizeof(msg));
							usleep(1000);
							bzero(msg.msg,sizeof(msg.msg));
						}
						msg.action = SENDING_FILE;
						msg.hasquestion = -1;
						write(fd,&msg,sizeof(msg));

						break;
					}
				case RECEVING_FILE:
					{
						if(msg.hasquestion == -1)
						{
							close(destfile_fd);
							sprintf(msg.msg,"###receive file %s successfully",msg.toname[1]);  //filename
							char temp[20];
							strcpy(temp,msg.toname[0]);
							strcpy(msg.toname[0],msg.name);  //对方name
							strcpy(msg.name,temp);           //自己name
							msg.action = CHAT_PRIVATE;
							//msg.hasquestion = 10086;       //代表是成功接收文件后的通知
							write(fd,&msg,sizeof(msg));      //通知对方已收到文件

							mvwprintw(show_chat_win,chatwin_order,1,"sys: receive file sucessfully!");
							wrefresh(show_chat_win);

						}
						write(destfile_fd,&msg.msg,msg.hasquestion);
						break;
					}
#endif
				case START_RECEIVE_FILE_ERROR:
					{
						touchwin(input_filename_win);
						mvwprintw(input_filename_win,1,2,"%20s"," ");
						mvwprintw(input_filename_win,2,2,"%18s"," ");
						mvwprintw(input_filename_win,1,2,"文件名不正确!");
						mvwprintw(input_filename_win,2,2,"对方没有发送此文件");
						wrefresh(input_filename_win);
						sleep(1);
						touchwin(mainwin_chat);
						wrefresh(mainwin_chat);

						break;
					}
				case START_RECEIVE_FILE:
					{
						//接收文件的名字
						destfile_fd = open(msg.msg,O_CREAT|O_EXCL|O_WRONLY);
						if(destfile_fd < 0)
						{
							//提醒用户文件已存在

							touchwin(input_filename_win);
							mvwprintw(input_filename_win,1,2,"%20s"," ");
							mvwprintw(input_filename_win,2,2,"%18s"," ");
							mvwprintw(input_filename_win,1,2,"文件已存在!");
							mvwprintw(input_filename_win,2,2,"请重新选择文件名");
							wrefresh(input_filename_win);
							sleep(1);
							touchwin(mainwin_chat);
							wrefresh(mainwin_chat);

						}
						break;
					}
				case RECEIVE_NOT_ONLINE:
					{
						touchwin(input_filename_win);
						mvwprintw(input_filename_win,1,2,"%20s"," ");
						mvwprintw(input_filename_win,2,2,"%18s"," ");
						mvwprintw(input_filename_win,1,2,"对方不在线!");
						mvwprintw(input_filename_win,2,2,"传输文件双方必须在线");
						wrefresh(input_filename_win);
						sleep(1);
						touchwin(mainwin_chat);
						wrefresh(mainwin_chat);
						break;
					}

				case GET_RECENT_GROUP_CHAT_RECORD:
					{
						//	break;
					}
				case CHAT_GROUP:
					{
						//打开当前群窗口
						if(strcmp(msg.toname[0],current_groupname) == 0)
						{
							int i;
							if(chatwin_order_group >= 14)
							{
								for(i = 1; i < 14; i++)
								{
									mvwprintw(show_chat_win_group,i,1,"%43s"," ");
								}
								wrefresh(show_chat_win_group);
								chatwin_order_group = 2;
							}
							if(chatwin_order_group == 2)
							{
								mvwprintw(show_chat_win_group,1,10,"%s",msg.chat_time);
							}
							if(strcmp(msg.name,self_name) == 0)
							{
								if(strlen(msg.msg) >= 35)
								{
									mvwprintw(show_chat_win_group,chatwin_order_group,1,"%s   :me",msg.msg);
								}
								else
								{
									mvwprintw(show_chat_win_group,chatwin_order_group,35 - strlen(msg.msg),"%s   :me",msg.msg);
								}

							}
							else
							{
								mvwprintw(show_chat_win_group,chatwin_order_group,1,"%s :%s",msg.name,msg.msg);
							}
							wrefresh(show_chat_win_group);
							chatwin_order_group++;
						}
						break;
					}
				case GET_GROUP_CREATE_TIME:
					{
						mvwprintw(mainwin_chat_group,2,8,"于%s加入此群",msg.toname[1]);
						mvwprintw(mainwin_chat_group,3,8,"%s 建群于%s",msg.name,msg.chat_time); 
						wrefresh(mainwin_chat_group);
						break;
					}
				case GET_GROUP_MEMBERS:
					{
						if(strlen(msg.name) > 0)
						{
							USER * newuser = (USER *)malloc(sizeof(USER));
							strcpy(newuser->name,msg.name);
							newuser->ban = 0;   //均默认不是禁言
							newuser->online = msg.online[0];

							//printf("insert\n\b");
							insert_user(head_group_members,newuser);
						}
						else
						{
							if(head_group_members != NULL)
							{
								//添加完毕 重画界面
								draw_group_member_list();	
							}
						}
						break;
					}
				case NOTIFY_UPDATE_GROUP_MEMBER_LIST:
					{
						//需要刷新窗口的联系人
						if(strcmp(current_groupname,msg.toname[0]) == 0)
						{
							//先将原链表清空 重新获取
							free_user(head_group_members);
							head_group_members = (USER *)malloc(sizeof(USER));
							head_group_members->next = NULL;

							msg.action = GET_GROUP_MEMBERS;
							//		strcpy(msg.toname[0],groupname);
							write(fd,&msg,sizeof(msg));
						}
						break;
					}
				case GET_BAN:
					{
						USER * tmp = head_group_members;
						tmp = tmp->next;

						while(tmp != NULL)
						{
							if(strcmp(msg.name,tmp->name) == 0)
							{
								tmp->ban = 1;
								break;
							}
							tmp = tmp->next;
						}
						break;
					}
				case NOTIFY_BAN:
					{
						mvwprintw(show_chat_win_group,chatwin_order_group,1,"您已被群主禁言!");
						wrefresh(show_chat_win_group);
						chatwin_order_group++;
						break;
					}
				case CHAT_PRIVATE:
					{
						if(flag_open_chatwin[0] == 1)  //下拉菜单已经打开
						{


							int i;
							for(i = 0; i < 4; i++)
							{
								if(strcmp(selectchatname[i],msg.name) == 0) {
									break;
								}
							}
							if(i < 4 && flag_open_chatwin[i + 1] == 1) //对应窗口是打开的
							{
								if(chatwin_order >= 14)
								{
									for(i = 1; i < 14; i++)
									{
										mvwprintw(show_chat_win,i,1,"%53s"," ");
									}
									wrefresh(show_chat_win);
									chatwin_order = 2;
								}
								if(chatwin_order == 2)
								{
									mvwprintw(show_chat_win,1,20,"%s",msg.chat_time);
								}
								mvwprintw(show_chat_win,chatwin_order,1,"%s :%s",msg.name,msg.msg);
								wrefresh(show_chat_win);
								chatwin_order++;

							}
							else                          //没有打开聊天对话框 信息存入undo_msg中 并且创建线程使其闪烁
							{

								//发送服务器 将信息标为未读
								msg.action = MSG_NOREAD;
								write(fd,&msg,sizeof(msg));

								if(i < 4)   
								{
									isrunning[i + 1] = 1;
									pthread_create(&pid[i + 1],NULL,show_blink,(void *)msg.name);  //调用显示闪烁函数
								}
							}

						}
						else                          //下拉列表没有打开
						{
							msg.action = MSG_NOREAD;
							write(fd,&msg,sizeof(msg));
							if(isrunning[0] == 0)       //这是下拉列表没有闪 使之闪烁 并且置ifunning[0] 为 1
							{
								pthread_create(&pid[0],NULL,show_blink_toplist,NULL);  //调用显示闪烁函数
								isrunning[0] = 1;
							}
						}
						wrefresh(show_chat_win);
						break;
					}

				default:
					{
						break;
					}
			}
			wrefresh(mainwin);
			refresh();
		}
	}

}
int main(int argc,char ** argv)
{
	if(argc != 2)
	{
		printf("usage:%s server_ip\n",argv[0]);
		exit(1);
	}

	int sockfd = tcp_client(argv[1]);
	//registerwin(sockfd);
	//Login(sockfd);
	pthread_t pid;
	pthread_create(&pid,NULL,read_msg,(void *)&sockfd);

	//	Friendwin(sockfd);
#if 1
	int ret = Login(sockfd);

	int leave = 0;
	while(1)
	{
		switch(ret)
		{
			case REGISTER_WIN:
				{
					if(registerwin(sockfd) == BACK_TO_LOGIN)
					{
						ret	= Login(sockfd);
					}
					break;
				}
			case GET_BACK_PASSWD:
				{
					getpasswd(sockfd);
					ret = Login(sockfd);
					break;
				}
			case FRIEND_WIN:
				{
					if(Friendwin(sockfd) == LEAVE_ONLINE)
					{
						leave = 1;
						break;
					}
				}
			case QUIT:
				{
					leave = 1;
					break;
				}
		}

		if(leave == 1)
		{
			break;
		}
	}
#endif
	pthread_cancel(pid);

	return 0;
}
