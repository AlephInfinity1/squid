#include<iostream>
#include<fstream>
#include<string>
#include<sstream>
#include<vector>
using namespace std;

bool run_command(string command){
    stringstream inp(command);
    string root_com;
    string temp;
    inp>>root_com;
    
    //cout<<root_com<<endl;
    //cout<<inp.str();    //debug
    
    if(root_com=="system"){
        cout<<"Run system command"<<endl;
        temp=command.substr(6,command.size()-1);
        system(temp.c_str());
    }else if(root_com=="output"){
        temp=command.substr(6,command.size()-1);
        cout<<temp<<endl;
    }else if(root_com=="exit"){
        cout<<"Bye!\nPress any key to exit"<<endl;
        getchar();
        return true;
    }else if(root_com=="runfile"){
        /*
        string path;
        path=command.substr(7,command.size()-1);
        ifsteam rf(path.c_str());
        vector<string> commands;
        */    //unfinish
    }else{
        cout<<"Unknown command"<<endl;
    }
    return false;
}



int main()
{
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
