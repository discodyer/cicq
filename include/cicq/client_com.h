#ifndef CLIENTCOM_H
#define CLIENTCOM_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <event2/event.h>
#include <event2/bufferevent.h>

void event_cb(struct bufferevent *bev, short events, void *ctx);
void read_cb(struct bufferevent *bev, void *ctx);

#endif // CLIENTCOM_H