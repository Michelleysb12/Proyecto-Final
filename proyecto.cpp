#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <iomanip>
#include <sstream>
#include "bloque.hpp"
#include "disco.hpp"
#include "raidsystem.hpp"
#include "raid0.hpp"
#include "raid1.hpp"
#include "raid5.hpp"
#include "raid10.hpp"

int main() {
    std::unique_ptr<RAIDSystem> raid = nullptr;
    int opcionNivel = 0;

    std::cout << "=========================================\n";
    std::cout << " SIMULADOR DE ARREGLOS RAID (0, 1, 5, 10)\n";
    std::cout << "=========================================\n";
    std::cout << "Seleccione el tipo de RAID a modelar:\n";
    std::cout << "1. RAID 0 (Striping)\n";
    std::cout << "2. RAID 1 (Mirroring)\n";
    std::cout << "3. RAID 5 (Striping + Paridad Distribuida)\n";
    std::cout << "4. RAID 10 (Striping + Mirroring)\n";
    std::cout << "Opcion: ";
    std::cin >> opcionNivel;

    switch (opcionNivel) {
        case 1: {
            int n;
            std::cout << "Ingrese numero de discos (>= 2): ";
            std::cin >> n;
            raid = std::make_unique<RAID0>(n < 2 ? 2 : n);
            break;
        }
        case 2:
            raid = std::make_unique<RAID1>();
            break;
        case 3: {
            int n;
            std::cout << "Ingrese numero de discos (>= 3): ";
            std::cin >> n;
            raid = std::make_unique<RAID5>(n < 3 ? 3 : n);
            break;
        }
        case 4: {
            int n;
            std::cout << "Ingrese numero de discos (Pares, >= 4): ";
            std::cin >> n;
            raid = std::make_unique<RAID10>(n);
            break;
        }
        default:
            std::cout << "Opcion no valida. Saliendo...\n";
            return 1;
    }

    int opcion = 0;
    std::string datos;

    do {
        std::cout << "\n-----------------------------------------\n";
        std::cout << "MENU DE OPERACIONES RAID\n";
        std::cout << "1. Escribir Datos\n";
        std::cout << "2. Leer y Recuperar Datos\n";
        std::cout << "3. Simular Fallo de Disco\n";
        std::cout << "4. Reconstruir Disco\n";
        std::cout << "5. Mostrar Distribucion Actual en Discos\n";
        std::cout << "6. Salir\n";
        std::cout << "Opcion: ";
        std::cin >> opcion;

        switch (opcion) {
            case 1:
                std::cout << "Ingrese la cadena de texto a almacenar: ";
                std::cin.ignore();
                std::getline(std::cin, datos);
                raid->escribirCadena(datos);
                break;
            case 2: {
                std::string recuperado = raid->leerCadena();
                if (!recuperado.empty()) {
                    std::cout << "[LECTURA EXITOSA] Cadena recuperada: \"" << recuperado << "\"\n";
                }
                break;
            }
            case 3: {
                int id;
                std::cout << "Ingrese ID del disco a simular fallo (0 a " << raid->getNumDiscos() - 1 << "): ";
                std::cin >> id;
                raid->simularFalloDisco(id);
                break;
            }
            case 4: {
                int id;
                std::cout << "Ingrese ID del disco a reconstruir (0 a " << raid->getNumDiscos() - 1 << "): ";
                std::cin >> id;
                raid->reconstruirDisco(id);
                break;
            }
            case 5:
                raid->mostrarEstado();
                break;
            case 6:
                std::cout << "Saliendo del simulador...\n";
                break;
            default:
                std::cout << "Opcion invalida.\n";
        }
    } while (opcion != 6);

    return 0;
}