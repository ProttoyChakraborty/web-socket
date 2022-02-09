#include<iostream>
#include<sys/types.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netdb.h>
#include<arpa/inet.h>
#include<string.h>
using namespace std;
int main(){
    //create socket
    int listener=socket(AF_INET,SOCK_STREAM,0);
    if(listener<0){
        cerr<<"Can't create socket!!";
        return -1;
    }
    //bind socket to IP/Port
    sockaddr_in server;
    server.sin_family=AF_INET;
    server.sin_port=htons(8000);
    inet_pton(AF_INET,"0.0.0.0",&server.sin_addr);

    if(bind(listener, (sockaddr*)&server , sizeof(server))==-1){
        cerr<<"Can't Bind to IP/Port!!";
        return -2;
    }

    
    //mark the socket for listening 

    if(listen(listener,SOMAXCONN)==-1){
        cerr<<"Can't Listen !!";
        return -3;
    }

    //accept a call
    sockaddr_in client;
    socklen_t cliensize;
    char host[NI_MAXHOST]; //size of input buffer
    char svc[NI_MAXSERV]; // size of output buffer

    int cliensocket=accept(listener,(sockaddr*)&client,&cliensize);
    if(cliensocket==-1){
        cerr<<"Problem with client connection....";
        return -4;
    }
    //close the listening socket
    close(listener);
    memset(host,0,NI_MAXHOST);
    memset(svc,0,NI_MAXSERV);

    int res=getnameinfo((sockaddr*)&client,sizeof(client),host,NI_MAXHOST,svc,NI_MAXSERV,0);
    if(res){
        cout<<host<<"connected on"<<svc<<endl;
    }
    else{
        inet_ntop(AF_INET,&client.sin_addr,host,NI_MAXHOST);
        cout<<host<<"connected on"<<ntohs(client.sin_port)<<endl;
    }
    char buffer[1024];
    while(true){
        //clear buffer
        memset(buffer,0,4096);

        //message recieved
        int bytesRecv=recv(cliensocket,buffer,4096,0);
        if(bytesRecv==-1){
            cout<<"Connection issue..."<<endl;
            break;
        }
        if(bytesRecv==0){
            break;
        }
        cout<<"Recieved: "<<string(buffer,0,bytesRecv)<<endl;
    
        //resend message
        send(cliensocket,buffer,bytesRecv+1,0);
    }

    close(cliensocket);
    return 0;
}