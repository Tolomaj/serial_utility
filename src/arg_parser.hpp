#include <list>
#include <string>
#include <vector>

struct parser_argument_node{
    std::string arg_name;
    std::string arg_value;
};

enum state { UNKNOWN ,PARAMETER , VALUE ,};
enum parser_return_value { OK , OK_DUPLICIT , OK_UNKNOWN_PARAMETER , OK_END_ERR , ERR_MISSING_ARGUMENT ,ERR_MISSING_VALUE, ERR_DUPLICIT_FILE};
std::string filename = "";

class ArgParser {
private:
    std::list<parser_argument_node> nodes;
    std::vector<std::string> commands;

    bool argument_duplicit(std::string name){
        if(name == "c" || name == "command" || name == "cmd"){
            return false;
        }
        for (auto node : nodes){
            if(node.arg_name == name){
                return true;
            }
        }
        return false;
    }

    parser_return_value parse_arguments(int argc, char** argv){
        parser_argument_node node;
        state parse_arg_state = UNKNOWN;

        for (int i = 0; i < argc; i++){
            std::string arg = argv[i];
            switch (parse_arg_state){
            case UNKNOWN:
            case PARAMETER:
                if(argv[i][0] == '-' && argv[i][1] == '-'){
                    node.arg_name = &argv[i][2];
                    parse_arg_state = VALUE;
                    continue;
                }else
                if(argv[i][0] == '-'){
                    node.arg_name = &argv[i][1];
                    parse_arg_state = VALUE;
                    continue;
                }
                return ERR_MISSING_ARGUMENT;
                break;
            case VALUE:
                parse_arg_state = PARAMETER;
                if(argument_duplicit(node.arg_name)){
                    std::cout << "duplicate argument" << std::endl;
                    continue;
                }
                node.arg_value = argv[i];
                nodes.push_back(node);
                break;
            }
        }
        
        if(parse_arg_state == VALUE){ return ERR_MISSING_VALUE; }
        return OK;
    }

    void print_args(){
        std::cout << filename << std::endl;
        for(auto node : nodes){
            std::cout << node.arg_name << " - " << node.arg_value << std::endl;
        }

        for(auto cmd : commands){
            std::cout << cmd << std::endl;
        }
    }

    parser_return_value set_settings(){
        parser_return_value rtrn;
        for(auto node : nodes){
            if(node.arg_name == "c" || node.arg_name == "command" || node.arg_name == "cmd"){
                commands.push_back(node.arg_value);
                continue;
            }
            rtrn = OK_UNKNOWN_PARAMETER;
        }
        return rtrn;
    }

public:

    std::vector<std::string> get_commands(){
        return commands;
    }

    parser_return_value process_arguments(int argc, char** argv){
        std::cout << "ARGS" << std::endl;

        parser_return_value stat = parse_arguments(argc, argv);
        if(stat > OK_END_ERR){ return stat; };

        stat = set_settings();
        if(stat > OK_END_ERR){ return stat; };

        print_args();
        return stat;
    }

};


