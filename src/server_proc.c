#include "server_proc.h"

extern UserList userList;

User *addUser(User **head, const char *username, const char *password)
{
    User *newUser = (User *)malloc(sizeof(User));
    newUser->username = strdup(username); // 需要释放
    newUser->password = strdup(password); // 需要释放
    newUser->next = *head;
    *head = newUser;
    return newUser;
}

User *findUser(User *head, const char *username)
{
    while (head != NULL)
    {
        if (strcmp(head->username, username) == 0)
        {
            return head;
        }
        head = head->next;
    }
    return NULL;
}

void freeUserList(User *head)
{
    User *tmp;
    while (head != NULL)
    {
        tmp = head;
        head = head->next;
        free(tmp->username);
        free(tmp->password);
        free(tmp);
    }
}

void accept_conn_cb(struct evconnlistener *listener, evutil_socket_t fd,
                    struct sockaddr *address, int socklen, void *ctx)
{
    struct event_base *base = evconnlistener_get_base(listener);
    struct bufferevent *bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);

    bufferevent_setcb(bev, read_cb, NULL, error_cb, ctx);
    bufferevent_enable(bev, EV_READ | EV_WRITE);
}

void read_cb(struct bufferevent *bev, void *ctx)
{
    UserList* userList = (UserList*)ctx;
    struct evbuffer *input = bufferevent_get_input(bev);
    size_t len = evbuffer_get_length(input);
    char *data = malloc(len + 1);
    evbuffer_remove(input, data, len);
    data[len] = '\0';

    // 解析JSON
    cJSON *json = cJSON_Parse(data);
    free(data); // 不再需要原始数据
    if (!json)
        return; // 解析错误处理

    cJSON *statuscode = cJSON_GetObjectItem(json, "statuscode");
    if (cJSON_IsNumber(statuscode) && statuscode->valueint == 101)
    {
        cJSON *username = cJSON_GetObjectItem(json, "username");
        cJSON *password = cJSON_GetObjectItem(json, "password");

        if (findUser(userList->head, username->valuestring))
        {
            // 用户已存在
            cJSON *response = cJSON_CreateObject();
            cJSON_AddNumberToObject(response, "statuscode", 400);
            cJSON_AddStringToObject(response, "message", "注册失败，用户已经存在。");
            char *responseStr = cJSON_PrintUnformatted(response);
            bufferevent_write(bev, responseStr, strlen(responseStr));
            free(responseStr);
            cJSON_Delete(response);
            printf("注册失败，用户已经存在。\n");
        }
        else
        {
            // 添加新用户
            addUser(&userList->head, username->valuestring, password->valuestring);
            cJSON *response = cJSON_CreateObject();
            cJSON_AddNumberToObject(response, "statuscode", 200);
            cJSON_AddStringToObject(response, "message", "注册成功。");
            char *responseStr = cJSON_PrintUnformatted(response);
            bufferevent_write(bev, responseStr, strlen(responseStr));
            free(responseStr);
            cJSON_Delete(response);
            printf("注册成功。\n");
        }
    }

    cJSON_Delete(json);
}

void error_cb(struct bufferevent *bev, short events, void *ctx)
{
    if (events & BEV_EVENT_ERROR)
        perror("Error");
    if (events & (BEV_EVENT_EOF | BEV_EVENT_ERROR))
    {
        bufferevent_free(bev);
    }
}
