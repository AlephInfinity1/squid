#include<iostream>
#include<fstream>
#include<string>
#include<sstream>
#include<vector>
using namespace std;

struct sct_var
{
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
    return 2147483647;
}

string compile_var(string command)
{
    /*
    state=0 δʶ�𵽱������ �����ȱʧ ֻ�к�벿��
    state=1 ����ȱʧ ֻ��ǰ�벿��
    state=2 ��ʶ�������ı������
    */
    int state = 0;
    int facing = 1;
    int notes=0, notee=0;
    for (int i = command.size() - 1; i > 0 && i < command.size(); i = i - facing) {
        if (facing == 1 && command[i] == '<' && command[i - 1] == '$') {
            notes = i + 1;
            facing = -1;
            state = 1;
        }
        else if (command[i] == '>') {
            notee = i - 1;
            state = 2;
        }
        //cout << i << " " << facing << endl; //debug
    }
    //cout << command.substr(notes, notee-1) << endl; //debug
    if (state == 2) {
        int plc = varname_place(command.substr(notes, notee-1));
        if (plc != 2147483647) {
            command.replace(notes-2, notee+1, flt2str(var_list[plc].valve));
            //cout << command << endl;    //debug
        }
        else {
            command.erase(notes-2, notee+1);
        }
    }
    return command;
}

bool run_command(string command,bool boardcast){
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
        ifstream rf(path.c_str());
        while (!rf.eof()) {
            getline(rf, temp);
            if (run_command(temp,false))
                return true;
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
            if (varname_place(temp_cg1) != 2147483647) {
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
            if (plc == 2147483647) {
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
                else {
                    cout << "Unknown oprator '" << temp_cg2 << "'" << endl;
                }
            }
        }
    }
    else{
        if (boardcast) cout<<"Unknown command"<<endl;
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
    cout << "Squid Beta  v0b1" << endl << "Copyright MineCommander 2020" << endl;
    string inp_com;
    while(1)
    {
        cout << ">>>";
        getline(cin,inp_com);
        if(run_command(inp_com,true))
            return 0;
    }
}
