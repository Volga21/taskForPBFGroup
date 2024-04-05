#include <iostream>
#include <string>
#include <fstream>
#include <fstream>
#include <thread>

#include <chrono>
#include <sys/types.h>
#include <sys/socket.h>
#include <cstdlib>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <ctime>

void handleConnection(int * );

class Server{
    public:

        Server(){
            server_socket = socket(AF_INET,SOCK_STREAM,0);
            if(server_socket == -1 ){
                std::cout<<"Can't create socket\n";
            }
            out.open("log.txt", std::ios::app | std::ios::out);
        }

        int makeBind(size_t port){
            
            hint.sin_family=AF_INET;
            hint.sin_port=htons(port);
            hint.sin_addr.s_addr= htons(INADDR_ANY);
            size = sizeof(hint);
            int ret = bind(server_socket, (sockaddr *) &hint, sizeof(hint));
            if(ret == -1){
                std::cerr<<"Can't bind to port\nPlease, check port\n";
                return -1;
            }   
            return 0;
        }
        
        int startListen(){
            if(listen(server_socket,SOMAXCONN) == -1){
                std::cerr<<"Can't listen";
                return -1;
            }
            else{
                while(true){
                    int client_socket = makeAccept(); 
                    if(client_socket == -1){
                        std::cout<<"Can't accept client\n";
                    }
                    else{      
                        int * point=&client_socket; 
                        std::thread f2(&Server::handleConnection,this,point);
                        f2.detach();
                    }
                }
                std::cout<<"Here2\n";
            }
            return 0;
        }

        int makeAccept(){
            return accept(server_socket,(sockaddr *) &hint,&size);
        }

        void writeMsg(std::string msg){
            if(out.is_open()){
                out<<msg;
            }
            else{
                out.open("log.txt", std::ios::app | std::ios::out);
                out<<msg;
            }
        }

        void handleConnection(int * p_client){
            char buffer[4096];
            int client = *(p_client);
            while(true){
                int byte = recv(client,buffer,4096,0);
                if(byte==0){
                    close(client);
                    out.close();
                    return;
                }    
                std::cout<<buffer;
                std::string s{buffer};
                writeMsg(buffer);
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            }
            
        }

        ~Server(){
            out.close();
            close(server_socket);
        }

    private:
        std::ofstream out;          // поток для записи
        int server_socket;
        int client_socket;
        socklen_t size;
        sockaddr_in hint;
        Server(Server& another_server){}
        Server & operator=(const Server & copy){return *this;}
};



int main(){
    Server tcp_server{};
    size_t port;
    std::ofstream out;
    std::cout<<"Input port:";
    std::cin>>port;
    if(tcp_server.makeBind(port)==-1){
        return 1;
    }
    if(tcp_server.startListen()==-1){
        return 2;
    }
    std::cout<<"Goodbye!\n";
    return 0;
};
