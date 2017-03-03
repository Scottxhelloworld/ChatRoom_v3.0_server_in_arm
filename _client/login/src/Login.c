#include "../../include/include.h"

int tcp_client(char * arg);

//WINDOW * mainwin;
WINDOW * uppad;
WINDOW * facepad;
WINDOW * idpad;
WINDOW * pwdpad;
WINDOW * login;
WINDOW * remindwin_l;

int pos[8][2] = {{1,64},{10,24},{10,57},{12,24},{12,57},{14,28},{14,41},{17,16}};
//int g_flag;
#if 0
void * read_msg_login(void * arg)
{
	int fd = *(int *)arg;
	int n_read;

	MSG msg;
#if 1
	while(1)
	{
		bzero(&msg.name,sizeof(msg.name));
		bzero(&msg.passwd,sizeof(msg.passwd));

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
				default:
					{
						break;
					}
			}
		}
	}
#endif
}
#endif
//int main(int argc,char ** argv)
int Login(int fd)
{


	int input_i = 1;
	int i;
	int re;
	char idtmp[20];
	char passwordtmp[20];
	int edit_id = 0;
	int edit_password = 0;
	int leave = 0;
	int stay_repwd = 0;
	int stay_regauto = 0;
	int ret_func;

//	pthread_t pid;


//	pthread_create(&pid,NULL,read_msg_login,(void *)&fd);
	bzero(&msg.name,sizeof(msg.name));
	bzero(&msg.passwd,sizeof(msg.passwd));
	bzero(idtmp,sizeof(idtmp));
	bzero(passwordtmp,sizeof(passwordtmp));

	setlocale(LC_ALL,"");
	initscr();
	noecho();
	curs_set(0);
	WINDOW * mainwin_l;
	keypad(stdscr,TRUE);
	mainwin_l = newwin(20,70,2,15);   //LINES = 25 ,COLS = 103	
	box(mainwin_l,0,0);
	refresh();
	wrefresh(mainwin_l);

	uppad = derwin(mainwin_l,9,70,0,0);	
	wborder(uppad,0,0,0,'=',0,0,'*','*');
	wrefresh(uppad);
	mvwaddstr(mainwin_l,1,65,"离开");
	mvwaddstr(mainwin_l,4,25,"Scott 欢迎您的回归");
	refresh();

	facepad = derwin(mainwin_l,7,16,9,5);      //
	box(facepad,0,0);
	mvwaddstr(facepad,2,4,"/\\");
	mvwaddstr(facepad,2,11,"/\\");
	mvwaddstr(facepad,4,7,"\\_/");
	wrefresh(facepad);

	idpad = derwin(mainwin_l,3,28,9,25);
	box(idpad,0,0);
	mvwaddstr(idpad,1,1,"此处输入帐号");
	//	scrollok(idpad,TRUE);
	wrefresh(idpad);

	pwdpad = derwin(mainwin_l,3,28,11,25);
	box(pwdpad,0,0);
	mvwaddstr(pwdpad,1,1,"此处输入密码");
	//	scrollok(pwdpad,TRUE);
	wrefresh(pwdpad);

	mvwaddstr(mainwin_l,10,58,"注册帐号");
	mvwaddstr(mainwin_l,12,58,"找回密码");
	mvwaddstr(mainwin_l,14,27,"[ ]记住密码");
	mvwaddstr(mainwin_l,14,40,"[ ]自动登录");

	login = derwin(mainwin_l,3,40,16,17);
	wborder(login,0,0,0,0,'+','+','+','+');
	mvwaddstr(login,1,15,"安 全 登 录");
	wrefresh(login);

	remindwin_l = newwin(5,40,(LINES - 5) / 2,(COLS - 40) / 2);
	box(remindwin_l,0,0);
	mvwprintw(remindwin_l,2,5,"请填入用户名和密码");
	mvwprintw(remindwin_l,3,5,"按任意键返回");

	mvwaddch(mainwin_l,pos[input_i][0],pos[input_i][1],'*');	
	wrefresh(mainwin_l);

	int ch;



	while(1)
	{	
		ch = getch();
		switch(ch)
		{
			case KEY_UP:
				{
					if((input_i == 5 && stay_repwd == 1) || (input_i == 6 && stay_regauto == 1))
					{

					}
					else
					{
						mvwaddch(mainwin_l,pos[input_i][0],pos[input_i][1],' ');
					}
					input_i = input_i - 2;
					break;
				}
			case KEY_DOWN:
				{
					if((input_i == 5 && stay_repwd == 1) || (input_i == 6 && stay_regauto == 1))
					{ 
					}
					else
					{
						mvwaddch(mainwin_l,pos[input_i][0],pos[input_i][1],' ');
					}
					input_i = input_i + 2;
					break;
				}
			case KEY_LEFT:
				{

					if((input_i == 5 && stay_repwd == 1) || (input_i == 6 && stay_regauto == 1))
					{

					}
					else
					{
						mvwaddch(mainwin_l,pos[input_i][0],pos[input_i][1],' ');
					}
					input_i = input_i - 1;
					break;
				}
			case KEY_RIGHT:
				{
					if((input_i == 5 && stay_repwd == 1) || (input_i == 6 && stay_regauto == 1))
					{

					}
					else
					{
						mvwaddch(mainwin_l,pos[input_i][0],pos[input_i][1],' ');
					}
					input_i = input_i + 1;
					break;
				}
			case '\n':
				{
					if(input_i == 1)
					{
						//	if(edit_id == 0)
						//	{
						mvwaddstr(idpad,1,1,"                ");
						wrefresh(idpad);
						//	}
						curs_set(1);
					//	move(12,41);
						wmove(mainwin_l,10,26);
						echo();
						bzero(idtmp,sizeof(idtmp));
						wgetnstr(mainwin_l,idtmp,20);
						curs_set(0);
						noecho();
						edit_id = 1;
					}
					else if(input_i == 2)
					{

						/*	if((ret_func = registerwin(fd)) == BACK_TO_LOGIN)
							{
							leave = 1;
							}*/

						ret_func = REGISTER_WIN;
						leave = 1;
					}
					else if(input_i == 4)
					{
						ret_func = GET_BACK_PASSWD;
						leave = 1;
					}
					else if(input_i == 3)
					{
						//	if(edit_password == 0)
						//	{
						mvwaddstr(pwdpad,1,1,"                ");
						wrefresh(pwdpad);
						//	}
						curs_set(1);
						move(14,41);

						noecho();

						bzero(passwordtmp,sizeof(passwordtmp));
						for(i = 0; i < 20; i++)
						{
							re = getch();
							if(re != '\n')
							{
								passwordtmp[i] = re;
								printw("*");
							}
							else
							{
								break;
							}
						}
						curs_set(0);
						//	echo();
						edit_password = 1;
					}
					else if(input_i == 5)
					{
						stay_repwd = !stay_repwd;
					}
					else if(input_i == 6)
					{
						stay_regauto = !stay_regauto;
					}
					else if(input_i == 7)                                    /*确认登录*/
					{
						strcpy(msg.name,idtmp);
						strcpy(msg.passwd,passwordtmp);
								
						int login_i = 0;
						if(strlen(msg.name) == 0 || strlen(msg.passwd) == 0)
						{
							touchwin(remindwin_l);
							wrefresh(remindwin_l);
							getch();
							touchwin(mainwin_l);
							break;
						}
						while(login_i <= 9)
						{
							if(login_i % 3 == 0)
							{	
								mvwaddstr(login,1,15,"                     ");
								mvwaddstr(login,1,15,"正在登录...");
								wrefresh(login);
							}
							else if(login_i % 3 == 1)
							{	
								mvwaddstr(login,1,15,"                     ");
								mvwaddstr(login,1,15,"正 在 登 录. . . ");
								wrefresh(login);
							}
							else
							{
								mvwaddstr(login,1,15,"                     ");
								mvwaddstr(login,1,15,"正  在  登  录.  .  .");
								wrefresh(login);
							}
							login_i++;
							usleep(60000);

						}
						msg.action = LOGIN;
						write(fd,&msg,sizeof(msg));
						usleep(600000);                //睡眠时间稍微长点 让之来得及改变全局变量  g_flag
						refresh();
#if 1
						switch(g_flag)               /*登录状态*/
						{
							case LOGIN_SUCCESS:
								{
									ret_func = FRIEND_WIN;
									leave = 1;
									break;
								}	
							case LOGIN_FAILURE:
								{
#if 1
									mvwprintw(remindwin_l,2,5,"                   ");
									mvwprintw(remindwin_l,3,5,"             ");
									mvwprintw(remindwin_l,2,5,"登录失败");
									mvwprintw(remindwin_l,3,5,"请检查用户名与密码是否正确");
									touchwin(remindwin_l);
									wrefresh(remindwin_l);
									getch();
									touchwin(mainwin_l);
									wrefresh(mainwin_l);
									mvwprintw(login,1,15,"                     ");
									mvwprintw(login,1,15,"安 全 登 录");
									wrefresh(login);
#endif
									break;
								}
							case LOGIN_ONLINE:
								{
									mvwprintw(remindwin_l,2,5,"                   ");
									mvwprintw(remindwin_l,3,5,"             ");
									mvwprintw(remindwin_l,2,5,"检测到您已登录");
									mvwprintw(remindwin_l,3,5,"请检查帐号是否被非法盗取!");

									touchwin(remindwin_l);
									wrefresh(remindwin_l);
									getch();
									touchwin(mainwin_l);
									wrefresh(mainwin_l);
									mvwprintw(login,1,15,"                    ");
									mvwprintw(login,1,15,"安 全 登 录");
									wrefresh(login);
									break;
								}
							default:
								{
									break;
								}

						}
			


#endif
					}
					else if(input_i == 0)
					{
						ret_func = QUIT;
						leave = 1;		
					}
				}
		}
		if(input_i < 0)
		{
			input_i = 0;
		}
		else if(input_i > 7)
		{
			input_i = 7;
		}

		if(leave == 1)
		{
			break;
		}
		mvwaddch(mainwin_l,pos[input_i][0],pos[input_i][1],'*');
		wrefresh(mainwin_l);

	}

	wclear(mainwin_l);
	wrefresh(mainwin_l);

	delwin(mainwin_l);
	endwin();
//	pthread_cancel(pid);  //每次退出一个程序时要将它的子线程强制退出  否则会有多个线程同时读结构体
	return ret_func;

}

