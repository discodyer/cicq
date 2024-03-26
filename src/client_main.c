#include "chatroom.h"

int main()
{
    User currentUser = {
        .username = "",
        .password = ""};

    Chatroom chat = {
        .ui_state = kWelcome,
        .user = &currentUser,
        .server_address = NULL};

    // 初始化ncurses
    initScreen();

    // 开始程序主循环
    startChatroom(&chat);

    // closeScreen();
    return 0;
}
