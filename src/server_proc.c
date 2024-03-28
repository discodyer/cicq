#include "server_proc.h"
#include "status_code.h"

// extern UserList userList;

User *addUser(User **head, const char *username, const char *password)
{
    User *newUser = (User *)malloc(sizeof(User));
    newUser->username = strdup(username); // 需要释放
    newUser->password = strdup(password); // 需要释放
    newUser->is_login = false;
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
    UserList *userList = (UserList *)ctx;
    struct evbuffer *input = bufferevent_get_input(bev);
    size_t len = evbuffer_get_length(input);
    char *data = malloc(len + 1);
    evbuffer_remove(input, data, len);
    data[len] = '\0';

    // 解析JSON
    cJSON *json = cJSON_Parse(data);
    free(data); // 不再需要原始数据
    if (!json)
    {
        cJSON *response = cJSON_CreateObject();
        cJSON_AddNumberToObject(response, "statuscode", STATUS_CODE_ERROR);
        cJSON_AddStringToObject(response, "message", "Internal error");
        char *responseStr = cJSON_PrintUnformatted(response);
        bufferevent_write(bev, responseStr, strlen(responseStr));
        free(responseStr);
        cJSON_Delete(response);
        printf("内部错误: json解析失败\n");
        return;
    } // 解析错误处理

    cJSON *statuscode = cJSON_GetObjectItem(json, "statuscode");
    if (cJSON_IsNumber(statuscode) && statuscode->valueint == STATUS_CODE_REGISTER)
    {
        register_cb(bev, json, userList);
    }
    else if (cJSON_IsNumber(statuscode) && statuscode->valueint == STATUS_CODE_LOGIN)
    {
        login_cb(bev, json, userList);
    }
    else if (cJSON_IsNumber(statuscode) && statuscode->valueint == STATUS_CODE_LOGOUT)
    {
        logout_cb(bev, json, userList);
    }
    else
    {
        cJSON *response = cJSON_CreateObject();
        cJSON_AddNumberToObject(response, "statuscode", STATUS_CODE_ERROR);
        cJSON_AddStringToObject(response, "message", "Internal error");
        char *responseStr = cJSON_PrintUnformatted(response);
        bufferevent_write(bev, responseStr, strlen(responseStr));
        free(responseStr);
        cJSON_Delete(response);
        printf("内部错误: 未知操作\n");
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

void login_cb(struct bufferevent *bev, cJSON *json, UserList *userList)
{
    cJSON *username = cJSON_GetObjectItem(json, "username");
    cJSON *password = cJSON_GetObjectItem(json, "password");
    User *current_user = findUser(userList->head, username->valuestring);
    if (current_user)
    { // 用户已存在
        cJSON *response = cJSON_CreateObject();
        if (strcmp(current_user->password, password->valuestring) == 0)
        { // 密码正确
            cJSON_AddNumberToObject(response, "statuscode", STATUS_CODE_LOGIN_SUCESSFUL);
            cJSON_AddStringToObject(response, "message", "登录成功");
            printf("用户 %s 登录成功\n", username->valuestring);
            current_user->is_login = true;
        }
        else
        { // 密码错误
            cJSON_AddNumberToObject(response, "statuscode", STATUS_CODE_LOGIN_FAILED_WROWN_PASSWORD);
            cJSON_AddStringToObject(response, "message", "登录失败: 密码错误");
            printf("用户 %s 登录失败: 密码错误\n", username->valuestring);
            current_user->is_login = false;
        }
        char *responseStr = cJSON_PrintUnformatted(response);
        bufferevent_write(bev, responseStr, strlen(responseStr));
        free(responseStr);
        cJSON_Delete(response);
    }
    else
    {
        // 用户不存在
        cJSON *response = cJSON_CreateObject();
        cJSON_AddNumberToObject(response, "statuscode", STATUS_CODE_LOGIN_FAILED_USER_NOT_FOUND);
        cJSON_AddStringToObject(response, "message", "登录失败: 用户不存在");
        char *responseStr = cJSON_PrintUnformatted(response);
        bufferevent_write(bev, responseStr, strlen(responseStr));
        free(responseStr);
        cJSON_Delete(response);
        printf("用户 %s 登录失败: 用户不存在\n", username->valuestring);
    }
}

void logout_cb(struct bufferevent *bev, cJSON *json, UserList *userList)
{
    cJSON *username = cJSON_GetObjectItem(json, "username");
    cJSON *password = cJSON_GetObjectItem(json, "password");
    User *current_user = findUser(userList->head, username->valuestring);
    if (current_user)
    { // 用户存在
        cJSON *response = cJSON_CreateObject();
        if (strcmp(current_user->password, password->valuestring) == 0)
        { // 密码正确
            cJSON_AddNumberToObject(response, "statuscode", STATUS_CODE_SUCESSFUL);
            cJSON_AddStringToObject(response, "message", "登出成功");
            printf("用户 %s 登出成功\n", username->valuestring);
            current_user->is_login = false;
        }
        else
        { // 密码错误
            cJSON_AddNumberToObject(response, "statuscode", STATUS_CODE_ERROR);
            cJSON_AddStringToObject(response, "message", "登出失败: 密码错误");
            printf("用户 %s 登出失败: 密码错误\n", username->valuestring);
            // current_user->is_login = false;
        }
        char *responseStr = cJSON_PrintUnformatted(response);
        bufferevent_write(bev, responseStr, strlen(responseStr));
        free(responseStr);
        cJSON_Delete(response);
    }
    else
    {
        // 用户不存在
        cJSON *response = cJSON_CreateObject();
        cJSON_AddNumberToObject(response, "statuscode", STATUS_CODE_ERROR);
        cJSON_AddStringToObject(response, "message", "登出失败: 用户不存在");
        char *responseStr = cJSON_PrintUnformatted(response);
        bufferevent_write(bev, responseStr, strlen(responseStr));
        free(responseStr);
        cJSON_Delete(response);
        printf("用户 %s 登出失败: 用户不存在\n", username->valuestring);
    }
}

void register_cb(struct bufferevent *bev, cJSON *json, UserList *userList)
{
    cJSON *username = cJSON_GetObjectItem(json, "username");
    cJSON *password = cJSON_GetObjectItem(json, "password");

    if (!findUser(userList->head, username->valuestring))
    {
        // 添加新用户
        addUser(&userList->head, username->valuestring, password->valuestring);
        cJSON *response = cJSON_CreateObject();
        cJSON_AddNumberToObject(response, "statuscode", STATUS_CODE_REGISTER_SUCESSFUL);
        cJSON_AddStringToObject(response, "message", "注册成功。");
        char *responseStr = cJSON_PrintUnformatted(response);
        bufferevent_write(bev, responseStr, strlen(responseStr));
        free(responseStr);
        cJSON_Delete(response);
        printf("用户 %s 注册成功。\n", username->valuestring);
    }
    else
    {
        // 用户已存在
        cJSON *response = cJSON_CreateObject();
        cJSON_AddNumberToObject(response, "statuscode", STATUS_CODE_REGISTER_FAILED_USERNAME_EXSIST);
        cJSON_AddStringToObject(response, "message", "注册失败，用户已经存在。");
        char *responseStr = cJSON_PrintUnformatted(response);
        bufferevent_write(bev, responseStr, strlen(responseStr));
        free(responseStr);
        cJSON_Delete(response);
        printf("用户 %s 注册失败，用户已经存在。\n", username->valuestring);
    }
}
