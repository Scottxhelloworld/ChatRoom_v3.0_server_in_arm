#include "include_server.h"

#define PORTNUMBER 8888
#define ERR_HANDLE(msg) \
	do {perror(msg); exit(EXIT_FAILURE);} while(0)

int my_isprefix(char * src1,char * src2);

void * server_handle_msg(void * arg)
{
	int fd = *((int *)arg);

	struct message clientmsg;
	int n_read;
	int ret ; 
	int page = 0;
	int pagesize = 4;

	int start_cr;                      //记录行数
	int start_cr_group;

	sqlite3 * db;
	char ** result;
	char * errmsg;
	int nrows;
	int ncols; 
	char sql[256];

	if((ret = sqlite3_open("./database.db",&db)) != SQLITE_OK)
	{
		ERR_HANDLE("sqlite_open:");
	}


	while(1)
	{
		n_read = read(fd,&clientmsg,sizeof(clientmsg));

		if(n_read == -1)
		{
			//ERR_HANDLE("read msg"); 
			printf("read msg error\n");
			pthread_exit(NULL);
		} 
		if(n_read == 0)
		{
			printf("the client is closed\n");
			pthread_exit(NULL);
		}
		else
		{
			switch(clientmsg.action)
			{
				case CHECK_NAME:
					{
						bzero(sql,sizeof(sql));
						printf("%s\n",clientmsg.name);
						sprintf(sql,"select * from usertable where name = '%s';",clientmsg.name);		
						ret = sqlite3_get_table(db,sql,&result,&nrows,&ncols,&errmsg);
						if(ret != SQLITE_OK)
						{
							printf("get table:%s",errmsg);
							break;
						}			

						if(nrows == 0)
						{
							printf("未被注册\n");
							clientmsg.action = NAME_SUCCESS;
							ret = write(fd,&clientmsg,sizeof(clientmsg));
							if(ret == -1)
							{
								printf("write error\n");
							}
						}
						else if(nrows >= 1)
						{
							printf("已经被注册\n");
							clientmsg.action = NAME_ERR;
							ret = write(fd,&clientmsg,sizeof(clientmsg));
							if(ret == -1)
							{
								printf("write error\n");
							}

						}

						sqlite3_free_table(result);
						break;
					}
				case REG_USER:
					{
						printf("start reg\n");
						bzero(sql,sizeof(sql));
						if(clientmsg.hasquestion == 1)
						{
							sprintf(sql,"insert into usertable (name,passwd,hasquestion,question,answer) values ('%s','%s',1,'%s','%s');",clientmsg.name,clientmsg.passwd,clientmsg.question,clientmsg.answer);
						}
						else
						{
							sprintf(sql,"insert into usertable (name,passwd) values ('%s','%s');",clientmsg.name,clientmsg.passwd);
						}
						if((ret = sqlite3_exec(db,sql,NULL,NULL,&errmsg)) != SQLITE_OK)
						{
							printf("exec error:%s",errmsg);
							clientmsg.action = REG_FAILURE;
							write(fd,&clientmsg,sizeof(clientmsg));
						}

						else 
						{
							clientmsg.action = REG_SUCCESS;
							write(fd,&clientmsg,sizeof(clientmsg));
						}

						break;
					}
				case LOGIN:
					{
						char sig[30];  //保存用户的个性签名
						bzero(sig,sizeof(sig));
						int user_correct = 0;
						int user_notonline = 0;
						bzero(sql,sizeof(sql));
						sprintf(sql,"select sig,status from usertable where name = '%s' and passwd = '%s';",clientmsg.name,clientmsg.passwd);		
						ret = sqlite3_get_table(db,sql,&result,&nrows,&ncols,&errmsg);
						if(ret != SQLITE_OK)
						{
							printf("get table:%s",errmsg);
							break;
						}			

						if(nrows == 0)
						{
							clientmsg.action = LOGIN_FAILURE;
							write(fd,&clientmsg,sizeof(clientmsg));
							printf("login no user\n");
						}
						else if(nrows == 1)           //用户名存在 下面判断已在线
						{
							user_correct = 1;
							if(result[2] != NULL)	
							{
								strcpy(sig,result[2]);
							}

							//用户权限  
							clientmsg.hasquestion = atoi(result[3]);
							sqlite3_free_table(result);
							bzero(sql,sizeof(sql));
							sprintf(sql,"select * from online where name = '%s';",clientmsg.name);	
							ret = sqlite3_get_table(db,sql,&result,&nrows,&ncols,&errmsg);
							if(ret != SQLITE_OK)
							{
								printf("get table:%s",errmsg);
								break;
							}			

							if(nrows == 0)
							{
								bzero(sql,sizeof(sql));
								sprintf(sql,"insert into online (name,fd) values ('%s',%d)",clientmsg.name,fd);
								sqlite3_exec(db,sql,NULL,NULL,NULL);
								clientmsg.action = LOGIN_SUCCESS;  //成功登录
								strcpy(clientmsg.msg,sig);
								printf("clientmsg = %s\n",clientmsg.msg);
								write(fd,&clientmsg,sizeof(clientmsg));  

								//成功登录后  告知其他在线客户端  如果此人在其他客户端的当前页面  刷新此人在线信息
								clientmsg.action = NOTIFY_OTHER;
								sprintf(sql,"select fd from online where fd != %d;",fd);
								ret = sqlite3_get_table(db,sql,&result,&nrows,&ncols,&errmsg);
								if(ret != SQLITE_OK)
								{
									printf("get table:%s",errmsg);
									break;
								}									

								int i;
								int to_fd;
								for(i = 1; i <= nrows; i++)
								{
									to_fd = atoi(result[i]);
									printf("to_fd = %d\n",to_fd);
									write(to_fd,&clientmsg,sizeof(clientmsg));
								}
								printf("login success %d\n",LOGIN_SUCCESS);

							}
							else if(nrows == 1)
							{
								//	msg.action = LOGIN_SUCCESS;
								//	write(fd,&msg,sizeof(msg));
								clientmsg.action = LOGIN_ONLINE;  //已在线
								write(fd,&clientmsg,sizeof(clientmsg));
								printf("login online\n");

							}
						}


						break;
					}
				case REG_VIP:
					{
						sprintf(sql,"update usertable set status = 1 where name = '%s';",clientmsg.name);
						sqlite3_exec(db,sql,NULL,NULL,NULL);
						break;
					}
				case GET_QUESTION:
					{
						sprintf(sql,"select question from usertable where name = '%s' and hasquestion = 1;",clientmsg.name);
						ret = sqlite3_get_table(db,sql,&result,&nrows,&ncols,&errmsg);
						if(ret != SQLITE_OK)
						{
							printf("get_table:%s\n",errmsg);
							break;	
						}

						if(nrows == 1)
						{
							strcpy(clientmsg.question,result[1]);
							write(fd,&clientmsg,sizeof(clientmsg));
						}
						else
						{
							clientmsg.action = NO_QUESTION;
							write(fd,&clientmsg,sizeof(clientmsg));
						}
						break;
					}
				case MODIFY_PASSWD:
					{
						sprintf(sql,"select * from usertable where name = '%s' and answer = '%s';",clientmsg.name,clientmsg.answer);
						printf("sql = %s\n",sql);
						ret = sqlite3_get_table(db,sql,&result,&nrows,&ncols,&errmsg);
						if(ret != SQLITE_OK)
						{
							printf("get_table:%s\n",errmsg);
							break;	
						}

						//正确 可以修改
						if(nrows == 1)
						{
							sprintf(sql,"update usertable set passwd = '%s' where name = '%s';",clientmsg.passwd,clientmsg.name);
							printf("sql = %s\n",sql);
							sqlite3_exec(db,sql,NULL,NULL,NULL);
							clientmsg.action = MODIFY_PASSWD_SUCCESS;
							write(fd,&clientmsg,sizeof(clientmsg));
						}
						else
						{
							clientmsg.action = WRONG_ANSWER;
							write(fd,&clientmsg,sizeof(clientmsg));
						}
						break;
					}
				case GET_RECENT_CHAT:
					{
						bzero(sql,sizeof(sql));
						sprintf(sql,"select toname from chat_record where name = '%s' group by toname order by chat_time",clientmsg.name);
						ret = sqlite3_get_table(db,sql,&result,&nrows,&ncols,&errmsg);
						if(ret != SQLITE_OK)
						{
							printf("get_table:%s\n",errmsg);
							break;	
						}

						int i;
						for(i = 0; i < 4; i++)
						{
							bzero(clientmsg.toname[i],sizeof(clientmsg.toname[i]));
						}
						//查询最后的三个联系人 
						int start  = nrows - 4 + 1;
						if(start < 1)
						{
							start = 1;
						}
						for(i = start; i < nrows + 1; i++)
						{
							strcpy(clientmsg.toname[i - start],result[i]);
							printf("toname[%d] = %s\n",i - start,clientmsg.toname[i - start]);
						}

						write(fd,&clientmsg,sizeof(clientmsg));
						break;
					}
				case GET_ALL_FRIENDS:
					{
						bzero(sql,sizeof(sql));
						sprintf(sql,"select name from usertable where name != '%s';",clientmsg.name);
						ret = sqlite3_get_table(db,sql,&result,&nrows,&ncols,&errmsg);
						if(ret != SQLITE_OK)
						{
							printf("get_table:%s\n",errmsg);
							break;	
						}
						int i;
						for(i = 1; i < nrows + 1; i++)
						{
							clientmsg.action = GET_ALL_FRIENDS;
							//一个个的发送所有联系人
							strcpy(clientmsg.toname[0],result[i]);
							write(fd,&clientmsg,sizeof(clientmsg));
							printf("%s\n",clientmsg.toname[0]);
							usleep(1000);
						}
						break;
					}
				case GET_GROUP:
					{
						sprintf(sql,"select groupname from group_chat where name = '%s';",clientmsg.name);
						ret = sqlite3_get_table(db,sql,&result,&nrows,&ncols,&errmsg);
						if(ret != SQLITE_OK)
						{
							printf("get_table:%s\n",errmsg);
							break;	
						}

						int i;
						for(i = 1; i < nrows + 1; i++)
						{
							clientmsg.action = GET_GROUP;
							strcpy(clientmsg.toname[0],result[i]);
							write(fd,&clientmsg,sizeof(clientmsg));
							usleep(100);
						}
						break;
					}
				case GET_GROUP_MEMBERS:
					{
						//群主置首
						sprintf(sql,"select name from group_chat where groupname = '%s' order by status desc;",clientmsg.toname[0]);
						ret = sqlite3_get_table(db,sql,&result,&nrows,&ncols,&errmsg);
						if(ret != SQLITE_OK)
						{
							printf("get_table:%s\n",errmsg);
							break;	
						}

						int nrows2,ncols2;
						char ** result2;  //再查询在线表

						int i;
						for(i = 1; i < nrows + 1; i++)
						{
							strcpy(clientmsg.name,result[i]);
							printf("group:%s\n",clientmsg.name);
							sprintf(sql,"select * from online where name = '%s' and status = 1;",result[i]);	
							ret = sqlite3_get_table(db,sql,&result2,&nrows2,&ncols2,&errmsg);
							if(ret != SQLITE_OK)
							{
								printf("get_table:%s\n",errmsg);
								break;	
							}

							if(nrows2 == 1)
							{
								clientmsg.online[0] = 1;
							}
							else
							{
								clientmsg.online[0] = 0;
							}

							//写到客户端群成员链表
							clientmsg.action = GET_GROUP_MEMBERS;
							write(fd,&clientmsg,sizeof(clientmsg));
							printf("get_members:%s\n",clientmsg.name);
							//usleep(100);
						}

						//最后发送一个空数据 代表已经传输成功
						bzero(clientmsg.name,sizeof(clientmsg.name));
						clientmsg.action = GET_GROUP_MEMBERS;
						write(fd,&clientmsg,sizeof(clientmsg));

						break;
					}
				case GET_BAN:       //群禁言人  仅管理员
					{
						sprintf(sql,"select name from group_chat where groupname = '%s' and ban_talk = 1;",clientmsg.toname[0]);
						ret = sqlite3_get_table(db,sql,&result,&nrows,&ncols,&errmsg);
						if(ret != SQLITE_OK)
						{
							printf("get_table:%s\n",errmsg);
							break;	
						}

						int i;
						for(i = 1; i < nrows + 1; i++)
						{
							strcpy(clientmsg.name,result[i]);
							write(fd,&clientmsg,sizeof(clientmsg));
						}
						break;
					}
				case SET_BAN:
					{
						sprintf(sql,"update group_chat set ban_talk = 1 where groupname = '%s' and name = '%s';",clientmsg.toname[0],clientmsg.name);
						sqlite3_exec(db,sql,NULL,NULL,NULL);
						break;
					}
				case CANCEL_BAN:
					{
						sprintf(sql,"update group_chat set ban_talk = 0 where groupname = '%s' and name = '%s';",clientmsg.toname[0],clientmsg.name);
						sqlite3_exec(db,sql,NULL,NULL,NULL);
						break;
					}

				case CHAT_GROUP:   // 群聊
					{
						//首先查看有无被禁言
						sprintf(sql,"select * from group_chat where groupname = '%s' and name = '%s' and ban_talk = 1;",clientmsg.toname[0],clientmsg.name);
						ret = sqlite3_get_table(db,sql,&result,&nrows,&ncols,&errmsg);
						if(ret != SQLITE_OK)
						{
							printf("get_table:%s\n",errmsg);
							break;	
						}
						if(nrows > 0)
						{
							clientmsg.action = NOTIFY_BAN;
							write(fd,&clientmsg,sizeof(clientmsg));
							break;
						}
						//自动将时间插入到表中
						sprintf(sql,"insert into chat_record_group (groupname,name,msg_record) values('%s','%s','%s');",clientmsg.toname[0],clientmsg.name,clientmsg.msg);
						sqlite3_exec(db,sql,NULL,NULL,NULL);

						strcpy(clientmsg.chat_time,get_current_time());

						sprintf(sql,"select name from group_chat where groupname = '%s' and name != '%s';",clientmsg.toname[0],clientmsg.name);
						ret = sqlite3_get_table(db,sql,&result,&nrows,&ncols,&errmsg);
						if(ret != SQLITE_OK)
						{
							printf("get_table:%s\n",errmsg);
							break;	
						}

						char ** result2;
						int nrows2,ncols2;

						//发送群消息
						int i;
						for(i = 1; i < nrows + 1; i++)
						{
							sprintf(sql,"select fd from online where name = '%s';",result[i]);
							ret = sqlite3_get_table(db,sql,&result2,&nrows2,&ncols2,&errmsg);
							if(ret != SQLITE_OK)
							{
								printf("get_table:%s\n",errmsg);
								break;	
							}
							if(nrows2 == 1)
							{
								int to_fd = atoi(result2[1]);
								write(to_fd,&clientmsg,sizeof(clientmsg));
							}
							else
							{
								//群成员不在线 未读消息
								//	sprintf(sql,"s");
							}
						}
						break;
					}
				case GET_RECENT_GROUP_CHAT_RECORD:
					{
						//查询此群的最近8条记录
						sprintf(sql,"select msg_record,chat_time,name from chat_record_group where groupname = '%s';",clientmsg.toname[0]);
						ret = sqlite3_get_table(db,sql,&result,&nrows,&ncols,&errmsg);
						if(ret != SQLITE_OK)
						{
							printf("get_table:%s\n",errmsg);
							break;	
						}
						//查询最近8条消息 不足8条从第一条开始读
						int start_rgr = nrows - 8 + 1;
						if(start_rgr < 1)
						{
							start_rgr = 1;
						}
						int i;	
						for(i = start_rgr * ncols; i < (nrows + 1) * ncols; i++ )
						{
							clientmsg.action = GET_RECENT_GROUP_CHAT_RECORD;
							strcpy(clientmsg.msg,result[i]);
							i++;
							strcpy(clientmsg.chat_time,result[i]);
							i++;
							strcpy(clientmsg.name,result[i]);
							write(fd,&clientmsg,sizeof(clientmsg));
						}
						break;
					}	
				case GET_PRE_FOUR_FRIENDS:
				case GET_NEXT_FOUR_FRIENDS:    //显示下面一页的四个联系人
				case GET_FIRST_FOUR_FRIENDS:  //只显示前四个联系人 在打开下拉菜单时执行
					{
						bzero(sql,sizeof(sql));
						sprintf(sql,"select name,sig from usertable where name != '%s';",clientmsg.name);
						printf("%s\n",sql);
						ret = sqlite3_get_table(db,sql,&result,&nrows,&ncols,&errmsg);
						if(ret != SQLITE_OK)
						{
							printf("get_table:%s\n",errmsg);
							break;	
						}

						//初始化工作
						int i;
						for(i = 0; i < 4; i++)
						{
							bzero(clientmsg.sigs[i],sizeof(clientmsg.sigs[i]));
							bzero(clientmsg.toname[i],sizeof(clientmsg.toname[i]));
							clientmsg.online[i] = 0;         
							clientmsg.has_undomsg[i] = 0;
						}

						if(clientmsg.action == GET_NEXT_FOUR_FRIENDS)
						{

							page++;
							if(page > (nrows - 1) /4)    //到了最后一页 仍然查询最后一页
							{
								page = (nrows - 1) / 4;
							}
							for(i  = page * pagesize * ncols + ncols; i < (page + 1) * pagesize * ncols + ncols; i++ )
							{
								if(i >= (nrows + 1) * ncols)
								{
									break;
								}
								else
								{

									strcpy(clientmsg.toname[(i - page * pagesize * ncols - ncols) /2],result[i]);
									if(result[++i] != NULL)  //存储个性签名 
									{
										strcpy(clientmsg.sigs[(i - page * pagesize * ncols - ncols) / 2] ,result[i]);
									}


								}

							}

						}
						else if(clientmsg.action == GET_PRE_FOUR_FRIENDS)
						{
							page--;
							if(page < 0)
							{
								page = 0;
							}
							for(i  = page * pagesize * ncols + ncols; i < (page + 1) * pagesize * ncols + ncols; i++ )
							{
								if(i >= (nrows + 1) * ncols)
								{
									break;
								}
								else
								{
									strcpy(clientmsg.toname[(i - page * pagesize * ncols - ncols) / 2],result[i]);
									if(result[++i] != NULL)
									{
										strcpy(clientmsg.sigs[(i - page * pagesize * ncols - ncols) / 2],result[i]);
									}

								}

							}

						}
						else if(clientmsg.action == GET_FIRST_FOUR_FRIENDS)
						{
							page = 0;
							for(i = ncols; i < 5 * ncols; i++ )
							{
								if(i >= (nrows + 1) * ncols)
								{
									break;
								}
								else
								{
									printf("result[%d] = %s\n",i,result[i]);
									strcpy(clientmsg.toname[(i - ncols) / 2],result[i]);  //  保存每个人名字
									if(result[++i] != NULL)
									{
										strcpy(clientmsg.sigs[(i - ncols) / 2],result[i]);//个性签名
										printf("result[%d] = %s\n",i,result[i]);
									}
								}
							}
						}

						i = 0;
						while(i < 4 && strlen(clientmsg.toname[i]) > 0)
						{
							bzero(sql,sizeof(sql));
							sprintf(sql,"select * from online where name = '%s' and status = 1",clientmsg.toname[i]);

							ret = sqlite3_get_table(db,sql,&result,&nrows,&ncols,&errmsg);
							if(ret != SQLITE_OK)
							{
								printf("get_table:%s\n",errmsg);
								break;	
							}

							if(nrows >= 1)
							{
								clientmsg.online[i] = 1;    //在线
							}
							else
							{
								clientmsg.online[i] = 0;    //离线或隐身
							}

							sprintf(sql,"select * from chat_record where name = '%s' and toname = '%s' and read = 0",
									clientmsg.toname[i],clientmsg.name);   //判断返回页面的4个好友是否有未读消息
							ret = sqlite3_get_table(db,sql,&result,&nrows,&ncols,&errmsg);
							if(ret != SQLITE_OK)
							{
								printf("get_table:%s\n",errmsg);
								break;	
							}
							if(nrows >= 1)
							{
								clientmsg.has_undomsg[i] = 1;    //有未读消息
							}
							else
							{
								clientmsg.has_undomsg[i] = 0;   
							}

							i++;
						}
						write(fd,&clientmsg,sizeof(clientmsg));
						sqlite3_free_table(result);
						break;
					}
				case CREATE_GROUP:
					{
						bzero(sql,sizeof(sql));
						sprintf(sql,"insert into group_chat (groupname,name,status)values ('%s','%s',1);",clientmsg.toname[0],clientmsg.name);
						sqlite3_exec(db,sql,NULL,NULL,NULL);
						break;
					}
				case LEAVE_GROUP:
					{
						sprintf(sql,"delete from group_chat where groupname = '%s' and name = '%s';",clientmsg.toname[0],clientmsg.name);
						sqlite3_exec(db,sql,NULL,NULL,NULL);

						sprintf(sql,"select fd from online where name in (select name from group_chat where groupname = '%s');",clientmsg.toname[0]);
						ret = sqlite3_get_table(db,sql,&result,&nrows,&ncols,&errmsg);
						if(ret != SQLITE_OK)
						{
							printf("get_table:%s\n",errmsg);
							break;	
						}

						int i;
						for(i = 1; i < nrows + 1; i++)
						{
							clientmsg.action = NOTIFY_UPDATE_GROUP_MEMBER_LIST;
							int to_fd = atoi(result[i]);
							write(to_fd,&clientmsg,sizeof(clientmsg));
						}

						break;
					}
				case REMOVE_GROUP_MEMBER:      //踢人
					{
						if(strlen(clientmsg.name) > 0)
						{
							bzero(sql,sizeof(sql));
							sprintf(sql,"delete from group_chat where groupname = '%s' and name = '%s';",clientmsg.toname[0],clientmsg.name);
							sqlite3_exec(db,sql,NULL,NULL,NULL);
						}

						//通知其他群成员跟信群列表
						else
						{
							sprintf(sql,"select fd from online where name in (select name from group_chat where groupname = '%s' and status != 1);",clientmsg.toname[0]);
							ret = sqlite3_get_table(db,sql,&result,&nrows,&ncols,&errmsg);
							if(ret != SQLITE_OK)
							{
								printf("get_table:%s\n",errmsg);
								break;	
							}

							int i;
							for(i = 1; i < nrows + 1; i++)
							{
								clientmsg.action = NOTIFY_UPDATE_GROUP_MEMBER_LIST;
								int to_fd = atoi(result[i]);
								write(to_fd,&clientmsg,sizeof(clientmsg));
							}



						}
						break;
					}
				case GET_NOT_IN_GROUP_USERS:  //获取不再本群中的联系人  用来拉人
					{
						sprintf(sql,"select name from usertable where name not in (select name from group_chat where groupname = '%s');",clientmsg.toname[0]);
						ret = sqlite3_get_table(db,sql,&result,&nrows,&ncols,&errmsg);
						if(ret != SQLITE_OK)
						{
							printf("get_table:%s\n",errmsg);
							break;	
						}

						int i;
						for(i = 1; i < nrows + 1; i++)
						{
							clientmsg.action = GET_NOT_IN_GROUP_USERS;
							strcpy(clientmsg.name,result[i]);
							write(fd,&clientmsg,sizeof(clientmsg));
						}
						break;
					}
				case ADD_GROUP_MEMBERS:
					{
						if(strlen(clientmsg.name) > 0)
						{
							sprintf(sql,"insert into group_chat (groupname,name) values ('%s','%s');",clientmsg.toname[0],clientmsg.name);
							sqlite3_exec(db,sql,NULL,NULL,NULL);
						}

						//通知其他群成员跟信群列表
						else
						{
							sprintf(sql,"select fd from online where name in (select name from group_chat where groupname = '%s' and status != 1);",clientmsg.toname[0]);
							ret = sqlite3_get_table(db,sql,&result,&nrows,&ncols,&errmsg);
							if(ret != SQLITE_OK)
							{
								printf("get_table:%s\n",errmsg);
								break;	
							}

							int i;
							for(i = 1; i < nrows + 1; i++)
							{
								clientmsg.action = NOTIFY_UPDATE_GROUP_MEMBER_LIST;
								int to_fd = atoi(result[i]);
								write(to_fd,&clientmsg,sizeof(clientmsg));
							}



						}
						break;
					}
				case CHECK_GROUP_NAME:
					{
						//将每组分组
						sprintf(sql,"select * from group_chat where groupname = '%s' group by groupname;",clientmsg.toname[0]);
						ret = sqlite3_get_table(db,sql,&result,&nrows,&ncols,&errmsg);
						if(ret != SQLITE_OK)
						{
							printf("get_table:%s\n",errmsg);
							break;	
						}
						if(nrows != 0)
						{
							clientmsg.action = GROUP_NAME_ERROR;
						}
						else
						{
							clientmsg.action = GROUP_NAME_OK;
						}
						write(fd,&clientmsg,sizeof(clientmsg));
						break;
					}
				case INSERT_GROUP:
					{
						sprintf(sql,"insert into group_chat (groupname,name) values('%s','%s');",clientmsg.toname[0],clientmsg.name);
						sqlite3_exec(db,sql,NULL,NULL,NULL);
						break;
					}
				case GET_GROUP_CREATE_TIME:
					{
						//加入时间
						sprintf(sql,"select time from group_chat where groupname = '%s' and name = '%s';",clientmsg.toname[0],clientmsg.name);
						ret = sqlite3_get_table(db,sql,&result,&nrows,&ncols,&errmsg);
						if(ret != SQLITE_OK)
						{
							printf("get_table:%s\n",errmsg);
							break;	
						}
						if(result[1] != NULL)
						{
							strcpy(clientmsg.toname[1],result[1]);
						}

						//查询群主和建立群时间
						sprintf(sql,"select name,time from group_chat where groupname = '%s' and status = 1;",clientmsg.toname[0]);
						ret = sqlite3_get_table(db,sql,&result,&nrows,&ncols,&errmsg);
						if(ret != SQLITE_OK)
						{
							printf("get_table:%s\n",errmsg);
							break;	
						}

						//群主
						if(result[2] != NULL)
						{
							strcpy(clientmsg.name,result[2]);
						}
						//建群时间
						if(result[3] != NULL)
						{
							strcpy(clientmsg.chat_time,result[3]);
						}

						write(fd,&clientmsg,sizeof(clientmsg));
						break;
					}
				case GET_PRE_CHAT_RECORD:
				case GET_NEXT_CHAT_RECORD:  
				case GET_FIRST_CHAT_RECORD: 
					{
						bzero(sql,sizeof(sql));
						sprintf(sql,"select name,msg_record,chat_time from chat_record where (name = '%s' and toname = '%s') or (name = '%s' and toname = '%s');",clientmsg.name,clientmsg.toname[0],clientmsg.toname[0],clientmsg.name);
						ret = sqlite3_get_table(db,sql,&result,&nrows,&ncols,&errmsg);
						if(ret != SQLITE_OK)
						{
							printf("get_table:%s\n",errmsg);
							break;	
						}
						if(nrows >= 1)
						{
							if(clientmsg.action == GET_NEXT_CHAT_RECORD)
							{
								start_cr = start_cr + 8;
								if(start_cr > nrows)   //若聊天记录已经从第一条开始  则上一页没有意义
								{
									start_cr -= 8;
									break;      
								}
								int i;	
								for(i = start_cr * ncols; i < (start_cr + 8) * ncols; i++ )
								{
									if(i >= (nrows + 1) * ncols)
									{
										break;
									}
									else
									{
										printf("name = %s ;msg = %s ;time = %s\n",result[i],result[i + 1],result[i + 2]);
										//		strcpy(clientmsg.toname[i - ncols],result[i]);
										strcpy(clientmsg.name,result[i]);       //发送方名字
										strcpy(clientmsg.msg,result[++i]);      //信息内容
										strcpy(clientmsg.chat_time,result[++i]); //聊天时间
										write(fd,&clientmsg,sizeof(clientmsg));
									}
								}

							}
							if(clientmsg.action == GET_FIRST_CHAT_RECORD)  //选择最后的8条信息  
							{

								start_cr = nrows - 8 + 1;
								if(start_cr < 1)
								{
									start_cr = 1;        //不足8行 则从第一行开始读
								}

								int i;	
								for(i = start_cr * ncols; i < (nrows + 1) * ncols; i++ )
								{
									if(i >= (nrows + 1) * ncols)
									{
										break;
									}
									else
									{
										printf("name = %s ;msg = %s ;time = %s\n",result[i],result[i + 1],result[i + 2]);
										//		strcpy(clientmsg.toname[i - ncols],result[i]);
										strcpy(clientmsg.name,result[i]);       //发送方名字
										strcpy(clientmsg.msg,result[++i]);
										strcpy(clientmsg.chat_time,result[++i]);
										write(fd,&clientmsg,sizeof(clientmsg));
									}
								}


							}
							else if(clientmsg.action == GET_PRE_CHAT_RECORD)
							{

								if(start_cr == 1)   //若聊天记录已经从第一条开始  则上一页没有意义
								{
									break;      
								}
								start_cr = start_cr - 8;
								if(start_cr < 1)
								{
									start_cr = 1;        //不足8行 则从第一行开始读
								}

								int i;	
								for(i = start_cr * ncols; i < (start_cr + 8) * ncols; i++ )
								{
									if(i >= (nrows + 1) * ncols)
									{
										break;
									}
									else
									{
										printf("name = %s ;msg = %s ;time = %s\n",result[i],result[i + 1],result[i + 2]);
										//		strcpy(clientmsg.toname[i - ncols],result[i]);
										strcpy(clientmsg.name,result[i]);       //发送方名字
										strcpy(clientmsg.msg,result[++i]);
										strcpy(clientmsg.chat_time,result[++i]);
										write(fd,&clientmsg,sizeof(clientmsg));
									}
								}
							}
						}
						break;
					}
				case GET_PRE_CHAT_RECORD_GROUP:
				case GET_NEXT_CHAT_RECORD_GROUP:  
				case GET_FIRST_CHAT_RECORD_GROUP: 
					{
						bzero(sql,sizeof(sql));
						sprintf(sql,"select name,msg_record,chat_time from chat_record_group where groupname = '%s';",clientmsg.toname[0]);
						ret = sqlite3_get_table(db,sql,&result,&nrows,&ncols,&errmsg);
						if(ret != SQLITE_OK)
						{
							printf("get_table:%s\n",errmsg);
							break;	
						}
						if(nrows >= 1)
						{
							if(clientmsg.action == GET_NEXT_CHAT_RECORD_GROUP)
							{
								start_cr_group = start_cr_group + 8;
								if(start_cr_group > nrows)   //若聊天记录已经从第一条开始  则上一页没有意义
								{
									start_cr_group -= 8;
									break;      
								}
								int i;	
								for(i = start_cr_group * ncols; i < (start_cr_group + 8) * ncols; i++ )
								{
									if(i >= (nrows + 1) * ncols)
									{
										break;
									}
									else
									{
										printf("name = %s ;msg = %s ;time = %s\n",result[i],result[i + 1],result[i + 2]);
										//		strcpy(clientmsg.toname[i - ncols],result[i]);
										strcpy(clientmsg.name,result[i]);       //发送方名字
										strcpy(clientmsg.msg,result[++i]);      //信息内容
										strcpy(clientmsg.chat_time,result[++i]); //聊天时间
										write(fd,&clientmsg,sizeof(clientmsg));
									}
								}

							}
							if(clientmsg.action == GET_FIRST_CHAT_RECORD_GROUP)  //选择最后的8条信息  
							{

								start_cr_group = nrows - 8 + 1;
								if(start_cr_group < 1)
								{
									start_cr_group = 1;        //不足8行 则从第一行开始读
								}
								int i;	
								for(i = start_cr_group * ncols; i < (nrows + 1) * ncols; i++ )
								{
									if(i >= (nrows + 1) * ncols)
									{
										break;
									}
									else
									{
										printf("name = %s ;msg = %s ;time = %s\n",result[i],result[i + 1],result[i + 2]);
										//		strcpy(clientmsg.toname[i - ncols],result[i]);
										strcpy(clientmsg.name,result[i]);       //发送方名字
										strcpy(clientmsg.msg,result[++i]);
										strcpy(clientmsg.chat_time,result[++i]);
										write(fd,&clientmsg,sizeof(clientmsg));
									}
								}


							}
							else if(clientmsg.action == GET_PRE_CHAT_RECORD_GROUP)
							{

								if(start_cr_group == 1)   //若聊天记录已经从第一条开始  则上一页没有意义
								{
									break;      
								}
								start_cr_group = start_cr_group - 8;
								if(start_cr_group < 1)
								{
									start_cr_group = 1;        //不足8行 则从第一行开始读
								}

								int i;	
								for(i = start_cr_group * ncols; i < (start_cr_group + 8) * ncols; i++ )
								{
									if(i >= (nrows + 1) * ncols)
									{
										break;
									}
									else
									{
										printf("name = %s ;msg = %s ;time = %s\n",result[i],result[i + 1],result[i + 2]);
										//		strcpy(clientmsg.toname[i - ncols],result[i]);
										strcpy(clientmsg.name,result[i]);       //发送方名字
										strcpy(clientmsg.msg,result[++i]);
										strcpy(clientmsg.chat_time,result[++i]);
										write(fd,&clientmsg,sizeof(clientmsg));
									}
								}
							}
						}
						break;
					}

				case UPDATE_SIG:  //更新个性签名
					{
						bzero(sql,sizeof(sql));
						sprintf(sql,"update usertable set sig = '%s' where name = '%s';",clientmsg.msg,clientmsg.name);
						ret = sqlite3_exec(db,sql,NULL,NULL,NULL);

						//只要有更新消息 发送给其他在线好友本人的签名已经改变  要刷新
						sprintf(sql,"select fd from online where fd != %d;",fd);
						ret = sqlite3_get_table(db,sql,&result,&nrows,&ncols,&errmsg);
						if(ret != SQLITE_OK)
						{
							printf("get table:%s",errmsg);
							break;
						}									

						int i;
						int to_fd;
						clientmsg.action = NOTIFY_UPDATE_SIG;
						for(i = 1; i <= nrows; i++)
						{	
							to_fd = atoi(result[i]);
							printf("to_fd update sig = %d,msg = %s\n",to_fd,clientmsg.msg);
							write(to_fd,&clientmsg,sizeof(clientmsg));
						}

						break;
					}
				case BEGIN_TRANSFER_FILE:
				case CHAT_PRIVATE:
					{
						bzero(sql,sizeof(sql));

						//首先看发送人在不在被发送人的黑名单中 若在 则存入消息记录 并 直接Break;
						sprintf(sql,"select * from deny_msg where self_name = '%s' and toname = '%s';",clientmsg.toname[0],clientmsg.name);
						ret = sqlite3_get_table(db,sql,&result,&nrows,&ncols,&errmsg);
						if(ret != SQLITE_OK)
						{
							printf("get table:%s\n",errmsg);
						}	

						if(nrows >= 1)  //在黑名单中
						{
							if(clientmsg.action == BEGIN_TRANSFER_FILE)
							{
								sprintf(sql,"insert into chat_record (name,toname,msg_record,status) values ('%s','%s','%s',0);",clientmsg.name,clientmsg.toname[0],clientmsg.msg); //0 代表是文件消息
							}
							else
							{
								sprintf(sql,"insert into chat_record (name,toname,msg_record) values ('%s','%s','%s');",clientmsg.name,clientmsg.toname[0],clientmsg.msg); 
							}
							ret = sqlite3_exec(db,sql,NULL,NULL,&errmsg);   //存入消息记录
							if(ret != SQLITE_OK)
							{
								printf("insert error!:%s\n",errmsg);
							}

							break;
						}
						if(clientmsg.action == BEGIN_TRANSFER_FILE)
						{
							sprintf(sql,"insert into send_file(name,toname,filename) values('%s','%s','%s')",clientmsg.name,clientmsg.toname[0],clientmsg.msg);
							ret = sqlite3_exec(db,sql,NULL,NULL,&errmsg);
							if(ret != SQLITE_OK)
							{
								printf("insert into send_file error:%s\n",errmsg);
							}
							char temp[64];
							strcpy(temp,clientmsg.msg);
							sprintf(clientmsg.msg,"###send you a file: %s###",temp);
						}

						sprintf(sql,"select fd from online where name = '%s';",clientmsg.toname[0]);
						printf("from:%s,to:%s",clientmsg.name,clientmsg.toname[0]);
						ret = sqlite3_get_table(db,sql,&result,&nrows,&ncols,&errmsg);
						if(ret != SQLITE_OK)
						{
							printf("get table:%s\n",errmsg);
						}	

						if(nrows >= 1)           //联系人在线
						{
							int to_fd = atoi(result[1]);
							if(my_isprefix("###receive/",clientmsg.msg) == 1)  //代表接收文件消息
							{
								char destfile[64]; //目的文件
								char srcfile[64]; //源文件
								printf("%s\n",clientmsg.msg);
								strtok(clientmsg.msg,"#/");   // 先将###receive取出来
								strcpy(srcfile,strtok(NULL,"#/"));
								strcpy(destfile,strtok(NULL,"#/"));
								printf("src = %s dest = %s\n",srcfile,destfile);					
								if(strlen(srcfile) > 0 && strlen(destfile) > 0)
								{
									//发送文件的name是toname[0]
									sprintf(sql,"select * from send_file where name = '%s' and toname = '%s' and filename = '%s';",clientmsg.toname[0],clientmsg.name,srcfile);
									ret = sqlite3_get_table(db,sql,&result,&nrows,&ncols,&errmsg);
									if(ret != SQLITE_OK)
									{
										printf("get table:%s\n",errmsg);
									}	
									if(nrows > 0)
									{
										clientmsg.action = START_RECEIVE_FILE;   //开始接收文件
										strcpy(clientmsg.msg,destfile);
										write(fd,&clientmsg,sizeof(clientmsg));    //通知客户端受文件

										usleep(1000);

										clientmsg.action = START_SEND_FILE;     //通知发送文件客户端开始发送文件
										strcpy(clientmsg.msg,srcfile);
										write(to_fd,&clientmsg,sizeof(clientmsg));
										printf("src = %s dest = %s\n",srcfile,destfile);
									}
									else
									{
										clientmsg.action = START_RECEIVE_FILE_ERROR;
										write(fd,&clientmsg,sizeof(clientmsg));
									}
								}
								break;
							}
							if(clientmsg.action == BEGIN_TRANSFER_FILE)
							{
								sprintf(sql,"insert into chat_record (name,toname,msg_record,status) values ('%s','%s','%s',0);",clientmsg.name,clientmsg.toname[0],clientmsg.msg); //0 代表是文件消息
							}
							else
							{
								sprintf(sql,"insert into chat_record (name,toname,msg_record) values ('%s','%s','%s');",clientmsg.name,clientmsg.toname[0],clientmsg.msg);
							}
							ret = sqlite3_exec(db,sql,NULL,NULL,&errmsg);   //存入消息记录
							if(ret != SQLITE_OK)
							{
								printf("insert error!:%s\n",errmsg);
							}

							bzero(clientmsg.chat_time,sizeof(clientmsg.chat_time));
							strcpy(clientmsg.chat_time,get_current_time());
							clientmsg.action = CHAT_PRIVATE;
							write(to_fd,&clientmsg,sizeof(clientmsg));	
							printf("write end name = %s,toname = %s msg = %s\n",clientmsg.name,clientmsg.toname[0],clientmsg.msg);
						}
						else
						{
							if(my_isprefix("###receive",clientmsg.msg) == 1)  //代表接收文件消息  对方不在线
							{
								clientmsg.action = RECEIVE_NOT_ONLINE;
								write(fd,&clientmsg,sizeof(clientmsg));
								break;
							}
							bzero(sql,sizeof(sql));
							if(clientmsg.action == BEGIN_TRANSFER_FILE)
							{
								sprintf(sql,"insert into chat_record (name,toname,msg_record,status,read) values ('%s','%s','%s',0,0);",clientmsg.name,clientmsg.toname[0],clientmsg.msg); //0 代表是文件消息
							}

							else
							{
								sprintf(sql,"insert into chat_record (name,toname,msg_record,read) values ('%s','%s','%s',0);",clientmsg.name,clientmsg.toname[0],clientmsg.msg); //联系人不在线 直接存入消息记录中 并且状态位置0 表示未读  离线消息						
							}
							ret = sqlite3_exec(db,sql,NULL,NULL,&errmsg);   //存入消息记录
							if(ret != SQLITE_OK)
							{
								printf("insert serror!:%s\n",errmsg);
							}
						}
						break;
					}
#if 1
				case SENDING_FILE:
					{
						sprintf(sql,"select fd from online where name = '%s';",clientmsg.toname[0]);
						printf("from:%s,to:%s file %d\n",clientmsg.name,clientmsg.toname[0],clientmsg.hasquestion);
						ret = sqlite3_get_table(db,sql,&result,&nrows,&ncols,&errmsg);
						if(ret != SQLITE_OK)
						{
							printf("get table:%s\n",errmsg);
						}	

						if(nrows >= 1)           //联系人在线
						{
							printf("to_fd file = %s writesize = %d\n",result[1],clientmsg.hasquestion);
							int to_fd = atoi(result[1]);
							clientmsg.action = RECEVING_FILE;
							write(to_fd,&clientmsg,sizeof(clientmsg));	
						}

						break;
					}
#endif
				case SEARCH_FRIENDS:
					{

						sprintf(sql,"select sig from usertable where name = '%s';",clientmsg.toname[0]);
						ret = sqlite3_get_table(db,sql,&result,&nrows,&ncols,&errmsg);
						if(ret != SQLITE_OK)
						{
							printf("get table:%s\n",errmsg);
						}	

						if(nrows == 1)  //查找成功
						{
							bzero(clientmsg.sigs[0],sizeof(clientmsg.sigs[0]));
							if(result[1] != NULL)
							{
								strcpy(clientmsg.sigs[0],result[1]);  //获取签名
							}
						}
						else
						{
							clientmsg.action = SEARCH_ERROR;
							write(fd,&clientmsg,sizeof(clientmsg));
							break;
						}

						sprintf(sql,"select * from online where name = '%s' and status = 1;",clientmsg.toname[0]);

						ret = sqlite3_get_table(db,sql,&result,&nrows,&ncols,&errmsg);
						if(ret != SQLITE_OK)
						{
							printf("get table:%s\n",errmsg);
						}	

						if(nrows == 1)  //查找成功
						{
							clientmsg.online[0] = 1;   //在线
						}
						else
						{
							clientmsg.online[0] = 0;  //离线或者隐身
						}

						write(fd,&clientmsg,sizeof(clientmsg));
						break;
					}
				case MSG_NOREAD: //发出消息后 对应窗口没有打开 消息未接受到 对应字段必须完全匹配才能标记未读
					{
						sprintf(sql,"update chat_record set read = 0 where name = '%s' and toname = '%s' and msg_record = '%s' and chat_time = '%s';",clientmsg.name,clientmsg.toname[0],clientmsg.msg,clientmsg.chat_time);

						printf("%s\n",sql);
						ret = sqlite3_exec(db,sql,NULL,NULL,&errmsg);
						if(ret != SQLITE_OK)
						{
							printf("update error!:%s\n",errmsg);
						}
						break;
					}
				case MSG_READ:  //客户端打开聊天窗口  读取未读消息和对应的发送时间
					{
						sprintf(sql,"select msg_record,chat_time from chat_record where name = '%s' and toname = '%s' and read = 0;",clientmsg.toname[0],clientmsg.name);
						ret = sqlite3_get_table(db,sql,&result,&nrows,&ncols,&errmsg);
						if(ret != SQLITE_OK)
						{
							printf("get table:%s\n",errmsg);
						}	

						if(nrows >= 1)
						{
							int start_x;   //读取消息记录的初始行号   因为最多读4行
							start_x = nrows - 4 + 1;
							if(start_x < 1)
							{
								start_x = 1;        //不足5行 则从第一行开始读
							}

							int i;
							for(i = start_x * ncols; i < (nrows + 1) * ncols; i++)
							{
								strcpy(clientmsg.msg,result[i]);
								strcpy(clientmsg.chat_time,result[++i]);
								write(fd,&clientmsg,sizeof(clientmsg));
							}


							sprintf(sql,"update chat_record set read = 1 where name = '%s' and toname = '%s';",clientmsg.toname[0],clientmsg.name);  //已读取未读消息 置相应位
							printf("%s\n",sql);
							ret = sqlite3_exec(db,sql,NULL,NULL,&errmsg);
							if(ret != SQLITE_OK)
							{
								printf("update error!:%s\n",errmsg);
							}	
						}		
						break;
					}
				case SEEK_UNREAD:  //刚登录或者退出好友列表展开时判断是否还有未读消息
					{
						sprintf(sql,"select * from chat_record where toname = '%s' and read = 0;",clientmsg.name);
						ret = sqlite3_get_table(db,sql,&result,&nrows,&ncols,&errmsg);
						if(ret != SQLITE_OK)
						{
							printf("get table:%s\n",errmsg);
						}	

						if(nrows >= 1)  //有未读消息  发送给客户端  客户端接受到此消息后 闪烁我的好友
						{
							write(fd,&clientmsg,sizeof(clientmsg));
						}

						break;
					}
				case BACK_ONLINE:
				case HIDING:
					{
						bzero(sql,sizeof(sql));
						if(clientmsg.action == HIDING)
						{
							sprintf(sql,"update online set status = 0 where name = '%s';",clientmsg.name);
							clientmsg.action = NOTIFY_LEAVE_ONLINE;                 //通知其他在线客户已经下线

						}
						else
						{
							sprintf(sql,"update online set status = 1 where name = '%s';",clientmsg.name);
							printf("update sql = %s\n",sql);
							clientmsg.action = NOTIFY_OTHER;
						}
						ret = sqlite3_exec(db,sql,NULL,NULL,NULL);
						if(ret != SQLITE_OK)
						{
							printf("error update:%s\n",errmsg);
						}

						sprintf(sql,"select fd from online where fd != %d;",fd);
						ret = sqlite3_get_table(db,sql,&result,&nrows,&ncols,&errmsg);
						if(ret != SQLITE_OK)
						{
							printf("get table:%s",errmsg);
							break;
						}									

						int i;
						int to_fd;
						for(i = 1; i <= nrows; i++)
						{	
							to_fd = atoi(result[i]);
							printf("to_fd = %d\n",to_fd);
							write(to_fd,&clientmsg,sizeof(clientmsg));
						}

						break;
					}
				case ADD_TO_BLACKLIST:
					{
						bzero(sql,sizeof(sql));
						sprintf(sql,"insert into deny_msg values('%s','%s');",clientmsg.name,clientmsg.toname[0]);
						ret = sqlite3_exec(db,sql,NULL,NULL,&errmsg);
						if(ret != SQLITE_OK)
						{
							printf("insert error:%s\n",errmsg);
						}
						break;
					}
				case LEAVE_ONLINE:
					{
						printf("%s leave online\n",clientmsg.name);
						bzero(sql,sizeof(sql));
						sprintf(sql,"delete from online where name = '%s';",clientmsg.name);
						printf("sql = %s\n",sql);
						ret = sqlite3_exec(db,sql,NULL,NULL,&errmsg);
						if(ret != SQLITE_OK)
						{
							printf("error delete from online:%s\n",errmsg);
						}

						clientmsg.action = NOTIFY_LEAVE_ONLINE;                 //通知其他在线客户已经下线
						sprintf(sql,"select fd from online where fd != %d;",fd);
						ret = sqlite3_get_table(db,sql,&result,&nrows,&ncols,&errmsg);
						if(ret != SQLITE_OK)
						{
							printf("get table:%s",errmsg);
							break;
						}									

						int i;
						int to_fd;
						for(i = 1; i <= nrows; i++)
						{
							to_fd = atoi(result[i]);
							printf("to_fd = %d\n",to_fd);
							write(to_fd,&clientmsg,sizeof(clientmsg));
						}
						break;
					}

			}
		}
	}
}

int main(int argc,char * argv[])
{
	int sockfd;
	int connect_fd;
	int sin_size;
	int n_read;
	int opt = 1;  //套接字选项

	struct sockaddr_in server_addr;
	struct sockaddr_in client_addr;

	if((sockfd = socket(AF_INET,SOCK_STREAM,0)) == -1)
	{
		ERR_HANDLE("socket");
	}

	bzero(&server_addr,sizeof(struct sockaddr_in));	
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORTNUMBER);	
	server_addr.sin_addr.s_addr = inet_addr("192.168.1.230");

	setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));
	if(bind(sockfd,(struct sockaddr *)(&server_addr),sizeof(struct sockaddr)) == -1)
	{
		ERR_HANDLE("bind");
	}

	if(listen(sockfd,10) == -1)
	{
		ERR_HANDLE("listen");
	}

	pthread_t id;

	/*服务器一直在accept*/
	while(1)
	{
		sin_size = sizeof(struct sockaddr_in);
		if((connect_fd = accept(sockfd,(struct sockaddr *)(&client_addr),&sin_size)) == -1)
		{
			ERR_HANDLE("accept:");
		}

		printf("Server get connection from %s\n",inet_ntoa(client_addr.sin_addr));

		pthread_create(&id,NULL,server_handle_msg,(void *)&connect_fd);


	}

	close(sockfd);

	return 0;


}

