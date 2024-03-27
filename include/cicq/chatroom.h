#ifndef CHATROOM_H
#define CHATROOM_H

#include <string.h>
#include <time.h>
#include <stdio.h>
#include <errno.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>
#include "json_wrapper.h"

typedef struct
{
    char username[32];
    char password[32];
} User;

enum UIState
{
    kWelcome,
    kLogin,
    kRegister,
    kChatroom,
    kExit
};

enum FlagRegisterStatusCode
{
    kRegisterSucessful = 200,
    kRegisterFailedTimeout = 401,
    kRegisterNormal = 100,
    kRegisterFailedUsernameExsist = 402,
    kRegisterError = 400
};


typedef struct
{
    enum UIState ui_state;
    volatile enum FlagRegisterStatusCode f_register_status; // 标志：是否收到注册响应

    char *server_address;
    MessageList message_list;
    User *user;

    struct event_base *base;
    struct bufferevent *bev;

} Chatroom;

void initScreen();
void closeScreen();
void getPassword(char *password, uint8_t length);
void printInMiddle(WINDOW *win, int starty, int startx, int width, char *string);
int min(int a, int b);
int max(int a, int b);
void renderHeader(const char *chatroom_name, const char *user_name);
int handleInput(WINDOW *input_win, User *current_user);
void renderWelcome();
int handleWelcomeOpinion(Chatroom *handler);
void startChatroom(Chatroom *handler);
void doWelcome(Chatroom *handler);
void doLogin(Chatroom *handler);
void doRegister(Chatroom *handler);
void doChatroom(Chatroom *handler);
void doExit(Chatroom *handler);

#endif // CHATROOM_H