#include<iostream>
#include<fstream>
#include<string>
#include<sstream>
#include<cmath>
#include<vector>
using namespace std;
const int VNP_ERROR = 2147483647;
struct ist {
    float x1 = 0, x2 = 0;
    string oprt;
    bool enable = false;
};
ist if_status;
struct sct_var {
    string name;
    float valve = 0;
};

vector<sct_var> var_list;

string subcommand(string command)
{
    int state = 0;
    for (int i = 0; i < command.size(); i++) {
        if (command[i] != ' ' && state == 0)
            state = 1;
        else if (command[i] == ' ' && state == 1)
            state = 2;
        else if (command[i] != ' ' && state == 2)
            return command.substr(i, command.size());
    }
    return "";
}

float str2flt(string str) {
    if (str.size() == 0) return 0;
    else {
        stringstream strm(str);
        float temp;
        strm >> temp;
        return temp;
    }
}

string flt2str(float flt) {
    stringstream strm;
    strm << flt;
    string temp(strm.str());
    return temp;
}

int varname_place(string var_name) {
    for (int i = 0; i < var_list.size(); i++) {
        if (var_name == var_list[i].name)
            return i;
    }
    return VNP_ERROR;
}

string var_valve_str(string varname) {
    int plc = varname_place(varname);
    if (plc == VNP_ERROR)
        return "";
    else
        return flt2str(var_list[plc].valve);
}

string compile_var(string cmd)  //我蜂了
{
    string varname;
    int state = 0;
    int ns = 0, ne = cmd.size() - 1;
    for (int i = 0; i < cmd.size(); i++) {
        if (cmd[i] == '$' && cmd[i + 1] == '<') {
            varname.clear();
            state = 1;
            ns = i;
            i += 2;
        }
        else if (state == 1 && cmd[i] == '>') {
            state = 2;
            ne = i;
            break;
        }
        varname.push_back(cmd[i]);
    }
    if (state == 2) {
        int ln = ne - ns + 1;
        cmd.replace(ns, ln, var_valve_str(varname));
        cmd = compile_var(cmd);
    }
    return cmd;
}

bool ifstatu(void) {
    int oprt = 0;
    if (if_status.oprt == ">" || if_status.oprt == "is_bigger_than")
        oprt = 1;
    else if (if_status.oprt == ">=" || if_status.oprt == "isnot_less_than")
        oprt = 2;
    else if (if_status.oprt == "<" || if_status.oprt == "is_less_than")
        oprt = 3;
    else if (if_status.oprt == "<=" || if_status.oprt == "isnot_bigger_than")
        oprt = 4;
    else if (if_status.oprt == "=" || if_status.oprt == "==" || if_status.oprt == "is")
        oprt = 5;
    else if (if_status.oprt == "!=" || if_status.oprt == "isnot")
        oprt = 6;
    if (if_status.enable == true)
        return (if_status.x1 > if_status.x2 && oprt == 1) || (if_status.x1 >= if_status.x2 && oprt == 2) || (if_status.x1 < if_status.x2&& oprt == 3) || (if_status.x1 <= if_status.x2 && oprt == 4) || (if_status.x1 == if_status.x2 && oprt == 5) || (if_status.x1 != if_status.x2 && oprt == 6);
    else
        return true;
}

bool run_command(string command,bool boardcast){
    if (!ifstatu) return false;
    command = compile_var(command);
    stringstream inp(command);
    string root_com;
    string temp;
    inp >> root_com;
    if (root_com == "system") {
        if (boardcast) cout << "Run system command" << endl;
        temp = subcommand(command);
        system(temp.c_str());
    }
    else if (root_com == "output") {
        if (boardcast) cout << subcommand(command) << endl;
    }
    else if (root_com == "exit") {
        cout << "Bye!\nPress any key to exit" << endl;
        getchar();
        return true;
    }
    else if (root_com == "runfile") {
        string path;
        path = subcommand(command);
        ifstream rf(path.c_str(), ios::_Nocreate);
        if (rf) {
            while (!rf.eof()) {
                getline(rf, temp);
                if (run_command(temp,false))
                    return true;
            }
        }
        else {
            cout << "File '" << path << "' does not exist" << endl;
        }
        rf.close();
    }
    else if (root_com == "var") {
        string temp_rc;
        string temp_cg1;
        string temp_cg2;
        string temp_cg3;
        stringstream trc(subcommand(command));
        trc >> temp_rc >> temp_cg1 >> temp_cg2 >> temp_cg3;
        if (temp_rc == "new") {
            if (varname_place(temp_cg1) != VNP_ERROR) {
                if (boardcast) cout << "The variable '" << temp_cg1 << "' was already exists" << endl;
            }
            else {
                sct_var temp_var;
                temp_var.name = temp_cg1;
                var_list.push_back(temp_var);
                if (boardcast) cout << "New variable '" << temp_cg1 << "' has been created" << endl;
            }
        }
        else if (temp_rc == "list") {
            cout << "There are " << var_list.size() << " variables exist:" << endl;
            for (int i = 0; i < var_list.size(); i++)
                cout << var_list[i].name << " = " << var_list[i].valve << endl;
        }
        else if (temp_rc == "operation" || temp_rc == "ope") {
            int plc = varname_place(temp_cg1);
            if (plc == VNP_ERROR) {
                if (boardcast) cout << "The variable '" << temp_cg1 << "' does not exist" << endl;
            }
            else {
                float cg_temp = str2flt(temp_cg3);
                if (temp_cg2 == "+" || temp_cg2 == "add" || temp_cg2 == "plus") {
                    var_list[plc].valve += cg_temp;
                    if (boardcast) cout << "Variable '" << temp_cg1 << "' has been added by " << cg_temp << " (now " << var_list[plc].valve << ")" << endl;
                }
                else if (temp_cg2 == "-" || temp_cg2 == "remove" || temp_cg2 == "minus") {
                    var_list[plc].valve -= cg_temp;
                    if (boardcast) cout << "Variable '" << temp_cg1 << "' has been removed by " << cg_temp << " (now " << var_list[plc].valve << ")" << endl;
                }
                else if (temp_cg2 == "*" || temp_cg2 == "multiply") {
                    var_list[plc].valve *= cg_temp;
                    if (boardcast) cout << "Variable '" << temp_cg1 << "' has been multiplied by " << cg_temp << " (now " << var_list[plc].valve << ")" << endl;
                }
                else if (temp_cg2 == "/" || temp_cg2 == "divide") {
                    if (cg_temp == 0) if (boardcast) cout << "Cannot be divided by 0" << endl; else;
                    else {
                        var_list[plc].valve /= cg_temp;
                        if (boardcast) cout << "Variable '" << temp_cg1 << "' has been divided by " << cg_temp << " (now " << var_list[plc].valve << ")" << endl;
                    }
                }
                else if (temp_cg2 == "=" || temp_cg2 == "set") {
                    var_list[plc].valve = cg_temp;
                    if (boardcast) cout << "Variable '" << temp_cg1 << "' has been setted to " << cg_temp << endl;
                }
                else if (temp_cg2 == "pow" || temp_cg2 == "power" || temp_cg2 == "^") {
                    var_list[plc].valve = pow(var_list[plc].valve, cg_temp);
                    if (boardcast) cout << "Variable '" << temp_cg1 << "' has been setted to " << var_list[plc].valve << endl;
                }
                else {
                    cout << "Unknown oprator '" << temp_cg2 << "'" << endl;
                }
            }
        }
    }
    else if (root_com == "if") {
        stringstream comps(subcommand(command));
        comps >> if_status.x1 >> if_status.oprt >> if_status.x2;
        if_status.enable = true;
        if (boardcast) cout << "Conditional judgment has been enabled";
    }
    else if (command == "(endif)") {
        if_status.enable = false;
        if (boardcast) cout << "Conditional judgment has been disabled";
    }
    else{
        if (boardcast) cout << "Unknown command '" << command << "'" << endl;
    }
    return false;
}

int main(int argc, char *argv[])
{
    /*
    string path("runfile ");
    path.append(argv[1]);
    if(argc>=2)
        if(run_command(path))
            return 0;
    */  //:thonk:
    cout << "Squid Beta  v0b1" << endl << "Copyright MineCommander (C) 2020" << endl;
    string inp_com;
    while(1)
    {
        cout << ">>>";
        getline(cin,inp_com);
        if(run_command(inp_com,true))
            return 0;
    }
}