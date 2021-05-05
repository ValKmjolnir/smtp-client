#ifndef __VSMTP_H__
#define __VSMTP_H__
class smtp
{
private:
    hostent* host_entry;
    sockaddr_in server_addr;
    int sd;
    std::string username;
    std::string password;
    std::string receiver;
    std::string subject;
    std::string content;
    char buf[1024];
    bool check(const char*,const char*);
    bool receive_ready();
    bool hello();
    bool login();
    bool send_mail_info();
    bool send_mail_head();
    bool send_mail_content();
    bool smtp_quit();
public:
    smtp();
    ~smtp();
    void smtp_connect(char*);
    void set_username(const char*);
    void set_password(const char*);
    void set_receiver(const char*);
    void set_subject();
    void set_content();
    void execute();
};

smtp::smtp()
{
#ifdef WIN32 
    WSADATA wsaData; 
    WSAStartup(0x0101, &wsaData); 
#endif
    /* clear struct server_addr */
    memset((char *)&server_addr,0,sizeof(server_addr));
    /* set family to Internet */
    server_addr.sin_family=AF_INET;
    /* create a socket. */
    sd=socket(PF_INET,SOCK_STREAM,0);
    if(sd<0)
    {
        std::cout<<"socket creation failed\n";
        exit(1);
    }
    return;
}

smtp::~smtp()
{
    /* close socket */
    closesocket(sd);
#ifdef WIN32
    WSACleanup();
#endif
    return;
}

bool smtp::check(const char* num,const char* err_info)
{
    if(buf[0]!=num[0] || buf[1]!=num[1] || buf[2]!=num[2])
    {
        std::cout<<num<<" reply not received from server."<<err_info<<'\n';
        return false;
    }
    return true;
}

void smtp::smtp_connect(char* hostname)
{
    server_addr.sin_port=htons(25);// smtp port
    host_entry=gethostbyname(hostname);
    memcpy(&server_addr.sin_addr,host_entry->h_addr,host_entry->h_length);

    /* Connect the socket to the specified server. */
    if(connect(sd,(sockaddr*)&server_addr,sizeof(server_addr))<0)
    {
        std::cout<<"connect failed\n";
        exit(1);
    }
    return;
}

bool smtp::receive_ready()
{
    buf[recv(sd,buf,1023,0)]='\0';
    std::cout<<buf;
    return check("220","server not ready.");
}
bool smtp::hello()
{
    std::string str="HELO "+username+"\r\n";
    if(send(sd,str.c_str(),str.length(),0)==SOCKET_ERROR)
    {
        std::cout<<"send HELO failed.\n";
        return false;
    }
    buf[recv(sd,buf,1023,0)]='\0';
    std::cout<<buf;
    return check("250","operation failed.");
}
bool smtp::login()
{
    std::string str;
    if(send(sd,"AUTH LOGIN\r\n",12,0)==SOCKET_ERROR)
    {
        std::cout<<"send login failed.\n";
        return false;
    }

    buf[recv(sd,buf,1023,0)]='\0';
    str=(buf+4);
    str=str.substr(0,str.length()-2);
    std::cout<<decode(str)<<'\n';
    if(!check("334","unable to input username"))
        return false;
    
    str=encode(username)+"\r\n";
    if(send(sd,str.c_str(),str.length(),0)==SOCKET_ERROR)
    {
        std::cout<<"send username failed.\n";
        return false;
    }
    
    buf[recv(sd,buf,1023,0)]='\0';
    str=(buf+4);
    str=str.substr(0,str.length()-2);
    std::cout<<decode(str)<<'\n';
    if(!check("334","unable to input password"))
        return false;

    str=encode(password)+"\r\n";
    if(send(sd,str.c_str(),str.length(),0)==SOCKET_ERROR)
    {
        std::cout<<"send password failed.\n";
        return false;
    }

    buf[recv(sd,buf,1023,0)]='\0';
    std::cout<<buf;
    return check("235","login failed.");
}
bool smtp::send_mail_info()
{
    std::string str="MAIL FROM:<"+username+">\r\n";
    if(send(sd,str.c_str(),str.length(),0)==SOCKET_ERROR)
    {
        std::cout<<"sender information transmission failed.\n";
        return false;
    }

    buf[recv(sd,buf,1023,0)]='\0';
    std::cout<<buf;
    if(!check("250",""))
        return false;

    str="RCPT To:<"+receiver+">\r\n";
    if(send(sd,str.c_str(),str.length(),0)==SOCKET_ERROR)
    {
        std::cout<<"receiver information transmission failed.\n";
        return false;
    }

    buf[recv(sd,buf,1023,0)]='\0';
    std::cout<<buf;
    if(!check("250",""))
        return false;
    
    str="DATA\r\n";
    if(send(sd,str.c_str(),str.length(),0)==SOCKET_ERROR)
    {
        std::cout<<"cannot send data.\n";
        return false;
    }

    buf[recv(sd,buf,1023,0)]='\0';
    std::cout<<buf;
    return check("354","cannot start sending data.");
}
bool smtp::send_mail_head()
{
    // set boundary to a special token 'valk'
    std::string str="From:valkmjolnir<"+username+">\r\n";
    str+="Subject:"+subject+"\r\n";
    str+="X-Mailer:smtp client by ValKmjolnir 2021.4.28 ver 1.0\r\n";
    str+="Content-type:multipart/mixed;boundary=valk\r\n\r\n";
    if(send(sd,str.c_str(),str.length(),0)==SOCKET_ERROR)
    {
        std::cout<<"send header failed.\n";
        return false;
    }
    return true;
}
bool smtp::send_mail_content()
{
    // use boundary to mark the beginning and the end of the content
    std::string str="--valk\r\n";
    str+="Content-type:text/plain;Charset=utf-8\r\n";
    str+="Content-Transfer-Encoding:8bit\r\n\r\n";
    str+=content+"\r\n--valk--"+"\r\n.\r\n";
    if(send(sd,str.c_str(),str.length(),0)==SOCKET_ERROR)
    {
        std::cout<<"send content failed.\n";
        return false;
    }

    buf[recv(sd,buf,1023,0)];
    std::cout<<buf;
    return check("250","cannot send content.");
}
bool smtp::smtp_quit()
{
    if(send(sd,"QUIT\r\n",6,0)==SOCKET_ERROR)
    {
        std::cout<<"quit failed.\n";
        return false;
    }
    buf[recv(sd,buf,1023,0)]='\0';
    std::cout<<buf;
    return check("221","process quited with errors.");
}
void smtp::execute()
{
    bool (smtp::*func[])()=
    {
        &smtp::receive_ready,
        &smtp::hello,
        &smtp::login,
        &smtp::send_mail_info,
        &smtp::send_mail_head,
        &smtp::send_mail_content,
        &smtp::smtp_quit,
        NULL
    };
    for(int i=0;func[i];++i)
        if(!(this->*func[i])())
            return;
    return;
}

void smtp::set_username(const char* argv)
{
    username=argv;
    return;
}
void smtp::set_password(const char* argv)
{
    password=argv;
    return;
}
void smtp::set_receiver(const char* argv)
{
    receiver=argv;
    return;
}
void smtp::set_subject()
{
    subject="";
    std::ifstream fin("subject",std::ios::binary);
    while(!fin.eof())
    {
        char c=fin.get();
        if(fin.eof()) break;
        subject+=c;
    }
    return;
}
void smtp::set_content()
{
    content="";
    std::ifstream fin("content",std::ios::binary);
    while(!fin.eof())
    {
        char c=fin.get();
        if(fin.eof()) break;
        content+=c;
    }
    return;
}
#endif