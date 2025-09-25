#include <iostream>
#include <fstream>
#include <cctype>
using namespace std;

// Función para leer archivo
unsigned char* leerCharPorChar(const char* nomArchivo, int* longitud) {
    ifstream file(nomArchivo, ios::binary);
    if (!file.is_open()) {
        cerr << "Error abriendo archivo: " << nomArchivo << endl;
        *longitud = 0;
        return NULL;
    }

    int capacidad = 1024;
    int usado = 0;
    unsigned char* datos = new unsigned char[capacidad];

    char c;
    while (file.get(c)) {
        if (usado >= capacidad) {
            capacidad *= 2;
            unsigned char* nuevo = new unsigned char[capacidad];
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

// Operaciones de bits
unsigned char rotarDerecha(unsigned char b, int n) {
    return (unsigned char)((b >> n) | (b << (8 - n)));
}

unsigned char rotarIzquierda(unsigned char b, int n) {
    return (unsigned char)((b << n) | (b >> (8 - n)));
}

unsigned char aplicarXor(unsigned char b, unsigned char clave) {
    return (unsigned char)(b ^ clave);
}

void desencriptar(const unsigned char* entrada, int tam, int n, unsigned char clave, unsigned char* salida) {
    for (int i = 0; i < tam; i++) {
        unsigned char temp = aplicarXor(entrada[i], clave);
        salida[i] = rotarDerecha(temp, n);
    }
}

void encriptar(const unsigned char* entrada, int tam, int n, unsigned char clave, unsigned char* salida) {
    for (int i = 0; i < tam; i++) {
        unsigned char temp = rotarIzquierda(entrada[i], n);
        salida[i] = aplicarXor(temp, clave);
    }
}

// ========== ALGORITMO RLE ==========
char* descomprimirRLE(const unsigned char* datos, int tamDatos) {
    int pos = 0;
    int capacidad = 1024;
    int usado = 0;
    char* salida = new char[capacidad];

    while (pos < tamDatos) {
        if (!isdigit(datos[pos])) {
            delete[] salida;
            return NULL;
        }

        int numero = 0;
        while (pos < tamDatos && isdigit(datos[pos])) {
            numero = numero * 10 + (datos[pos] - '0');
            pos++;
        }

        if (pos >= tamDatos) {
            delete[] salida;
            return NULL;
        }

        char simbolo = (char)datos[pos];
        pos++;

        if (usado + numero + 1 > capacidad) {
            capacidad = usado + numero + 1024;
            char* nuevo = new char[capacidad];
            for (int i = 0; i < usado; i++) {
                nuevo[i] = salida[i];
            }
            delete[] salida;
            salida = nuevo;
        }

        for (int i = 0; i < numero; i++) {
            salida[usado++] = simbolo;
        }
    }

    salida[usado] = '\0';
    return salida;
}

// Función para comprimir con RLE (útil para pruebas)
unsigned char* comprimirRLE(const char* texto, int* tamResultado) {
    int capacidad = 1024;
    int usado = 0;
    unsigned char* resultado = new unsigned char[capacidad];

    int i = 0;
    while (texto[i] != '\0') {
        char actual = texto[i];
        int contador = 1;

        while (texto[i + contador] == actual && texto[i + contador] != '\0') {
            contador++;
        }

        // Convertir número a cadena
        char numStr[20];
        int numLen = 0;
        int temp = contador;
        while (temp > 0) {
            numStr[numLen++] = '0' + (temp % 10);
            temp /= 10;
        }

        // Verificar capacidad
        if (usado + numLen + 2 > capacidad) {
            capacidad *= 2;
            unsigned char* nuevo = new unsigned char[capacidad];
            for (int j = 0; j < usado; j++) {
                nuevo[j] = resultado[j];
            }
            delete[] resultado;
            resultado = nuevo;
        }

        // Escribir número (en orden inverso)
        for (int j = numLen - 1; j >= 0; j--) {
            resultado[usado++] = numStr[j];
        }

        // Escribir carácter
        resultado[usado++] = actual;

        i += contador;
    }

    resultado[usado] = '\0';
    *tamResultado = usado;
    return resultado;
}

// ========== ALGORITMO LZ78 ==========
struct ParLZ78 {
    unsigned int indice;
    unsigned char caracter;
};

int mi_strlen(const char* str) {
    int length = 0;
    while (str[length] != '\0') {
        length++;
    }
    return length;
}

// Descomprimir LZ78
char* descomprimirLZ78(const unsigned char* datos, int tamDatos, int* tamSalida) {
    if (tamDatos % 3 != 0) return NULL;

    int numPares = tamDatos / 3;
    char** diccionario = new char*[65536];
    int tamDiccionario = 0;

    int capacidad = 1024;
    int usado = 0;
    char* salida = new char[capacidad];

    for (int i = 0; i < numPares; i++) {
        // Leer par (2 bytes índice + 1 byte carácter)
        unsigned int indice = (datos[i * 3] << 8) | datos[i * 3 + 1];
        unsigned char caracter = datos[i * 3 + 2];

        if (indice == 0) {
            // Caso simple
            if (usado + 2 > capacidad) {
                capacidad *= 2;
                char* nuevo = new char[capacidad];
                for (int j = 0; j < usado; j++) {
                    nuevo[j] = salida[j];
                }
                delete[] salida;
                salida = nuevo;
            }
            salida[usado++] = (char)caracter;

            // Agregar al diccionario
            diccionario[tamDiccionario] = new char[2];
            diccionario[tamDiccionario][0] = (char)caracter;
            diccionario[tamDiccionario][1] = '\0';
            tamDiccionario++;
        } else {
            if (indice - 1 < tamDiccionario) {
                char* prefijo = diccionario[indice - 1];
                int lenPrefijo = mi_strlen(prefijo);

                if (usado + lenPrefijo + 2 > capacidad) {
                    capacidad = usado + lenPrefijo + 1024;
                    char* nuevo = new char[capacidad];
                    for (int j = 0; j < usado; j++) {
                        nuevo[j] = salida[j];
                    }
                    delete[] salida;
                    salida = nuevo;
                }

                // Copiar prefijo
                for (int j = 0; j < lenPrefijo; j++) {
                    salida[usado++] = prefijo[j];
                }

                // Copiar carácter
                salida[usado++] = (char)caracter;

                // Agregar nueva entrada al diccionario
                diccionario[tamDiccionario] = new char[lenPrefijo + 2];
                for (int j = 0; j < lenPrefijo; j++) {
                    diccionario[tamDiccionario][j] = prefijo[j];
                }
                diccionario[tamDiccionario][lenPrefijo] = (char)caracter;
                diccionario[tamDiccionario][lenPrefijo + 1] = '\0';
                tamDiccionario++;
            }
        }
    }

    salida[usado] = '\0';
    *tamSalida = usado;

    // Liberar diccionario
    for (int i = 0; i < tamDiccionario; i++) {
        delete[] diccionario[i];
    }
    delete[] diccionario;

    return salida;
}

// ========== DETECCIÓN AUTOMÁTICA ==========
bool esPosibleRLE(const unsigned char* datos, int tam) {
    int i = 0;
    while (i < tam) {
        // Debe empezar con dígito
        if (!isdigit(datos[i])) return false;

        // Leer número
        while (i < tam && isdigit(datos[i])) i++;

        // Debe seguir un carácter
        if (i >= tam) return false;

        // El carácter debe ser letra o dígito (según especificación)
        char c = (char)datos[i];
        if (!((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9'))) {
            return false;
        }
        i++;
    }
    return true;
}

bool esPosibleLZ78(const unsigned char* datos, int tam) {
    // LZ78 debe tener tamaño múltiplo de 3 y estructura específica
    if (tam % 3 != 0) return false;

    // Verificar que los índices sean razonables
    for (int i = 0; i < tam; i += 3) {
        unsigned int indice = (datos[i] << 8) | datos[i + 1];
        // El índice no debería ser demasiado grande en datos reales
        if (indice > 1000 && i < 10) return false; // Heurística simple
    }
    return true;
}

// Función para probar diferentes combinaciones de n y K
bool probarDescompresion(const unsigned char* encriptado, int tamEncriptado,
                         const char* fragmentoConocido, int n, unsigned char K,
                         char** resultado, int* metodo) {

    // Desencriptar
    unsigned char* desencriptado = new unsigned char[tamEncriptado];
    desencriptar(encriptado, tamEncriptado, n, K, desencriptado);

    // Probar RLE primero
    if (esPosibleRLE(desencriptado, tamEncriptado)) {
        char* rleResultado = descomprimirRLE(desencriptado, tamEncriptado);
        if (rleResultado) {
            // Verificar si contiene el fragmento conocido
            bool encontrado = false;
            int lenFragmento = mi_strlen(fragmentoConocido);
            int lenResultado = mi_strlen(rleResultado);

            for (int i = 0; i <= lenResultado - lenFragmento; i++) {
                bool coincide = true;
                for (int j = 0; j < lenFragmento; j++) {
                    if (rleResultado[i + j] != fragmentoConocido[j]) {
                        coincide = false;
                        break;
                    }
                }
                if (coincide) {
                    encontrado = true;
                    break;
                }
            }

            if (encontrado) {
                *resultado = rleResultado;
                *metodo = 1; // RLE
                delete[] desencriptado;
                return true;
            }
            delete[] rleResultado;
        }
    }

    // Probar LZ78
    if (esPosibleLZ78(desencriptado, tamEncriptado)) {
        int tamLZ78;
        char* lz78Resultado = descomprimirLZ78(desencriptado, tamEncriptado, &tamLZ78);
        if (lz78Resultado) {
            // Verificar si contiene el fragmento conocido
            bool encontrado = false;
            int lenFragmento = mi_strlen(fragmentoConocido);

            for (int i = 0; i <= tamLZ78 - lenFragmento; i++) {
                bool coincide = true;
                for (int j = 0; j < lenFragmento; j++) {
                    if (lz78Resultado[i + j] != fragmentoConocido[j]) {
                        coincide = false;
                        break;
                    }
                }
                if (coincide) {
                    encontrado = true;
                    break;
                }
            }

            if (encontrado) {
                *resultado = lz78Resultado;
                *metodo = 2; // LZ78
                delete[] desencriptado;
                return true;
            }
            delete[] lz78Resultado;
        }
    }

    delete[] desencriptado;
    return false;
}


int main() {
    //const char* nomArchivo = "C:/Users/USER/Desktop/Desafio_1/datasetDesarrollo/Encriptado1.txt";
    const char* nomArchivo = "./debug/Encriptado3.txt";
    const char* fragmentoConocido = "EJEMPLO"; // Reemplaza con el fragmento real

    int tam = 0;
    unsigned char* datosEncriptados = leerCharPorChar(nomArchivo, &tam);
    if (!datosEncriptados) return 1;

    cout << "Archivo leido con " << tam << " bytes." << endl;
    cout << "Primeros 50 bytes:" << endl;
    for (int i = 0; i < 50 && i < tam; i++) {
        if (isprint(datosEncriptados[i])) {
            cout << datosEncriptados[i];
        } else {
            printf("\\x%02X", datosEncriptados[i]);
        }
    }
    cout << endl << endl;

    // Buscar parámetros correctos
    int nEncontrado = -1;
    unsigned char KEncontrado = 0;
    char* textoOriginal = NULL;
    int metodoUsado = 0;

    cout << "Buscando parametros correctos..." << endl;

    for (int n = 1; n < 8; n++) {
        for (unsigned char K = 0; K < 255; K++) {
            char* resultado;
            int metodo;

            if (probarDescompresion(datosEncriptados, tam, fragmentoConocido, n, K, &resultado, &metodo)) {
                nEncontrado = n;
                KEncontrado = K;
                textoOriginal = resultado;
                metodoUsado = metodo;
                break;
            }
        }
        if (nEncontrado != -1) break;
    }

    if (nEncontrado != -1) {
        cout << "✓ PARAMETROS ENCONTRADOS:" << endl;
        cout << "  Rotación (n): " << nEncontrado << endl;
        cout << "  Clave XOR (K): 0x" << hex << (int)KEncontrado << dec << endl;
        cout << "  Método: " << (metodoUsado == 1 ? "RLE" : "LZ78") << endl;
        cout << endl << "TEXTO ORIGINAL:" << endl;
        cout << textoOriginal << endl;

        delete[] textoOriginal;
    } else {
        cout << " No se encontraron parametros validos." << endl;
    }

    delete[] datosEncriptados;
    return 0;
}
