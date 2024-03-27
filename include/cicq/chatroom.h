#ifndef CHATROOM_H
#define CHATROOM_H

#include <ncurses.h>
#include <string.h>
#include <time.h>
#include <stdio.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
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

typedef struct 
{
    enum UIState ui_state;
    char* server_address;
    MessageList message_list;
    User* user;

    bool f_register_received;

} Chatroom;


void initScreen();
void closeScreen();
void getPassword(char *password, uint8_t length);
// void chatroom(User *currentUser);
void printInMiddle(WINDOW *win, int starty, int startx, int width, char *string);
int min(int a, int b);
int max(int a, int b);
void renderHeader(const char *chatroom_name, const char *user_name);
int handleInput(WINDOW *input_win, User *current_user);
void renderWelcome();
int handleWelcomeOpinion(Chatroom *handler);
void startChatroom(Chatroom* handler);
void doWelcome(Chatroom* handler);
void doLogin(Chatroom* handler);
void doRegister(Chatroom* handler);
void doChatroom(Chatroom* handler);
void doExit(Chatroom* handler);

// int registerAndWait(Chatroom* handler, uint16_t timeout);
// void registerUser();


#endif // CHATROOM_H