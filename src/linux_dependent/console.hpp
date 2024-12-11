/** Toto je soubor LINUX konzole (PRO_LINUX) */
#include <iostream>
#include <sys/ioctl.h>
#include <termios.h> 
#include <unistd.h> 
#include <stdio.h> /* reads from keypress, doesn't echo */ 
#include <fcntl.h>

class Console {
private:

    void setTerminalMode(bool enable) {
        struct termios term;
        tcgetattr(STDIN_FILENO, &term);
        if (enable) {
            // Disable canonical mode and enable character-at-a-time input
            term.c_lflag &= ~(ICANON | ECHO);  // Disable canonical mode and echo
            term.c_cc[VMIN] = 1;  // Minimum number of characters to read
            term.c_cc[VTIME] = 0; // No timeout
            tcsetattr(STDIN_FILENO, TCSANOW, &term);
        } else {
            // Restore terminal settings to normal (canonical mode)
            term.c_lflag |= (ICANON | ECHO);
            tcsetattr(STDIN_FILENO, TCSANOW, &term);
        }
    }

public:
    Console(){
      setTerminalMode(true);
    };

    // print string wia fastest platform dependent way 
    void platform_print(std::string text){
        std::cout << text ; //todo pokud bude pomalé předělat
    };

    // nastaví kurzor n znaků dozadu
    void set_cursor_back(int n){
        if(n > 0){
            printf("\033[%dD",n); 
        }
    }

    // nastaví pozici kurzoru na poslední lince
    int get_cursor_X_position(){
        // Send the ANSI escape code to query the cursor position
        int x,y;
        printf("\033[6n");  
        scanf("\033[%d;%dR", &x, &y);
        return x; 
    }

    // je nějaký znak k přijetí ?
    bool has_any_char(){ //todo revizuj
        struct termios oldt, newt;
        
        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
        int oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
        fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
        
        int ch = getchar();
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
        fcntl(STDIN_FILENO, F_SETFL, oldf);
        
        if(ch != EOF){
            ungetc(ch, stdin);
            return 1;
        }
        return 0;
    }

    // returns character
    int get_character(){
        return getchar();
    }

    int get_console_width(){
        struct winsize w;
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
        return w.ws_col;
    }

    // schová/ukáže kurzor
    void hide_cursor(bool hide){
        std::cout << (hide ? "\033[?25h" : "\033[?25l");
    }

    void clear(){
        std::system("clear");
    }

    ~Console(){
        setTerminalMode(false);
    };
};
