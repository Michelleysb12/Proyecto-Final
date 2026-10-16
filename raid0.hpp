#pragma once
#include <iostream>
#include <vector>
#include <string>
#include "disco.hpp"
#include "raidsystem.hpp"

class RAID0 : public RAIDSystem {
public:
    RAID0(int nDiscos) : RAIDSystem(nDiscos) {}

    void escribirCadena(const std::string& datos) override {
        for (auto& d : discos) d.bloques.clear();

        for (size_t i = 0; i < datos.length(); ++i) {
            int discoTarget = i % numDiscos;
            std::string c(1, datos[i]);
            discos[discoTarget].bloques.push_back(Bloque{c, false});
        }
        std::cout << "[RAID 0] Datos guardados mediante distribuicion por bloques (Striping).\n";
    }

    std::string leerCadena() override {
        std::string res = "";
        size_t maxBloques = 0;
        for (const auto& d : discos) {
            if (!d.activo) {
                std::cout << "[ERROR] RAID 0 cayo: El disco " << d.id << " fallo. Imposible recuperar datos.\n";
                return "";
            }
            if (d.bloques.size() > maxBloques) maxBloques = d.bloques.size();
        }

        for (size_t b = 0; b < maxBloques; ++b) {
            for (int d = 0; d < numDiscos; ++d) {
                if (b < discos[d].bloques.size()) {
                    res += discos[d].bloques[b].contenido;
                }
            }
        }
        return res;
    }

    bool reconstruirDisco(int discoId) override {
        std::cout << "[RAID 0] No es posible reconstruir. RAID 0 no posee redundancia de datos.\n";
        return false;
    }

    void mostrarEstado() const override {
        std::cout << "\n=== ESTADO DEL ARREGLO RAID 0 ===\n";
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
