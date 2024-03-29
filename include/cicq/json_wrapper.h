#ifndef JSONWRAPPER_H
#define JSONWRAPPER_H

#include <cjson/cJSON.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

enum MsgType{
    kMsgPrivate,
    kMsgBroadcast,
    kMsgError,
};
typedef struct
{
    char *username;
    char *contact;
    char *payload;
    time_t rawtime;
    enum MsgType msg_type;
} Message;

typedef struct MessageNode
{
    Message message;
    struct MessageNode *prev;
    struct MessageNode *next;
} MessageNode;

typedef struct
{
    MessageNode *head;
    MessageNode *tail;
} MessageList;

void msgEnpack(Message *msg, cJSON **json);
void msgDepack(cJSON *json, Message *msg);
void printMsg(Message *msg);
void addMessage(MessageList *list, Message message);
MessageNode *findMessageByUsername(MessageList *list, const char *username);
void freeMessageList(MessageList *list);
MessageNode *getNext(MessageNode *node);
MessageNode *getPrev(MessageNode *node);
MessageNode *getTail(MessageList *list);
MessageNode *getHead(MessageList *list);

#endif // JSONWRAPPER_H