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
#include "status_code.h"

typedef struct
{
    char *username;
    char *password;
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
    kRegisterSucessful = STATUS_CODE_REGISTER_SUCESSFUL,
    kRegisterFailedTimeout = STATUS_CODE_REGISTER_FAILED_TIMEOUT,
    kRegisterNormal = STATUS_CODE_REGISTER_NORMAL,
    kRegisterFailedUsernameExsist = STATUS_CODE_REGISTER_FAILED_USERNAME_EXSIST,
    kRegisterError = STATUS_CODE_ERROR
};


typedef struct
{
    enum UIState ui_state;
    volatile uint16_t f_status_code;

    // char *server_address;
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
void initChatroomWindows(WINDOW **chat_win, WINDOW **input_win, int max_y, int max_x);
void printInChatWin(WINDOW *chat_win, const char *msg);
void sendMsgGroup(Chatroom *handler, const char *msg);
void sendMsgPrivate(Chatroom *handler, const char *msg, const char* contact);

#endif // CHATROOM_H
