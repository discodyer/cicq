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

typedef struct User
{
    char *username;
    char *password;
    struct User *next;
} User;

typedef struct UserList
{
    User *head;
} UserList;

User *addUser(User **head, const char *username, const char *password);
User *findUser(User *head, const char *username);
void freeUserList(User *head);

void accept_conn_cb(struct evconnlistener *listener, evutil_socket_t fd,
                    struct sockaddr *address, int socklen, void *ctx);
void read_cb(struct bufferevent *bev, void *ctx);
void error_cb(struct bufferevent *bev, short events, void *ctx);

#endif // SERVERPROC_H