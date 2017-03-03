#ifndef INCLUDE_H
#define INCLUDE_H
#include <ncurses.h>
#include <locale.h>
#include <stdlib.h> 
#include <stdio.h> 
#include <errno.h> 
#include <string.h> 
#include <netdb.h> 
#include <sys/types.h> 
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <netinet/in.h> 
#include <sys/socket.h> 
#include "Message.h"
#include <pthread.h>
#include <sqlite3.h>

MSG msg;
WINDOW * input_filename_win;
WINDOW * notify_file_win;
WINDOW * mainwin;
WINDOW * noticewin;
WINDOW * mainwin_c;
WINDOW * facewin_c;
WINDOW * sigwin_c;
WINDOW * searchwin_c;
WINDOW * mainwin_chat;
WINDOW * upwin_chat;
WINDOW * show_chat_win;
WINDOW * middle_win_chat;
WINDOW * sendwin_chat;
WINDOW * mainwin_cr;  //聊天记录主界面
WINDOW * mainwin_cr_group;
WINDOW * result_search_win;
WINDOW * mainwin_create;
WINDOW * mainwin_chat_group;
WINDOW * show_chat_win_group;
WINDOW * mainwin_gpwd;
WINDOW * questionwin;

int g_flag;
int reg;                    //全局变量  判断是否注册成功
int chatwin_order;          //记录聊天窗口的显示行数  顺序
int chatwin_order_group;
int cr_order;               //记录聊天记录的显示行书
int flag_open_chatwin[5];   //标志是否打开对应功能窗口  下标为0代表好友列表是否展开  1～4代表每个对应的selectchatname是否打开聊天窗口  从而控制闪烁


char  date[20];                //获取消息记录日期 
char selectchatname[4][20]; //每个页面对应的4个联系人名字
char sigs[4][30];        //每个页面4个联系人个性签名
char self_name[20];
sqlite3 * db;
int online[4];            //每个页面4个联系人是否显示在线
pthread_t pid[5];         //我的好友 和对应页面下的4个联系人线程闪烁的pid
int isrunning[5];         //对应上面的5个线程是否在运行
int flag_search;          //打开的窗口是否是由于搜索联系人而打开的  
int flag_vip;             //标记是否是vip  可否创建群
int destfile_fd;          //接收文件的fd
char current_groupname[20];        //当前组在链表

USER * head;              //创建群 获取所有好友，包括最近联系人  ；在拉人时复用
USER * head_group;        //获取群
USER * head_group_members;//群中人链表  (只是为了显示用)

char * get_current_time();
char * get_user(USER * head1,int i);
int get_user_count();
int get_list_count(USER * head1);
USER * get_user_node(USER * head1,int i);
#endif
