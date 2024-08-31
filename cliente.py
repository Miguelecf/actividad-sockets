import socket

def main():
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.connect(('127.0.0.1', 8080))

    while True:
        option = input("Ingrese una de las opciones\n1. Generar un usuario\n2. Generar una contraseña\n3. Salir\n")
        if option == '3':
            sock.sendall(option.encode())
            print("Saliendo...")
            break

        longitud = input("Ingrese la longitud: \n* Usuario: mayor a 5 y menor a 15\n* Contrasenia: Mayor o igual a 8 y menor a 50\n")
        message = f"{option} {longitud}"
        sock.sendall(message.encode())

        response = sock.recv(1024).decode()
        print(f"Respuesta del servidor: {response}")

    sock.close()

if __name__ == "__main__":
    main()
