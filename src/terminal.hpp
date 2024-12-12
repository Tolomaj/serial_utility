#include <string>
#include <ostream>
#include <stdio.h>
#include "colors.h"
#include <iostream>
#include <chrono>
#include <functional>
#include <time.h>
#include <list>
#include <vector>
#include <regex>
#include <fstream>
#include <chrono>

#include "settings.hpp"
#include "help_file.hpp"
#include "command_list.h"

#ifdef WINDOWS
    #include "win_dependent/console.hpp"
    #include "win_dependent/timer.hpp"
    #include <conio.h>
#endif
#ifdef LINUX
    #include "linux_dependent/console.hpp"
    #include "linux_dependent/timer.hpp"
#endif

using namespace std;

// struktura držící veškeré informace o přijaté lince
struct RecivedLine {
    Priority pryority;
    std::string file;
    int file_position = -1;
    std::string message;
    int is_folover = 0; // následuje linku která se má vykreslovat spolu sní ale je na novém řádku (0 není folover, 1 je poslední folover , 2 je neposlední folover)
    int64_t arrive_time; // příchod zprávy
};


class Terminal{
private:

    // uložení všechl linek které jsme dostali // pro vyhledávání a procházení
    std::list<RecivedLine> stash;
    int lines_to_print = 0; // kolik linek není ještě vykresleno z stash (proto aby text nepřekrýval user input)

    // linka ukazující příkaz
    std::string command = "";
    std::vector<std::string> used_commands;
    int selected_command = 0;
    int last_command_printed_len = 0; // Délka posledního vykresleného příkazu (pro překreslování)
    int last_help_printed_len = 0; // Délka textu za commandem (pro překreslování)

    // zobrazení nad příkazem (nedokončená linka nebo odpověď na příkaz)
    std::string prewiew = "";
    bool prewiew_command_printed = false; // uložení jestli je na obrazovce vykreslené prewiew
    int show_response = 0;            // je ukázáno prewiew něčeho ?
    int last_printed_line_header = 0; // kolik zabírá posledně vykreslená hlavička (pro odsazování)

    std::string custom_status_message = ""; // ukáže status který je nad prewiew

    // neuzavřená linka (příchozí po seriové lince)
    std::string tmp_line = "";

    // funkce volaná po přijetí příkazu
    std::string (*command_handler)(std::string);

    // pomocná proměná která zajištuje že se nevykreslijí řádky pořád a uživatel tak vydí co píše
    Timer * timer;

    // handler konsole
    Console * console;

    int64_t log_start; //čas začátku programu

    // position of cursor in command (from end)
    int cursor_position = 0;

    //filtry pro vyhledávání
    std::string text_filter = "";
    std::string file_filter = "";

    const char * file_checker_regex = "[a-zA-Z]?:?[a-zA-Z0-9_\\\\\\/-][a-zA-Z0-9_\\\\\\/-]*\\.[a-zA-Z0-9_][a-zA-Z0-9_]*:[0-9][0-9]*";

    bool serial_console_connected = false;

private:

    std::string composite_footer();

    std::string del_footer();

    std::string shorten_file_name(std::string name);

    std::string composite_line(RecivedLine line);

    // zvírazní části textu 
    std::string advanced_higlight(std::string text,std::string default_color);

    std::string get_waiting_to_print();// sestaví všechny řádky které ještš nebyli vykresleny

    // hromadně vykresluje linky (aby člověk aspoň trochu viděl co píše do cmd)
    void print_lines(std::string str);

    std::string get_brackets(string str,int start, int * end);

    // převod string -> Priority
    Priority extract_priority(std::string str);

    // kvuli stoi čísla nad 9 znaků bereme jako chybu (tak dlouhé soubory nejsou potřeba)
    int get_int_safly(std::string num);

    // přidá přijatý řádek do pole
    void add_complete_line(std::string line);
    
    // nastaví text na zobrazení
    void set_prewiew(std::string text);

    // smaže duplicitní element (jeden více není třeba)
    void remove_duplicate_command(std::string to_remove);

    // podívá se jestli v bufferu je nějaká dodělaná linka
    void check_lineCompletion();

    /// extrahuje část textu od @start_pos do znaku @stop_char. start pos přepíše na konec extrakce
    std::string extract(std::string text, int * start_pos , char stop_char);

    // nastaví kurzor do slova podle toho kde zrovna kurzor je
    void set_cursor_in_command();

    // nastaví daný tip správy na hiden / shown a vrátí string obsahující po řadě které zprávy jsou vyplé a které zaplé. (pomocná funkce do_command() )
    std::string hiden_messages(std::string prioryty_string, bool value);

    // najdě příkaz pro nápovědu
    std::array<std::string, 2> guess_command(std::string cmd);
    
    // vytboří jak vypadá commandline
    std::string composite_command_line();

    // vezme délku utf8 stringu
    std::size_t strlen_utf8(const std::string& str);

    // zkrátí text tak aby nepřetekl příkazový řádek
    std::string shorten_to_line(std::string & text,int reserved_space = 0);

public:

    // funkce která se volá při spuštění příkazu (na konci funcke se volá funkce předaná pro další příkazy mimo terminal)
    std::string do_command(std::string command);

    Terminal(std::string(*command_handler)(std::string),Console * console);

    ~Terminal();

    // nastaví simbol u terminálu (připojeno / odpojeno)
    void set_serial_connected_status(bool status);

    // přepne terminál do status módu a ukazuje jen správu
    void show_custom_status_message(std::string message);

    // vypne status mód a vrátí terminál do původního stavu
    void hide_custom_status_message();

    // uloží stash s logy do souboru last_log v interním formátu
    int save_logs(bool save_only_if_not_empty = false);

    // načte poslední log v speciálním formátu který obsahuje i časy
    int load_logs();

    // načte logy z textové podoby jakou mají linky z seriové linky
    int load_logs_f_file(std::string path);

    // načte logy z textové podoby jakou mají linky z seriové linky
    int save_logs_to_file(std::string path);

    void redraw();

    // vypíše přijatou zprávu a naparsuje jí
    void print(std::string text);

    // vypíše přijatou zprávu a naparsuje jí
    void print(char letter);

    // vymaže všechen text který obdržel a resetuje hodiny
    void clear_hystory();

    void set_command_response(std::string response);

    // funkce ovládající pohyb v historii příkazů
    void command_controll(char key);

    // nastaví filtr a překreslí buffer pro zprávu
    void set_text_filter(std::string filter);

    // nastaví filtr a překreslí buffer pro soubor
    void set_file_filter(std::string filter);

    void reset_clocks();
    
    void tick(bool consume_input = true);

};
 
std::size_t Terminal::strlen_utf8(const std::string& str) {
    std::size_t length = 0;
    for (char c : str) {
        if ((c & 0xC0) != 0x80) {
            ++length;
        }
    }
    return length;
}


void Terminal::set_serial_connected_status(bool status){
    serial_console_connected = status;
    std::string print = del_footer() + composite_footer();

    console->platform_print(print);
    this->set_cursor_in_command();
}


void Terminal::show_custom_status_message(std::string message){
    custom_status_message = message;
    console->hide_cursor(true); 
}


void Terminal::hide_custom_status_message(){
    custom_status_message = "";
    console->hide_cursor(false);
}


std::string Terminal::composite_footer(){
    std::string retrn = "";
    if(tmp_line.length() > 0 || show_response > 0){
        retrn.append(FOREGRY);
        if(show_response > 0){
            show_response--;
            retrn.append(shorten_to_line(prewiew) + '\n'); // pokud je něco na zobrazení jiného tak to ukáže na chvíli (a zkrátí aby to nepřetékalo)

        }else{
            //todo zkrácení prewiew na šířku monitoru!
            static int last_tmp_line_lenght = 0;
            retrn.append(tmp_line + '\n'); // pokud není nic co zobrazit tak ukazuje přijatou linku
            
            if(last_tmp_line_lenght != tmp_line.length()){

            }
            last_tmp_line_lenght = tmp_line.length();
        }
        retrn.append(RESETTEXT);
        prewiew_command_printed = true;
    }else{
        prewiew_command_printed = false;
    }
    // ukážeme command pokud není custom message ukázané
    if(custom_status_message == ""){
        retrn.append(composite_command_line());
    }else{
        // nastavíme custom status 
        retrn.append(FORECYN);
        retrn.append(custom_status_message);
        retrn.append(RESETTEXT);
        last_command_printed_len = custom_status_message.length();
    }   
    return retrn;
}


std::string Terminal::del_footer(){
    std::string delstr = "";
    // dont have prewiew
    delstr += '\r';
    // vymazání posledního příkazu (zamazání mezerami)
    for (int i = 0; i < last_command_printed_len + last_help_printed_len + 5; i++){ delstr += ' '; }

    if(prewiew_command_printed == true){
        delstr.append("\033[A\033[2K\r"); // clear 2 lines
    }else{
        delstr += '\r';
    }
    return delstr;
}


std::string Terminal::shorten_file_name(std::string name){
    // pro linux cestu
    int position = name.find("/");  
    while (position != std::string::npos){
        name = name.substr(position+1); 
        position = name.find("/"); 
    }

    // pro windows cestu
    position = name.find("\\");   
    while (position != std::string::npos){
        name = name.substr(position+1);
        position = name.find("\\"); 
    }
    return name;
}


std::string Terminal::composite_line(RecivedLine line){
// Podléhá linka nastaveným filtrům ? ----------------------------------------------------------------------------------------------------------------
    // vyhledáváme pomocí regulárního výrazu
    if(text_filter != "" && settings.use_regs_finding){
        if (!std::regex_search(line.message, std::regex(text_filter))){ return ""; } // linka neobsahuje hledaný víraz
    }

    if(file_filter != "" && settings.use_regs_finding){
        if (!std::regex_search(line.file, std::regex(file_filter))){ return ""; } // linka neobsahuje hledaný víraz
    }

// Příprava linky k vykreslení (příprava dat) -------------------------------------------------------------------------------------------------------
    // poslední 2 znaky zprávy nemůžou být /n /r ' '
/*
    if(line.message.back() == '\r'){ line.message.pop_back(); }
    if(line.message.back() == '\n'){ line.message.pop_back(); }
    if(line.message.back() == ' ' ){ line.message.pop_back(); }
    if(line.message.back() == '\r'){ line.message.pop_back(); }
    if(line.message.back() == '\n'){ line.message.pop_back(); }
    if(line.message.back() == ' ' ){ line.message.pop_back(); }

    // první 2 znaky zprávy nemůžou být /n /r
    if(line.message.front() == '\r'){ line.message.substr(1,-1); }
    if(line.message.front() == '\n'){ line.message.substr(1,-1); }
    if(line.message.front() == '\r'){ line.message.substr(1,-1); }
    if(line.message.front() == '\n'){ line.message.substr(1,-1); }
*/

    // pokud není definovaná priorita ale na konci je ! nastavíme jako warning (! může být na posledních 3 pozicích) 
    if(settings.in_text_highliting){
        int text_lenght = line.message.length();
        if(line.pryority == NOT_DEFINED && (line.message[text_lenght] == '!' || 
        (text_lenght >= 2 && line.message[text_lenght-1] == '!') || (text_lenght >= 2 && line.message[text_lenght-2] == '!'))){
            line.pryority = PR_WARINING;
        }
    }

    // Ppokud je priorita vyplá zprávy neukazujeme
    if(settings.hide_messages[line.pryority] == true){ return ""; }

    // pokud není definovaná priorita přiřadíme jí defaultní 
    if(line.pryority == NOT_DEFINED){ line.pryority = settings.default_priority; }

// příprava barev zprávy -----------------------------------------------------------------------------------------------------------------------------

    std::string line_header_color = FOREGRY;    // barva hlavičky zprávy
    std::string line_message_color = RESETTEXT; // barva samotné zprávy

    if(settings.priority_coloring){
        // set string default color
        switch (line.pryority){
            case PR_DBG:      line_message_color = FOREMAG; break;
            case PR_ERROR:    line_message_color = FORERED; break;
            case PR_INFO:     line_message_color = FOREBLU; break;
            case PR_WARINING: line_message_color = FOREYEL; break;
            case PR_OK:       line_message_color = FOREGRN; break;
            case PR_TESTOK:   line_message_color = FOREGRN; break;
            case PR_TESTNOK:  line_message_color = FORERED; break;
            case PR_SILENT:   line_message_color = FOREGRY; break;
            case PR_BEGIN:    line_message_color = FORECYN; break;
            case PR_SOFT_BRAKEPOIT: line_message_color = FOREYEL; break;
            default: break;
        }

        // pokud je to jen bod bez zprávy. zobrazíme barevně u místo v souboru
        if(line.message == "" && !(line.pryority == PR_TESTNOK || line.pryority == PR_TESTOK)){
            line_header_color = line_message_color;
        }
    }

// vytvoření hlavičky zprávy která naní folover ------------------------------------------------------------------------------------------------------
    std::string header = ""; // tady je hlavička před reálnou zprávou
    int header_lenght;

    // pro linky co nejsou folover vytvoříme hlavičku
    if(line.is_folover == 0){
        // přidáme čas příchodu zprávy
        if(settings.print_arival_time){
            if(line.is_folover == 0){
                char array[15];
                if(settings.time_precision_format){
                    //now_time in millis;
                    if(line.arrive_time > 1000000){
                        sprintf(array,"[%07lld]",(long long int)line.arrive_time);
                    }else{
                        sprintf(array,"[%010lld]",(long long int)line.arrive_time);
                    }
                }else{
                    //now_time in h:min:s;
                    int sec = (int)(line.arrive_time/1000);
                    sprintf(array,"[%02d:%02d:%02d]",(sec/3600)%60,(sec/60)%60,sec%60);
                }
                header.append(array);
            }
        }

        // přidáme místo sousboru
        if(line.file != "" && settings.print_file_location){
            std::string filepath = "[";

            if(settings.use_short_file_names){
                filepath.append(shorten_file_name(line.file));
            }else{
                filepath.append(line.file);
            }

            filepath += ':';
            filepath.append(to_string(line.file_position));  
            filepath += ']';
            if(line.is_folover == 0){
                header.append(filepath);  
            }
        }
        // zapíšeme si délku poslední hlavičky
        this->last_printed_line_header = header.length();
        header_lenght = header.length();
    }

// vytvoříme hlavičku pro follover zprávy -------------------------------------------------------------------------------------------------------------------
    if(line.is_folover != 0){
        // dorovná hlavní zprávu mezerami
        for (int i = 0; i < this->last_printed_line_header - 1; i++){
            header += ' ';
        }
        if(settings.print_foloving_sign == true){
            // přidá znak značící že patří k zpráve před ní
            if(line.is_folover == 1){
                header += "╚";
            }else if(line.is_folover == 2){
                header += "╠";
            }
        }else{
            header += ' ';
        }
        header_lenght = this->last_printed_line_header;
    }
    // tady máme obě hlavičky stejně dlouhé

//dorovnáme hlavičky ----------------------------------------------------------------------------------------------------------------------------------

    std::string filler = "";

    // přidání mezer před hlavičku (pokud to je potřeba)
    int filler_needed = settings.header_fillup - header_lenght;
    for (int i = 0; i < filler_needed; i++){
        filler += ' ';
    }

    std::string cosmetic_space = "";

    // přidání mezery pokud není v samotné správě (vizuální oddělení od file nebo time) (pokud nemá hlavičku neřešíme) -----------------------------
    if((settings.print_arival_time || settings.print_file_location || settings.header_fillup != 0) && ((line.message.length() > 0 && line.message[0] != ' ') || ((line.pryority == PR_TESTOK || line.pryority == PR_TESTNOK) && line.message.length() == 0))){
        cosmetic_space = " ";
    }

// vytvoříme zprávu ----------------------------------------------------------------------------------------------------------------------------------
    std::string message = "";

    // další text už je barevně
    if(line.message == ""){
        if(line.pryority == PR_TESTOK){
            message.append("test success.");
        }else 
        if(line.pryority == PR_TESTNOK){
            message.append("test fail.");
        }
    }

    if(settings.in_text_highliting){
        message.append(advanced_higlight(line.message,line_message_color));
    }else{
        message.append(line.message);
    }

    return filler + line_header_color + header + line_message_color + cosmetic_space + message + RESETTEXT + '\n';
}


// zvírazní části textu 
std::string Terminal::advanced_higlight(std::string text,std::string default_color){
    std::string line = "";
    char last_char = 0;

    //in text numbers
    int text_lenght = text.length();

    for (size_t i = 0; i < text_lenght; i++){
        //v textu zvírazníme slova err
        if(text_lenght - i > 4){
            // nezáleží na velikosti písmen
            if((text[i]=='e'||text[i]=='E') && (text[i+1]=='r'||text[i+1]=='R') && (text[i+2]=='r'||text[i+2]=='R') && (text[i+3]=='o'||text[i+3]=='O') && (text[i+4]=='r'||text[i+4]=='R')){
                line.append(FORERED);
                for (size_t ii = 0; ii < 5; ii++) {
                    line += text[i];
                    i++;
                }
                i--;
                line.append(default_color);
                continue;
            }
        }

        //v textu zvírazníme ok
        if(text_lenght - i > 2){
            // nezáleží na velikosti písmen
            if((text[i]=='o'||text[i]=='O') && (text[i+1]=='k'||text[i+1]=='K')){
                line.append(FOREGRN);
                for (size_t ii = 0; ii < 2; ii++) {
                    line += text[i];
                    i++;
                }
                i--;
                line.append(default_color);
                continue;
            }
        }
        
        bool m_char, l_char;
        // tečky a čárky nemění barvu textu a ani se nepočítají do přehodů
        if(text[i] == '.' || text[i] == ',' || text[i] == '%'){
            line += text[i];
            continue;
        }

        l_char = (last_char >= '0' && last_char <= '9');
        m_char = (text[i] >= '0' && text[i] <= '9');

        // zvíraznění čísel
        if(l_char && !m_char){
            // přechod z čísla na znak
            line.append(default_color);
        }else if(!l_char && m_char){
            //přechod z znaku na číslo
            line.append(RESETTEXT);
        }
           
        last_char = text[i];
        line += text[i];
    }
    
    return line;
}


std::string Terminal::get_waiting_to_print(){ // sestaví všechny řádky které ještš nebyli vykresleny
    std::string retrn = "";

    for (RecivedLine & line : stash) {
        if(lines_to_print <= 0){ return retrn; } // už není co vykreslit

        retrn = composite_line(line) + retrn;
        lines_to_print--;
    }
 
    return retrn;
}


// hromadně vykresluje linky (aby člověk aspoň trochu viděl co píše do cmd)
void Terminal::print_lines(std::string str){
    std::string print;

    print = del_footer();
    print.append(str);
    print.append(composite_footer());

    console->platform_print(print);
    this->set_cursor_in_command();
}


std::string Terminal::get_brackets(string str,int start, int * end){
    *end = start;
    int pos = str.find('[',start); 
    int pos_end = str.find(']',pos); 
    //linak neobsahuje čtveratý formát
    if(pos == std::string::npos){return "";};
    if(pos_end == std::string::npos){return "";};

    *end = pos_end+1;
    return str.substr(pos+1,pos_end - pos-1);
};


// převod string -> Priority
Priority Terminal::extract_priority(std::string str){
    if(str == "ERROR" || str == "ERR" || str == "E"){
        return PR_ERROR;
    }
    if(str == "INFO" || str == "I"){
        return PR_INFO;
    }
    if(str == "DEBUG" || str == "DBG" || str == "D"){
        return PR_DBG;
    }
    if(str == "WARNING" || str == "W"){
        return PR_WARINING;
    }
    if(str == "NORMAL" || str == "N"){
        return PR_NORMAL;
    }
    if(str == "OK" || str == "K"){
        return PR_OK;
    }
    if(str == "SILENT" || str == "S"){
        return PR_SILENT;
    }
    if(str == "TEST_OK" || str == "TK"){
        return PR_TESTOK;
    }
    if(str == "TEST_NOK" || str == "TN"){
        return PR_TESTNOK;
    }
    if(str == "BEGIN" || str == "BGN" || str == "B"){
        return PR_BEGIN;
    }
    if(str == "SOFT_BRAKEPINT" || str == "SBK"){
        return PR_SOFT_BRAKEPOIT;
    }
    return NOT_DEFINED;
}


// kvuli stoi čísla nad 9 znaků bereme jako chybu (tak dlouhé soubory nejsou potřeba)
int Terminal::get_int_safly(std::string num){
    if(num.length() < 10){
        return std::stoi(num);
    }else{
        return -1;
    }   
}


// přidá přijatý řádek do pole
void Terminal::add_complete_line(std::string line){
    if(line == "" || line == " "){
        return; // prázdná linka není potřeba přidávat ochran 
    }

    int first_line_start = 0;
    int sec_line_start = 0;
    int message_start = 0;
    std::string s = get_brackets(line ,        0       ,&first_line_start);
    std::string s2 = get_brackets(line,first_line_start,&sec_line_start);
    RecivedLine recived_line;
    recived_line.pryority = NOT_DEFINED;
    recived_line.file = "";

    // vezmeme čas kdy zpráva dorazila
    recived_line.arrive_time = double(clock()) / CLOCKS_PER_SEC * 1000 - log_start;
    

    // zpráva nemá ani jednu hlavičku tudíž je to zpráva normální (šetříme výpočetní čas)
    if(s == "" && s2 == ""){
        recived_line.message = line;
        stash.push_front(recived_line);
        lines_to_print++;
        return;
    }

    // je zpráva návazná která je za zprávou před ní. (taková nemá barvu ani místo v kódu a je napsané hned za zprávu hlavní)
    if(s == "~" || s == "FOLOWING"){
        recived_line = *stash.begin();
        // pokud byl předním folover, zmení ho na neposledního folovera
        if(stash.begin()->is_folover == 1){
            stash.begin()->is_folover = 2;
        }

        recived_line.is_folover = 1;
        recived_line.message = line.substr(first_line_start, -1);
        stash.push_front(recived_line);
        lines_to_print++;
        timer->reset(); // resetujeme timer kdyby náhodou přišel další folower. //! pokud folower nedojde dlouho i tak se tiká aby jsme viděli co došlo (považuju za lepší vědět co došlo něž to aby tam nebyla vizuální neperfekce)
        return;
    }

    // test priority na první pozici
    Priority pr = extract_priority(s);
    if(pr != NOT_DEFINED){ // pokud je priorita na 1 pozici definována [PRIORITY][??]TEXT 
        recived_line.pryority = pr; 
        if(std::regex_match(s2, regex(file_checker_regex))){ // je na druhé pozici soubor ?
            // set file and text start
            recived_line.file = s2.substr(0,s2.find_last_of(":"));

            recived_line.file_position = get_int_safly(s2.substr(s2.find_last_of(":")+1,-1));
            
            // oba argumenty jsou validní. tudíž zpráva začíná až za nimi
            message_start = sec_line_start;
        }else{
            // soubor není nastavený tudíž soubor není žádný a začínáme zprávu brát za prvním paramterem
            message_start = first_line_start;
        }
    }else

    //pokud není definovaná priorita je soubor ??
    if(std::regex_match(s, regex(file_checker_regex))){
        // set file
        recived_line.file = s.substr(0,s.find_last_of(":"));

        recived_line.file_position = get_int_safly(s.substr(s.find_last_of(":")+1,-1));

        message_start = first_line_start;

        // je na další pozici priorita ??
        Priority pr = extract_priority(s2);
        if(pr != NOT_DEFINED){ 
            recived_line.pryority = pr;
            message_start = sec_line_start;
        }
    }else{
        // ani jedena hlavička není validní tudíž celé to je zpráva
        message_start = 0;
    }

    recived_line.message = line.substr(message_start, -1);

    if(recived_line.pryority == PR_BEGIN){
        this->reset_clocks();
        recived_line.arrive_time = double(clock()) / CLOCKS_PER_SEC * 1000 - log_start;
    }

    stash.push_front(recived_line);
    lines_to_print++;
}


// nastaví text na zobrazení
void Terminal::set_prewiew(std::string text){
    prewiew = text;
    std::string print = del_footer() + composite_footer();

    console->platform_print(print);

    this->set_cursor_in_command();
}


// smaže duplicitní element (jeden více není třeba)
void Terminal::remove_duplicate_command(std::string to_remove){
    // najdeme element k vymazaní
    auto it = std::find(used_commands.begin(), used_commands.end(), to_remove);

    // pokud existuje smazat
    if (it != used_commands.end()) {
        used_commands.erase(it);
    }
}


// podívá se jestli v bufferu je nějaká dodělaná linka
void Terminal::check_lineCompletion(){
    // naseká linku na řádky
    while(true){
        auto pos = tmp_line.find('\n'); 
        if(pos == std::string::npos){return;}; // linka není ještě uzavřená
        std::string line = tmp_line.substr(0,pos);
        line.erase( std::remove(line.begin(), line.end(), '\r'), line.end() );
        add_complete_line(line);
        tmp_line.erase(0,pos+1);
    }
}


/// extrahuje část textu od @start_pos do znaku @stop_char. start pos přepíše na konec extrakce
std::string Terminal::extract(std::string text, int * start_pos , char stop_char){
    int original_start = *start_pos;
    int pos = text.find(stop_char,original_start);
    *start_pos = pos +1;
    return text.substr(original_start,pos - original_start);
}

// nastaví kurzor do slova podle toho kde zrovna kurzor je
void Terminal::set_cursor_in_command(){
    console->set_cursor_back(last_help_printed_len + cursor_position);
}


Terminal::Terminal(std::string(*command_handler)(std::string),Console * console){
    this->command_handler = command_handler;
    this->console = console;

    console->platform_print(composite_footer());

    timer  = new Timer(settings.buffer_millis);

    log_start = double(clock()) / CLOCKS_PER_SEC * 1000;

}


Terminal::~Terminal(){ }


// uloží stash s logy do souboru last_log v interním formátu
int Terminal::save_logs(bool save_only_if_not_empty){
    
    // pokud nejsou žádná data a uživatel si nepřeje ukládání prázdných tak neukádej
    if(save_only_if_not_empty && stash.size() <=0 ){
        return 0;
    }

    #ifdef WINDOWS
        std::string path = getenv("APPDATA");
        path.append("/serial_utility");
        CreateDirectory(path.c_str(),NULL);
        path.append("/last_log");
    #endif
    #ifdef LINUX
        std::string path = "./last_log";
    #endif
    std::ofstream save_file(path); 

    // existuje soubor ?
    if(!save_file){ return -1; }

    //log_line
    for (auto line = stash.begin(); line != stash.end(); ++line){
        save_file << line->arrive_time << '|';
        save_file << line->is_folover << '|';
        save_file << (int)line->pryority << '|';
        save_file << line->file_position << '|';
        save_file << line->file << '|';
        save_file << line->message << '\n';
    }
    save_file.close();
    return 0;
}


// načte poslední log v speciálním formátu který obsahuje i časy
int Terminal::load_logs(){
    std::string path = getenv("APPDATA");
    path.append("/serial_utility/last_log");
    std::ifstream open_file(path); 

    // existuje soubor ?
    if(!open_file){ return -1; }

    std::string line = "";
    RecivedLine rec_line;
    while ( getline (open_file,line) ){
        int pos = 0;
        try{
            rec_line.arrive_time  = std::stoi(this->extract(line,&pos,'|'));

            rec_line.is_folover = std::stoi(this->extract(line,&pos,'|'));
            rec_line.pryority = (Priority)std::stoi(this->extract(line,&pos,'|'));
            rec_line.file_position = std::stoi(this->extract(line,&pos,'|'));
            rec_line.file = this->extract(line,&pos,'|');
            rec_line.message = this->extract(line,&pos,'|');

            this->stash.push_back(rec_line);
        }catch( ... ){
            // file is in wrong format
            return -2;
        }
    }

    open_file.close();
    return 0;
}


// načte logy z textové podoby jakou mají linky z seriové linky
int Terminal::load_logs_f_file(std::string path){
    std::ifstream open_file(path); 

    // existuje soubor ?
    if(!open_file){ return -1; }

    int lines_loaded = 0;

    std::string line = "";
    while ( getline (open_file,line) ){
        this->add_complete_line(line);
        lines_loaded++;
    }

    open_file.close();
    return lines_loaded;
}


// načte logy z textové podoby jakou mají linky z seriové linky
int Terminal::save_logs_to_file(std::string path){
    std::ofstream save_file(path); 
    
    if(!save_file){
        return -1;
    }

    for (auto line = stash.rbegin(); line != stash.rend(); ++line) {
    //for (RecivedLine line : stash){
        if (line->is_folover){
            save_file << "[~]" << line->message << "\n";
        }else{

            switch (line->pryority){
                case PR_NORMAL:   save_file << "[NORMAL]";  break;
                case PR_DBG:      save_file << "[DEBUG]";   break;
                case PR_INFO:     save_file << "[INFO]";    break;
                case PR_WARINING: save_file << "[WARNING]"; break;
                case PR_ERROR:    save_file << "[ERROR]";   break;
                case PR_OK:       save_file << "[OK]";      break;
                case PR_SILENT:   save_file << "[SILENT]";  break;
                case PR_TESTOK:   save_file << "[TEST_OK]"; break;
                case PR_TESTNOK:  save_file << "[TEST_NOK]";break;
                case PR_BEGIN:    save_file << "[BEGIN]";   break;
                default: break;
            }

            if(line->file != ""){
                save_file << "[" << line->file << ":" << line->file_position << "]";
            }

            save_file << line->message << "\n";
        }
    }
    return stash.size();
}


void Terminal::redraw(){
    console->clear();

    std::string print = "";
    for (RecivedLine & line : stash){
        print = composite_line(line) + print;
    }
    print.append(composite_footer());

    console->platform_print(print);
    this->set_cursor_in_command();
}


// vypíše přijatou zprávu a naparsuje jí
void Terminal::print(std::string text){
    if(text == ""){return;}
    tmp_line.append(text);
    check_lineCompletion();
}


// vypíše přijatou zprávu a naparsuje jí
void Terminal::print(char letter){
    tmp_line += letter;
    check_lineCompletion();
}


// vymaže všechen text který obdržel a resetuje hodiny
void Terminal::clear_hystory(){
    stash.clear();
    tmp_line = "";
    lines_to_print = 0;
    
    // vymaže konzoli
    console->clear();

    // zobrazí spodek konzole
    std::string print = composite_footer();
    console->platform_print(print);

    this->set_cursor_in_command();
}


void Terminal::set_command_response(std::string response){
    //nastavíme response text
    prewiew = response;  
    if(response == ""){
        show_response = 1;
    }else{
        show_response = settings.response_show_ticks; //? jak dlouho se ukáže na terminálu. (? možná předělat na timer ?)
    }
}


// funkce ovládající pohyb v historii příkazů
void Terminal::command_controll(char key){
    #ifdef LINUX
        int ch = getchar();
        if(ch == '['){ 
            key = getchar();
        }else{
            ungetc(ch, stdin);
        }
    #endif

    #ifdef WINDOWS
        if(key == 27){ // pro windows 27 je modifikátor šipek
            key = (char)console->get_character();
        }
    #endif

    switch (key){
    case ARROW_RIGHT: // ARROW_RIGHT
        cursor_position--;
        if(cursor_position < 0){
            cursor_position = 0;
        }
        break;   
    case ARROW_LEFT: // ARROW_LEFT
        cursor_position++;
        if(cursor_position >= command.length()){
            cursor_position = command.length();
        }
        break;           
    case ARROW_UP: // ARROW_UP
        if(used_commands.size() > selected_command){
            selected_command++;
            command = used_commands.at(selected_command-1);
        }
        cursor_position = 0;
        break;
    case ARROW_DOWN: // ARROW_DOWN
        if(selected_command > 1 ){
            selected_command--;
            command = used_commands.at(selected_command-1);
        }else if(selected_command -1 == 0){
            selected_command = 0;
            command = "";
        }
        cursor_position = 0;
        break;  
    case 27: // ESC
        command = "";
        selected_command = 0;
        cursor_position = 0;
        break;
    default: 
        break;
    }
}


// nastaví filtr a překreslí buffer pro zprávu
void Terminal::set_text_filter(std::string filter){
    text_filter = filter;
    redraw();
}


// nastaví filtr a překreslí buffer pro soubor
void Terminal::set_file_filter(std::string filter){
    file_filter = filter;
    redraw();
}

void Terminal::reset_clocks(){
    // nastaví noví začátek hodin
    this->log_start = double(clock()) / CLOCKS_PER_SEC * 1000;
}

Priority translate_priority(std::string prioryty_string){
    if(prioryty_string == "normal"  || prioryty_string == "n" )                            { return PR_NORMAL;   }
    if(prioryty_string == "debug"   || prioryty_string == "dbg" || prioryty_string == "d" ){ return PR_DBG;      }
    if(prioryty_string == "info"    || prioryty_string == "i" )                            { return PR_INFO;     }
    if(prioryty_string == "warning" || prioryty_string == "wrng"|| prioryty_string == "w" ){ return PR_WARINING; }
    if(prioryty_string == "error"   || prioryty_string == "err" || prioryty_string == "e" ){ return PR_ERROR;    }
    if(prioryty_string == "ok"      || prioryty_string == "k" )                            { return PR_OK;       }
    if(prioryty_string == "silent"  || prioryty_string == "sil" || prioryty_string == "s" ){ return PR_SILENT;   }
    if(prioryty_string == "testok"  || prioryty_string == "tok" || prioryty_string == "tk"){ return PR_TESTOK;   }
    if(prioryty_string == "testnok" || prioryty_string == "ton" || prioryty_string == "tn"){ return PR_TESTNOK;  }
    if(prioryty_string == "begin"   || prioryty_string == "bgn" || prioryty_string == "b" ){ return PR_BEGIN;    }
    return NOT_DEFINED;
}

// nastaví daný tip správy na hiden / shown a vrátí string obsahující po řadě které zprávy jsou vyplé a které zaplé. (pomocná funkce do_command() )
std::string Terminal::hiden_messages(std::string prioryty_string, bool value){
    Priority prioryty_id = translate_priority(prioryty_string);

    if(prioryty_id == NOT_DEFINED){
        std::string msg;
        msg.append(FORERED);
        msg.append("Unknown priority: ");
        msg.append(FOREWHT);
        msg.append(prioryty_string);
        msg.append(FORERED);
        msg.append("!");
        msg.append(RESETTEXT);
        return msg;
    }

    settings.hide_messages[prioryty_id] = value;
    this->redraw();

    std::string hiden_messages = "messages list:";
    for (size_t i = 0; i < PRIORYTY_ENUM_END; i++){
        if(settings.hide_messages[i]){
            hiden_messages.append("hidden,");
        }else{
            hiden_messages.append("shown,");
        }
    }
    hiden_messages.pop_back();
    return hiden_messages;
}


// funkce která se volá při spuštění příkazu (na konci funcke se volá funkce předaná pro další příkazy mimo terminal)
std::string Terminal::do_command(std::string command){

    std::string lower_command = command;
    std::transform(lower_command.begin(), lower_command.end(), lower_command.begin(), [](unsigned char c){ return std::tolower(c); } );

    if(lower_command == cmd::name(cmd::RESET_CLOCK)  || lower_command == cmd::name(cmd::RESET_CLOCK1) || lower_command == cmd::name(cmd::RESET_CLOCK2)){
        //začneme počítat hodiny on nuly
        this->reset_clocks();
        return "Clocks reseted.";
    }

    // PRECISON CONTROLL commands
    if(lower_command == cmd::name(cmd::PRECISIONS_TIME_ON) || lower_command == cmd::name(cmd::PRECISIONS_TIME_ON1)){
        settings.time_precision_format = true;
        this->redraw();
        return "Using precision time.";
    }
    if(lower_command == cmd::name(cmd::PRECISIONS_TIME_OFF) || lower_command == cmd::name(cmd::PRECISIONS_TIME_OFF1)){
        settings.time_precision_format = false;
        this->redraw();
        return "Using precision time.";
    }

    if(lower_command == cmd::name(cmd::RESET) || lower_command == cmd::name(cmd::RESET1) || lower_command == cmd::name(cmd::RESET2) ){
        for (size_t i = 0; i < PRIORYTY_ENUM_END; i++){
            settings.hide_messages[i] = false;
        }
        settings.hide_messages[PR_SILENT] = true;
        settings.hide_messages[PR_TESTOK] = true;
        this->set_text_filter("");
        this->set_file_filter("");
        this->redraw();
        return "Reseting all filters.";
    }

    // příkaz RESET_FILTER pro oddělání všech filtrů  -----------------------------------------------------------------------------------------------------------------------------
    if(lower_command == cmd::name(cmd::RESET_FILTERS) || lower_command == cmd::name(cmd::RESET_FILTERS1) || lower_command == cmd::name(cmd::RESET_FILTERS2)){
        this->set_text_filter("");
        this->set_file_filter("");
        return "Filters reseted!";
    }

    // příkaz LOAD_LOG pro oddělání všech filtrů  -----------------------------------------------------------------------------------------------------------------------------
    if(lower_command == cmd::name(cmd::LOAD_LOG) || lower_command == cmd::name(cmd::LOAD_LOG1)){
        this->clear_hystory();

        if(this->load_logs() != 0){
            return "Cant load file!";
        }else{
            this->redraw();
            return "Logs loaded!";
        };
    }

    // příkaz SAVE  -----------------------------------------------------------------------------------------------------------------------------
    if(lower_command.length() >= cmd::name_len(cmd::SAVE) && lower_command.substr(0,cmd::name_len(cmd::SAVE)) == cmd::name(cmd::SAVE)){
        int retval = this->save_logs_to_file(command.substr(cmd::name_len(cmd::SAVE),-1));
        if(retval < 0){
            return "Directories not exist! File cant be writed!";
        }else{
            return "Saved " + std::string(FOREWHT) + to_string(retval) + std::string(FOREGRY) + " lines to file:" + std::string(FOREWHT) + command.substr(5,-1) + std::string(FOREGRY) + ".";
        }  
    }
    if(lower_command.length() >= cmd::name_len(cmd::SAVE1) && lower_command.substr(0,cmd::name_len(cmd::SAVE1)) == cmd::name(cmd::SAVE1)){
        int retval = this->save_logs_to_file(command.substr(cmd::name_len(cmd::SAVE1),-1));
        if(retval < 0){
            return "Directories not exist! File cant be writed!";
        }else{
            return "Saved " + std::string(FOREWHT) + to_string(retval) + std::string(FOREGRY) + " lines to file:" + std::string(FOREWHT) + command.substr(5,-1) + std::string(FOREGRY) + ".";
        }  
    }


    // příkaz LOAD  -----------------------------------------------------------------------------------------------------------------------------
    if(lower_command.length() >= cmd::name_len(cmd::LOAD) && lower_command.substr(0,cmd::name_len(cmd::LOAD)) == cmd::name(cmd::LOAD)){
        this->clear_hystory();
        int retval = this->load_logs_f_file(command.substr(cmd::name_len(cmd::LOAD),-1));
        if(retval < 0){
            return "File dont exist!";
        }else{
            return "Loaded " + std::string(FOREWHT) + to_string(retval) + std::string(FOREGRY) + " lines from file:" + std::string(FOREWHT) + command.substr(cmd::name_len(cmd::LOAD),-1) + std::string(FOREGRY) + ".";
        }  
    }
    if(lower_command.length() >= cmd::name_len(cmd::LOAD1) && lower_command.substr(0,cmd::name_len(cmd::LOAD1)) == cmd::name(cmd::LOAD1)){
        this->clear_hystory();
        int retval = this->load_logs_f_file(command.substr(cmd::name_len(cmd::LOAD1),-1));
        if(retval < 0){
            return "File dont exist!";
        }else{
            return "Loaded " + std::string(FOREWHT) + to_string(retval) + std::string(FOREGRY) + " lines from file:" + std::string(FOREWHT) + command.substr(cmd::name_len(cmd::LOAD1),-1) + std::string(FOREGRY) + ".";
        }  
    }

    // příkaz FIND pro hledání v zprávách  ---------------------------------------------------------------------------------------------------------------------
    if(lower_command.length() >= cmd::name_len(cmd::FIND) && lower_command.substr(0,cmd::name_len(cmd::FIND)) == cmd::name(cmd::FIND)){
        this->set_text_filter(command.substr(cmd::name_len(cmd::FIND),-1));
        return "Searching for: " + command.substr(cmd::name_len(cmd::FIND),-1) + " in text.";
    }
    if(lower_command.length() >= cmd::name_len(cmd::FIND1) && lower_command.substr(0,cmd::name_len(cmd::FIND1)) == cmd::name(cmd::FIND1)){
        this->set_text_filter(command.substr(cmd::name_len(cmd::FIND1),-1));
        return "Searching for: " + command.substr(cmd::name_len(cmd::FIND1),-1) + " in text.";
    }

    // příkaz FILE pro hledání v souborech zprávy  -----------------------------------------------------------------------------------------------------------------------------
    if(lower_command.length() >= cmd::name_len(cmd::FILE) && lower_command.substr(0,cmd::name_len(cmd::FILE)) == cmd::name(cmd::FILE)){
        this->set_file_filter(command.substr(cmd::name_len(cmd::FILE),-1));
        return "Searching for: " + command.substr(cmd::name_len(cmd::FILE),-1) + " in file name.";
    }
    if(lower_command.length() >= cmd::name_len(cmd::FILE1) && lower_command.substr(0,cmd::name_len(cmd::FILE1)) == cmd::name(cmd::FILE1)){
        this->set_file_filter(command.substr(cmd::name_len(cmd::FILE1),-1));
        return "Searching for: " + command.substr(cmd::name_len(cmd::FILE1),-1) + " in file name.";
    }       

    // příkaz OFFSET  ---------------------------------------------------------------------------------------------------------------------
    if(lower_command.length() >= cmd::name_len(cmd::HEAD) && lower_command.substr(0,cmd::name_len(cmd::HEAD)) == cmd::name(cmd::HEAD)){
        try{
            settings.header_fillup = std::stoi(command.substr(cmd::name_len(cmd::HEAD),-1));
            this->redraw();
            return "new head offset is: " + command.substr(cmd::name_len(cmd::HEAD),-1);
        } catch(...) {
            return "invalid number";
        }
    }
    if(lower_command.length() >= cmd::name_len(cmd::HEAD1) && lower_command.substr(0,cmd::name_len(cmd::HEAD1)) == cmd::name(cmd::HEAD1)){
        try{
            settings.header_fillup = std::stoi(command.substr(cmd::name_len(cmd::HEAD1),-1));
            this->redraw();
            return "new head offset is: " + command.substr(cmd::name_len(cmd::HEAD1),-1);
        } catch(...) {
            return "invalid number";
        }
    }


    // příkaz HIDE ALL  --------------------------------------------------------------------------------------------------------------------
    if(lower_command == cmd::name(cmd::HIDE_ALL) || lower_command == cmd::name(cmd::HIDE_ALL1) || lower_command == cmd::name(cmd::HIDE_ALL2)){
        for (size_t i = 0; i < PRIORYTY_ENUM_END; i++){
            settings.hide_messages[i] = true;
        }
        this->redraw();
        return "All messages type are hidden.";
    }


    // příkaz SHOW ALL  --------------------------------------------------------------------------------------------------------------------
    if(lower_command == cmd::name(cmd::SHOW_ALL) || lower_command == cmd::name(cmd::SHOW_ALL1) || lower_command == cmd::name(cmd::SHOW_ALL2)){
        for (size_t i = 0; i < PRIORYTY_ENUM_END; i++){
            settings.hide_messages[i] = false;
        }
        this->redraw();
        return "All messages type are shown.";
    }


    // příkaz SHOW RESET  -----------------------------------------------------------------------------------------------------------------
    if(lower_command == cmd::name(cmd::SHOW_RESET) || lower_command == cmd::name(cmd::SHOW_RESET1) || lower_command == cmd::name(cmd::SHOW_RESET2)){
        for (size_t i = 0; i < PRIORYTY_ENUM_END; i++){
            settings.hide_messages[i] = false;
        }
        settings.hide_messages[PR_SILENT] = true;
        settings.hide_messages[PR_TESTOK] = true;
        this->redraw();
        return "Showing all messages exept sucesful tests and silent.";
    }

    // příkaz ndef  ---------------------------------------------------------------------------------------------------------------------
    if(lower_command.length() >= cmd::name_len(cmd::UNDEFINED_PRIORITY) && lower_command.substr(0,cmd::name_len(cmd::UNDEFINED_PRIORITY)) == cmd::name(cmd::UNDEFINED_PRIORITY)){
        Priority d_priority = translate_priority(lower_command.substr(cmd::name_len(cmd::UNDEFINED_PRIORITY),-1));
        if(d_priority == NOT_DEFINED){
            return "Unknown pryority! Keeping the: " + to_string(settings.default_priority) + " pryority.";
        }
        settings.default_priority = d_priority;
        this->redraw();
        return "Default prioryty is seted to:" + to_string(settings.default_priority);
    }
    if(lower_command.length() >= cmd::name_len(cmd::UNDEFINED_PRIORITY1) && lower_command.substr(0,cmd::name_len(cmd::UNDEFINED_PRIORITY1)) == cmd::name(cmd::UNDEFINED_PRIORITY1)){
        Priority d_priority = translate_priority(lower_command.substr(cmd::name_len(cmd::UNDEFINED_PRIORITY1),-1));
        if(d_priority == NOT_DEFINED){
            return "Unknown pryority! Keeping the: " + to_string(settings.default_priority) + " pryority.";
        }
        settings.default_priority = d_priority;
        this->redraw();
        return "Default prioryty is seted to:" + to_string(settings.default_priority);
    }

    // příkaz SHOW  ---------------------------------------------------------------------------------------------------------------------
    if(lower_command.length() >= cmd::name_len(cmd::SHOW) && lower_command.substr(0,cmd::name_len(cmd::SHOW)) == cmd::name(cmd::SHOW)){
        std::string prio_list = this->hiden_messages(lower_command.substr(cmd::name_len(cmd::SHOW),-1),false);
        this->redraw();
        return prio_list + ".";
    }
    if(lower_command.length() >= cmd::name_len(cmd::SHOW1) && lower_command.substr(0,cmd::name_len(cmd::SHOW1)) == cmd::name(cmd::SHOW1)){
        std::string prio_list = this->hiden_messages(lower_command.substr(cmd::name_len(cmd::SHOW1),-1),false);
        this->redraw();
        return prio_list + ".";
    }

    // příkaz HIDE  ---------------------------------------------------------------------------------------------------------------------
    if(lower_command.length() >= cmd::name_len(cmd::HIDE) && lower_command.substr(0,cmd::name_len(cmd::HIDE)) == cmd::name(cmd::HIDE)){
        std::string prio_list = this->hiden_messages(lower_command.substr(cmd::name_len(cmd::HIDE),-1),true);
        this->redraw();
        return prio_list + ".";
    }
    if(lower_command.length() >= cmd::name_len(cmd::HIDE1) && lower_command.substr(0,cmd::name_len(cmd::HIDE1)) == cmd::name(cmd::HIDE1)){
        std::string prio_list = this->hiden_messages(lower_command.substr(cmd::name_len(cmd::HIDE1),-1),true);
        this->redraw();
        return prio_list + ".";
    }

    // příkaz SHOW ONLY  ---------------------------------------------------------------------------------------------------------------------
    if(lower_command.length() >= cmd::name_len(cmd::SHOW_ONLY) && lower_command.substr(0,cmd::name_len(cmd::SHOW_ONLY)) == cmd::name(cmd::SHOW_ONLY)){
        for (size_t i = 0; i < PRIORYTY_ENUM_END; i++){
            settings.hide_messages[i] = true;
        }
        std::string prio_list = this->hiden_messages(lower_command.substr(cmd::name_len(cmd::SHOW_ONLY),-1),false);
        this->redraw();
        return prio_list + ".";
    }
    if(lower_command.length() >= cmd::name_len(cmd::SHOW_ONLY1) && lower_command.substr(0,cmd::name_len(cmd::SHOW_ONLY1)) == cmd::name(cmd::SHOW_ONLY1)){
        for (size_t i = 0; i < PRIORYTY_ENUM_END; i++){
            settings.hide_messages[i] = true;
        }
        std::string prio_list = this->hiden_messages(lower_command.substr(cmd::name_len(cmd::SHOW_ONLY1),-1),false);
        this->redraw();
        return prio_list + ".";
    }


    // příkaz HIDE ONLY  ---------------------------------------------------------------------------------------------------------------------
    if(lower_command.length() >= cmd::name_len(cmd::HIDE_ONLY) && lower_command.substr(0,cmd::name_len(cmd::HIDE_ONLY)) == cmd::name(cmd::HIDE_ONLY)){
        for (size_t i = 0; i < PRIORYTY_ENUM_END; i++){
            settings.hide_messages[i] = false;
        }
        std::string prio_list = this->hiden_messages(lower_command.substr(cmd::name_len(cmd::HIDE_ONLY),-1),true);
        this->redraw();
        return prio_list + ".";
    }
    if(lower_command.length() >= cmd::name_len(cmd::HIDE_ONLY1) && lower_command.substr(0,cmd::name_len(cmd::HIDE_ONLY1)) == cmd::name(cmd::HIDE_ONLY1)){
        for (size_t i = 0; i < PRIORYTY_ENUM_END; i++){
            settings.hide_messages[i] = false;
        }
        std::string prio_list = this->hiden_messages(lower_command.substr(cmd::name_len(cmd::HIDE_ONLY1),-1),true);
        this->redraw();
        return prio_list + ".";
    }

    // příkaz TIME  ---------------------------------------------------------------------------------------------------------------------
    if(lower_command == cmd::name(cmd::TIME_OFF) || lower_command == cmd::name(cmd::TIME_OFF1)){
        settings.print_arival_time = false;
        this->redraw();
        return "";
    }
    if(lower_command == cmd::name(cmd::TIME_ON) || lower_command == cmd::name(cmd::TIME_ON1)){
        settings.print_arival_time = true;
        this->redraw();
        return "";
    }

    // příkaz HINT  ---------------------------------------------------------------------------------------------------------------------
    if(lower_command == cmd::name(cmd::HINT_OFF) || lower_command == cmd::name(cmd::HINT_OFF1)){
        settings.use_hinting = false;
        return "Napovídání vypnuto.";
    }
    if(lower_command == cmd::name(cmd::HINT_ON) || lower_command == cmd::name(cmd::HINT_ON1)){
        settings.use_hinting = true;
        return "Napovídání zapnuto.";
    }

    // příkaz LOCATION  ---------------------------------------------------------------------------------------------------------------------
    if(lower_command == cmd::name(cmd::FILE_LOCATION_OFF) || lower_command == cmd::name(cmd::FILE_LOCATION_OFF1)){
        settings.print_file_location = false;
        this->redraw();
        return "";
    }
    if(lower_command == cmd::name(cmd::FILE_LOCATION_ON) || lower_command == cmd::name(cmd::FILE_LOCATION_ON1)){
        settings.print_file_location = true;
        this->redraw();
        return "";
    }

    // příkaz SHORT  ---------------------------------------------------------------------------------------------------------------------
    if(lower_command == cmd::name(cmd::FILE_SHORTTING_OFF) || lower_command == cmd::name(cmd::FILE_SHORTTING_OFF1)){
        settings.use_short_file_names = false;
        this->redraw();
        return "";
    }
    if(lower_command == cmd::name(cmd::FILE_SHORTTING_ON) || lower_command == cmd::name(cmd::FILE_SHORTTING_ON1)){
        settings.use_short_file_names = true;
        this->redraw();
        return "";
    }

    // příkaz HIGHLITING  ---------------------------------------------------------------------------------------------------------------------
    if(lower_command == cmd::name(cmd::HIGHLITING_OFF) || lower_command == cmd::name(cmd::HIGHLITING_OFF1)){
        settings.in_text_highliting = false;
        this->redraw();
        return "";
    }
    if(lower_command == cmd::name(cmd::HIGHLITING_ON) || lower_command == cmd::name(cmd::HIGHLITING_ON1)){
        settings.in_text_highliting = true;
        this->redraw();
        return "";
    }

    // příkaz CLEAR pro vymazání přijatých zpráv  ---------------------------------------------------------------------------------------------------------------------
    if(lower_command == cmd::name(cmd::CLEAR) || lower_command == cmd::name(cmd::CLEAR1) || lower_command == cmd::name(cmd::CLEAR2) ){
        this->clear_hystory();
        return "Done";
    }

    return command_handler(command);
}


std::string Terminal::shorten_to_line(std::string & text,int reserved_space){
    //snížení chyby pro dlouhý terminál
    int wcorrect = (5*(console->get_console_width()<80));

    //zarovnání nápovědy na šířku get_console_width() aby se text neduplikoval
    int char_owerhead = console->get_console_width() - (reserved_space + text.length() + wcorrect) - 3; // (-3) je pro ... přidaného na konec řádku
    if(char_owerhead < 0 && (0-char_owerhead < text.length())){
        return text.substr(0,text.length() + char_owerhead) + "..."; // příkaz je delší než řádek proto ho zkracujeme
    }else if(char_owerhead < 0 && 0-char_owerhead > text.length()){
        return ""; // na příkaz není vůbec místo ořežeme co můžeme //* necháme uživatele vytrestat že jako debil má příkazovou řádku o mikrošířce :)
    }
    return text;
}


std::string Terminal::composite_command_line(){
    std::string out = "";
    if(serial_console_connected){
        out.append("\x1B[32m➤ ");
    }else{
        out.append("\x1B[31m➤ ");
    }
    out.append(FOREBLU);
    out.append(command);
    out.append(RESETTEXT);

    last_command_printed_len = command.length();

    if(settings.use_hinting){
        std::array<std::string, 2> g_cmd = guess_command(command);
        if(!g_cmd.at(0).empty()){
            std::string cmd_completer = g_cmd.at(0).substr(command.length(),-1);
            out.append(FOREGRY);
            out.append(cmd_completer);
            out.append(RESETTEXT);

            out.append(" : ");

            std::string help_text = shorten_to_line(g_cmd.at(1),5+command.length()); // zarovnáme help aby nepřetekl cmd line

            out.append(FOREWHT);
            out.append(help_text); // pridáme help
            out.append(RESETTEXT);
            
            last_help_printed_len = cmd_completer.length() + strlen_utf8(help_text) + 3; 
        }else{
            last_help_printed_len = 0; 
        }
    }else{
        last_help_printed_len = 0; 
    }

    return out;
};

std::array<std::string, 2> Terminal::guess_command(std::string cmd){
    if(command == ""){
        return {"",""};
    }

    std::string small_cmd = cmd;
    std::transform(small_cmd.begin(), small_cmd.end(), small_cmd.begin(), [](unsigned char c){ return std::tolower(c); } );

    int probable_index = -1;

    for (size_t i = 0; i < cmd::MAX_COMMAND; i++){
        if(cmd::commands[i].command.compare(0,small_cmd.length(),small_cmd) == 0){
            if(cmd::commands[i].command == small_cmd){
                probable_index = i;
                break; // 100%tní shoda
            }

            if(probable_index == -1){
                probable_index = i;
            }else if(cmd::commands[i].command.length() < cmd::commands[probable_index].command.length()){
                probable_index = i;
            }
        }
    }

    if(probable_index == -1){
        return {"", ""};
    }

    if(cmd::commands[probable_index].command.back() == ' '){
        return {cmd::commands[probable_index].command + "[...]", cmd::commands[probable_index].description};
    }else{
        return {cmd::commands[probable_index].command, cmd::commands[probable_index].description};
    }

}

void Terminal::tick(bool consume_input){

    // každých 5000 ticků vypíse nastrádané linky. to je to aby se dalo číst comand co člověk píše
    if(timer->ticked()){

        //todo (zamezit aby se vykreslovalo pořád i když není linka) (optimalizace??)
        print_lines(get_waiting_to_print());
    }

    if(!consume_input){ return; } // terminál je nastavený tak aby nekonzumoval žádné znaky

    // braní příkazů z příkazové řádky
    if(console->has_any_char() || (continuing_with_cahracter != -1 && settings.use_confirm_character)){
        repeat_input:
        // rewrite only command
        std::string deler;
        char input;
        if(continuing_with_cahracter != -1 && settings.use_confirm_character){
            input = continuing_with_cahracter;
            continuing_with_cahracter = -1;
        }else{
            input = (char)console->get_character();
        }

        switch (input){
        case 127: // linux backward
        case '\b':
            if(command.size() >= (cursor_position + 1)){
                command.erase(command.size() - (cursor_position + 1), 1);
            }
            break;
        case '\n':
        case '\r':
            set_command_response(do_command(command)); // handler mimo objekt

            // odstraníme stejný dříve použitý příkaz
            remove_duplicate_command(command);

            // pridáme do historie příkazů
            used_commands.insert(used_commands.begin(),command);
            command = "";
            selected_command = 0;
            cursor_position = 0;
            break;
        case 9:
            command = guess_command(command).at(0);
        break;
        case -32:
        case 27:
            //šipky a escape sequence
            command_controll(input);
            break;
        default:
            // pridá znak na místo kurzoru
            command.insert(command.end() - this->cursor_position ,input);
            break;
        }
        
        // vymažeme předešlý příkaz
        deler += '\r'; 
        for (size_t i = 0; i < last_command_printed_len + last_help_printed_len + 5; i++){
            deler += ' ';
        }

        // vykreslit samotný příkaz
        deler += '\r';
        deler.append(composite_command_line());


        // poslat buffer do commandline
        console->platform_print(deler);

        //nastaví pozici kursoru
        this->set_cursor_in_command();
    }
    return;
}