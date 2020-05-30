#include<iostream>
#include<fstream>
#include<string>
#include<sstream>
#include<vector>
using namespace std;

string subcommand(string command)
{
    int state=0;
    for(int i=0;i<command.size();i++){
        if(command[i]!=' ' && state==0)
            state=1;
        else if(command[i]==' ' && state==1)
            state=2;
        else if(command[i]!=' ' && state==2)
            return command.substr(i,command.size());
    }
    return "";
}

bool run_command(string command){
    stringstream inp(command);
    string root_com;
    string temp;
    inp>>root_com;
    if(root_com=="system"){
        cout<<"Run system command"<<endl;
        temp=subcommand(command);
        system(temp.c_str());
    }else if(root_com=="output"){
        cout<<subcommand(command)<<endl;
    }else if(root_com=="exit"){
        cout<<"Bye!\nPress any key to exit"<<endl;
        getchar();
        return true;
    }else if(root_com=="runfile"){
        string path;
        path=subcommand(command);
        ifstream rf(path.c_str());
        while(!rf.eof()){
            getline(rf,temp);
            if(run_command(temp))
                return true;
        }
        rf.close();
        
    }else{
        cout<<"Unknown command"<<endl;
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
    cout<<"Squid Beta  v0b1"<<endl<<"Copyright MineCommander 2020"<<endl;
    string inp_com;
    while(1)
    {
        cout<<">>>";
        getline(cin,inp_com);
        if(run_command(inp_com))
            return 0;
    }
}
