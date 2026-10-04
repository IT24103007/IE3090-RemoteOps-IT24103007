#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/sysinfo.h>

#define AGENT_PORT 9410
#define BACKLOG 10
#define AUTH_TOKEN "OPS-3007"
#define SID "7003"

int main(void)
{
    int server_fd, client_fd;
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);

    /* Create TCP socket */
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket");
        return EXIT_FAILURE;
    }

    /* Allow the port to be reused after restarting the Agent */
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR,
                   &opt, sizeof(opt)) < 0) {
        perror("setsockopt");
        close(server_fd);
        return EXIT_FAILURE;
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(AGENT_PORT);

    /* Bind socket to personalised port 9410 */
    if (bind(server_fd,
             (struct sockaddr *)&server_addr,
             sizeof(server_addr)) < 0) {
        perror("bind");
        close(server_fd);
        return EXIT_FAILURE;
    }

    /* Start listening for Controller connections */
    if (listen(server_fd, BACKLOG) < 0) {
        perror("listen");
        close(server_fd);
        return EXIT_FAILURE;
    }

    printf("RemoteOps Agent started.\n");
    printf("Listening on TCP port %d...\n", AGENT_PORT);

    /* Accept one Controller for this initial test */
    client_fd = accept(server_fd,
                       (struct sockaddr *)&client_addr,
                       &client_len);

    if (client_fd < 0) {
        perror("accept");
        close(server_fd);
        return EXIT_FAILURE;
    }

    printf("Controller connected from %s:%d\n",
           inet_ntoa(client_addr.sin_addr),
           ntohs(client_addr.sin_port));

   

    /* Receive AUTH command from Controller */
    char buffer[1024];
    ssize_t bytes_received;

    memset(buffer, 0, sizeof(buffer));
    bytes_received = recv(client_fd, buffer, sizeof(buffer) - 1, 0);

    if (bytes_received > 0) {
        buffer[bytes_received] = '\0';

        /* Remove newline characters */
        buffer[strcspn(buffer, "\r\n")] = '\0';

        if (strcmp(buffer, "AUTH " AUTH_TOKEN) == 0) {
            char response[128];
            snprintf(response, sizeof(response),
                     "OK AUTHENTICATED SID:%s\n", SID);

            send(client_fd, response, strlen(response), 0);
            printf("Controller authenticated successfully.\n");
/* Wait for the next command after authentication */
memset(buffer, 0, sizeof(buffer));
bytes_received = recv(client_fd, buffer, sizeof(buffer) - 1, 0);

if (bytes_received > 0) {
    buffer[bytes_received] = '\0';
    buffer[strcspn(buffer, "\r\n")] = '\0';

    if (strcmp(buffer, "SYSINFO") == 0) {
        char sys_response[256];
struct sysinfo info;
double cpu_load = 0.0;
long mem_used_mb = 0;
long uptime_sec = 0;

if (sysinfo(&info) == 0) {
    cpu_load = (double)info.loads[0] / (1 << SI_LOAD_SHIFT);

    unsigned long long total_ram =
        (unsigned long long)info.totalram * info.mem_unit;
    unsigned long long free_ram =
        (unsigned long long)info.freeram * info.mem_unit;

    mem_used_mb = (long)((total_ram - free_ram) /
                         (1024 * 1024));

    uptime_sec = info.uptime;
}

snprintf(sys_response, sizeof(sys_response),
         "OK SYSINFO %.2f %ld %ld SID:%s\n",
         cpu_load, mem_used_mb, uptime_sec, SID);

        send(client_fd, sys_response, strlen(sys_response), 0);
        printf("SYSINFO request processed.\n");
    }
}
        } else {
            char response[128];
            snprintf(response, sizeof(response),
                     "ERR 001 AUTH_FAILED SID:%s\n", SID);

            send(client_fd, response, strlen(response), 0);
            printf("Controller authentication failed.\n");
        }
    } close(client_fd);
    close(server_fd);

    return EXIT_SUCCESS;
}
