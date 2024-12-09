/** Toto je soubor spouštěče který spouští programy (PRO_LINUX) */

#include <string>
#include <cstdlib>

enum ExecuterState { RUNNING, ENDED, SLEEPING}; // program runing | program just ended | program not runung and executer sleeps

class ProgramExecuter{
private:
    ExecuterState state = SLEEPING;
public:

    ProgramExecuter(){ state = SLEEPING; }

    // spustí program a přesměruje stdio vrátí jestli návratová hodnota byla 0 (sucess)
    int execute(std::string command,bool force_to_show_returncode = false){
        // příkaz byl proveden a běží v příkazovém módu
        state = RUNNING;

        // spustíme program (předává se jak stdout tak stdin)
        int return_val = std::system(command.c_str());

        // program skončil (není žádný program který by běžel déle než system() )
        state = ENDED;

        return return_val;
    }


    void tick(){
        //! nepoužívané 
        //je pro případ kdyby se přidělávali nějaké interní programy
    }


    // vrátí zdali program pořád běží (bez toho aby ovlivnil příznak ENDED)
    bool is_runing(){ return state == RUNNING; }


    // vrátí svůj stav (Ended je pro jedno přečtení více ne)
    ExecuterState get_state(){
        if(state == ENDED){
            state = SLEEPING;
            return ENDED;
        }
        return state;
    }
};
