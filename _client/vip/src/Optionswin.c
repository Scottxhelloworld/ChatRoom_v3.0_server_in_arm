//#include "include.h"
#include "../../include/include.h"

int optionswin(int fd)
{
	WINDOW * notice = newwin(3,20,24,90);
	box(notice,0,0);

	WINDOW * optionswin = newwin(5,20,24,90);
	box(optionswin,0,0);

	char items[3][40] = {"1. 会员充值","2. 新建群(仅会员)","3. EXIT"};
	int i;
	for(i = 1; i < 4; i++)
	{
		if(1 == i)
		{
			wattron(optionswin,A_REVERSE);	
		}
		mvwprintw(optionswin,i,2,"%s",items[i - 1]);
		if(1 == i)
		{
			wattroff(optionswin,A_REVERSE);
		}
	}
	wrefresh(optionswin);

	int ch;
	int choice;
	int old_choice;

	choice = old_choice = 0;  // 当前选中和先前选中
	while(1)
	{
		ch = getch();
		switch(ch)
		{
			case KEY_UP:
				{
					choice = (choice == 0 ? 2 : choice - 1);
					break;
				}
			case KEY_DOWN:
				{
					choice = (choice == 2 ? 0 : choice + 1);
					break;
				}
			case '\n':
				{
					switch(choice)
					{
						case 0:
							{
								if(flag_vip == 0)
								{
									strcpy(msg.name,self_name);
									msg.action = REG_VIP;
									write(fd,&msg,sizeof(msg));
									wattron(mainwin_c,A_BOLD|A_UNDERLINE);
									mvwprintw(mainwin_c,2,13,"vip");
									wattroff(mainwin_c,A_BOLD|A_UNDERLINE);
									flag_vip = 1;
								}
								break;
							}
						case 1:
							{
								if(flag_vip == 1)
								{
									creategroupchatwin(fd);	
								}
								else
								{
									mvwprintw(notice,1,2,"请注册vip!!!");	
									touchwin(notice);
									wrefresh(notice);
									getch();
									touchwin(mainwin_c);
									wrefresh(mainwin_c);
								}
								break;
							}
					}
					goto end;
				}
			default:
				{
					break;
				}
		}

		mvwprintw(optionswin,old_choice + 1,2,"%s",items[old_choice]);//先将原来的文字重划
		wattron(optionswin,A_REVERSE);
		mvwprintw(optionswin,choice + 1,2,"%s",items[choice]);
		wattroff(optionswin,A_REVERSE);
		old_choice = choice;
		wrefresh(optionswin);


	}

end:
	delwin(optionswin);	
	touchwin(mainwin_c);
	wrefresh(mainwin_c);

}
