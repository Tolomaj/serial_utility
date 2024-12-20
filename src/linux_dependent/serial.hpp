/** Toto je knihovna pro seriovou komunikaci (PRO_WINDOWS) */

#include <string>
#include <limits.h>
#include <iostream>
#include <vector>
#include <fcntl.h>
#include <fstream>
#include <filesystem>
#include <sys/ioctl.h>
#include <unistd.h>
#include <termios.h>
#include <errno.h> // Error integer and strerror() function

#define NOTHING_TO_READ INT_MAX

// keydefs pro linux
#define ARROW_UP    'A'
#define ARROW_DOWN  'B'
#define ARROW_LEFT  'D'
#define ARROW_RIGHT 'C'

#define PAGE_UP  'I'  //todo//?  for linux
#define PAGE_DOWN 'Q' //todo//?  for linux

class Serial {
private:
    int serial_speed = 9600;
    int serial_port = -1;
    std::string port_name = "";
    struct termios tty;


    std::vector<std::string> get_scan(){
        std::vector<std::string> list;

        namespace fs = std::filesystem;
        const std::string DEV_PATH = "/dev/serial/by-id";
        try{
            fs::path p(DEV_PATH);
            if (!fs::exists(DEV_PATH)) return list;
            for (fs::directory_entry de: fs::directory_iterator(p)){
                if (fs::is_symlink(de.symlink_status())){
                    list.push_back("/dev/" + fs::read_symlink(de).filename().string());
                }
            }
        } catch (const fs::filesystem_error &ex) {}
        return list;
    }

public:

    // print ports to std::cout
    void scan_and_print(){
        std::cout << "Active ports:" << std::endl;
        bool gotPort = false;    // in case the port is not found

        for(std::string file_name : get_scan()){
            std::cout << " " << file_name << std::endl;
            gotPort = true;
        }

        if(!gotPort){ std::cout << " No port!" << std::endl; } // to display error message incase no ports found
    };

    // get if port is opened
    inline bool opened(){
        return !(serial_port == -1);
    };

    // get currently opened port
    inline std::string get_port_name(){
        if(opened()){
            return port_name;
        }
        return "NONE";
    };

    // close actual open port
    void close_port(){
        if(serial_port < 2){
            serial_port = -1;
            return;
        }
        close(serial_port);
        serial_port = -1;
    };

    // open port 
    int open_port(std::string string_number){
        
        for(std::string path : get_scan()){
            if(string_number.length() < path.length()){
                // pokud se shoduje koneczprávy nahrádime ho celou cestou
                if(path.substr(path.length() - string_number.length()) == string_number){
                    string_number = path;
                    break;
                }
            }
        }

        serial_port = open(string_number.c_str(), O_RDWR);

        if (serial_port < 0) { goto cant_open; }
        if(tcgetattr(serial_port, &tty) != 0) { goto cant_open; }

        cfsetispeed(&tty, serial_speed);
        cfsetospeed(&tty, serial_speed);

        // Save tty settings, also checking for error

        if (tcsetattr(serial_port, TCSANOW, &tty) != 0) { goto cant_open; }

        port_name = string_number;

        return 0;

      cant_open:
        this->close_port();
        return -1;
    };

    //nastaví rychlost sběrnice
    void set_speed(int speed){
        serial_speed = speed;
        if(opened()){
            std::string opened = get_port_name();
            this->close_port();
            this->open_port(opened);
        }
    };

    //vrátí aktuální nastavenou rychlost seriového portu
    int get_speed(){
        return serial_speed;
    };

    //přečte jeden znak z seriové linky
    int read_char(){
        int bytes, c;
        ioctl(serial_port, FIONREAD, &bytes);
        if(bytes > 0){
            read(serial_port, (char*)(&c), 1);
            return c;
        }
        return NOTHING_TO_READ;
    };

    // přečte vše co došlo na seriovou linku
    std::string read_s(){
        int serial_read = read_char();
        std::string input = "";
        while(serial_read != NOTHING_TO_READ){
            input += (char)serial_read;
            serial_read = read_char();
        }
        return input;
    };

    // zapíše znak na seriovou linku
    bool write(char ch){
        //todo
        return false;
    };
    
    ~Serial(){
        close_port();
    };
};