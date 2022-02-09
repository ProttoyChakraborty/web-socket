#include<iostream>
#include<sys/types.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netdb.h>
#include<arpa/inet.h>
#include<typeinfo>
#include<ctime>
#include<string.h>
using namespace std;
string fin="FIN";
string ack="ACK";

int Startclient(string serverIP,int portNum,int ping_time){
    time_t ti,tf;
    //create socket
    int sock=socket(AF_INET,SOCK_STREAM,0);
    if(sock<0){
        cerr<<"Can't intiate a socket";
        return -1;
    }

    int port = portNum;
    string ipAdd=serverIP;
    int t=ping_time;

    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(port);
    inet_pton(AF_INET, ipAdd.c_str(), &hint.sin_addr);

    //	Connect to the server on the socket
    int connectRes = connect(sock, (sockaddr*)&hint, sizeof(hint));
    if (connectRes == -1)
    {
        return 1;
    }

    //	While loop:
    char bufferc[1000];
    char userInput[1000];
    memset(userInput,'0',1000);

    int countSet=0;
    ti=time(NULL);
    bool flag=true;
   
    do {
        tf=time(NULL);
        countSet++;
        //		Enter lines of text

        //		Send to server
        int sendRes = send(sock, userInput, 1000, 0);
        if (sendRes == -1)
        {
            cout << "Could not send to server! Whoops!\r\n";
            continue;
        }

        //		Wait for response
        memset(bufferc, 0, 1000);
        int bytesReceived = recv(sock, bufferc, 1000, 0);
        if (bytesReceived == -1)
        {
            cout << "There was an error getting response from server\r\n";
        }
    } while(difftime(tf,ti)<t);
    
   
     while(flag){
        int sendRes = send(sock,fin.c_str(),1000,0);
        cout<<sendRes<<endl;
        memset(bufferc, 0, 1001);
        int bytesReceived = recv(sock, bufferc,1001, 0);
       if(bytesReceived==-1){
           cerr<<"Connection with server failed";
           break;
       }
       // ACK Recieve not working properly
        if(strcmp(bufferc,ack.c_str())==0){
            flag=false;
        }
        else{
            break;
        }
    }
    float b=countSet/(1000*t);
    cout<<"Sent: "<<countSet<<" KB"<<","<<"Rate: "<<b<<" Mbps"<<endl;


      //	Close the socket
    close(sock);

    return 0;
}




int Startserver(int portNum){
    //create socket
    int listener=socket(AF_INET,SOCK_STREAM,0);
    if(listener<0){
        cerr<<"Can't create socket!!";
        return -1;
    }
    //bind socket to IP/Port
    sockaddr_in server;
    server.sin_family=AF_INET;
    server.sin_port=htons(portNum);
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
    time_t t1,t2;
    int count=0;
    char buffer[1000];
    t1=time(NULL);
    while(true){
        count++;
        //clear buffer
        memset(buffer,0,1000);

        //message recieved
        int bytesRecv=recv(cliensocket,buffer,1000,0);
        if(strcmp(buffer,fin.c_str())==0){
            cout<<"FIN recieved";
            send(cliensocket,ack.c_str(),1000,0);
            t2=time(NULL);
            break;
        }
        if(bytesRecv==-1){
            cout<<"Connection issue..."<<endl;
            break;
        }
        if(bytesRecv==0){
            cout<<"client disconnected";
            break;
        }
        //resend message
        send(cliensocket,buffer,bytesRecv+1,0);
    }
    count--;
    int time_taken=t2-t1;
    float b=count/(1000*time_taken);
    cout<<"Recieved: "<<count<<" KB"<<","<<"Rate: "<<b<<" Mbps"<<endl;
    close(cliensocket);
    return 0;
}

int main(int argc,char* argv[]){
if(strcmp(argv[1], "-s") == 0){
    if(argc!=4){
        cerr<<"invalid arguments"<<endl;
        return -1;
    }
    int portNum= atoi(argv[3]);
    if(portNum<1024||portNum>65535){
        cerr<<"port number not in range"<<endl;
        return -1;
    }
    int out=Startserver(portNum);
    if(out<0){
        cerr<<"Something broke on server side , shutting gracefully!!";
        return -1;
    }
}
else if (strcmp(argv[1], "-c") == 0){
    if(argc!=8){
        cerr<<"Error:invalid arguments"<<endl;
        return -1;
    }
    string serverIP=argv[3];
    int portNum=atoi(argv[5]);
    if(portNum<1024||portNum>65535){
        cerr<<"Error:port number not in range"<<endl;
        return -1;
    }
    int time=atoi(argv[7]);
    if(time<=0){
        cerr<<"time argument must be greater than zero";
        return -1;
    }
    int out= Startclient(serverIP,portNum,time);
    if(out<0){
        cerr<<"Something broke on client side , shutting gracefully!!";
        return -1;
    }
}
else{
    cerr<<"Error: Invalid arguments"<<endl;
    return -1;
}
return 0;
}
