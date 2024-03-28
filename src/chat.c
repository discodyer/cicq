#include <ncurses.h>
#include <string.h>

void init_windows(WINDOW **chat_win, WINDOW **input_win, int max_y, int max_x);
void print_in_chat_win(WINDOW *chat_win, const char *msg);

int main() {
    // 初始化ncurses
    initscr();
    cbreak();
    echo(); // 允许显示输入
    keypad(stdscr, TRUE);

    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);

    // 创建聊天窗口和输入窗口
    WINDOW *chat_win, *input_win;
    init_windows(&chat_win, &input_win, max_y, max_x);

    char input[max_x - 15];
    while (1) {
        // 显示输入提示并获取输入
        mvwprintw(input_win, 0, 0, "Enter message:[");
        mvwprintw(input_win, 0, max_x-1, "]");
        wmove(input_win, 0, 15); // 将光标移动到输入区域
        wgetnstr(input_win, input, sizeof(input) - 1);

        if (strcmp(input, "quit") == 0) break; // 输入quit退出程序

        // 打印到聊天窗口
        print_in_chat_win(chat_win, input);

        // 清理输入行
        werase(input_win);
    }

    // 清理
    delwin(chat_win);
    delwin(input_win);
    endwin();

    return 0;
}

void init_windows(WINDOW **chat_win, WINDOW **input_win, int max_y, int max_x) {
    *chat_win = newwin(max_y - 1, max_x, 0, 0);
    scrollok(*chat_win, TRUE);

    *input_win = newwin(1, max_x, max_y - 1, 0);
}

void print_in_chat_win(WINDOW *chat_win, const char *msg) {
    wprintw(chat_win, "%s\n", msg); // 打印消息并换行
    wrefresh(chat_win);
}
