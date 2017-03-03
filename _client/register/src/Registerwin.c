#include "../../include/include.h"
//#include "include.h"

//WINDOW * mainwin;
WINDOW * idwin;
WINDOW * passwdwin;
WINDOW * passwdrewin;
WINDOW * questionwin;
WINDOW * answerwin;
WINDOW * regwin;
WINDOW * introwin;
WINDOW * remindwin;
//WINDOW * noticewin;

//int reg = 0;

int judge_passwd(char * passwd);
#if 0
void * read_msg_reg(void * arg)
{
	int fd = *(int *)arg;
	int n_read;

	MSG msg;

	while(1)
	{
		bzero(&msg,sizeof(msg));


		if((n_read = read(fd,&msg,sizeof(MSG))) == -1)
		{

			perror("read\n");
			exit(-1);
		}
		else if(n_read == 0)
		{
		
			printf("server is closed");
			pthread_exit(NULL);
		}
		else
		{
			switch(msg.action)
			{
				case NAME_ERR:
					{
						mvwprintw(mainwin,4,51,"            ");
						mvwprintw(mainwin,4,51,"昵称已被注册");
						mvwprintw(mainwin,4,20,"                    ");
						wrefresh(mainwin);
						break;
					}
				case NAME_SUCCESS:
					{
						mvwprintw(mainwin,4,51,"            ");
						mvwprintw(mainwin,4,51,"昵称有效");
						wrefresh(mainwin);
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
#endif
int registerwin(int fd)
	//int main()
{
	int pos[7][2] = {{4,7},{6,7},{8,7},{10,7},{12,7},{16,23},{19,14}};
	int input_i = 0;
//	pthread_t id;

//	pthread_create(&id,NULL,read_msg_reg,(void *)&fd);
	int stay_li = 0;
	int pass_wd = 0;
	int hasquestion = 0;

	char nametmp[20];
	char passwd[20];
	char repasswd[20];
	char question[30];
	char answer[30];
	

	MSG msg;

	//清空msg
	bzero(msg.name,sizeof(msg.name));
	bzero(msg.passwd,sizeof(msg.passwd));
	bzero(msg.question,sizeof(msg.question));
	bzero(msg.answer,sizeof(msg.answer));
	bzero(nametmp,sizeof(nametmp));

	setlocale(LC_ALL,"");
	initscr();
	noecho();
	curs_set(0);
	keypad(stdscr,TRUE);

	mainwin = newwin(25,70,0,15);  //23lines 50cols
	box(mainwin,0,0);
	refresh();


	mvwaddstr(mainwin,1,5,"|注册帐号");
	wmove(mainwin,2,5);
	whline(mainwin,'-',60);

	mvwaddstr(mainwin,4,8,"昵 称:");

	idwin = derwin(mainwin,1,30,5,20);
	//	box(idwin,0,0);
	wborder(idwin,' ',' ',' ',0,' ',' ',' ',' ' );
	//	scrollok(mainwin,TRUE);
	wrefresh(idwin);

	mvwaddstr(mainwin,6,8,"密 码:");

	passwdwin = derwin(mainwin,1,30,7,20);
	//	box(passwdwin,' ',0);
	wborder(passwdwin,' ',' ',' ',0,' ',' ',' ',' ' );
	wrefresh(passwdwin);

	mvwaddstr(mainwin,8,8,"确 认 密 码:");

	passwdrewin = derwin(mainwin,1,30,9,20);
	wborder(passwdrewin,' ',' ',' ',0,' ',' ',' ',' ' );
	wrefresh(passwdrewin);

	mvwaddstr(mainwin,10,8,"密 保 问 题:");

	questionwin = derwin(mainwin,1,30,11,20);
	wborder(questionwin,' ',' ',' ',0,' ',' ',' ',' ' );
	mvwprintw(mainwin,10,51,"(选填)");
	wrefresh(questionwin);

	mvwaddstr(mainwin,12,8,"答 案:");

	answerwin = derwin(mainwin,1,30,13,20);
	wborder(answerwin,' ',' ',' ',0,' ',' ',' ',' ' );
	mvwprintw(mainwin,12,51,"(选填)");
	wrefresh(answerwin);

	mvwprintw(mainwin,16,22,"[ ]相 关 服 务 条 款");

	regwin = derwin(mainwin,3,40,18,15);
	box(regwin,0,0);
	mvwprintw(regwin,1,13,"立 即 注 册");
	wrefresh(regwin);

	mvwprintw(mainwin,22,15,"^O^温馨提示: 忘记密码可以通过密保问题找回");

	mvwaddch(mainwin,pos[input_i][0],pos[input_i][1],'*');
	wrefresh(mainwin);

	introwin = newwin(5,30,(LINES - 5) / 2,(COLS - 30) / 2);
	box(introwin,0,0);
	mvwprintw(introwin,2,5,"SCOTT聊天室");
	mvwprintw(introwin,3,5,"版权所有:余杰");

	remindwin = newwin(5,40,(LINES - 5) / 2,(COLS - 40) / 2);
	box(remindwin,0,0);
	mvwprintw(remindwin,2,5,"请确认意查看服务条款");
	mvwprintw(remindwin,3,5,"再确认用户名和密码是否正确录入");

	noticewin = newwin(5,40,(LINES - 5) / 2,(COLS - 40) / 2);
	box(noticewin,0,0);
	mvwprintw(noticewin,2,5,"正在注册");
	mvwprintw(noticewin,3,5,"请稍候. . .");


	int ch;
	while(1)
	{
		ch = getch();
		switch(ch)
		{
			case KEY_UP:
				{
					if((input_i == 5 && stay_li == 1))
					{

					}
					else
					{
						mvwaddch(mainwin,pos[input_i][0],pos[input_i][1],' ');
					}
					input_i = input_i - 1;
					break;
				}
			case KEY_DOWN:
				{
					if((input_i == 5 && stay_li == 1))
					{

					}
					else
					{
						mvwaddch(mainwin,pos[input_i][0],pos[input_i][1],' ');
					}
					input_i = input_i + 1;
					break;
				}
			case KEY_LEFT:
				{	
					break;				
				}
			case KEY_RIGHT:
				{
					break;
				}
			case '\n':
				{
					switch (input_i)
					{
						case 0:
							{
								echo();
								bzero(nametmp,sizeof(nametmp));
								mvwprintw(mainwin,4,20,"                    ");
								wrefresh(mainwin);
								curs_set(1);
								//move(4,36);
								wmove(mainwin,4,21);

							//	scanw("%s",nametmp);
								wgetnstr(mainwin,nametmp,20);
								//	getch();
								//	refresh();
								mvwprintw(mainwin,4,21,"%s",nametmp);
								wrefresh(mainwin);
								curs_set(0);
								noecho();
								msg.action = CHECK_NAME;
								strcpy(msg.name,nametmp);	
								write(fd,&msg,sizeof(msg));
								break;
							}
						case 1:  //密码
							{
								pass_wd = 0;
								curs_set(1);
								move(6,36);

								noecho();  //不回显
								mvwprintw(mainwin,6,20,"                     ");
								wrefresh(mainwin);

								int i;
								int re;
								int re_judge_passwd;

								bzero(passwd,sizeof(passwd));

								for(i = 0; i < 20; i++)
								{
									re = getch();
									if(re != '\n')
									{
										passwd[i] = re;
										printw("*");

										re_judge_passwd = judge_passwd(passwd);
#if 1
										switch(re_judge_passwd)
										{
											case PASSWD_EASY:
												{
													mvwprintw(mainwin,6,51,"             ");
													mvwprintw(mainwin,6,51,"密码强度:简单");
													break;
												}
											case PASSWD_NORMAL:
												{
													mvwprintw(mainwin,6,51,"             ");
													mvwprintw(mainwin,6,51,"密码强度:一般");
													break;
												}
											case PASSWD_HARD:
												{
													mvwprintw(mainwin,6,51,"             ");
													mvwprintw(mainwin,6,51,"密码强度:高");
													break;
												}


										}
#endif
										mvwprintw(mainwin,6,21 + i,"*");
										wrefresh(mainwin);

									}
									else
									{
										break;
									}
								}

								curs_set(0);

								wrefresh(mainwin);
								refresh();

								break;
							}
						case 2: //确认密码
							{
								curs_set(1);
								move(8,36);

								noecho();  //不回显
								mvwprintw(mainwin,8,20,"                     ");
								wrefresh(mainwin);

								int i;
								int re;

								bzero(repasswd,sizeof(repasswd));

								for(i = 0; i < 20; i++)
								{
									re = getch();
									if(re != '\n')
									{
										repasswd[i] = re;
										printw("*");

										mvwprintw(mainwin,8,21 + i,"*");
										wrefresh(mainwin);
									}
									else
									{
										break;
									}
								}

								if(strcmp(passwd,repasswd) == 0)
								{
									mvwprintw(mainwin,8,51,"           ");
									mvwprintw(mainwin,8,51,"密码通过!");
									pass_wd = 1;
								}
								else
								{
									mvwprintw(mainwin,8,51,"           ");
									mvwprintw(mainwin,8,51,"密码不一致!");
								}
								curs_set(0);
								wrefresh(mainwin);
								refresh();

								break;
							}
						case 3:  //密保问题
							{
								echo();
								bzero(question,sizeof(question));
								mvwprintw(mainwin,10,20,"                    ");
								hasquestion = 0;
								wrefresh(mainwin);
								curs_set(1);
								wmove(mainwin,10,21);
								//scanw("%s",question);
								wgetnstr(mainwin,question,20);
								if(strlen(question) > 0)
								{
									hasquestion = 1;
								}
								mvwprintw(mainwin,10,21,"%s",question);
								curs_set(0);
								break; 
							}
						case 4:
							{
								echo();
#if 1
								bzero(answer,sizeof(answer));
								mvwprintw(mainwin,12,20,"                    ");
								wrefresh(mainwin);
								curs_set(1);
								wmove(mainwin,12,21);
								//scanw("%s",answer);
								wgetnstr(mainwin,answer,20);
								mvwprintw(mainwin,12,21,"%s",answer);
								curs_set(0);
								break; 
#endif

							}
						case 5:     //关于
							{
								stay_li = 1;
								touchwin(introwin);
								wrefresh(introwin);
								getch();
								touchwin(mainwin);
								break;
							}
						case 6:  //确认注册
							{
								if(strlen(msg.name) > 0 && pass_wd == 1 && stay_li == 1)
								{	
									msg.action = REG_USER;
									strcpy(msg.passwd,passwd);
									if(hasquestion == 1)
									{
										strcpy(msg.question,question);
										strcpy(msg.answer,answer);
									}
									msg.hasquestion = hasquestion;

									touchwin(noticewin);
									wrefresh(noticewin);
									int local_i = 0;
									int local_ret;
								

									while(local_i <= 6)
									{
										mvwprintw(noticewin,3,5,"                ");
										local_ret = local_i % 3;
										switch(local_ret)
										{
											case 0:
												{
													mvwprintw(noticewin,3,5,"请稍候...");
													break;
												}
											case 1:
												{
													mvwprintw(noticewin,3,5,"请稍候. . .");
													break;
												}
											case 2:
												{
													mvwprintw(noticewin,3,5,"请稍候.  .  .");
													break;
												}
										}

										local_i++;
										wrefresh(noticewin);
										usleep(800000);
										
									}
									//touchwin(mainwin);
									write(fd,&msg,sizeof(msg));
									usleep(80000);
		                         	getch();
#if 1
									if(reg == 0)
									{
										touchwin(mainwin);
									}
									else if(reg == 1)
									{
										wclear(mainwin);
										delwin(mainwin);
										clear();
									//	pthread_cancel(id);
										endwin();
										return BACK_TO_LOGIN;
									}
#endif

								}
								else
								{
									
									wrefresh(mainwin);

									touchwin(remindwin);
									//mvwprintw(remindwin,1,1,"%d\n",strlen(msg.name));
									wrefresh(remindwin);
									getch();
									touchwin(mainwin);
								}
								break;
							}
					}
					break;
				}
		}
		if(input_i < 0)
		{
			input_i = 0;
		}
		else if(input_i > 6)
		{
			input_i = 6;
		}


		mvwaddch(mainwin,pos[input_i][0],pos[input_i][1],'*');		
		wrefresh(mainwin);

	}


	wclear(mainwin);
	return 0;

}

