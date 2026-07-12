#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <mysql/mysql.h>
#include <pthread.h>
#include <time.h>

MYSQL *conn;
int points = 0;
char *username = "c_user1";
int port = 12001;

void* update_points_thread(void* arg) {
    char query[256];

    while(1) {
        points += 10;
        sprintf(query, "UPDATE scoreboard SET points=%d, datetime_stamp=NOW() WHERE user='%s'", points, username);

        if (mysql_query(conn, query)) {
            fprintf(stderr, "UPDATE failed: %s\n", mysql_error(conn));
        } else {
            printf("%s points updated to %d\n", username, points);
        }

        sleep(30);
    }
    return NULL;
}

int main()
{
    int server_fd;
    int client_socket;
    struct sockaddr_in server_addr;
    MYSQL_RES *result;
    MYSQL_ROW row;
    char query[256];
    char message[50];
    pthread_t tid;

    conn = mysql_init(NULL);
    if (conn == NULL) { fprintf(stderr, "mysql_init failed\n"); return 1; }

    // Wait for MySQL to be ready
    int attempts = 0;
    while (attempts < 60) {
        if (mysql_real_connect(conn, "gp_database", "root", "root", "GP_Database", 0, NULL, 0)) {
            printf("Connected to MySQL\n");
            break;
        }
        printf("Attempt %d: Waiting for MySQL...\n", attempts + 1);
        sleep(1);
        attempts++;
    }
    if (attempts == 60) { fprintf(stderr, "Failed to connect to MySQL\n"); return 1; }

    // Load existing points from DB
    sprintf(query, "SELECT points FROM scoreboard WHERE user='%s'", username);
    if (mysql_query(conn, query) == 0) {
        result = mysql_store_result(conn);
        if ((row = mysql_fetch_row(result)) != NULL) {
            points = atoi(row[0]);
            printf("Resuming from points: %d\n", points);
        }
        mysql_free_result(result);
    }

    pthread_create(&tid, NULL, update_points_thread, NULL);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    listen(server_fd, 5);

    printf("C Server 1 Running\n");

    while(1) {
        char request[50] = {0};
        client_socket = accept(server_fd, NULL, NULL);

        recv(client_socket, request, sizeof(request), 0);
        request[strcspn(request, "\r\n")] = '\0';

        if (strcmp(request, "reset") == 0) {
            points = 0;
            sprintf(query, "UPDATE scoreboard SET points=%d, datetime_stamp=NOW() WHERE user='%s'", points, username);
            if (mysql_query(conn, query)) {
                sprintf(message, "RESET FAILED");
            } else {
                sprintf(message, "%s score reset to 0", username);
            }
        } else {
            sprintf(query, "SELECT points FROM scoreboard WHERE user='%s'", username);
            if (mysql_query(conn, query)) {
                sprintf(message, "0");
            } else {
                result = mysql_store_result(conn);
                if ((row = mysql_fetch_row(result)) != NULL) {
                    sprintf(message, "%s score: %s", username, row[0]);
                }
                mysql_free_result(result);
            }
        }

        send(client_socket, message, strlen(message), 0);
        close(client_socket);
    }

    mysql_close(conn);
    return 0;
}
