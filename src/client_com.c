#include "client_com.h"
#include "chatroom.h"

void read_cb(struct bufferevent *bev, void *ctx)
{
    Chatroom *chatroom = (Chatroom *)ctx;
    char response[1024];
    // 读取响应数据
    bufferevent_read(bev, response, sizeof(response));
    response[sizeof(response) - 1] = '\0';
    // 处理响应
    printf("Server response: %s\n", response);
    chatroom->f_register_status = kRegisterSucessful; // 标记响应已接收
}

void event_cb(struct bufferevent *bev, short events, void *ctx)
{
    if (events & BEV_EVENT_EOF) {
        printf("Connection closed.\n");
    } else if (events & BEV_EVENT_ERROR) {
        printf("Got an error on the connection: %s\n", strerror(errno));
    }
    // 标记响应已接收，即使是通过错误或EOF
    ((Chatroom *)ctx)->f_register_status = kRegisterSucessful;
}

void sendRegistrationRequest(struct bufferevent *bev, const char *username, const char *password)
{
    char request[256];
    // 构造注册请求的JSON字符串，简化为直接格式化
    snprintf(request, sizeof(request), "{\"statuscode\":101, \"username\":\"%s\", \"password\":\"%s\"}", username, password);
    bufferevent_write(bev, request, strlen(request)); // 发送注册请求
}

void registerWithServer(Chatroom *chatroom, const char *username, const char *password) {
    // 配置bufferevent的回调
    bufferevent_setcb(chatroom->bev, read_cb, NULL, event_cb, chatroom);
    bufferevent_enable(chatroom->bev, EV_READ | EV_WRITE);

    // 发送注册请求
    sendRegistrationRequest(chatroom->bev, username, password);

    // 等待响应或超时
    // while (!chatroom->f_register_received) {
    //     event_base_loop(chatroom->base, EVLOOP_NONBLOCK); // 非阻塞模式运行事件循环
    // }
}