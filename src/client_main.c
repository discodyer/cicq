#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "chatroom.h"
#include "client_com.h"

// 事件循环线程的函数
void *event_loop_thread(void *arg) {
    struct event_base *base = (struct event_base *)arg;
    event_base_dispatch(base); // 在这个线程中运行事件循环
    return NULL;
}

int main(int argc, char **argv)
{
    User currentUser = {
        .username = "",
        .password = ""};

    Chatroom chat = {
        .ui_state = kWelcome,
        .user = &currentUser,
        .server_address = NULL,
        .message_list.head = NULL,
        .message_list.tail = NULL,
        .f_register_received = false};

    // 初始化ncurses
    initScreen();

    struct event_base *base;
    struct bufferevent *bev;
    struct sockaddr_in sin;

    // 检查命令行参数
    // if (argc != 4) {
    //     fprintf(stderr, "Usage: %s <server_ip> <port> <username>\n", argv[0]);
    //     return 1;
    // }

    // 初始化libevent
    base = event_base_new();
    if (!base) {
        perror("event_base_new");
        return 1;
    }

    // 设置服务器地址
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    // sin.sin_addr.s_addr = inet_addr(argv[1]);
    // sin.sin_port = htons(atoi(argv[2]));
    sin.sin_addr.s_addr = inet_addr("127.0.0.1");
    sin.sin_port = htons(atoi("8888"));

    // 创建bufferevent用于TCP连接
    bev = bufferevent_socket_new(base, -1, BEV_OPT_CLOSE_ON_FREE);
    if (!bev) {
        perror("bufferevent_socket_new");
        event_base_free(base);
        return 1;
    }

    // 设置回调函数处理读写和事件
    bufferevent_setcb(bev, read_cb, NULL, event_cb, NULL);
    bufferevent_enable(bev, EV_READ | EV_WRITE);

    // 连接服务器
    if (bufferevent_socket_connect(bev, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
        perror("bufferevent_socket_connect");
        bufferevent_free(bev);
        event_base_free(base);
        return 1;
    }

    // 构建JSON格式的注册信息
    char info[512];
    // snprintf(info, sizeof(info), "{\"statuscode\":101, \"username\":\"%s\", \"password\":\"password123\"}", argv[3]);
    snprintf(info, sizeof(info), "{\"statuscode\":101, \"username\":\"%s\", \"password\":\"password123\"}", "cody");
    bufferevent_write(bev, info, strlen(info)); // 发送注册信息

    // 进入事件循环
    // event_base_dispatch(base);
    // 创建一个线程来运行libevent的事件循环
    pthread_t thread_id;
    if (pthread_create(&thread_id, NULL, event_loop_thread, (void *)base) != 0) {
        perror("pthread_create failed");
        event_base_free(base);
        return 1;
    }

    // 开始程序主循环
    startChatroom(&chat);

    // 等待事件循环线程结束（如果需要）
    pthread_join(thread_id, NULL);

    // 清理资源
    // bufferevent_free(bev);
    event_base_free(base);
    // freeMessageList(list);
    closeScreen();
    return 0;
}
