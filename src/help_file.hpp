#include <iostream>
#include "colors.h"
#include "command_list.h"

void print_help(){
    std::cout << RESETTEXT << "----- Manual pro program -----" << std::endl;

    std::string last_leading_name = "";
    for (int i = 0; i < cmd::MAX_COMMAND; i++){

        if(!cmd::commands[i].command_alias){
            last_leading_name = cmd::commands[i].command;
            if(last_leading_name.back() == ' '){
                last_leading_name.pop_back();
            }
            std::cout << std::endl;
        }

        std::string complete_command = cmd::commands[i].command;

        if(cmd::commands[i].command.back() == ' '){
            complete_command.append("[...]");
        }
        
        int head = 25-complete_command.length();
        for (int p = 0; p < head; p++) { std::cout << " "; };
        std::cout << BLD << complete_command << RESETTEXT;

        if(!cmd::commands[i].command_alias){
            std::cout << FOREGRY << " - " << cmd::commands[i].description << RESETTEXT << std::endl;
        }else{
            std::cout << FOREGRY << " - Je aliasem " << last_leading_name << "." << RESETTEXT << std::endl;
        }  
    }
}