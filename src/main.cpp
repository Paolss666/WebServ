# include "../incl/webserve.hpp"
# include "ServerV.hpp"
// # include <iostream>

int main(int ac, char **av)
{
    ServerV  server;
    try {
        if (ac < 2)
            return (std::cerr << "ERREUR\n", 1);
        else
            server.initServer(av[1]);
        // server.loopServer();
    }
    catch(std::exception& c){
        std::cerr << c.what() << std::endl;
        return 1;
    }
}

// int main(void)
// {
//     int server_fd, new_socket; long valread;
//     struct sockaddr_in address;
//     int addrlen = sizeof(address);
    
//     char *hello = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!";
    
//     // Creating socket file descriptor
//     if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
//     {
//         perror("In socket");
//         exit(EXIT_FAILURE);
//     }
    

//     address.sin_family = AF_INET; // rapresente le protocole IP 
//     address.sin_addr.s_addr = INADDR_ANY;
//     // sin_port est l'addresse de la port auquel nous souhaitons nous connecter
//     address.sin_port = htons( PORT ); // conerts a shot integer (port)
//                                         // to a network representation
//     memset(address.sin_zero, '\0', sizeof address.sin_zero);
    
    
//     if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0)
//     {
//         perror("In bind");
//         exit(EXIT_FAILURE);
//     }
//     if (listen(server_fd, 10) < 0)
//     {
//         perror("In listen");
//         exit(EXIT_FAILURE);
//     }
//     while(1)
//     {
//         printf("\n+++++++ Waiting for new connection ++++++++\n\n");
//         if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
//         {
//             perror("In accept");
//             exit(EXIT_FAILURE);
//         }
        
//         char buffer[30000] = {0};
//         valread = read( new_socket , buffer, 30000);
//         printf("%s\n",buffer );
//         write(new_socket , hello , strlen(hello));
//         printf("------------------Hello message sent-------------------\n");
//         close(new_socket);
//     }
//     return 0;
// }