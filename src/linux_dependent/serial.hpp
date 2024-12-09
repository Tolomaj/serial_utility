/** Toto je knihovna pro seriovou komunikaci (PRO_WINDOWS) */

#include <string>
#include <limits.h>
#include <iostream>
#include <string>
#include <fcntl.h>
#include <fstream>
#include <filesystem>
#include <sys/ioctl.h>
#include <unistd.h>
#include <termios.h>

#define NOTHING_TO_READ INT_MAX


class Serial {
private:
    int serial_speed = 9600;
    int serial_port = 0;
    std::string port_name = "";
    struct termios tty;
public:

    // print ports to std::cout
    void scan_and_print(){
        std::cout << "Active ports:" << std::endl;
        bool gotPort = false;    // in case the port is not found

        namespace fs = std::filesystem;
        const std::string DEV_PATH = "/dev/serial/by-id";
        try{
            fs::path p(DEV_PATH);
            if (!fs::exists(DEV_PATH)) goto end;
            for (fs::directory_entry de: fs::directory_iterator(p)){
                if (fs::is_symlink(de.symlink_status())){
                    fs::path symlink_points_at = fs::read_symlink(de);
                    std::cout << symlink_points_at.filename() << std::endl;
                    gotPort = true;
                }
            }
        } catch (const fs::filesystem_error &ex) {}
        
        end:
        if(!gotPort){ std::cout << "No port!" << std::endl; } // to display error message incase no ports found
    };

    // get if port is opened
    inline bool opened(){
        return serial_port != 0;
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
        close(serial_port);
    };

    // open port 
    int open_port(std::string string_number){
        
        serial_port = open(string_number.c_str(), O_RDWR);

        if (serial_port < 0) { goto cant_open; }
        if(tcgetattr(serial_port, &tty) != 0) { goto cant_open; }

        cfsetispeed(&tty, serial_speed);
        cfsetospeed(&tty, serial_speed);

        // Save tty settings, also checking for error

        if (tcsetattr(serial_port, TCSANOW, &tty) != 0) { goto cant_open; }

        port_name = string_number;

        std::cout << "opening: " << string_number << std::endl;
        return true;

      cant_open:
        this->close_port();
        return false;
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
        return 0; // todo
    };

    // přečte vše co došlo na seriovou linku
    std::string read(){
        return ""; //todo
    };

    // zapíše znak na seriovou linku
    bool write(char ch){
        return false;
    };
    
    ~Serial(){};
};