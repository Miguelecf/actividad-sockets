#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080

int main(void)
{

    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};
    int option;
    int longitud;
    char input[100];

    // Crear el socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Error al crear el socket");
        exit(EXIT_FAILURE);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convertir dirección IPv4 de texto a binario
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
    {
        perror("Dirección inválida/No soportada\n");
        exit(EXIT_FAILURE);
    }

    // Conectarse al servidor
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("Error en la conexión");
        exit(EXIT_FAILURE);
    }

    while (1)
    {

        // Menú de opciones
        printf("Ingrese una de las opciones\n");
        printf("1. Generar un usuario\n");
        printf("2. Generar una contraseña\n");
        printf("3. Salir\n");

        // Leer y validar la opción
        if (!fgets(input, sizeof(input), stdin) || sscanf(input, "%d", &option) != 1 || option < 1 || option > 3)
        {
            printf("Opción inválida. Por favor, ingresa un número entre 1 y 3.\n");
            continue;
        }

        if (option == 3)
        {
            printf("Saliendo...\n");
            close(sock);
            return 0;
        }

        // Leer y validar la longitud
        printf("Ingrese la longitud: \n* Usuario: mayor a 5 y menor a 15 \n* Contrasenia: Mayor o igual a 8 y menor a 50 \n");
        if (fgets(input, sizeof(input), stdin) == NULL || sscanf(input, "%d", &longitud) != 1 || longitud <= 0)
        {
            printf("Longitud inválida.\n");
            continue;
        }

        // Enviar la opción y la longitud al servidor
        snprintf(buffer, sizeof(buffer), "%d %d", option, longitud);
        send(sock, buffer, strlen(buffer), 0);

        // Limpieza de buffer y recibir respuesta
        memset(buffer, 0, sizeof(buffer));
        int valread = read(sock, buffer, sizeof(buffer) - 1); // Reservar espacio para '\0'
        if (valread < 0)
        {
            perror("Error al leer del servidor");
            close(sock);
            exit(EXIT_FAILURE);
        }
        buffer[valread] = '\0'; // Asegurarse de que el buffer esté terminado en nulo
        printf("\nRespuesta del servidor: %s\n\n", buffer);
    }

    close(sock);
    return 0;
}
