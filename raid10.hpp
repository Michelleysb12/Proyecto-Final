#pragma once
#include <iostream>
#include <vector>
#include <string>
#include "disco.hpp"
#include "raidsystem.hpp"

class RAID10 : public RAIDSystem {
public:
    RAID10(int nDiscos) : RAIDSystem((nDiscos < 4 || nDiscos % 2 != 0) ? 4 : nDiscos) {}

    void escribirCadena(const std::string& datos) override {
        for (auto& d : discos) d.bloques.clear();

        int numParejas = numDiscos / 2;

        for (size_t i = 0; i < datos.length(); ++i) {
            int parejaTarget = i % numParejas;
            int disco1 = parejaTarget * 2;
            int disco2 = disco1 + 1;

            std::string s(1, datos[i]);
            discos[disco1].bloques.push_back(Bloque{s, false});
            discos[disco2].bloques.push_back(Bloque{s, false});
        }
        std::cout << "[RAID 10] Datos distribuidos (RAID 0) y reflejados en parejas (RAID 1).\n";
    }

    std::string leerCadena() override {
        int numParejas = numDiscos / 2;

        for (int p = 0; p < numParejas; ++p) {
            if (!discos[p * 2].activo && !discos[p * 2 + 1].activo) {
                std::cout << "[ERROR] RAID 10 cayo: La pareja de espejos " << p << " perdio ambos discos.\n";
                return "";
            }
        }

        std::string res = "";
        size_t maxBloques = 0;
        for (const auto& d : discos) {
            if (d.bloques.size() > maxBloques) maxBloques = d.bloques.size();
        }

        for (size_t b = 0; b < maxBloques; ++b) {
            for (int p = 0; p < numParejas; ++p) {
                int disco1 = p * 2;
                int disco2 = disco1 + 1;

                int discoUsar = discos[disco1].activo ? disco1 : disco2;
                if (b < discos[discoUsar].bloques.size()) {
                    res += discos[discoUsar].bloques[b].contenido;
                }
            }
        }
        return res;
    }

    bool reconstruirDisco(int discoId) override {
        if (discoId < 0 || discoId >= numDiscos) return false;
        int pareja = discoId / 2;
        int discoEspejo = (discoId % 2 == 0) ? discoId + 1 : discoId - 1;

        if (!discos[discoEspejo].activo) {
            std::cout << "[ERROR] Imposible reconstruir Disco " << discoId << ": Su espejo (Disco " << discoEspejo << ") tambien esta caido.\n";
            return false;
        }

        discos[discoId].reemplazarYReconstruir(discos[discoEspejo].bloques.size());
        discos[discoId].bloques = discos[discoEspejo].bloques;
        std::cout << "[RAID 10] Disco " << discoId << " reconstruido desde el espejo " << discoEspejo << " (Pareja " << pareja << ").\n";
        return true;
    }

    void mostrarEstado() const override {
        std::cout << "\n=== ESTADO DEL ARREGLO RAID 10 ===\n";
        for (int i = 0; i < numDiscos; ++i) {
            if (i % 2 == 0) std::cout << "--- Pareja RAID 1 (" << i / 2 << ") ---\n";
            std::cout << "Disco " << discos[i].id << " [" << (discos[i].activo ? "OK" : "FALLO") << "]: ";
            if (!discos[i].activo) {
                std::cout << "(Sin datos)\n";
                continue;
            }
            for (const auto& b : discos[i].bloques) {
                std::cout << "[" << b.contenido << "] ";
            }
            std::cout << "\n";
        }
    }
};
