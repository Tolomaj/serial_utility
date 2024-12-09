/** Toto je soubor windows konzole (PRO_WINDOWS) */
#include <windows.h>

class Console {
private:
    // handler console
    HANDLE WINAPI con ,conIN;
public:
    Console(){
        con = GetStdHandle(STD_OUTPUT_HANDLE);
        conIN = GetStdHandle(STD_INPUT_HANDLE);
        SetConsoleOutputCP(CP_UTF8);
        SetConsoleCP(CP_UTF8);
    };

    // print string wia fastest platform dependent way 
    void platform_print(std::string text){
        DWORD lr;
        WriteConsoleA(con, text.c_str(), text.length(), &lr, NULL);
    };

    // nastaví pozici kurzoru na poslední lince
    void set_cursor_back(int positionX){
        CONSOLE_SCREEN_BUFFER_INFO cbsi;
        if (GetConsoleScreenBufferInfo(con, &cbsi)){
            cbsi.dwCursorPosition.X = cbsi.dwCursorPosition.X - positionX;
            SetConsoleCursorPosition( con, cbsi.dwCursorPosition); 
        }
    }

    // nastaví pozici kurzoru na poslední lince
    int get_cursor_X_position(){
        CONSOLE_SCREEN_BUFFER_INFO cbsi;
        if (GetConsoleScreenBufferInfo(con, &cbsi)){
            return cbsi.dwCursorPosition.X;
        }
        return 0;
    }

    bool has_any_char(){
        DWORD char_num;
        GetNumberOfConsoleInputEvents(conIN,&char_num);
        return char_num > 1;
    }

    int get_character(){
        int character = (char)getch();
        return character;
    }

    int get_console_width(){
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        int columns, rows;
    
        GetConsoleScreenBufferInfo(con, &csbi);
        return csbi.srWindow.Right - csbi.srWindow.Left + 1;
    }

    // schová/ukáže kurzor
    void hide_cursor(bool hide){
        CONSOLE_CURSOR_INFO     cursorInfo;
        GetConsoleCursorInfo(con, &cursorInfo);
        cursorInfo.bVisible = !hide; 
        SetConsoleCursorInfo(con, &cursorInfo);
    }

    void clear(){
        system("cls");
    }

    ~Console(){

    };
};
