//#include "include.h"
#include "../../include/include.h"

void getpasswd(int fd)
{
	noecho();
	curs_set(0);

	mainwin_gpwd = newwin(25,70,0,15);
	box(mainwin_gpwd,0,0);

	touchwin(mainwin_gpwd);
	mvwprintw(mainwin_gpwd,4,10,"姓名");
	mvwprintw(mainwin_gpwd,7,10,"密保问题");
	mvwprintw(mainwin_gpwd,10,10,"答案");
	mvwprintw(mainwin_gpwd,13,10,"新密码");
	mvwprintw(mainwin_gpwd,16,10,"确认密码");

	WINDOW * namewin = derwin(mainwin_gpwd,3,40,3,20);
	box(namewin,0,0);
	wrefresh(namewin);

	questionwin = derwin(mainwin_gpwd,3,40,6,20);
	box(questionwin,0,0);
	wrefresh(questionwin);

	WINDOW * answerwin = derwin(mainwin_gpwd,3,40,9,20);
	box(answerwin,0,0);
	wrefresh(answerwin);

	/*WINDOW * notifypasswdwin = derwin(mainwin_gpwd,5,40,12,20);
	  box(notifypasswdwin,0,0);
	  wrefresh(notifypasswdwin);*/

	WINDOW * passwdwin = derwin(mainwin_gpwd,3,40,12,20);
	box(passwdwin,0,0);
	wrefresh(passwdwin);

	WINDOW * confirmpasswdwin = derwin(mainwin_gpwd,3,40,15,20);
	box(confirmpasswdwin,0,0);
	wrefresh(confirmpasswdwin);

	WINDOW * gowin = derwin(mainwin_gpwd,3,20,18,30);
	mvwprintw(gowin,1,8,"确认");
	box(gowin,0,0);
	wrefresh(gowin);

	int pos_passwd[][2] = {{1,67},{4,9},{10,9},{13,9},{16,9},{19,29}};
	int ch;
	int select_i = 1; 

	mvwprintw(mainwin_gpwd,1,68,"X");
	mvwprintw(mainwin_gpwd,pos_passwd[select_i][0],pos_passwd[select_i][1],"*");
	wrefresh(mainwin_gpwd);

	int leave = 0;
	char passwd[20];
	while(1)
	{
		ch = getch();
		mvwprintw(mainwin_gpwd,pos_passwd[select_i][0],pos_passwd[select_i][1]," ");
		switch(ch)
		{
			case KEY_UP:
			case KEY_LEFT:
				{
					select_i--;
					break;
				}
			case KEY_DOWN:
			case KEY_RIGHT:
				{
					select_i++;
					break;
				}
			case '\n':
				{
					if(select_i == 0)
					{
						leave = 1;
					}
					else if(select_i == 1)
					{
						curs_set(1);
						echo();
						mvwprintw(mainwin_gpwd,4,21,"%18s"," ");
						wrefresh(mainwin_gpwd);
						//move(4,36);
						wmove(mainwin_gpwd,4,21);
						//	scanw("%s",nametmp);
						wgetnstr(mainwin_gpwd,msg.name,20);
						mvwprintw(mainwin_gpwd,4,21,"%s",msg.name);
						//	wrefresh(mainwin);
						curs_set(0);
						noecho();
						msg.action = GET_QUESTION;
						write(fd,&msg,sizeof(msg));
					}
					else if(select_i == 2)
					{
						curs_set(1);
						echo();
						mvwprintw(mainwin_gpwd,10,21,"%18s"," ");
						wrefresh(mainwin_gpwd);
						//move(4,36);
						wmove(mainwin_gpwd,10,21);
						//	scanw("%s",nametmp);
						wgetnstr(mainwin_gpwd,msg.answer,20);
						mvwprintw(mainwin_gpwd,10,21,"%s",msg.answer);
						//	wrefresh(mainwin);
						curs_set(0);
						noecho();
					}
					else if(select_i == 3)
					{
						curs_set(1);

						noecho();  //不回显
						mvwprintw(mainwin_gpwd,13,21,"%18s"," ");
						wmove(mainwin_gpwd,13,21);
						wrefresh(mainwin_gpwd);

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
								//printw("*");

								re_judge_passwd = judge_passwd(passwd);
#if 1
								switch(re_judge_passwd)
								{
									case PASSWD_EASY:
										{
											mvwprintw(mainwin_gpwd,13,45,"             ");
											mvwprintw(mainwin_gpwd,13,45,"密码强度:简单");
											break;
										}
									case PASSWD_NORMAL:
										{
											mvwprintw(mainwin_gpwd,13,45,"             ");
											mvwprintw(mainwin_gpwd,13,45,"密码强度:一般");
											break;
										}
									case PASSWD_HARD:
										{
											mvwprintw(mainwin_gpwd,13,45,"             ");
											mvwprintw(mainwin_gpwd,13,45,"密码强度:高");
											break;
										}


								}
#endif
								mvwprintw(mainwin_gpwd,13,21 + i,"*");
								wrefresh(mainwin_gpwd);
							}
							else
							{
								break;
							}
						}

						curs_set(0);

						wrefresh(mainwin_gpwd);
						refresh();
					}
					else if(select_i == 4)
					{
						curs_set(1);
					
						noecho();  //不回显
						mvwprintw(mainwin_gpwd,16,21,"                     ");
						wmove(mainwin_gpwd,16,21);
						wrefresh(mainwin_gpwd);
						int i;
						int re;

						char repasswd[20];
						bzero(repasswd,sizeof(repasswd));

						for(i = 0; i < 20; i++)
						{
							re = getch();
							if(re != '\n')
							{
								repasswd[i] = re;
							//	printw("*");

								mvwprintw(mainwin_gpwd,16,21 + i,"*");
								wrefresh(mainwin_gpwd);
							}
							else
							{
								break;
							}
						}

						if(strcmp(passwd,repasswd) == 0)
						{
							mvwprintw(mainwin_gpwd,16,45,"           ");
							mvwprintw(mainwin_gpwd,16,48,"密码通过!");
							//pass_wd = 1;
						}
						else
						{
							mvwprintw(mainwin_gpwd,16,45,"           ");
							mvwprintw(mainwin_gpwd,16,45,"密码不一致!");
						}
						curs_set(0);
						wrefresh(mainwin_gpwd);
			//			refresh();

					}
					else if(select_i == 5)
					{
						msg.action = MODIFY_PASSWD;
						strcpy(msg.passwd,passwd);		
						write(fd,&msg,sizeof(msg));	
					}
					break;
				}

			default:
				{
					break;
				}	
		}

		if(select_i < 0)
		{
			select_i = 0;
		}
		if(select_i > 5)
		{
			select_i = 5;
		}
		if(leave == 1)
		{
			break;
		}
		mvwprintw(mainwin_gpwd,pos_passwd[select_i][0],pos_passwd[select_i][1],"*");
		wrefresh(mainwin_gpwd);
	}

	wclear(mainwin_gpwd);
	wrefresh(mainwin_gpwd);
	delwin(mainwin_gpwd);

}
