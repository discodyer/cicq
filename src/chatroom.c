#include "chatroom.h"
#include "client_com.h"

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
    clear();
    printw("Chatroom!\nPress Enter to Exit. ");
    getch(); // 等待用户输入回车
    handler->ui_state = kExit;
    return;
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
