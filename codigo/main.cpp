#include <iostream>
#include <fstream>
using namespace std;

void leerCharPorChar(const char* nomArchivo, int* longitud) {  // Corregido: nounArchivo -> nomArchivo
    ifstream file(nomArchivo, ios::binary);
    if (!file.is_open()) {
        cerr << "Error abriendo archivo: " << nomArchivo << endl;
        *longitud = 0;
        return NULL;
    }

    int capacidad = 1024;
    int usado = 0;
    unsigned char* datos = new unsigned char*[capacidad];

    char c;
    while (file.get(c)) {
        if (usado >= capacidad) {
            // duplicar capacidad
            capacidad *= 2;
            unsigned char* nuevo = new unsigned char[capacidad];
            // copiar lo que ya estaba
            for (int i = 0; i < usado; i++) {
                nuevo[i] = datos[i];
            }
            delete[] datos;
            datos = nuevo;
        }
        datos[usado++] = (unsigned char)c;
    }
    file.close();
    *longitud = usado;
    return datos;
}


//Operaciones de bits

unsigned char rotarDerecha(unsigned char b, int n) {
    return (unsigned char)((b >> n) | (b << (8 - n)));
}


unsigned char aplicarXor(unsigned char b, unsigned char clave) {
    return (unsigned char)(b ^ clave);
}

//Este sirve para desencriptar un arreglo de bytes
void desencriptar(const unsigned char* entrada, int tam, int n, unsigned char clave, unsigned char* salida) {
    for (int i = 0; i < tam; i++) {
        unsigned char temp = aplicarXor(entrada[i], clave);
        salida[i] = rotarDerecha(temp, n);
    }
    salida[tam] = 0;
}

int main() {
    const char* nomArchivo = "./debug/Encriptado1.txt";
    int tam = 0;

    unsigned char* datos = leerCharPorChar(nomArchivo, &tam);
    if (!datos) return 1;

    cout << "Archivo leído con " << tam << " caracteres." << endl;

    //Para mostrar los primeros 50 caracteres del mensaje encriptado solo para verificar si la lectura funciona
    for (int i = 0; i < tam && i < 50; i++) {
        cout << datos[i];
    }
    cout << endl;

    delete[] datos; // liberar memoria

    return 0;
}
