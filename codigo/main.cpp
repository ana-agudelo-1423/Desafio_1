#include <iostream>
#include <fstream>
using namespace std;

void leerCharPorChar(const char* nomArchivo) {  // Corregido: nounArchivo -> nomArchivo
    ifstream file(nomArchivo);
    if (!file.is_open()) {
        cerr << "Error abriendo archivo: " << nomArchivo << endl;
        return;
    }

    cout << "Leyendo char por char:" << endl;
    char c;
    while (file.get(c)) {
        cout << c;
    }
    file.close();
}

int main() {
    const char* nomArchivo = "./debug/Encriptado1.txt";
    leerCharPorChar(nomArchivo);
    return 0;
}
