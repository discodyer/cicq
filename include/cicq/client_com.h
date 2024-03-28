#ifndef CLIENTCOM_H
#define CLIENTCOM_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <event2/event.h>
#include <event2/bufferevent.h>
#include "chatroom.h"

void event_cb(struct bufferevent *bev, short events, void *ctx);
void read_statuscode_cb(struct bufferevent *bev, void *ctx);
void sendRegistrationRequest(struct bufferevent *bev, const char *username, const char *password);
void sendLoginRequest(struct bufferevent *bev, const char *username, const char *password);
void registerWithServer(Chatroom *chatroom, const char *username, const char *password);
void loginWithServer(Chatroom *chatroom, const char *username, const char *password);

#endif // CLIENTCOM_H