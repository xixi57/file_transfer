#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h> /* getprotobyname */
#include <netinet/in.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <unistd.h>

int main(int argc, char **argv) {
    char protoname[] = "tcp";
    struct protoent *protoent;
    char *input_filename = "input.tmp";
    char *output_filename = "output.tmp";
    char *server_hostname = "127.0.0.1";
    char buffer[10];
    in_addr_t in_addr;
    in_addr_t server_addr;
    int filefd;
    int sockfd;
    int file_name_size = 25;
    ssize_t i;
    ssize_t read_return;
    struct hostent *hostent;
    struct sockaddr_in sockaddr_in;
    unsigned short server_port = 12345;

    if (argc > 1) 
    {
        input_filename = argv[1];
        if (argc > 2) 
        {
            output_filename = argv[2];
            if (argc > 3)
            {
                server_hostname = argv[3];
                if (argc > 4) 
                {
                    server_port = strtol(argv[4], NULL, 10);
                }
            }
        }
    }

    /*
    * setting up socket
    */

    struct sockaddr_in sin;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd == -1)
    {
        printf("error opening socket");
        return -1;
    }

    sin.sin_port = htons(server_port);
    sin.sin_addr.s_addr = inet_addr(server_hostname);;
    sin.sin_family = AF_INET;

    if(connect(sockfd, (struct sockaddr *)&sin,sizeof(struct sockaddr_in) ) == -1)
    {
        printf("error binding socket");
        return -1;
    }

    /* 
    * finish setting up socket here
    * citation: I refer to this website for socket setting up: 
    * http://stackoverflow.com/questions/204169/how-to-create-a-tcp-socket-connect-using-c-to-a-predefined-port
    */

    char name_buf[file_name_size];
    ssize_t len;
    strcpy(name_buf, output_filename);
    
    len = sizeof(name_buf);

    /* Send filename first*/
    if (write(sockfd, name_buf, len) == -1) 
    {
        perror("error occurs while writting to file");
        exit(EXIT_FAILURE);
    }

    filefd = open(input_filename, O_RDONLY);
    if (filefd == -1) {
        perror("error occurs while opening the file");
        exit(EXIT_FAILURE);
    }

    while (1) {
        read_return = read(filefd, buffer, file_name_size);
        if (read_return == 0)
        {
            break;
        }

        if (read_return == -1) 
        {
            perror("error occurs while reading the file");
            exit(EXIT_FAILURE);
        }

        if (write(sockfd, buffer, read_return) == -1) 
        {
            perror("error occurs while writing to the socket");
            exit(EXIT_FAILURE);
        }
    }

    close(filefd);
    exit(EXIT_SUCCESS);
}
