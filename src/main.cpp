#undef WINDOWS
#undef LINUX

#define WINDOWS // WINDOWS | LINUX

#include "stdint.h"

// windows dependencies
#ifdef WINDOWS
    #include "win_dependent/timer.hpp"
    #include "win_dependent/executer.hpp"
    #include "win_dependent/serial.hpp" 
    #include <conio.h>
#endif
#ifdef LINUX
    #include "linux_dependent/timer.hpp"
    #include "linux_dependent/executer.hpp"
    #include "linux_dependent/serial.hpp"
#endif

// globální knihvona
#include "settings.hpp" 

//ostatní knihovny
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <sstream>  
#include <signal.h>
#include "colors.h"
#include "terminal.hpp"
#include "arg_parser.hpp"
#include "tests.hpp"
#include "command_list.h"

using namespace std;


// objekt termiálu
Terminal * terminal;
Console * console;
Serial * serial;
ProgramExecuter * executer;
bool serial_direct_input = false;

// uložení a skončení programu
void save_and_exit_appdata(){
    terminal->save_logs(true);

    if(settings.save_on_end){
        save_settings();
    }

    delete serial;
    delete terminal;
    delete executer;
    console->clear();
    exit(0);
}

// do command from terminal
std::string on_command(std::string command){
    // vytvoříme lower command kde jsou písmena na menší
    std::string lower_command = command;
    std::transform(lower_command.begin(), lower_command.end(), lower_command.begin(), [](unsigned char c){ return std::tolower(c); } );

    if(lower_command == cmd::name(cmd::QUIT) || lower_command == cmd::name(cmd::QUIT1)){
        save_and_exit_appdata();
        return ""; // tady už program skončil 
    }

    if (lower_command == cmd::name(cmd::SAVE_ON_QUIT_OFF) || lower_command == cmd::name(cmd::SAVE_ON_QUIT_OFF1) || lower_command == cmd::name(cmd::SAVE_ON_QUIT_OFF2)){
        settings.save_on_end = false;
        return "Saving settings is off.";
    }
    if (lower_command == cmd::name(cmd::SAVE_ON_QUIT_ON) || lower_command == cmd::name(cmd::SAVE_ON_QUIT_ON1)){
        settings.save_on_end = true;
        return "Saving settings is on.";
    }


    if(lower_command == cmd::name(cmd::PING)){
        return "pong";
    }

    if(lower_command == cmd::name(cmd::TEST)){
        tests(terminal);
        return "tests";
    }

    if(lower_command == cmd::name(cmd::FTEST)){
        ftests(terminal);
        return "ftests";
    }

    if(lower_command == cmd::name(cmd::HELP)){
        console->clear();
        print_help();
        std::cout << "Press any character to continue";
        continuing_with_cahracter = (char)console->get_character();
        terminal->redraw();
        return "";
    }

    if(lower_command == cmd::name(cmd::HALT)){
        // zavřeme port
        std::string revive_port = serial->get_port_name();
        serial->close_port();

        // čekáme na interakci od uživatele
        std::cout << std::endl << "Serial line halted." << std::endl;
        std::cout << "Press any character to continue" << std::endl;
        continuing_with_cahracter = (char)console->get_character();

        // překreslíme texty
        terminal->redraw();

        //znovu otevřeme seriovou linku
        if(serial->open_port(revive_port) != 0){
            return "Cant reopen serial line!!";
        };
        return "Serial line reopened sucesfuly!";
    }

    if(lower_command == cmd::name(cmd::REOPEN) || lower_command == cmd::name(cmd::REOPEN1)){
        // zavřeme port
        std::string revive_port = serial->get_port_name();
        serial->close_port();

        //znovu otevřeme seriovou linku
        if(serial->open_port(revive_port) != 0){
            return "Cant reopen serial line!!";
        };
        return "Serial line reopened sucesfuly!";
    }

    if(lower_command.length() >= cmd::name_len(cmd::SEND) && lower_command.substr(0,cmd::name_len(cmd::SEND)) == cmd::name(cmd::SEND)){
        if(!serial->opened()){ return "Serial port is closed! So cannot send message!"; }
        std::string str = command.substr(cmd::name_len(cmd::SEND),-1);
        for(char a : str){
            serial->write(a);
        }
        serial->write('\n');
        return "Message:" + str + " printed.";
    }
    if(lower_command.length() >= cmd::name_len(cmd::SEND1) && lower_command.substr(0,cmd::name_len(cmd::SEND1)) == cmd::name(cmd::SEND1)){
        if(!serial->opened()){ return "Serial port is closed! So cannot send message!"; }
        std::string str = command.substr(cmd::name_len(cmd::SEND1),-1);
        for(char a : str){
            serial->write(a);
        }
        serial->write('\n');
        return "Message:" + str + " printed.";
    }
    
    // příkaz SPEED pro nastavení seril speedu
    if(lower_command.length() >= cmd::name_len(cmd::SPEED) && lower_command.substr(0,cmd::name_len(cmd::SPEED)) == cmd::name(cmd::SPEED)){
        try{
            settings.serial_speed = std::stoi(command.substr(cmd::name_len(cmd::SPEED),-1));
            serial->set_speed(settings.serial_speed);
            return "New serial speed is: " + command.substr(cmd::name_len(cmd::SPEED),-1);
        } catch(...) {
            return "Invalid number";
        }
    }
    if(lower_command.length() >= cmd::name_len(cmd::SPEED1) && lower_command.substr(0,cmd::name_len(cmd::SPEED1)) == cmd::name(cmd::SPEED1)){
        try{
            settings.serial_speed = std::stoi(command.substr(cmd::name_len(cmd::SPEED1),-1));
            serial->set_speed(settings.serial_speed);
            return "New serial speed is: " + command.substr(cmd::name_len(cmd::SPEED1),-1);
        } catch(...) {
            return "Invalid number";
        }
    }
    if(lower_command.length() >= cmd::name_len(cmd::SPEED2) && lower_command.substr(0,cmd::name_len(cmd::SPEED2)) == cmd::name(cmd::SPEED2)){
        try{
            settings.serial_speed = std::stoi(command.substr(cmd::name_len(cmd::SPEED2),-1));
            serial->set_speed(settings.serial_speed);
            return "New serial speed is: " + command.substr(cmd::name_len(cmd::SPEED2),-1);
        } catch(...) {
            return "Invalid number";
        }
    }

    // příkaz SPEED pro zobrazení rychlosti serioveho portu
    if(lower_command.length() >= cmd::name_len(cmd::SPEED_STAT) && lower_command.substr(0,cmd::name_len(cmd::SPEED_STAT)) == cmd::name(cmd::SPEED_STAT)){
            return "Actual serial speed is: " + std::to_string(serial->get_speed()) ;
    }
    if(lower_command.length() >= cmd::name_len(cmd::SPEED_STAT1) && lower_command.substr(0,cmd::name_len(cmd::SPEED_STAT1)) == cmd::name(cmd::SPEED_STAT1)){
            return "Actual serial speed is: " + std::to_string(serial->get_speed()) ;
    }
    if(lower_command.length() >= cmd::name_len(cmd::SPEED_STAT2) && lower_command.substr(0,cmd::name_len(cmd::SPEED_STAT2)) == cmd::name(cmd::SPEED_STAT2)){
            return "Actual serial speed is: " + std::to_string(serial->get_speed()) ;
    }

    // příkaz OPEN
    std::string port_opening_name = "";
    if(lower_command.length() >= cmd::name_len(cmd::OPEN) && lower_command.substr(0,cmd::name_len(cmd::OPEN)) == cmd::name(cmd::OPEN)){
        port_opening_name = command.substr(cmd::name_len(cmd::OPEN),-1);
    }else
    if(lower_command.length() >= cmd::name_len(cmd::OPEN1) && lower_command.substr(0,cmd::name_len(cmd::OPEN1)) == cmd::name(cmd::OPEN1)){
        port_opening_name = command.substr(cmd::name_len(cmd::OPEN1),-1);
    }
    if(!port_opening_name.empty()){
        //začneme počítat hodiny on nuly
        terminal->reset_clocks();
        bool opened = serial->open_port(port_opening_name) == 0;

        terminal->set_serial_connected_status(opened);
        if(opened){
            settings.port = port_opening_name;
            return "Port opened.";
        }else{
            return "Cant open port!";
        }
    }

    // příkaz SCAN
    if(lower_command == cmd::name(cmd::SCAN)){
        console->clear();
        serial->scan_and_print();
        std::cout << "Press any character to continue";

        continuing_with_cahracter = (char)console->get_character();
        
        terminal->redraw();
        return "";
    }
    
    // příkaz PORT
    if(lower_command == cmd::name(cmd::PORT) || lower_command == cmd::name(cmd::PORT1)){
        return "Opened Port is: " + serial->get_port_name();
    }
    
    // příkaz CLOSE pro uzavření portu
    if(lower_command == cmd::name(cmd::CLOSE) || lower_command == cmd::name(cmd::CLOSE1)){
        serial->close_port();
        terminal->set_serial_connected_status(serial->opened());
        settings.port = "NONE";
        return "Port closed.";
    }

    // příkaz CLOSE pro uzavření portu
    if(lower_command == cmd::name(cmd::DIRECT) || lower_command == cmd::name(cmd::DIRECT1) || lower_command == cmd::name(cmd::DIRECT2)){
        if(!serial->opened()){ return "Serial port is closed! So cannot run direct mode!"; }
        serial_direct_input = true;
        terminal->show_custom_status_message("Direct mode is runing.(press ESC to exit)");
        return "";
    }

    // příkaz SAVE_SETTINGS pro uložení nastavení
    if(lower_command == cmd::name(cmd::SAVE_SETTING) || lower_command == cmd::name(cmd::SAVE_SETTING1)){
        save_settings();
        return "Settings Saved";
    }

    // příkaz show_zero_return_value pro uložení nastavení
    if(lower_command == cmd::name(cmd::SHOW_RETURN_ZERO_ON) || lower_command == cmd::name(cmd::SHOW_RETURN_ZERO_ON1)){
        settings.show_sucess_program_return_code = true;
        return "show_return_zero is seted on";
    }

    // příkaz show_zero_return_value pro uložení nastavení
    if(lower_command == cmd::name(cmd::SHOW_RETURN_ZERO_OFF) || lower_command == cmd::name(cmd::SHOW_RETURN_ZERO_OFF1)){
        settings.show_sucess_program_return_code = false;
        return "show_return_zero is seted off";
    }

    std::string command_to_run = "";
    bool exec_save = false;

    // příkaz MAKE
    if(lower_command == cmd::name(cmd::MAKE) || lower_command.length() >= cmd::name_len(cmd::MAKE1) && lower_command.substr(0,cmd::name_len(cmd::MAKE1)) == cmd::name(cmd::MAKE1)){
        command_to_run = command;
    }

    int force_to_show_returncode = false;
    // příkaz EXEC
    if(lower_command.length() >= cmd::name_len(cmd::EXEC) && lower_command.substr(0,cmd::name_len(cmd::EXEC)) == cmd::name(cmd::EXEC)){
        command_to_run = command.substr(cmd::name_len(cmd::EXEC),-1);
    }
    // příkaz EXEC
    if(lower_command.length() >= cmd::name_len(cmd::EXEC1) && lower_command.substr(0,cmd::name_len(cmd::EXEC1)) == cmd::name(cmd::EXEC1)){
        command_to_run = command.substr(cmd::name_len(cmd::EXEC1),-1);
    }
    // příkaz EXEC s neukončováním v žádném return code 
    if(lower_command.length() >= cmd::name_len(cmd::EEXEC) && lower_command.substr(0,cmd::name_len(cmd::EEXEC)) == cmd::name(cmd::EEXEC)){
        command_to_run = command.substr(cmd::name_len(cmd::EEXEC),-1);
        force_to_show_returncode = true;
    }



    // příkaz SAFE_EXEC
    if(lower_command.length() >= cmd::name_len(cmd::SAFE_EXEC) && lower_command.substr(0,cmd::name_len(cmd::SAFE_EXEC)) == cmd::name(cmd::SAFE_EXEC)){
        command_to_run = command.substr(cmd::name_len(cmd::SAFE_EXEC),-1);
        exec_save = true;
    }
    // příkaz SAFE_EXEC
    if(lower_command.length() >= cmd::name_len(cmd::SAFE_EXEC1) && lower_command.substr(0,cmd::name_len(cmd::SAFE_EXEC1)) == cmd::name(cmd::SAFE_EXEC1)){
        command_to_run = command.substr(cmd::name_len(cmd::SAFE_EXEC1),-1);
        exec_save = true;
    }
    // příkaz SAFE_EXEC
    if(lower_command.length() >= cmd::name_len(cmd::SAFE_EXEC2) && lower_command.substr(0,cmd::name_len(cmd::SAFE_EXEC2)) == cmd::name(cmd::SAFE_EXEC2)){
        command_to_run = command.substr(cmd::name_len(cmd::SAFE_EXEC2),-1);
        exec_save = true;
    }


    // zpracování příkazu EXEC & MAKE
    if(command_to_run != ""){

        // vymazání dosud cmd
        console->clear();

        std::string revive_port = "";
        // některé make příkazi chtějí vyplou seriovou linku.
        if(is_serial_command(command_to_run) || exec_save){
            revive_port = serial->get_port_name();
            serial->close_port();
        }
        
        int return_value = executer->execute(command_to_run,force_to_show_returncode);

        // pokud je to nastavené při ůspěchu rovnou program končí
        if(!(settings.show_sucess_program_return_code && return_value == 0 && force_to_show_returncode == false)){ 
            // čekání na userinput (+ zobrazení návratové hodnoty)
            std::cout << FOREGRY << "Command done with return value [" << FOREWHT << return_value << FOREGRY << "]. Press any character to continue" << RESETTEXT;
            continuing_with_cahracter = (char)console->get_character();
        }

        //začneme počítat hodiny on nuly
        terminal->reset_clocks();

        // pokud před příkazem byla zavřená seriová linka znovu jí otevře
        if(revive_port != ""){
            if(serial->open_port(revive_port) != 0){
                return "Cant reopen serial line!!";
            };
        }
        std::string message = "Execution Complete. With return code ";
        if(return_value == 0){
            message.append(FOREWHT);
        }else{
            message.append(FORERED); 
        }
        message.append(to_string(return_value));
        message.append(RESETTEXT); 
        return message.c_str(); // výpis si řeší executor :)
    }

    // příkaz FOLOWING_SIGN pro nastavení znamínka náslendíka
    if(lower_command == cmd::name(cmd::FOLLOW_SIGN_ON) || lower_command == cmd::name(cmd::FOLLOW_SIGN_ON1)){
        settings.print_foloving_sign = true;
        terminal->redraw();
        return "Sign on.";
    }
    if(lower_command == cmd::name(cmd::FOLLOW_SIGN_OFF) || lower_command == cmd::name(cmd::FOLLOW_SIGN_OFF1)){
        settings.print_foloving_sign = false;
        terminal->redraw();
        return "Sign off.";
    }

    // příkaz PRIORITY_COLOR pro nastavení znamínka náslendíka
    if(lower_command == cmd::name(cmd::PRIORITY_COLOR_ON) || lower_command == cmd::name(cmd::PRIORITY_COLOR_ON1)){
        settings.priority_coloring = true;
        terminal->redraw();
        return "Sign on.";
    }
    if(lower_command == cmd::name(cmd::PRIORITY_COLOR_OFF) || lower_command == cmd::name(cmd::PRIORITY_COLOR_OFF1)){
        settings.priority_coloring = false;
        terminal->redraw();
        return "Sign off.";
    }

    // příkaz DECORATION OFF
    if(lower_command == cmd::name(cmd::DECORATION_OFF) || lower_command == cmd::name(cmd::DECORATION_OFF1)){
        settings.print_arival_time = false;
        settings.print_file_location = false;
        settings.print_foloving_sign = false;
        settings.in_text_highliting = false;
        settings.priority_coloring = false;
        settings.header_fillup = 0;
        terminal->redraw();
        return "Decorations off.";
    }

    // příkaz DECORATION ON
    if(lower_command == cmd::name(cmd::DECORATION_ON) || lower_command == cmd::name(cmd::DECORATION_ON1)){
        settings.print_arival_time = true;
        settings.print_file_location = true;
        settings.print_foloving_sign = true;
        settings.in_text_highliting = true;
        settings.priority_coloring = true;
        settings.header_fillup = 30;
        terminal->redraw();
        return "Decorations on.";
    }
    
    // příkaz USE_CONFIRM_CHARACTER
    if(lower_command == cmd::name(cmd::USE_CONFIRM_CHARACTER_ON) || lower_command == cmd::name(cmd::USE_CONFIRM_CHARACTER_ON1)){
        settings.use_confirm_character = true;
        return "Usage of confirm character is on.";
    }
    if(lower_command == cmd::name(cmd::USE_CONFIRM_CHARACTER_OFF) || lower_command == cmd::name(cmd::USE_CONFIRM_CHARACTER_OFF1)){
        settings.use_confirm_character = false;
        return "Usage of confirm character is off.";
    }

    return "Neznami prikaz";
}


//Obsluha CTRL + C signálu
void signal_callback_handler(int signum) {
    if(executer->is_runing()){
        // Vnější program stále pěží. Předáváme tedy signál tam.
        std::cout << FOREYEL << "Signal " << FOREWHT << signum << FOREYEL << " propagated from master app!" << RESETTEXT << std::endl;
        signal(SIGINT, signal_callback_handler); // nastavíme hlídání signálu znovu
    }else{
        // uživatel chce ukončit náš program. (uložíme a skončíme)
        console->clear();
        std::cout << FOREGRY << "Saving settings and exiting. Bye :)" << RESETTEXT << std::endl;
        save_and_exit_appdata();
    }
}


int main(int argc, char** argv){
    console = new Console();

    console->clear();
    load_settings();
    if(signal(SIGINT, signal_callback_handler) == SIG_ERR ) exit(EXIT_FAILURE );


    serial = new Serial();
    terminal = new Terminal(&on_command,console);
    executer = new ProgramExecuter();
    ArgParser arg_parser;


    // PARSE PROGRAM --------------------------------------
    std::string parsing_error = "";

    if(argc > 1){
        switch (arg_parser.process_arguments(argc-1,&argv[1])) {

        case OK_DUPLICIT:
            parsing_error = "Warning: Duplicit argument!";
            break;
        case OK_UNKNOWN_PARAMETER:
            parsing_error = "Warning: Unknown parameter!";
            break;

        case OK:
        case OK_END_ERR: break;

        case ERR_MISSING_ARGUMENT:
            std::cout << "Argument without -- or - !" << std::endl;
            exit(ERR_MISSING_ARGUMENT);
            break;
        case ERR_MISSING_VALUE:
            std::cout << "Missing argument value !" << std::endl;
            exit(ERR_MISSING_VALUE);
            break;
        case ERR_DUPLICIT_FILE:
            std::cout << "Duplicit file finded!" << std::endl;
            exit(ERR_DUPLICIT_FILE);
            break;
        }

        for (auto command : arg_parser.get_commands()){
            terminal->do_command(command);
        }

    }


    // OPEN SERIAL PORT IF NOT OPENED -------------------------

    serial->set_speed(settings.serial_speed);

    if(settings.port != "NONE" && settings.port != ""  ){ // todo neotevírat pokud je none nebo ""
        if(!serial->opened() && serial->open_port(settings.port) != 0 ){
            terminal->set_command_response( "Cant open serial line!! " + parsing_error);
        }else{
            terminal->set_serial_connected_status(true);
            terminal->set_command_response( "Serial debug toolbox Started without problem and opens last port. " + parsing_error);
        };
    }else{
        terminal->set_command_response( "Serial debug toolbox Started without problem. ");
    }

    // main loop -------------------------------------------------------------------------------------
    while (true){
        uni_sleep(4);
        ProcessTimerMessages(); // handler windows timerů // todo crete as platform depemndent
        // příkaový režim
        switch (executer->get_state()){
            case RUNNING:
                // běží příkaz
                executer->tick();
                break;
            case ENDED:
                // skončil nějaký program
                console->clear();               
                terminal->redraw();
                break; 
            case SLEEPING:
                // neprobíhá žádný program (beží terminálová apliakce)
                if(serial_direct_input){
                    //mód posílání klávesnice přímo na seriovou linku

                    if(console->has_any_char()){
                        char input = console->get_character();
                        if(input == 27){ // ESC
                            //konec módu přeposílání na seriovou linku
                            serial_direct_input = false;
                            terminal->hide_custom_status_message();
                            terminal->set_command_response("Direct serial mode ended!");
                            terminal->redraw();
                        }else{
                            serial->write(input);
                        }
                    }

                    // přijmutí zpráv ale nekonzumování inputu z klávesnice
                    terminal->tick(false);
                }else{
                    // normální příkazový mód terminálu
                    terminal->tick();
                }
                break;         
        }

        // čtení seriové linky
        if(serial->opened()){
            terminal->print(serial->read_s());
        }
    }

    return 0; // sem se program nikdy nedostane. ukončuje se pomocí příkazu "q" nebo CTRL+C
}