#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/sysinfo.h>
#include <pthread.h>

#define AGENT_PORT 9410
#define BACKLOG 10
#define AUTH_TOKEN "OPS-3007"
#define SID "7003"
void *handle_client(void *arg);

int main(void)
{
int server_fd;
struct sockaddr_in server_addr;
    /* Create TCP socket */
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket");
        return EXIT_FAILURE;
    }

    /* Allow port reuse */
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

    /* Bind to personalised TCP port */
    if (bind(server_fd,
             (struct sockaddr *)&server_addr,
             sizeof(server_addr)) < 0) {
        perror("bind");
        close(server_fd);
        return EXIT_FAILURE;
    }

    /* Listen for Controller connection */
    if (listen(server_fd, BACKLOG) < 0) {
        perror("listen");
        close(server_fd);
        return EXIT_FAILURE;
    }

    printf("RemoteOps Agent started.\n");
    printf("Listening on TCP port %d...\n", AGENT_PORT);
while (1) {
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);

    int *client_fd = malloc(sizeof(int));

    if (client_fd == NULL) {
        perror("malloc");
        continue;
    }

    *client_fd = accept(server_fd,
                        (struct sockaddr *)&client_addr,
                        &client_len);

    if (*client_fd < 0) {
        perror("accept");
        free(client_fd);
        continue;
    }

    printf("Controller connected from %s:%d\n",
           inet_ntoa(client_addr.sin_addr),
           ntohs(client_addr.sin_port));

    pthread_t thread;

    if (pthread_create(&thread,
                       NULL,
                       handle_client,
                       client_fd) != 0) {
        perror("pthread_create");
        close(*client_fd);
        free(client_fd);
        continue;
    }

    pthread_detach(thread);
}
close(server_fd);
return EXIT_SUCCESS;
}

void *handle_client(void *arg)
{
    int client_fd = *(int *)arg;
    free(arg);

    char buffer[1024];
    ssize_t bytes_received;

    /* Receive AUTH command */
    memset(buffer, 0, sizeof(buffer));
    bytes_received = recv(client_fd,
                          buffer,
                          sizeof(buffer) - 1,
                          0);

    if (bytes_received <= 0) {
        close(client_fd);
        return NULL;
    }

    buffer[bytes_received] = '\0';
    buffer[strcspn(buffer, "\r\n")] = '\0';

    /* Validate authentication */
    if (strcmp(buffer, "AUTH " AUTH_TOKEN) != 0) {
        char response[128];

        snprintf(response,
                 sizeof(response),
                 "ERR 001 AUTH_FAILED SID:%s\n",
                 SID);

        send(client_fd,
             response,
             strlen(response),
             0);

        printf("Controller authentication failed.\n");

        close(client_fd);
        return NULL;
    }

    /* Authentication successful */
    {
        char response[128];

        snprintf(response,
                 sizeof(response),
                 "OK AUTHENTICATED SID:%s\n",
                 SID);

        send(client_fd,
             response,
             strlen(response),
             0);
    }

    printf("Controller authenticated successfully.\n");

    /* Process commands until Controller disconnects */
    while (1) {

        memset(buffer, 0, sizeof(buffer));

        bytes_received = recv(client_fd,
                              buffer,
                              sizeof(buffer) - 1,
                              0);

        if (bytes_received <= 0) {
            break;
        }

        buffer[bytes_received] = '\0';

/* Keep track of where the command line ends.
   This is needed for PUT because TCP may deliver
   the PUT header and some file bytes together. */
char *newline = strchr(buffer, '\n');
size_t command_length = bytes_received;
size_t extra_bytes = 0;

if (newline != NULL) {
    command_length = (size_t)(newline - buffer);

    if (command_length > 0 &&
        buffer[command_length - 1] == '\r') {
        buffer[command_length - 1] = '\0';
    } else {
        buffer[command_length] = '\0';
    }

    extra_bytes =
        bytes_received - ((size_t)(newline - buffer) + 1);
}
        

        /* SYSINFO command */
        if (strcmp(buffer, "SYSINFO") == 0) {

            char response[256];
            struct sysinfo info;

            double cpu_load = 0.0;
            long mem_used_mb = 0;
            long uptime_sec = 0;

            if (sysinfo(&info) == 0) {

                cpu_load =
                    (double)info.loads[0] /
                    (1 << SI_LOAD_SHIFT);

                unsigned long long total_ram =
                    (unsigned long long)info.totalram *
                    info.mem_unit;

                unsigned long long free_ram =
                    (unsigned long long)info.freeram *
                    info.mem_unit;

                mem_used_mb =
                    (long)((total_ram - free_ram) /
                    (1024 * 1024));

                uptime_sec = info.uptime;
            }

            snprintf(response,
                     sizeof(response),
                     "OK SYSINFO %.2f %ld %ld SID:%s\n",
                     cpu_load,
                     mem_used_mb,
                     uptime_sec,
                     SID);

            send(client_fd,
                 response,
                 strlen(response),
                 0);

            printf("SYSINFO request processed.\n");
        }

        /* LISTPROC command */
        else if (strcmp(buffer, "LISTPROC") == 0) {

            char proc_response[1024];
            FILE *fp;

            strcpy(proc_response, "OK PROCS ");

            fp = popen("ps -e -o pid=,comm= | head -10", "r");

            if (fp != NULL) {

                char line[128];

                while (fgets(line,
                             sizeof(line),
                             fp) != NULL) {

                    line[strcspn(line, "\n")] = '\0';

                    if (strlen(proc_response) +
                        strlen(line) + 3 <
                        sizeof(proc_response)) {

                        strcat(proc_response, line);
                        strcat(proc_response, ",");
                    }
                }

                pclose(fp);
            }

            strcat(proc_response,
                   " SID:" SID "\n");

            send(client_fd,
                 proc_response,
                 strlen(proc_response),
                 0);

            printf("LISTPROC request processed.\n");
        }

/* EXEC command with whitelist */
else if (strncmp(buffer, "EXEC ", 5) == 0) {

    char *exec_cmd = buffer + 5;
    const char *shell_cmd = NULL;

    if (strcmp(exec_cmd, "DATE") == 0) {
        shell_cmd = "date";
    }
    else if (strcmp(exec_cmd, "UPTIME") == 0) {
        shell_cmd = "uptime";
    }
    else if (strcmp(exec_cmd, "DISKFREE") == 0) {
        shell_cmd = "df -h /";
    }
    else if (strcmp(exec_cmd, "HOSTNAME") == 0) {
        shell_cmd = "hostname";
    }
    else if (strcmp(exec_cmd, "WHOAMI") == 0) {
        shell_cmd = "whoami";
    }

    if (shell_cmd == NULL) {

        char response[128];

        snprintf(response,
                 sizeof(response),
                 "ERR 002 COMMAND_NOT_ALLOWED SID:%s\n",
                 SID);

        send(client_fd,
             response,
             strlen(response),
             0);

        printf("EXEC command rejected: %s\n",
               exec_cmd);
    }
    else {

        char exec_response[1024];
        char output[128];
        FILE *fp;

        strcpy(exec_response, "OK EXEC_RESULT ");

        fp = popen(shell_cmd, "r");

        if (fp != NULL) {

            while (fgets(output,
                         sizeof(output),
                         fp) != NULL) {

                output[strcspn(output, "\r\n")] = ' ';

                if (strlen(exec_response) +
                    strlen(output) + 20 <
                    sizeof(exec_response)) {

                    strcat(exec_response, output);
                }
            }

            pclose(fp);
        }

        strcat(exec_response,
               " SID:" SID "\n");

        send(client_fd,
             exec_response,
            strlen(exec_response),
             0);

        printf("EXEC command processed: %s\n",
               exec_cmd);
    }
}

        /* Unknown command */
        /* PUT command - upload file from Controller to Agent */
        else if (strncmp(buffer, "PUT ", 4) == 0) {
            char filename[256];
            long filesize;

            if (sscanf(buffer + 4, "%255s %ld", filename, &filesize) == 2) {
/* Reject unsafe filenames */
if (strstr(filename, "..") != NULL ||
    strchr(filename, '/') != NULL ||
    strchr(filename, '\\') != NULL) {

    char response[128];

    snprintf(response,
             sizeof(response),
             "ERR 006 INVALID_FILENAME SID:%s\n",
             SID);

    send(client_fd,
         response,
         strlen(response),
         0);

    continue;
}

/* Maximum upload size: 5 MB */
if (filesize < 0 || filesize > 5 * 1024 * 1024) {
    char response[128];

    snprintf(response,
             sizeof(response),
             "ERR 004 FILE_TOO_LARGE SID:%s\n",
             SID);

    send(client_fd,
         response,
         strlen(response),
         0);

    continue;
}
                char filepath[512];

                snprintf(filepath,
                         sizeof(filepath),
                         "agentfiles/IT24103007/%s",
                         filename);

                FILE *file = fopen(filepath, "wb");

                if (file != NULL) {
                    long total_received = 0;
                    char file_buffer[1024];
                /* Save any file bytes that arrived together with the PUT header */
if (extra_bytes > 0) {
    size_t header_end = (size_t)(newline - buffer) + 1;

    size_t initial_bytes = extra_bytes;

    if ((long)initial_bytes > filesize) {
        initial_bytes = (size_t)filesize;
    }

    fwrite(buffer + header_end,
           1,
           initial_bytes,
           file);

    total_received += (long)initial_bytes;
} 


                    while (total_received < filesize) {
                        long remaining = filesize - total_received;
                        size_t to_receive =
                            remaining < (long)sizeof(file_buffer)
                                ? (size_t)remaining
                                : sizeof(file_buffer);

                        ssize_t n = recv(client_fd,
                                         file_buffer,
                                         to_receive,
                                         0);

                        if (n <= 0) {
                            break;
                        }

                        fwrite(file_buffer, 1, (size_t)n, file);
                        total_received += n;
                    }

                    fclose(file);

                    if (total_received == filesize) {
                        char response[512];

                        snprintf(response,
                                 sizeof(response),
                                 "OK FILE_RECEIVED %s SID:%s\n",
                                 filename,
                                 SID);

                        send(client_fd,
                             response,
                             strlen(response),
                             0);

                        printf("PUT completed: %s (%ld bytes)\n",
                               filename,
                               filesize);
                    }
                }
            }
        }
else if (strncmp(buffer, "GET ", 4) == 0) {
    char filename[256];
    char filepath[512];

    if (sscanf(buffer + 4, "%255s", filename) == 1) {
/* Reject unsafe filenames */
if (strstr(filename, "..") != NULL ||
    strchr(filename, '/') != NULL ||
    strchr(filename, '\\') != NULL) {

    char response[128];

    snprintf(response,
             sizeof(response),
             "ERR 006 INVALID_FILENAME SID:%s\n",
             SID);

    send(client_fd,
         response,
         strlen(response),
         0);

    continue;
}

        snprintf(filepath,
                 sizeof(filepath),
                 "agentfiles/IT24103007/%s",
                 filename);

        FILE *file = fopen(filepath, "rb");

        if (file == NULL) {
            char response[256];

            snprintf(response,
                     sizeof(response),
                     "ERR 005 FILE_NOT_FOUND SID:%s\n",
                     SID);

            send(client_fd,
                 response,
                 strlen(response),
                 0);
        } else {
            fseek(file, 0, SEEK_END);
            long filesize = ftell(file);
            rewind(file);

            char response[512];

            snprintf(response,
                     sizeof(response),
                     "OK FILE_SEND %s %ld SID:%s\n",
                     filename,
                     filesize,
                     SID);

            send(client_fd,
                 response,
                 strlen(response),
                 0);

            char file_buffer[1024];
            size_t bytes_read;

            while ((bytes_read =
                    fread(file_buffer,
                          1,
                          sizeof(file_buffer),
                          file)) > 0) {

                send(client_fd,
                     file_buffer,
                     bytes_read,
                     0);
            }

            fclose(file);

            printf("GET completed: %s (%ld bytes)\n",
                   filename,
                   filesize);
        }
    }
}       
 else {
            char response[128];

            snprintf(response,
                     sizeof(response),
                     "ERR 006 UNKNOWN_COMMAND SID:%s\n",
                     SID);

            send(client_fd,
                 response,
                 strlen(response),
                 0);

            printf("Unknown command received: %s\n",
                   buffer);
        }
    }

    printf("Controller disconnected.\n");

    close(client_fd);
    

    return EXIT_SUCCESS;
}
