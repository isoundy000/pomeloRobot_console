//
//  main.cpp
//  pomeloRobot_console
//
//  Created by qiong on 14-2-24.
//  Copyright (c) 2014年 qiong. All rights reserved.
//

#include <iostream>
#include <cstdlib>
#include <vector>
#include <functional>
#include <unistd.h>
#include "TaskRunner.h"
#include "TaskRunnerContainer.h"
using namespace std;

const char* HOST = "127.0.0.1";
const int PORT = 3050;

void spawn(int n)
{
    if(n)
    {
        if(fork())//parent process
        {
            signal(SIGCHLD, SIG_IGN);
            if(n)
            {
                spawn(n-1);
            }
            else
                return;
        }
    }
}

int main(int argc, const char * argv[])
{
    if (argc!=4) {
        cout<<"Usage: ./pomelo_robot [child_process_nums] [thread_nums] [clients_nums]"<<endl;
        return 0;
    }
    int proc_nums = atoi(argv[1]);
    int clients_nums = atoi(argv[3]);
    int thread_nums = atoi(argv[2]);
    
    if (clients_nums == 0) {
        clients_nums = 1;
    }
    if (thread_nums == 0) {
        thread_nums = 1;
    }
    
    //create more than one process
    spawn(proc_nums);
    
    TaskRunnerContainer trc(HOST,PORT,clients_nums);
    trc.setGenerateFunc(bind([](int id)->TaskRunner*{
        TaskRunner* tr = new TaskRunner(id);
        char username[20];
        sprintf(username, "%d-%d",getpid(), id);
        
        /*login request*/
        json_t *msg = json_object();
        json_object_set_new(msg, "username", json_string(username));
        json_object_set_new(msg, "rid", json_string("room"));
        tr->addRequestTask("connector.entryHandler.enter",msg, 1);
        
        /*send msg request*/
//        json_t *msg1 = json_object();
//        json_object_set_new(msg1, "content", json_string("I wanna fuck u"));
//        json_object_set_new(msg1, "rid", json_string("room"));
//        json_object_set_new(msg1, "from", json_string(username));
//        json_object_set_new(msg1, "target", json_string("*"));
//        tr->addRequestTask("chat.chatHandler.send",msg1, 1);
        return tr;
    }, placeholders::_1));
    trc.startRun();
    return 0;
}

