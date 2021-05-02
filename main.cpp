#include "client.h"
// g++ -std=c++11 main.cpp -o main.exe -lwsock32
auto main(int argc,char* argv[])->int
{
    if(argc<8)
    {
        std::cout<<"lack argument(s).\n";
        exit(1);
    }
    smtp mail_sender;
    mail_sender.smtp_connect(argv[1],argv[2]);
    mail_sender.set_username(argv[3]);
    mail_sender.set_password(argv[4]);
    mail_sender.set_receiver(argv[5]);
    mail_sender.set_subject(argv[6]);
    mail_sender.set_content(argv[7]);
    mail_sender.execute();
    return 0;
}

