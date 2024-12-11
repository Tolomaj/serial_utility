#include "ini.h"
#include <string>
#include <vector>

#pragma once
using namespace std;

// pokud je program pozastaven. sem se uloží znak kterým byl program znovuspuštěný
int continuing_with_cahracter = -1;

enum Priority { NOT_DEFINED,PR_NORMAL , PR_DBG, PR_INFO, PR_WARINING, PR_ERROR, PR_OK , PR_SILENT, PR_TESTOK , PR_TESTNOK , PR_BEGIN , PR_SOFT_BRAKEPOIT ,  PRIORYTY_ENUM_END};

struct Settings{
    bool print_file_location = true;   // vypíše kde se výpis nachází v souborech
    bool in_text_highliting = true;    // zvírazňuje slova jako error a čísla zobrazuje vždy čitelně
    bool priority_coloring = true;    // zvírazňuje slova jako error a čísla zobrazuje vždy čitelně

    bool hide_messages[PRIORYTY_ENUM_END] = {false}; // schování zpráv (řazené stejně jako ENUM Priority)
    bool time_precision_format = true; // ukáže minuty:sekundy od začátku logování. jinak hod:min:sec

    bool print_arival_time = true;     // vypíše kdy zpráva přišla
    bool print_foloving_sign = true;   //ukáže znak před foloverem
    bool use_regs_finding = true;      // použít regulární výrazi na hledání (TODO fičura co asi není potřeba (not implemented))
    bool use_short_file_names = true;  //ukazovat jen jméno souboru a né celou cestu pokud jí soubor má
    bool show_sucess_program_return_code = true;
    int header_fillup = 20;            // kolik znaků minimálnězabírá hlavička (pokud to nespňuje doplní znaky)
    Priority default_priority = PR_NORMAL;             // jakou mají správy bez hlavičky defaultně prioritu
    Priority guess_exclamation_priority = PR_WARINING; // nastavuje prioritu přiřazenou řádku bez hlavičky s ! na konci
    std::string port = "NONE";          // port na který se připojujeme
    int buffer_millis = 150;           // pro přehlednost seskupujeme správy a vypisujeme je součastně.Toto je počet millisekund který maximálně čekáme na zobrazení
    int response_show_ticks = 10;      //kolik ticků terminálu je response vyditený na terminálu.
    std::vector<std::string> serial_commands; // příkazy které chtějí zavřenou seriovou linku.
    bool use_hinting = true;

    bool save_on_end = true; //nenačítá se ze souboru. nastavuje se jen pokud se nějaké nastavení mení flagem 
    bool use_confirm_character = true;

    int serial_speed = 115200;
}settings;


std::string bool_to_string(bool value){
    if(value == true){
        return "true";
    }else{
        return "false";
    }
}   


void save_settings(){
    #ifdef WINDOWS
        std::string path = getenv("APPDATA");
        path.append("/serial_utility");
        CreateDirectory(path.c_str(),NULL);
        path.append("/settings.ini");
    #endif
    #ifdef LINUX
        std::string path = "./settings.ini";
    #endif

    mINI::INIFile file(path.c_str());
    mINI::INIStructure ini;
    file.read(ini);

    ini["settings"]["show_sucess_program_return_code"] = bool_to_string(settings.show_sucess_program_return_code);
    ini["settings"]["time_precision_format"] = bool_to_string(settings.time_precision_format);
    ini["settings"]["in_text_highliting"] = bool_to_string(settings.in_text_highliting);
    ini["settings"]["use_short_file_names"] = bool_to_string(settings.use_short_file_names);
    ini["settings"]["print_file_location"] = bool_to_string(settings.print_file_location);
    ini["settings"]["print_arival_time"] = bool_to_string(settings.print_arival_time);
    ini["settings"]["use_regs_finding"] = bool_to_string(settings.use_regs_finding);
    ini["settings"]["header_fillup"] = to_string(settings.header_fillup);  
    ini["settings"]["default_priority"] = to_string((int)settings.default_priority);  
    ini["settings"]["guess_exclamation_priority"] = to_string((int)settings.guess_exclamation_priority);  
    ini["settings"]["port"] = settings.port;  
    ini["settings"]["buffer_millis"] = to_string(settings.buffer_millis);  
    ini["settings"]["response_show_ticks"] = to_string(settings.response_show_ticks);   
    ini["settings"]["priority_coloring"] = bool_to_string(settings.priority_coloring);
    ini["settings"]["use_confirm_character"] = bool_to_string(settings.use_confirm_character);
    ini["settings"]["use_hinting"] = bool_to_string(settings.use_hinting);
    ini["settings"]["serial_speed"] = to_string((int)settings.serial_speed);

    //composite show string
    std::string show_prio = "";
    for(int i = 0; i < PRIORYTY_ENUM_END; i++){
        show_prio.append(bool_to_string(settings.hide_messages[i]));
        show_prio += ',';
    }
    // remove last , at array
    if(show_prio.back() == ','){ show_prio.pop_back(); }
    ini["settings"]["hide_messages"] = show_prio;

    // composite commands that want closed serial
    std::string all_commands = "";
    for (string serial_cmd : settings.serial_commands){
        all_commands.append(serial_cmd);
        all_commands += ',';
    }

    // remove last , at array
    if(!all_commands.empty() && all_commands.back() == ','){ all_commands.pop_back(); }

    ini["settings"]["serial_commands"] = all_commands; 

    file.write(ini);
}


void load_settings(){
    // načtení defaultních hodnot do pole
    settings.hide_messages[PR_SILENT] = true;
    settings.hide_messages[PR_TESTOK] = true;

    //načtení defaultní cesty s nastavením
    #ifdef WINDOWS
        std::string path = getenv("APPDATA");
        path.append("/serial_utility/settings.ini");
    #endif
    #ifdef LINUX
        std::string path = "./settings.ini";
    #endif

    mINI::INIFile file(path.c_str());
    mINI::INIStructure ini;
    file.read(ini);
    try{
        settings.header_fillup = std::stoi(ini["settings"]["header_fillup"]);  
        settings.default_priority = (Priority)std::stoi(ini["settings"]["default_priority"]);  
        settings.guess_exclamation_priority = (Priority)std::stoi(ini["settings"]["guess_exclamation_priority"]);  
        settings.port = ini["settings"]["port"]; 
        settings.buffer_millis = std::stoi(ini["settings"]["buffer_millis"]);
        settings.response_show_ticks = std::stoi(ini["settings"]["response_show_ticks"]);
        settings.print_file_location = ini["settings"]["print_file_location"] == "true";
        settings.print_arival_time = ini["settings"]["print_arival_time"] == "true";
        settings.use_regs_finding = ini["settings"]["use_regs_finding"] == "true";
        settings.use_short_file_names = ini["settings"]["use_short_file_names"] == "true";
        settings.in_text_highliting = ini["settings"]["in_text_highliting"] == "true";
        settings.show_sucess_program_return_code = ini["settings"]["show_sucess_program_return_code"] == "true";
        settings.time_precision_format = ini["settings"]["time_precision_format"] == "true";
        settings.priority_coloring = ini["settings"]["priority_coloring"] == "true";
        settings.use_confirm_character = ini["settings"]["use_confirm_character"] == "true";
        settings.use_hinting = ini["settings"]["use_hinting"] == "true";
        settings.serial_speed = std::stoi(ini["settings"]["serial_speed"]);

        std::string s = ini["settings"]["serial_commands"];
        std::string tmp; 
        std::stringstream serial_commands_stream(s);
        settings.serial_commands.clear();
        while(getline(serial_commands_stream, tmp, ',')){
            settings.serial_commands.push_back(tmp);
        }


        std::string mesage_hidens = ini["settings"]["serial_commands"];
        std::string tmp_message_hiden; 
        std::stringstream mesage_hidens_stream(mesage_hidens);
        int index = 0;
        while(getline(mesage_hidens_stream, tmp_message_hiden, ',')){
            settings.hide_messages[index] = (tmp_message_hiden == "true");
        }

    }catch(...){
        // problém s čtením nastavení
        save_settings(); // uložíme defaultní
    }
}


bool is_serial_command(std::string command){
    for (string serial_cmd : settings.serial_commands){
        if(serial_cmd == command){
            return true;
        }
    }
    return false;
}
