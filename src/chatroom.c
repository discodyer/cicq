#include "chatroom.h"

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

/// @brief 处理用户选项
/// @param handler
int handleWelcomeOpinion(Chatroom *handler)
{
    char choice;
    scanw("%c", &choice);
    switch (choice)
    {
    case '1':
        handler->ui_state = kRegister;
        return 0;
    case '2':
        handler->ui_state = kLogin;
        return 0;
    case '3':
    case 'q':
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
        case kWelcome:
            doWelcome(handler);
            break;
        case kLogin:
            doLogin(handler);
            break;
        case kRegister:
            doRegister(handler);
            break;
        case kChatroom:
            doChatroom(handler);
            break;
        case kExit:
            doExit(handler);
            return;
        default:
            return;
            break;
        }
    }
    doExit(handler);
}

void doWelcome(Chatroom *handler)
{
    // 显示开始菜单
    renderWelcome();
    // 处理用户选项
    handleWelcomeOpinion(handler);
}

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

    if ((strcmp(username_, handler->user->username) == 0) && (strcmp(password_, handler->user->password) == 0))
    {
        printw("\n");
        printw("Login sucessful! Press any key to enter chatroom. ");
        getch(); // 等待用户输入
        handler->ui_state = kChatroom;
        return;
    }
    else
    {
        printw("\n");
        printw("Login Failed! Press any key to continue. ");
        getch(); // 等待用户输入
        handler->ui_state = kWelcome;
        return;
    }
}

void doRegister(Chatroom *handler)
{
    clear();
    printw("Registering CICQ\n");
    printw("Enter username: ");
    getnstr(handler->user->username, 31);
    noecho();
    printw("Enter password: ");
    getPassword(handler->user->password, 31);

    printw("\nUsername: %s\n", handler->user->username);
    printw("Password: %s\n", handler->user->password);

    getch(); // 等待用户输入回车

    handler->ui_state = kWelcome;
    return;
}

void doChatroom(Chatroom *handler)
{
    clear();
    printw("Chatroom!\nPress Enter to Exit. ");
    getch(); // 等待用户输入回车
    handler->ui_state = kExit;
    return;
}

void doExit(Chatroom *handler)
{
    clear();
    printw("Bye~\nPress Enter to Exit. ");
    getch(); // 等待用户输入回车
    endwin();
}

void initScreen()
{
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE); // 启用功能键
    start_color();        // 启用颜色
    use_default_colors(); // 使用默认颜色
}

void closeScreen()
{
    endwin();
}

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
