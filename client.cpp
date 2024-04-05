#include <vector>
#include <iostream>
#include <string>
#include <thread>
#include <iomanip>
#include <typeinfo>
#include <sstream> 

#include <sys/types.h>
#include <sys/socket.h>
#include <cstdlib>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <ctime>
#include <chrono>

#define SERVER_IP_ "127.0.0.1"

class Client{
    friend void startSend(int server){};
    public:
        Client(size_t seconds,char const name[4096],size_t port){
            client_socket = socket(AF_INET,SOCK_STREAM,0);
            this->seconds=seconds;
            this->port=port;
            strcpy(buffer,name);
            if(client_socket == -1 ){
                throw "Can't create socket\n";
            }
        }

        int makeConnect(){
            hint.sin_family=AF_INET;
            hint.sin_port=htons(port);
            inet_pton(AF_INET,SERVER_IP_,&hint.sin_addr);
            size = sizeof(hint);
            int ret = connect(client_socket, (sockaddr *) &hint, sizeof(hint));
            if(ret == -1){
                throw "Can't bind to ip/port";
            }
            else{
                return ret;
            }
        }

        std::string getTime(){
            time_t time_s = time(NULL);
            tm* timeinfo = localtime(&time_s);
            auto now=std::chrono::system_clock::now();
            auto ms =std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch())%1000;
            std::stringstream textStream;

            textStream<<std::put_time(timeinfo, "[%Y-%m-%d %H.%M.%S.");
            std::string s=textStream.str()+std::to_string(ms.count())+"]\n";

            return textStream.str()+std::to_string(ms.count())+"]\n";
        }
        void sendInfo(){
            std::string name{buffer};
            for(size_t i=0;i<seconds;i++){
                std::string info=name+getTime();
                strcpy(buffer,info.c_str());
                send(client_socket,buffer,4096,0);
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            }
        }

        ~Client(){
            exit(hint.sin_port);
        }
    private:
        int server_socket;
        int client_socket;
        char buffer[4096];
        size_t port;
        socklen_t size;
        sockaddr_in hint;
        size_t seconds;
        Client(Client& another_server){}
        Client & operator=(const Client & copy){
            return *this;
    }
};

int main(int argc, char* argv[] ){
        try{
            Client tcp_server{(size_t)atoi(argv[3]),argv[1],(size_t)atoi(argv[2])};
            tcp_server.makeConnect();
            tcp_server.sendInfo();
        }
        catch (const char* error_message){
            std::cout << error_message << std::endl;
        }
    return 0;
};
