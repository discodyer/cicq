#ifndef SERVERPROC_H
#define SERVERPROC_H

#include <stdlib.h>
#include <string.h>
#include <cjson/cJSON.h>
#include <string.h>
#include <event2/event.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>
#include <event2/listener.h>
#include <stdbool.h>

typedef struct User
{
    char *username;
    char *password;
    bool is_login;
    struct bufferevent* bev; // 指向用户TCP连接的bufferevent
    struct User *next;
} User;

typedef struct UserList
{
    User *head;
} UserList;

User *addUser(User **head, const char *username, const char *password, struct bufferevent* bev);
User *findUser(User *head, const char *username);
void freeUserList(User *head);
void broadcastMessage(UserList *userList, const char *message, const char *username, time_t msg_time);

void accept_conn_cb(struct evconnlistener *listener, evutil_socket_t fd,
                    struct sockaddr *address, int socklen, void *ctx);
void read_cb(struct bufferevent *bev, void *ctx);
void error_cb(struct bufferevent *bev, short events, void *ctx);
void login_cb(struct bufferevent *bev, cJSON *json, UserList *userList);
void register_cb(struct bufferevent *bev, cJSON *json, UserList *userList);
void logout_cb(struct bufferevent *bev, cJSON *json, UserList *userList);
void msg_private_cb(struct bufferevent *bev, cJSON *json, UserList *userList);
void msg_group_cb(struct bufferevent *bev, cJSON *json, UserList *userList);

#endif // SERVERPROC_H