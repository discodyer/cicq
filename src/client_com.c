#include "client_com.h"
#include "chatroom.h"
#include "status_code.h"

void read_statuscode_cb(struct bufferevent *bev, void *ctx)
{
    Chatroom *chatroom = (Chatroom *)ctx;
    char response[1024];
    // 读取响应数据
    bufferevent_read(bev, response, sizeof(response));
    response[sizeof(response) - 1] = '\0';
    // 处理响应

    // 解析JSON
    cJSON *json = cJSON_Parse(response);
    // free(response); // 不再需要原始数据
    if (!json)
    {
        chatroom->f_status_code = STATUS_CODE_ERROR; // 标记服务器错误
        return;
    }
    cJSON *statuscode = cJSON_GetObjectItem(json, "statuscode");
    if (cJSON_IsNumber(statuscode) && statuscode->valueint > 0 && statuscode->valueint < 600)
    {
        // 获取状态码
        chatroom->f_status_code = statuscode->valueint; // 标记响应已接收
    }
    else
    {
        // 解析错误处理
        chatroom->f_status_code = STATUS_CODE_ERROR; // 标记服务器错误
        return;
    }
}

void read_message_cb(struct bufferevent *bev, void *ctx)
{
    Chatroom *chatroom = (Chatroom *)ctx;
    char response[1024];
    // 读取响应数据
    bufferevent_read(bev, response, sizeof(response));
    response[sizeof(response) - 1] = '\0';
    // 处理响应

    // 解析JSON
    cJSON *json = cJSON_Parse(response);
    // free(response); // 不再需要原始数据
    if (!json)
    {
        chatroom->f_status_code = STATUS_CODE_ERROR; // 标记服务器错误
        return;
    }
    cJSON *statuscode = cJSON_GetObjectItem(json, "statuscode");
    if (cJSON_IsNumber(statuscode) && statuscode->valueint > 0 && statuscode->valueint < 600)
    {
        // 获取状态码
        chatroom->f_status_code = statuscode->valueint; // 标记响应已接收
    }
    else
    {
        // 解析错误处理
        chatroom->f_status_code = STATUS_CODE_ERROR; // 标记服务器错误
        return;
    }
    if (statuscode->valueint == STATUS_CODE_MSG_BOARDCAST)
    {
        get_broadcast_msg_cb(chatroom, json);
    }
}

void event_cb(struct bufferevent *bev, short events, void *ctx)
{
    if (events & BEV_EVENT_EOF)
    {
        printf("Connection closed.\n");
    }
    else if (events & BEV_EVENT_ERROR)
    {
        printf("Got an error on the connection: %s\n", strerror(errno));
    }
    // 标记响应已接收，即使是通过错误或EOF
    ((Chatroom *)ctx)->f_status_code = STATUS_CODE_ERROR;
}

void sendRegistrationRequest(struct bufferevent *bev, const char *username, const char *password)
{
    char request[256];
    // 构造注册请求的JSON字符串，简化为直接格式化
    snprintf(request, sizeof(request), "{\"statuscode\":%d, \"username\":\"%s\", \"password\":\"%s\"}", STATUS_CODE_REGISTER, username, password);
    bufferevent_write(bev, request, strlen(request)); // 发送注册请求
}

void sendLoginRequest(struct bufferevent *bev, const char *username, const char *password)
{
    char request[256];
    // 构造注册请求的JSON字符串，简化为直接格式化
    snprintf(request, sizeof(request), "{\"statuscode\":%d, \"username\":\"%s\", \"password\":\"%s\"}", STATUS_CODE_LOGIN, username, password);
    bufferevent_write(bev, request, strlen(request)); // 发送登录请求
}

void sendMsgRequest(struct bufferevent *bev, const char *msg)
{
    if (!msg)
    {
        return;
    }
    bufferevent_write(bev, msg, strlen(msg)); // 发送消息
}

void registerWithServer(Chatroom *chatroom, const char *username, const char *password)
{
    // 配置bufferevent的回调
    bufferevent_setcb(chatroom->bev, read_statuscode_cb, NULL, event_cb, chatroom);
    bufferevent_enable(chatroom->bev, EV_READ | EV_WRITE);

    // 发送注册请求
    sendRegistrationRequest(chatroom->bev, username, password);
}

void loginWithServer(Chatroom *chatroom, const char *username, const char *password)
{
    // 配置bufferevent的回调
    bufferevent_setcb(chatroom->bev, read_statuscode_cb, NULL, event_cb, chatroom);
    bufferevent_enable(chatroom->bev, EV_READ | EV_WRITE);

    // 发送登录请求
    sendLoginRequest(chatroom->bev, username, password);
}

void get_broadcast_msg_cb(Chatroom *handler, cJSON *json)
{
    cJSON *username = cJSON_GetObjectItem(json, "username");
    cJSON *payload = cJSON_GetObjectItem(json, "payload");
    cJSON *rawtime = cJSON_GetObjectItem(json, "rawtime");
    if (!cJSON_IsString(username) &&
        !cJSON_IsString(payload) &&
        !cJSON_IsNumber(rawtime))
    {
        return;
    }
    Message msg = {.payload = strdup(payload->valuestring),
                   .username = strdup(username->valuestring),
                   .rawtime = (time_t)rawtime->valuedouble,
                   .msg_type = kMsgBroadcast};
    addMessage(handler->message_list, msg);
    handler->new_message_received = true;
}