#include <iostream>
#include <unistd.h>
#include <string>
#include <cstring>
#include <sys/types.h>
#include <fstream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <cmath>
#include <sstream>

using namespace std;
char name[100] = "server: ";
double buffer[100];

int PORT;
// char *buffer;
string string_use, save_to_print, cname, sname, message;

double p = 11;
double q = 13;
double e = 7;
double d, n;
double clnt, temp1, temp2, temp3;

int PORT_server;

void sending();
void receiving(int server_fd);
void *receive_thread(void *server_fd);

int gcd(int a, int h)
{
    int temp;
    while (1)
    {
        temp = a % h;
        if (temp == 0)
            return h;
        a = h;
        h = temp;
    }
}

void rsa()
{
    n = p * q;
    int count;
    int totient = (p - 1) * (q - 1);

    // for checking co-prime which satisfies e>1
    while (e < totient)
    {
        count = gcd(e, totient);
        if (count == 1)
            break;
        else
            e++;
    }

    // private key
    // d stands for decrypt

    // k can be any arbitrary value
    double k = 6;

    // choosing d such that it satisfies d*e = 1 + k * totient
    d = (1 + (k * totient)) / e;
}

class node
{
public:
    double n1, e1, d1;
    string name;
    int port;
    double clien;
    node *next;
};

class link_list
{

public:
    node *head;
    link_list()
    {
        head = NULL;
    }

    void insert()
    {

        node *temp = new node;
        temp->n1 = n;
        temp->d1 = d;
        temp->e1 = e;
        temp->port = PORT_server;
        temp->clien = clnt;

        temp->next = NULL;

        if (head != NULL)
        {

            temp->next = head;
            head = temp;
        }

        else
            head = temp;
    }

    node *search(double x)
    {
        node *current = head;

        while (current != NULL)
        {
            if (current->clien == x)
            {
                return current;
            }

            else
            {
                current = current->next;
            }
        }
    }
};

int main(int argc, char const *argv[])
{

    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int k = 0;

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        cout << " Socket Failure " << endl;
        exit(EXIT_FAILURE);
    }
    // Forcefully attaching socket to the port

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8888);

    // Printed the server socket addr and port
    // cout << " IP: " << inet_ntoa(address.sin_addr) << endl;
    cout << " Port: " << (int)ntohs(address.sin_port) << endl;

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        cout << " Bind Fail: " << endl;
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 5) < 0)
    {
        cout << " Listen " << endl;
        exit(EXIT_FAILURE);
    }

    int ch;
    pthread_t tid;
    pthread_create(&tid, NULL, &receive_thread, &server_fd); // Creating thread to keep receiving message in real time

    while (1)

    {
        cout << " Enter: ";
        cin >> ch;
        switch (ch)
        {
        case 1:
            sending();
            break;
        case 0:
            cout << "\nLeaving\n";
            break;
        default:
            cout << "\nWrong choice\n";
        }
    }

    close(server_fd);

    return 0;
}

// Sending messages to port
void sending()
{

    // Fetching port number

    // IN PEER WE TRUST
    cout << "Enter the port to send message:"; // Considering each peer will enter different port
    cin >> PORT_server;

    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char hello[1024] = {0};
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        cout << "\n Socket creation error \n";
        return;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY; // INADDR_ANY always gives an IP of 0.0.0.0
    serv_addr.sin_port = htons(PORT_server);

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        cout << "\nConnection Failed \n";
        return;
    }

    buffer[0] = temp1;
    buffer[1] = temp2;
    buffer[2] = temp3;
    buffer[3] = 2000000;
    send(sock, buffer, sizeof(buffer), 0);
    printf("\nMessage sent\n");
    close(sock);
}

// Calling receiving every 2 seconds
void *receive_thread(void *server_fd)
{
    int s_fd = *((int *)server_fd);
    while (1)
    {
        sleep(2);
        receiving(s_fd);
    }
}

// Receiving messages on our port
void receiving(int server_fd)
{
    link_list lk;
    struct sockaddr_in address;
    double valread;
    double buffer[100];

    char *subString, *subString1, *subString2;

    int addrlen = sizeof(address);
    fd_set current_sockets, ready_sockets;

    // Initialize my current set
    FD_ZERO(&current_sockets);
    FD_SET(server_fd, &current_sockets);
    int k = 0;
    while (1)
    {
        k++;
        ready_sockets = current_sockets;

        if (select(FD_SETSIZE, &ready_sockets, NULL, NULL, NULL) < 0)
        {
            perror("Error");
            exit(EXIT_FAILURE);
        }

        for (int i = 0; i < FD_SETSIZE; i++)
        {
            if (FD_ISSET(i, &ready_sockets))
            {

                if (i == server_fd)
                {
                    int client_socket;

                    if ((client_socket = accept(server_fd, (struct sockaddr *)&address,
                                                (socklen_t *)&addrlen)) < 0)
                    {
                        perror("accept");
                        exit(EXIT_FAILURE);
                    }
                    FD_SET(client_socket, &current_sockets);
                }
                else
                {
                    valread = recv(i, buffer, sizeof(buffer), 0);
                    // cout << buffer[1];
                    clnt = buffer[0];

                    if (buffer[1] == 0)
                    {
                        cout << " Hey! Im server! " << endl;
                        rsa();
                        lk.insert();
                    }

                    if (buffer[1] != 0)
                    {
                        node *obj1 = lk.search(buffer[0]);

                        node *obj2 = lk.search(buffer[1]);

                        temp1 = obj1->d1;
                        
                        temp2 = obj2->e1;
                        
                        temp3 = obj1->n1;
                        
                    }

                    FD_CLR(i, &current_sockets);
                }
            }
        }

        if (k == (FD_SETSIZE * 2))
            break;
    }
}
