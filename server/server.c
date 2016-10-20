#include <stdio.h>
#include <stdlib.h>

#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h> /* getprotobyname */
#include <netinet/in.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char **argv) {
    char buffer[5];
    char protoname[] = "tcp";
    int client_sockfd;
    int filefd;
    int i;
    int server_sockfd;
    int filename_len = 25;
    int sock;
    socklen_t client_len;
    ssize_t read_return;
    struct sockaddr_in addr;
    struct sockaddr_in client_address;
    int rc; /* holds return code of system calls */
    unsigned short server_port = 12345;

    if (argc > 2) 
    {
        server_port = strtol(argv[2], NULL, 10);
    }

    /* create Internet domain socket */
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) 
    {
        fprintf(stderr, "unable to create socket");
        exit(1);
    }

    /* fill in socket structure */
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY; // assign local ip address
    addr.sin_port = htons(server_port);

    /* bind socket to the port */
    rc = bind(sock, (struct sockaddr *)&addr, sizeof(addr));
    if (rc == -1) 
    {
        fprintf(stderr, "unable to bind to socket");
        exit(1);
    }

    /* listen for clients on the socket */
    rc = listen(sock, 1);
    if (rc == -1) {
    fprintf(stderr, "listen failed: ");
    exit(1);
    }

    /* 
    * finish setting up socket here
    * citation: I refer to this website for socket setting up: 
    * http://stackoverflow.com/questions/204169/how-to-create-a-tcp-socket-connect-using-c-to-a-predefined-port
    */

    while (1) 
    {
        client_len = sizeof(client_address);
        puts("waiting for client");
        client_sockfd = accept(
            sock,
            (struct sockaddr*)&client_address,
            &client_len
        );
    	ssize_t len;
    	char file_name[filename_len];
        len = read(client_sockfd, file_name, filename_len);

        filefd = open(&file_name[0], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR); // get full permission for the file created by server

        if (filefd == -1)
        {
            perror("error occurs while opening th efile");
            exit(EXIT_FAILURE);
        }
        while (1)
        {
            read_return = read(client_sockfd, buffer, 5);
            if (read_return == 0)
            {
                break;
            }
            if (read_return == -1)
            {
                perror("error occurs while reading the file");
                exit(EXIT_FAILURE);
            }
            if (write(filefd, buffer, read_return) == -1)
            {
                perror("error occurs while write to the file");
                exit(EXIT_FAILURE);
            }
        }
        close(filefd);
        close(client_sockfd);
    }
    return EXIT_SUCCESS;
}
