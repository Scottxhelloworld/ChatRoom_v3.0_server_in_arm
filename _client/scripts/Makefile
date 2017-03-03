CC := gcc
CFLAGS :=  -O3
Libs += -lpthread -lncursesw -lsqlite3
Target := client
Source := $(wildcard src/*.c)
ObjS := $(patsubst %.c,%.o,$(Source))
Modules += func chatrecord friend getpasswd login register chat connect groupchat vip test
AllObjs := $(addsuffix /src/*.o,$(Modules))
