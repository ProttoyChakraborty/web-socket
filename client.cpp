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
    int sock=socket(AF_INET,SOCK_STREAM,0);
    if(sock<0){
        cerr<<"Can't intiate a socket";
        return -1;
    }

    int port = 8000;
    string ipAdd="127.0.0.1";

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
    char buf[4096];
    string userInput;

    do {
        //		Enter lines of text
        cout << "> ";
        getline(cin, userInput);

        //		Send to server
        int sendRes = send(sock, userInput.c_str(), userInput.size() + 1, 0);
        if (sendRes == -1)
        {
            cout << "Could not send to server! Whoops!\r\n";
            continue;
        }

        //		Wait for response
        memset(buf, 0, 4096);
        int bytesReceived = recv(sock, buf, 4096, 0);
        if (bytesReceived == -1)
        {
            cout << "There was an error getting response from server\r\n";
        }
        else
        {
            //		Display response
            cout << "SERVER> " << string(buf, bytesReceived) << "\r\n";
        }
    } while(true);

      //	Close the socket
    close(sock);

    return 0;

}