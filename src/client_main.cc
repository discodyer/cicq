#include <ncurses.h>
#include <string.h>
#include <time.h>

typedef struct
{
    char username[20];
    char password[20];
} user;

void init_screen();
void close_screen();
void get_password(char *password);
void chatroom(user *currentUser);
void print_in_middle(WINDOW *win, int starty, int startx, int width, char *string);
int min(int a, int b);
int max(int a, int b);
void render_header(const char *chatroom_name, const char *user_name);
int handle_input(WINDOW *input_win, user *current_user);

int main()
{
    user currentUser;
    char password[20];
    char username[20];
    int choice;

    // 初始化ncurses
    init_screen();

    // 用户注册/登录界面
    printw("1. Register\n");
    printw("2. Login\n");
    printw("Choose an option: ");
    echo();

    scanw("%d", &choice);
    printw("Enter username: ");
    getnstr(username, 19);
    noecho();
    printw("Enter password: ");
    get_password(password);

    strcpy(currentUser.username, username);
    strcpy(currentUser.password, password);

    // 转到聊天室
    chatroom(&currentUser);

    // 关闭ncurses
    close_screen();
    return 0;
}

void init_screen()
{
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE); // 启用功能键
    start_color();        // 启用颜色
    use_default_colors(); // 使用默认颜色
}

void close_screen()
{
    endwin();
}

void get_password(char *password)
{
    int ch, i = 0;
    while ((ch = getch()) != '\n' && i < 19)
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

void chatroom(user *currentUser)
{
    int row, col;
    // char message[80];
    char input[256];
    time_t rawtime;
    struct tm *timeinfo;

    clear();
    getmaxyx(stdscr, row, col);

    render_header("CICQ", currentUser->username);

    WINDOW *input_win = newwin(1, col - 2, row - 1, 1);

    while (1)
    {
        // mvprintw(row - 2, 0, "Enter message: ");
        // echo();
        // getnstr(message, 79);
        // noecho();
        // if (strcmp(message, "exit") == 0)
        //     break;

        // // 获取当前时间
        // time(&rawtime);
        // timeinfo = localtime(&rawtime);

        // mvprintw(row - 4, 0, "[%02d:%02d:%02d] %s: %s", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, currentUser->username, message);
        // refresh();

        if (wgetnstr(stdscr, input, 255) == ERR)
        {
            continue; // 处理错误或用户取消输入
        }

        // 检查是否以!开头
        if (input[0] == '!')
        {
            attron(A_REVERSE); // 反色显示指令
            mvprintw(LINES - 2, 0, "Enter message: %s", input);
            attroff(A_REVERSE);
            refresh();
            // 稍作延迟以便用户看到反色效果
            napms(500);
        }

        if (handle_input(input_win, currentUser) == -1)
        {
            return;
        }

        move(LINES - 2, 0);
        clrtoeol();
    }
}

void print_in_middle(WINDOW *win, int starty, int startx, int width, char *string)
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

void render_header(const char *chatroom_name, const char *user_name)
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
    print_in_middle(stdscr, 1, 0, col, header);
}

int handle_input(WINDOW *input_win, user *current_user)
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