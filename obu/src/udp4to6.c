#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>

char buffer[1500];

int create_socket(int net_proto, short int listen_port, char *interface, size_t interface_name_len) {

    int sockfd;
    if ( (sockfd = socket(net_proto, SOCK_DGRAM, IPPROTO_UDP)) < 0 ) 
    {
        fprintf(stderr, "Error creating socket");
        exit(1);
    }

    if(interface != NULL) {
        if(setsockopt(sockfd, SOL_SOCKET, SO_BINDTODEVICE, interface, interface_name_len) < 0) 
        {
            fprintf(stderr, "setsockopt SO_BINDTODEVICE failed\n");
            close(sockfd);
            return -1;
        }
        printf("Bound socket %d to device %s\n", sockfd, interface);
        fflush(stdout);
    }

    struct sockaddr_storage servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    if(net_proto == AF_INET || 1) {
        ((struct sockaddr_in *)&servaddr)->sin_family      = AF_INET;
        ((struct sockaddr_in *)&servaddr)->sin_addr.s_addr = INADDR_ANY;
        ((struct sockaddr_in *)&servaddr)->sin_port        = htons(listen_port);
    }
    else if(net_proto == AF_INET6) {
        ((struct sockaddr_in6 *)&servaddr)->sin6_family = AF_INET6;
        ((struct sockaddr_in6 *)&servaddr)->sin6_port = htons(listen_port);
        ((struct sockaddr_in6 *)&servaddr)->sin6_addr = in6addr_any;
    }
    else { fprintf(stderr, "Invalid net proto\n"); exit(1); }

    if ( bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0 )
    {
        perror("Error binding socket");
        exit(1);
    }
    return sockfd;
}

int forward(int in_sockfd, int out_sockfd, struct sockaddr *dest, socklen_t addrlen) {
    memset(buffer, 0, sizeof(buffer));
    size_t len = recv(in_sockfd, buffer, sizeof(buffer), 0);
    return sendto(out_sockfd, buffer, len, 0, dest, addrlen);
}

int main(int argc, char *argv[]) {
    struct sockaddr_in6 remote_dest;
    memset(&remote_dest, 0, sizeof(remote_dest));
    remote_dest.sin6_family = AF_INET6;
    remote_dest.sin6_port = htons(1337);
    inet_pton(AF_INET6, "fe80::20d:b9ff:fe33:fd7a", &remote_dest.sin6_addr);

    int eth_sockfd = create_socket(AF_INET, 1337, "brtrunk", sizeof("brtrunk") );
    int wifi_sockfd = create_socket(AF_INET6, 1338, "brtrunk", sizeof("brtrunk") );
    while(1) {
        if(-1 == forward(eth_sockfd, wifi_sockfd, (struct sockaddr *)&remote_dest, sizeof(remote_dest))) {
            perror("Error forwarding packet");
        }
    }
}