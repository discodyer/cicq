#include "chatroom.h"
#include "client_com.h"
#include "string.h"

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_var = PTHREAD_COND_INITIALIZER;

void printInMiddle(WINDOW *win, int starty, int startx, int width, char *string)
{
    int length, x, y;
    float temp;

    if (win == NULL)
        win = stdscr;
    getyx(win, y, x);
    if (startx != 0)
        x = startx;
    if (starty != 0)
        y = starty;
    if (width == 0)
        width = 80;
    length = strlen(string);
    temp = (width - length) / 2;
    x = startx + (int)temp;
    mvwprintw(win, y, x, "%s", string);
    refresh();
}

int min(int a, int b)
{
    return (a < b) ? a : b;
}

int max(int a, int b)
{
    return (a > b) ? a : b;
}

void renderHeader(const char *chatroom_name, const char *user_name)
{
    int row, col;
    char header[80];

    // 获取屏幕尺寸
    getmaxyx(stdscr, row, col);

    // 格式化标题字符串
    snprintf(header, sizeof(header), "*** Welcome to the Chatroom %s - User: %s ***", chatroom_name, user_name);

    // 清屏
    clear();

    // 在屏幕顶部居中显示标题
    printInMiddle(stdscr, 1, 0, col, header);
}

int handleInput(WINDOW *input_win, User *current_user)
{
    char input[80];
    wclear(input_win);
    mvwprintw(input_win, 0, 0, "Enter message: ");
    wrefresh(input_win);

    wgetnstr(input_win, input, 79);

    if (input[0] == '!')
    {
        // 处理命令
        if (strcmp(input, "!exit") == 0)
        {
            return -1; // 返回-1表示退出
        }
        else if (strcmp(input, "!bottom") == 0)
        {
            // 可以在这里添加处理"!bottom"命令的代码
        }
        // 添加更多命令的处理逻辑...
    }
    else
    {
        // 在这里处理正常消息的发送
        // 由于当前版本没有实际的网络功能，我们将简单地回显消息
        int row, col;
        getmaxyx(stdscr, row, col);
        mvwprintw(stdscr, row - 3, 0, "%s: %s", current_user->username, input);
        refresh();
    }
    return 0; // 返回0表示继续
}

/// @brief 绘制欢迎界面
void renderWelcome()
{
    // 清屏
    clear();
    // 用户注册/登录界面
    printw(" __          __  _                            _           _____ _____ _____ ____    _ \n"
           " \\ \\        / / | |                          | |         / ____|_   _/ ____/ __ \\  | |\n"
           "  \\ \\  /\\  / /__| | ___ ___  _ __ ___   ___  | |_ ___   | |      | || |   | |  | | | |\n"
           "   \\ \\/  \\/ / _ \\ |/ __/ _ \\| '_ ` _ \\ / _ \\ | __/ _ \\  | |      | || |   | |  | | | |\n"
           "    \\  /\\  /  __/ | (_| (_) | | | | | |  __/ | || (_) | | |____ _| || |___| |__| | |_|\n"
           "     \\/  \\/ \\___|_|\\___\\___/|_| |_| |_|\\___|  \\__\\___/   \\_____|_____\\_____\\___\\_\\ (_)\n"
           "\n");
    printw("1. Register\n");
    printw("2. Login\n");
    printw("3. Exit\n");
    printw("Choose an option(1,2,3): ");
    echo();
}

/// @brief 处理欢迎界面用户选项
/// @param handler
int handleWelcomeOpinion(Chatroom *handler)
{
    char choice;
    scanw("%c", &choice);
    switch (choice)
    {
    case '1': // 跳转注册
        handler->ui_state = kRegister;
        return 0;
    case '2': // 跳转登录
        handler->ui_state = kLogin;
        return 0;
    case '3':
    case 'q': // 退出程序
        handler->ui_state = kExit;
        return 0;
    default:
        clear();
        printw("Invalid option, please try again.\n");
        getch(); // 等待用户输入
        handler->ui_state = kWelcome;
        return -1;
    }
}

/// @brief UI 主状态机循环
/// @param handler
void startChatroom(Chatroom *handler)
{
    while (handler->ui_state != kExit)
    {
        switch (handler->ui_state)
        {
        case kWelcome: // 欢迎
            doWelcome(handler);
            break;
        case kLogin: // 登录
            doLogin(handler);
            break;
        case kRegister: // 注册
            doRegister(handler);
            break;
        case kChatroom: // 聊天室
            doChatroom(handler);
            break;
        case kExit: // 退出
            doExit(handler);
            return;
        default:
            return;
            break;
        }
    }
    doExit(handler);
}

/// @brief 欢迎界面
/// @param handler
void doWelcome(Chatroom *handler)
{
    // 显示开始菜单
    renderWelcome();
    // 处理用户选项
    handleWelcomeOpinion(handler);
}

/// @brief 登录界面
/// @param handler
void doLogin(Chatroom *handler)
{
    char username_[32] = "";
    char password_[32] = "";
    clear();
    printw("Login to CICQ\n");
    printw("Enter username: ");
    getnstr(username_, 31);
    noecho();
    printw("Enter password: ");
    getPassword(password_, 31);

    printw("\nSending request to server...\n");

    handler->f_status_code = STATUS_CODE_LOGIN;

    // 发送数据给服务器
    loginWithServer(handler, username_, password_);

    // 阻塞并等待服务器响应或超时
    while (handler->f_status_code == STATUS_CODE_LOGIN)
    {
        continue;
    }

    printw("\n");
    switch (handler->f_status_code)
    {
    case STATUS_CODE_LOGIN_SUCESSFUL:
        printw("Login sucessful! Press any key to enter chatroom. ");
        handler->user->username = strdup(username_); // 需要释放
        handler->user->password = strdup(password_); // 需要释放
        handler->ui_state = kChatroom;
        break;
    case STATUS_CODE_LOGIN_FAILED_WROWN_PASSWORD:
        printw("Login Failed: Wrong password. Press any key to continue. ");
        handler->ui_state = kWelcome;
        break;
    case STATUS_CODE_LOGIN_FAILED_USER_NOT_FOUND:
        printw("Login Failed: User not found. Press any key to continue. ");
        handler->ui_state = kWelcome;
        break;
    default:
        printw("Login Failed: Internal error. Press any key to continue. ");
        handler->ui_state = kWelcome;
        break;
    }
    getch(); // 等待用户输入
    return;
}

/// @brief 注册界面
/// @param handler
void doRegister(Chatroom *handler)
{
    // 渲染界面
    char username_[32] = "";
    char password_[32] = "";
    clear();
    printw("Registering CICQ\n");
    printw("Enter username: ");
    getnstr(username_, 31);
    noecho(); // 关闭回显
    printw("Enter password: ");
    getPassword(password_, 31);

    printw("\nSending request to server...\n");

    handler->f_status_code = STATUS_CODE_REGISTER_NORMAL;

    // 发送数据给服务器
    registerWithServer(handler, username_, password_);

    // 阻塞并等待服务器响应或超时
    while (handler->f_status_code == STATUS_CODE_REGISTER_NORMAL)
    {
        continue;
    }

    // 判断返回的结果
    switch (handler->f_status_code)
    {
    case STATUS_CODE_REGISTER_SUCESSFUL: // 注册成功
        printw("Registion Sucessful!\n");
        break;
    case STATUS_CODE_REGISTER_FAILED_TIMEOUT: // 注册失败 - 超时
        printw("Registion Failed: Timeout\n");
        break;
    case STATUS_CODE_REGISTER_FAILED_USERNAME_EXSIST: // 注册失败 - 用户名已存在
        printw("Registion Failed: Username Exsist\n");
        break;
    case STATUS_CODE_ERROR: // 注册失败 - 服务器错误
        printw("Registion Failed: Server internal Error\n");
        break;
    default: // 注册失败 - 未知错误
        printw("Registion Failed: Unknown Error\n");
        break;
    }

    printw("Press any key to continue.\n");
    getch(); // 等待用户输入回车

    handler->ui_state = kWelcome;
    return;
}

/// @brief 聊天室界面
/// @param handler
void doChatroom(Chatroom *handler)
{
    // 配置bufferevent的回调
    bufferevent_setcb(handler->bev, read_message_cb, NULL, event_cb, handler);
    bufferevent_enable(handler->bev, EV_READ | EV_WRITE);

    clear();
    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);

    // 创建聊天窗口和输入窗口
    WINDOW *chat_win, *input_win;
    initChatroomWindows(&chat_win, &input_win, max_y, max_x);

    char input[max_x - 15];
    int ch;    // 用于存储读取的字符
    int i = 0; // 输入缓冲区的索引

    // 设置input_win为非阻塞读取
    nodelay(input_win, TRUE);
    keypad(input_win, TRUE); // 开启特殊键读取，如F1, F2, arrow keys等

    memset(input, 0, sizeof(input));

    printInChatWin(chat_win, "You are now on Server!");

    while (1)
    {
        // 检查是否有新消息
        if (handler->new_message_received)
        {
            // 这里刷新聊天窗口显示新消息
            printMsgInChatWin(chat_win, &handler->message_list->tail->message);
            handler->new_message_received = false;
        }

        // 显示输入提示
        mvwprintw(input_win, 0, 0, "Enter message: [");
        mvwprintw(input_win, 0, max_x - 1, "]");
        wrefresh(input_win);

        // 非阻塞地读取字符
        ch = wgetch(input_win);

        if (ch != ERR)
        { // ERR 表示没有读取到输入
            if (ch == '\n')
            { // Enter键发送消息
                if (strcmp(input, "quit") == 0)
                {
                    sendLogoutRequest(handler->bev, handler->user->username, handler->user->password);
                    handler->ui_state = kExit;
                    break; // 输入quit退出程序
                }

                // 检查私聊消息
                // 检查输入是否以"PM:"开头
                if (strncmp(input, "PM:", 3) == 0)
                {
                    const char *firstColon = strchr(input, ':');
                    const char *secondColon = strchr(firstColon + 1, ':');

                    // 确保找到了两个冒号
                    if (firstColon && secondColon && secondColon > firstColon)
                    {
                        size_t usernameLength = secondColon - firstColon - 1;
                        size_t messageLength = strlen(secondColon) - 1;

                        if (usernameLength > 0 && usernameLength < sizeof(sizeof(char) * 255) && messageLength > 0)
                        {
                            char contact[usernameLength + 1];
                            strncpy(contact, firstColon + 1, usernameLength);
                            contact[usernameLength] = '\0'; // 确保用户名字符串以null终止

                            char message[messageLength + 1];
                            strncpy(message, secondColon + 1, messageLength);
                            message[messageLength] = '\0'; // 确保消息字符串以null终止

                            sendMsgPrivate(handler, message, contact);
                        }
                    }
                }

                // 发送消息到服务器
                sendMsgGroup(handler, input);

                // 清理输入行和输入缓冲区
                memset(input, 0, sizeof(input));
                i = 0;
                werase(input_win);
            }
            else if (ch == KEY_BACKSPACE)
            { // 处理退格键
                if (i > 0)
                    input[--i] = '\0';
            }
            else if (i < sizeof(input) - 2)
            {
                input[i++] = (char)ch;
                input[i] = '\0';
            }

            // 刷新输入窗口以显示当前输入内容
            mvwprintw(input_win, 0, 16, "%s", input);
            wrefresh(input_win);
        }

        // 使用小延迟来避免CPU过度使用
        napms(50);
    }

    // 清理资源
    delwin(chat_win);
    delwin(input_win);
}

void initChatroomWindows(WINDOW **chat_win, WINDOW **input_win, int max_y, int max_x)
{
    *chat_win = newwin(max_y - 1, max_x, 0, 0);
    scrollok(*chat_win, TRUE);

    *input_win = newwin(1, max_x, max_y - 1, 0);
}

void printInChatWin(WINDOW *chat_win, const char *msg)
{
    wprintw(chat_win, "%s\n", msg); // 打印消息并换行
    wrefresh(chat_win);
}

void printMsgInChatWin(WINDOW *chat_win, Message *msg)
{
    char msg_[strlen(msg->payload) + 64];
    struct tm *timeinfo;
    timeinfo = localtime(&msg->rawtime);

    if (msg->msg_type == kMsgBroadcast)
    {
        sprintf(msg_, "[Group][%02d:%02d:%02d][%s]: %s",
                timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec,
                msg->username, msg->payload);
    }
    else if (msg->msg_type == kMsgPrivate)
    {
        sprintf(msg_, "[PM][%02d:%02d:%02d][%s]->[%s]: %s",
                timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec,
                msg->username, msg->contact, msg->payload);
    }
    printInChatWin(chat_win, msg_);
}

void sendMsgPrivate(Chatroom *handler, const char *msg, const char *contact)
{
    // 创建一个新的JSON对象
    cJSON *json = cJSON_CreateObject();

    // 向JSON对象中添加数据
    cJSON_AddStringToObject(json, "username", handler->user->username);
    cJSON_AddStringToObject(json, "contact", contact);
    cJSON_AddStringToObject(json, "payload", msg);
    cJSON_AddNumberToObject(json, "rawtime", (double)time(NULL));
    cJSON_AddNumberToObject(json, "statuscode", (double)STATUS_CODE_MSG_SEND_PRIVATE);
    char *request = cJSON_PrintUnformatted(json);
    sendMsgRequest(handler->bev, request);
    free(request);
    cJSON_Delete(json);
}

void sendMsgGroup(Chatroom *handler, const char *msg)
{
    // 创建一个新的JSON对象
    cJSON *json = cJSON_CreateObject();

    // 向JSON对象中添加数据
    cJSON_AddStringToObject(json, "username", handler->user->username);
    cJSON_AddStringToObject(json, "payload", msg);
    cJSON_AddNumberToObject(json, "rawtime", (double)time(NULL));
    cJSON_AddNumberToObject(json, "statuscode", (double)STATUS_CODE_MSG_SEND_GROUP);
    char *request = cJSON_PrintUnformatted(json);
    sendMsgRequest(handler->bev, request);
    free(request);
    cJSON_Delete(json);
}

/// @brief 退出界面
/// @param handler
void doExit(Chatroom *handler)
{
    clear();
    printw("Bye~\nPress Enter to Exit. ");
    getch(); // 等待用户输入回车
    endwin();
}

/// @brief ncurses 初始化
void initScreen()
{
    initscr();
    // atexit(closeScreen);  // 注册退出时调用的函数
    cbreak();
    noecho();
    keypad(stdscr, TRUE); // 启用功能键
    start_color();        // 启用颜色
    use_default_colors(); // 使用默认颜色
}

/// @brief ncurses 界面退出
void closeScreen()
{
    endwin();
}

/// @brief 获取密码 - 带 * 遮挡
/// @param password
/// @param length
void getPassword(char *password, uint8_t length)
{
    int ch, i = 0;
    while ((ch = getch()) != '\n' && i < length)
    {
        if (ch == KEY_BACKSPACE)
        {
            if (i > 0)
            {
                i--;
                printw("\b \b");
            }
        }
        else
        {
            password[i++] = ch;
            printw("*");
        }
    }
    password[i] = '\0';
}
