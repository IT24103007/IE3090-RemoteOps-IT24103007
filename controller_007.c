#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define AGENT_PORT 9410
#define AGENT_IP "127.0.0.1"
#define AUTH_TOKEN "OPS-3007"

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
    /* Send authentication command */
    char auth_command[128];
    char response[2048];

    snprintf(auth_command, sizeof(auth_command),
             "AUTH %s\n", AUTH_TOKEN);

    send(sock_fd, auth_command, strlen(auth_command), 0);

    memset(response, 0, sizeof(response));
    ssize_t bytes_received =
        recv(sock_fd, response, sizeof(response) - 1, 0);

    if (bytes_received > 0) {
        response[bytes_received] = '\0';
        printf("Agent response: %s", response);
    }

   
/* Request SYSINFO after successful authentication */
const char *sysinfo_command = "SYSINFO\n";
send(sock_fd, sysinfo_command, strlen(sysinfo_command), 0);

memset(response, 0, sizeof(response));
bytes_received = recv(sock_fd, response, sizeof(response) - 1, 0);

if (bytes_received > 0) {
    response[bytes_received] = '\0';
    printf("Agent response: %s", response);
}

/* Request LISTPROC after SYSINFO */
const char *listproc_command = "LISTPROC\n";
send(sock_fd, listproc_command, strlen(listproc_command), 0);

memset(response, 0, sizeof(response));
bytes_received = recv(sock_fd, response, sizeof(response) - 1, 0);

if (bytes_received > 0) {
    response[bytes_received] = '\0';
    printf("Agent response: %s", response);
}
/* Request EXEC DATE */
const char *exec_command = "EXEC DATE\n";
send(sock_fd, exec_command, strlen(exec_command), 0);

memset(response, 0, sizeof(response));
bytes_received = recv(sock_fd, response, sizeof(response) - 1, 0);

if (bytes_received > 0) {
    response[bytes_received] = '\0';
    printf("Agent response: %s", response);
}


    /* PUT command - upload test file to Agent */
    const char *put_filename = "test_upload.txt";
    const char *put_data = "RemoteOps file upload test - IT24103007\n";
    long put_size = strlen(put_data);

    char put_command[512];

    snprintf(put_command,
             sizeof(put_command),
             "PUT %s %ld\n",
             put_filename,
             put_size);

    send(sock_fd,
         put_command,
         strlen(put_command),
         0);

    send(sock_fd,
         put_data,
         put_size,
         0);

    memset(response, 0, sizeof(response));

    bytes_received = recv(sock_fd,
                          response,
                          sizeof(response) - 1,
                          0);

    if (bytes_received > 0) {
        response[bytes_received] = '\0';
        printf("Agent response: %s", response);
    }
/* GET command - download test file from Agent */
const char *get_filename = "test_upload.txt";
char get_command[512];

snprintf(get_command,
         sizeof(get_command),
         "GET %s\n",
         get_filename);

send(sock_fd,
     get_command,
     strlen(get_command),
     0);

/* Receive GET header one byte at a time until newline.
   This keeps the file bytes separate from the header. */
char get_header[512];
size_t header_pos = 0;

while (header_pos < sizeof(get_header) - 1) {
    char ch;
    ssize_t n = recv(sock_fd, &ch, 1, 0);

    if (n <= 0) {
        break;
    }

    get_header[header_pos++] = ch;

    if (ch == '\n') {
        break;
    }
}

get_header[header_pos] = '\0';

printf("Agent response: %s", get_header);

char received_filename[256];
long get_filesize;
char received_sid[64];

if (sscanf(get_header,
           "OK FILE_SEND %255s %ld SID:%63s",
           received_filename,
           &get_filesize,
           received_sid) == 3) {

    FILE *download_file = fopen("downloaded_test_upload.txt", "wb");

    if (download_file != NULL) {
        long total_received = 0;
        char file_buffer[1024];

        while (total_received < get_filesize) {
            long remaining = get_filesize - total_received;

            size_t to_receive =
                remaining < (long)sizeof(file_buffer)
                    ? (size_t)remaining
                    : sizeof(file_buffer);

            ssize_t n = recv(sock_fd,
                             file_buffer,
                             to_receive,
                             0);

            if (n <= 0) {
                break;
            }

            fwrite(file_buffer, 1, (size_t)n, download_file);
            total_received += n;
        }

        fclose(download_file);

        if (total_received == get_filesize) {
            printf("GET download completed: %s (%ld bytes)\n",
                   received_filename,
                   get_filesize);
        }
    }
}
 close(sock_fd);

    return EXIT_SUCCESS;
}
