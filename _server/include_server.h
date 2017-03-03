#ifndef INCLUDE_SERVER_H
#define INCLUDE_SERVER_H
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
#include "sqlite3.h"

sqlite3 * db;


char * get_current_time();
#endif
