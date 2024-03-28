#include "json_wrapper.h"

void msgEnpack(Message *msg, cJSON **json)
{
    // 创建一个新的JSON对象
    *json = cJSON_CreateObject();

    // 向JSON对象中添加数据
    cJSON_AddStringToObject(*json, "username", msg->username);
    cJSON_AddStringToObject(*json, "payload", msg->payload);
    cJSON_AddNumberToObject(*json, "rawtime", (double)msg->rawtime);
}

void msgDepack(cJSON *json, Message *msg)
{
    // 从JSON对象中提取数据
    cJSON *username = cJSON_GetObjectItemCaseSensitive(json, "username");
    cJSON *payload = cJSON_GetObjectItemCaseSensitive(json, "payload");
    cJSON *rawtime = cJSON_GetObjectItemCaseSensitive(json, "rawtime");

    if (cJSON_IsString(username) && (username->valuestring != NULL))
    {
        msg->username = strdup(username->valuestring);
    }

    if (cJSON_IsString(payload) && (payload->valuestring != NULL))
    {
        msg->payload = strdup(payload->valuestring);
    }

    if (cJSON_IsNumber(rawtime))
    {
        msg->rawtime = (time_t)rawtime->valuedouble;
    }
}

void printMsg(Message *msg)
{
    struct tm *timeinfo;
    timeinfo = localtime(&msg->rawtime);

    printf("Username: %s\n", msg->username);
    printf("Message: %s\n", msg->payload);
    printf("Time: %02d:%02d:%02d\n", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
}

/// @brief 添加新消息
/// @param list
/// @param message
void addMessage(MessageList *list, Message message)
{
    MessageNode *newNode = (MessageNode *)malloc(sizeof(MessageNode));
    newNode->message = message; // 注意：这里是浅拷贝
    newNode->next = NULL;

    if (list->head == NULL)
    {
        newNode->prev = NULL;
        list->head = list->tail = newNode;
    }
    else
    {
        list->tail->next = newNode;
        newNode->prev = list->tail;
        list->tail = newNode;
    }
}

/// @brief 按用户名查找消息
/// @param list
/// @param username
/// @return
MessageNode *findMessageByUsername(MessageList *list, const char *username)
{
    MessageNode *current = list->head;
    while (current != NULL)
    {
        if (strcmp(current->message.username, username) == 0)
        {
            return current;
        }
        current = current->next;
    }
    return NULL; // 未找到
}

/// @brief 释放空间
/// @param list
void freeMessageList(MessageList *list)
{
    MessageNode *current = list->head;
    while (current != NULL)
    {
        MessageNode *next = current->next;
        free(current->message.username);
        free(current->message.payload);
        free(current);
        current = next;
    }
    list->head = list->tail = NULL;
}

MessageNode *getNext(MessageNode *node)
{
    return node->next;
}

MessageNode *getPrev(MessageNode *node)
{
    return node->prev;
}

MessageNode *getTail(MessageList *list)
{
    return list->tail;
}

MessageNode *getHead(MessageList *list)
{
    return list->head;
}