#pragma once
#include <iostream>
#include <vector>
#include <string>
#include "disco.hpp"
#include "raidsystem.hpp"

class RAID1 : public RAIDSystem {
public:
    RAID1() : RAIDSystem(2) {}

    void escribirCadena(const std::string& datos) override {
        for (auto& d : discos) d.bloques.clear();

        for (char c : datos) {
            std::string s(1, c);
            discos[0].bloques.push_back(Bloque{s, false});
            discos[1].bloques.push_back(Bloque{s, false});
        }
        std::cout << "[RAID 1] Datos guardados y duplicados en el espejo.\n";
    }

    std::string leerCadena() override {
        std::string res = "";
        int discoValido = -1;
        if (discos[0].activo) discoValido = 0;
        else if (discos[1].activo) discoValido = 1;

        if (discoValido == -1) {
            std::cout << "[ERROR] Ambos discos fallaron en RAID 1. Datos perdidos.\n";
            return "";
        }

        for (const auto& b : discos[discoValido].bloques) {
            res += b.contenido;
        }
        return res;
    }

    bool reconstruirDisco(int discoId) override {
        if (discoId < 0 || discoId >= 2) return false;
        int otroDisco = (discoId == 0) ? 1 : 0;

        if (!discos[otroDisco].activo) {
            std::cout << "[ERROR] Imposible reconstruir: El disco fuente tambien fallo.\n";
            return false;
        }

        discos[discoId].reemplazarYReconstruir(discos[otroDisco].bloques.size());
        discos[discoId].bloques = discos[otroDisco].bloques;
        std::cout << "[RAID 1] Disco " << discoId << " reconstruido exitosamente desde el espejo (Disco " << otroDisco << ").\n";
        return true;
    }

    void mostrarEstado() const override {
        std::cout << "\n=== ESTADO DEL ARREGLO RAID 1 ===\n";
        for (const auto& d : discos) {
            std::cout << "Disco " << d.id << " [" << (d.activo ? "OK" : "FALLO") << "]: ";
            if (!d.activo) {
                std::cout << "(Sin datos)\n";
                continue;
            }
            for (const auto& b : d.bloques) {
                std::cout << "[" << b.contenido << "] ";
            }
            std::cout << "\n";
        }
    }
};
