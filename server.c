#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

int port;

int socket_desc;
struct sockaddr_in server_addr, client_addr;
char server_message[2], client_message[2];
int client_struct_length = sizeof(client_addr);

int BuildSocket() {
    memset(server_message, '\0', sizeof(server_message));
    memset(client_message, '\0', sizeof(client_message));

    socket_desc = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if(socket_desc < 0){
        printf("Ошибка при создании сокета\n");
        return -1;
    }
    printf("Сокет успешно создан\n");

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if(bind(socket_desc, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0){
        printf("Не удалось привязать сокет к порту\n");
        return -1;
    }

    printf("Сервер начал работу\n\n");
}

// Хэндлер для первого садовника
void HandleFirst(int message) {
    if (message == -1) {
        printf("Первый садовник начал свою работу\n");
        return;
    }
    if (message == -2) {
        printf("Первый садовник захотел полить тот же цветок, что и второй, но всё обошлось!\n");
        return;
    }
    printf("Первый садовник полил цветок под номером %d\n", message);
}

// Хэндлер для второго садовника
void HandleSecond(int message) {
    if (message == -1) {
        printf("Второй садовник начал свою работу\n");
        return;
    }
    if (message == -2) {
        printf("Второй садовник захотел полить тот же цветок, что и первый, но всё обошлось!\n");
        return;
    }
    printf("Второй садовник полил цветок под номером %d\n", message);
}

void HandleClient(int from, int message) {
    if (from == 1) { // Обработка запроса для первого садовника
        HandleFirst(message);
        return;
    }

    if (from == 2) { // Обработка запроса для второго садовника
        HandleSecond(message);
        return;
    }
}

int main(int argc, char *argv[]) {
    // Задаём произвольный порт
    port = 11111;

    if (argc != 2) {
        printf("ERROR: Неверный формат команды запуска");
        exit(EXIT_FAILURE);
    }

    port = atoi(argv[1]);
    if (port < 1 || port > 65536) {
        printf("Порт должен быть в заданном диапазоне [1, 65536]\n");
        exit(EXIT_FAILURE);
    }

    BuildSocket();

    while (1) {
        if (recvfrom(socket_desc, client_message, sizeof(client_message), 0,
                     (struct sockaddr*)&client_addr, &client_struct_length) < 0){
            printf("Не удалось получить сообщение\n");
            return -1;
        }

        int from = (int)client_message[0];
        int message = (int)client_message[1];

        if (from == -1) {
            break;
        }

        HandleClient(from, message);
    }

    printf("Сервер завершает работу...\n");
    sleep(2);

    close(socket_desc);

    return 0;
}