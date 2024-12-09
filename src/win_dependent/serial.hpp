/** Toto je knihovna pro seriovou komunikaci (PRO_WINDOWS) */

#include <string>
#include <limits.h>
#include <windows.h>

//inspirováno https://stackoverflow.com/questions/6036716/serial-comm-using-writefile-readfile

#define NOTHING_TO_READ INT_MAX

class Serial {
private:
    HANDLE file = NULL;
    COMMTIMEOUTS timeouts;
    std::string opened_port = "NONE";
    char buffer[1];
    DCB port;
    int speed = 115200;
public:

    // print ports to std::cout
    void scan_and_print();

    // get currently opened port
    inline std::string get_port_name();

    // get if port is opened
    inline bool opened();

    // close actual open port
    void close_port();

    // open port 
    int open_port(std::string string_number);

    //nastaví rychlost sběrnice
    void set_speed(int speed);

    //vrátí aktuální nastavenou rychlost seriového portu
    int get_speed();

    //přečte jeden znak z seriové linky
    int read_char();

    // přečte vše co došlo na seriovou linku
    std::string read();

    // zapíše znak na seriovou linku
    bool write(char ch);
    
    ~Serial();
};


// print ports
void Serial::scan_and_print() {
    std::cout << "Active ports:" << std::endl;

    char lpTargetPath[5000]; // buffer to store the path of the COMPORTS
    bool gotPort = false;    // in case the port is not found

    // checking ports from COM0 to COM255
    for(int i=0; i<255; i++) {
        std::string ComName = "COM" + std::to_string(i); // converting to COM0, COM1, COM2

        // Test the return value and error if any
        if(QueryDosDevice(ComName.c_str(), lpTargetPath, 5000) != 0) { //QueryDosDevice returns zero if it didn't find an object
            std::cout << ComName << std::endl;
            gotPort = true;
        }
    }

    if(!gotPort){ std::cout << "No port!" << std::endl; } // to display error message incase no ports found
}


inline std::string Serial::get_port_name(){
    return opened_port;
}


inline bool Serial::opened(){
    return file != NULL;
}


void Serial::close_port(){
    if(file == NULL){
        return; // port is already closed
    }
    CloseHandle(file);
    file = NULL;
    opened_port = "NONE"; 
}


//int Serial::open(int port_id){
//    return open(std::to_string(port_id));
//}


int Serial::open_port(std::string string_number){
    
    // pokud je port "" nebo none neřešíme (nastavení třeba otevírá port na NONE)
    if(string_number == "" || string_number == "NONE"){
        return 4; 
    }
    
    //pokud je otevřený jiný port, zavře ho
    if(file != NULL){ close_port(); }

    std::string port_name = "";

    // funkce přijímá jak 12 tak COM12 
    if(string_number.find_first_not_of("0123456789") == std::string::npos){
        port_name = "\\\\.\\COM" + string_number;
    }else{
        port_name = "\\\\.\\" + string_number;
    }
    
    file = CreateFile(port_name.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);

    if ( INVALID_HANDLE_VALUE == file) {
        file = NULL;
        return 1; 
    }

    // get the current DCB, and adjust a few bits to our liking.
    memset(&port, 0, sizeof(port));
    port.DCBlength = sizeof(port);
    std::string com_settings_string = "baud=" + std::to_string(speed) + " parity=n data=8 stop=1";
    if (!GetCommState(file, &port) || !BuildCommDCB(com_settings_string.c_str(), &port) || !SetCommState(file, &port)){
        close_port();
        return 2;
    }

    timeouts.ReadIntervalTimeout = 1;
    timeouts.ReadTotalTimeoutMultiplier = 1;
    timeouts.ReadTotalTimeoutConstant = 1;
    timeouts.WriteTotalTimeoutMultiplier = 1;
    timeouts.WriteTotalTimeoutConstant = 1;

    if (!SetCommTimeouts(file, &timeouts)){
        close_port();
        return 3;
    }

    opened_port = port_name;
    return 0;
}


//nastaví rychlost sběrnice
void Serial::set_speed(int speed){  
    this->speed = speed; 
    if(file != NULL){
        // reopen port
        std::string port_name = get_port_name();
        close_port();
        open_port(port_name);
    }
}


//vrátí aktuální nastavenou rychlost seriového portu
int Serial::get_speed(){  
    return this->speed; 
}


//přečte jeden znak z seriové linky
int Serial::read_char(){
    DWORD read = false;
    
    ReadFile(file, buffer, sizeof(buffer), &read, NULL);
    if ( read ){
        return buffer[0];
    }
    return NOTHING_TO_READ;
}


// přečte vše co došlo na seriovou linku
std::string Serial::read(){
    int serial_read = read_char();
    std::string input = "";
    while(serial_read != NOTHING_TO_READ){
        input += (char)serial_read;
        serial_read = read_char();
    }
    return input;
}


// zapíše znak na seriovou linku
bool Serial::write(char ch){
    DWORD writte = false;
    WriteFile(file, &ch, 1, &writte, NULL);
    return (bool)writte;
}


Serial::~Serial(){
    close_port();
}