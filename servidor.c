#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <ctype.h>
#include <arpa/inet.h>

#define PORT 8080

// Funciones utilizadas. 
char *generarUsuario(int longitudUsuario);
char *generarContrasenia(int longitudContrasenia);
int validarLongitudUsuario(int longitudUsuario);
int validarLongitudContrasenia(int longitudContrasenia);
int esVocal(char letra);

int main(void)
{
    srand(time(NULL)); // Semilla para empezar a usar el random.
    int server_fd, new_socket;  // Descriptores de archivos para el socket
    struct sockaddr_in address; // Almacenará la dirección del socket
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};

    // Buffer estático para las respuestas
    char response[1024];

    // Crear el descriptor socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("falló la creación del socket");
        exit(EXIT_FAILURE);
    }

    // Adjuntar el socket al puerto 8080
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
    {
        perror("falló setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Vincular el socket al puerto
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("fallo bind");
        exit(EXIT_FAILURE);
    }

    // Escuchar conexiones
    if (listen(server_fd, 3) < 0)
    {
        perror("falló listen");
        exit(EXIT_FAILURE);
    }

    while (1)
    {
        // Aceptar una conexión
        new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
        if (new_socket < 0)
        {
            perror("falló accept");
            exit(EXIT_FAILURE);
        }

        while (1)
        {
            // Limpiar buffer antes de leer
            memset(buffer, 0, sizeof(buffer));

            // Leer mensaje del cliente
            int valread = read(new_socket, buffer, 1024);

            if (valread == 0)
            {
                // Cliente cerró la conexión
                printf("Cliente desconectado.\n");
                break; // Salir del bucle interno para aceptar nuevas conexiones
            }
            else if (valread < 0)
            {
                perror("Error al leer del cliente");
                break; // Salir del bucle interno para aceptar nuevas conexiones
            }

            printf("Mensaje recibido: %s\n", buffer);

            int longitud = 0;
            int option = 0;
            char *dynamicResponse = NULL; // Inicializamos el puntero dinámico a NULL

            // Validamos la opción elegida. 
            if (sscanf(buffer, "%d", &option) != 1 || option < 1 || option > 3)
            {
                snprintf(response, sizeof(response), "Entrada inválida. Por favor, ingresa un número entre 1 y 3.\n");
                send(new_socket, response, strlen(response), 0);
                continue;
            }

            if (option == 1)
            {
                // El cliente quiere generar un usuario
                longitud = atoi(buffer + 2);
                if (validarLongitudUsuario(longitud))
                {
                    dynamicResponse = generarUsuario(longitud);
                }
                else
                {
                    snprintf(response, sizeof(response), "Longitud de usuario inválida.\n");
                    dynamicResponse = response; // Apuntamos a la respuesta estática
                }
            }
            else if (option == 2)
            {
                // El cliente pidió generar una contraseña
                longitud = atoi(buffer + 2);
                if (validarLongitudContrasenia(longitud))
                {
                    dynamicResponse = generarContrasenia(longitud);
                }
                else
                {
                    snprintf(response, sizeof(response), "Longitud de contrasenia inválida.\n");
                    dynamicResponse = response; // Apuntamos a la respuesta estática
                }
            }
            else if (option == 3)
            {
                // El cliente pidió desconectar
                snprintf(response, sizeof(response), "Desconectando...\n");
                dynamicResponse = response; // Apuntamos a la respuesta estática
                send(new_socket, dynamicResponse, strlen(dynamicResponse), 0);
                break; // Salir del bucle interno para aceptar nuevas conexiones
            }
            else
            {
                snprintf(response, sizeof(response), "Opción inválida.\n");
                dynamicResponse = response; // Apuntamos a la respuesta estática
            }

            // Enviar respuesta al cliente
            send(new_socket, dynamicResponse, strlen(dynamicResponse), 0);
            printf("Respuesta enviada: %s\n", dynamicResponse);

            // Liberar memoria si fue asignada
            if (dynamicResponse != response)
            {
                free(dynamicResponse);
            }
        }
        close(new_socket); // Cerrar el socket del cliente después de que el cliente se desconecta
    }
    close(server_fd);

    return 0;
}

char *generarUsuario(int longitudUsuario)
{
    char *usuario = (char *)malloc((longitudUsuario + 1) * sizeof(char)); //
    // Genera la primera letra aleatoriamente
    usuario[0] = 'a' + rand() % 26;

    for (int i = 1; i < longitudUsuario; i++)
    {
        if (esVocal(usuario[i - 1]) == 1)
        {
            // Si la letra anterior es vocal, la siguiente debe ser consonante
            char consonantes[] = "bcdfghjklmnpqrstvwxyz";
            usuario[i] = consonantes[rand() % strlen(consonantes)];
        }
        else
        {
            // Si la letra anterior es consonante, la siguiente debe ser vocal
            char vocales[] = "aeiou";
            usuario[i] = vocales[rand() % strlen(vocales)]; // genera vocal minuscula
        }
    }
    usuario[longitudUsuario] = '\0';

    return usuario;
}

int esVocal(char letra)
{
    // Convertir la letra a minúscula para simplificar la comparación
    letra = tolower(letra);

    // Comparar con las vocales
    if (letra == 'a' || letra == 'e' || letra == 'i' || letra == 'o' || letra == 'u')
    {
        return 1; // Es vocal
    }
    return 0; // No es vocal
}

char *generarContrasenia(int longitudContrasenia)
{
    char *contrasenia = (char *)malloc((longitudContrasenia + 1) * sizeof(char));
    for (int i = 0; i < longitudContrasenia; i++)
    {
        int tipo = (i % 3) + 1; // Alterna entre 1, 2 y 3

        switch (tipo)
        {
        case 1:
            contrasenia[i] = 'A' + rand() % 26; // Mayúsculas
            break;
        case 2:
            contrasenia[i] = 'a' + rand() % 26; // Minúsculas
            break;
        case 3:
            contrasenia[i] = '0' + rand() % 10; // Números del 0 al 9
            break;
        }
    }
    contrasenia[longitudContrasenia] = '\0';
    return contrasenia;
}

int validarLongitudUsuario(int longitudUsuario)
{
    int flag = 0;
    if (longitudUsuario > 5 && longitudUsuario < 15)
    {
        flag = 1; // es valida
    }

    return flag; // no es valida
}

int validarLongitudContrasenia(int longitudContrasenia)
{
    int flag = 0;
    if (longitudContrasenia >= 8 && longitudContrasenia < 50)
    {
        flag = 1; // es valida
    }

    return flag; // no es valida
}
