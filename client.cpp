#include <iostream>
#include <unistd.h>
#include <string>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <cmath>

using namespace std;

int PORT;
double name;
double sendd[100];

// double d = 103, e = 7, n = 143;
double d, e, n;
int cal(int pow, int val, int MOD)
{
    if (pow == 0)
        return 1;
    int v = cal(pow / 2, val, MOD);
    if (pow % 2 == 0)
        return (v * v) % MOD;
    else
        return (((v * val) % MOD) * v) % MOD;
}

void rsa(string at)
{
    // cout << at << endl;
    // cout << at.length() << endl;
    for (int i = 0; i < at.length(); i++)
    {
        double temp = double(at[i]);
        int c = cal(e, temp, n);
        // cout << c << endl;

        sendd[i] = c;
    }
}

void rsa2(double bu[])
{
    int lent = sizeof(bu);
    string ch;
    // cout << "lent " << lent << endl;
    cout << endl;
    cout << "Client: ";
    for (int i = 0; i < lent; i++)
    {
        double temp1 = double(bu[i]);

        int m = cal(d, temp1, n);
        // cout << m<< endl;
        char chr = char(m);
        ch +=chr;
        //printf("%c", chr);
        // //int check = int(m);
        // cout << char(m);
    }
    cout << ch << endl;
}

void sending();
void receiving(int server_fd);
void *receive_thread(void *server_fd);

int main(int argc, char const *argv[])
{
    cout << " Enter your name:-";
    cin >> name;

    cout << " Enter your port number:";
    cin >> PORT;

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
    address.sin_port = htons(PORT);

    // Printed the server socket addr and port
    cout << " IP: " << inet_ntoa(address.sin_addr) << endl;
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
    cout << "\n*****At any point in time press the following:*****\n1.Send message\n0.Quit\n";

    do
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
    } while (ch);

    close(server_fd);

    return 0;
}

// Sending messages to port
void sending()
{

    double buffer[2000] = {0};
    double client;
    // Fetching port number
    int PORT_server;

    // IN PEER WE TRUST

    cout << "Enter the client to send message:"; // Considering each peer will enter different port
    cin >> client;

    cout << "Enter the port to send message:"; // Considering each peer will enter different port
    cin >> PORT_server;

    sendd[0] = name;
    sendd[1] = client;

    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    string hello;
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

    if (PORT_server != 8888)
    {

        cout << "Enter your message:";
        cin >> hello;
        rsa(hello);
    }

    send(sock, sendd, sizeof(sendd), 0);
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
    struct sockaddr_in address;
    int valread;
    double buffer[100];
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

                    if (buffer[3] == 2000000)
                    {
                        cout << " Message from the Server " << endl;
                        d = buffer[0];
                        e = buffer[1];
                        n = buffer[2];
                    }

                    else
                    {
                        // int len = sizeof(buffer) / sizeof(double);
                        rsa2(buffer);
                    }

                    FD_CLR(i, &current_sockets);
                }
            }
        }

        if (k == (FD_SETSIZE * 2))
            break;
    }
}
