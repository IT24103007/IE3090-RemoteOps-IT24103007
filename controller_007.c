#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define AGENT_PORT 9410
#define AGENT_IP "127.0.0.1"

int main(void)
{
    int sock_fd;
    struct sockaddr_in agent_addr;

    /* Create TCP socket */
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) {
        perror("socket");
        return EXIT_FAILURE;
    }

    memset(&agent_addr, 0, sizeof(agent_addr));
    agent_addr.sin_family = AF_INET;
    agent_addr.sin_port = htons(AGENT_PORT);

    /* Convert Agent IP address */
    if (inet_pton(AF_INET, AGENT_IP, &agent_addr.sin_addr) <= 0) {
        perror("inet_pton");
        close(sock_fd);
        return EXIT_FAILURE;
    }

    printf("Connecting to RemoteOps Agent at %s:%d...\n",
           AGENT_IP, AGENT_PORT);

    /* Connect to Agent */
    if (connect(sock_fd,
                (struct sockaddr *)&agent_addr,
                sizeof(agent_addr)) < 0) {
        perror("connect");
        close(sock_fd);
        return EXIT_FAILURE;
    }

    printf("Connected to RemoteOps Agent successfully.\n");

    close(sock_fd);

    return EXIT_SUCCESS;
}
