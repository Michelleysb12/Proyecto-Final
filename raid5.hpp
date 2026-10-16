#pragma once
#include <iostream>
#include <vector>
#include <string>
#include "disco.hpp"
#include "raidsystem.hpp"

class RAID5 : public RAIDSystem {
private:
    char calcularXOR(const std::vector<char>& caracteres) {
        char res = 0;
        for (char c : caracteres) res ^= c;
        return res;
    }

public:
    RAID5(int nDiscos) : RAIDSystem(nDiscos) {
        if (nDiscos < 3) {
            std::cout << "RAID 5 requiere minimo 3 discos. Ajustando a 3.\n";
            numDiscos = 3;
            discos.emplace_back(2);
        }
    }

    void escribirCadena(const std::string& datos) override {
        for (auto& d : discos) d.bloques.clear();

        size_t dataPerStripe = numDiscos - 1;
        size_t totalStripes = (datos.length() + dataPerStripe - 1) / dataPerStripe;

        size_t dataIdx = 0;
        for (size_t stripe = 0; stripe < totalStripes; ++stripe) {
            int parityDisk = (numDiscos - 1) - (stripe % numDiscos);
            std::vector<char> stripeChars;

            for (int d = 0; d < numDiscos; ++d) {
                if (d == parityDisk) {
                    discos[d].bloques.push_back(Bloque{"P", true});
                } else {
                    char c = (dataIdx < datos.length()) ? datos[dataIdx++] : ' ';
                    stripeChars.push_back(c);
                    discos[d].bloques.push_back(Bloque{std::string(1, c), false});
                }
            }

            char paridadChar = calcularXOR(stripeChars);
            discos[parityDisk].bloques[stripe].contenido = std::string(1, paridadChar);
        }
        std::cout << "[RAID 5] Datos y paridad distribuida almacenados correctamente.\n";
    }

    std::string leerCadena() override {
        int discosCaidos = 0;
        int discoCaidoId = -1;
        for (const auto& d : discos) {
            if (!d.activo) {
                discosCaidos++;
                discoCaidoId = d.id;
            }
        }

        if (discosCaidos > 1) {
            std::cout << "[ERROR] RAID 5 no puede soportar el fallo de " << discosCaidos << " discos al mismo tiempo.\n";
            return "";
        }

        std::string res = "";
        size_t totalStripes = 0;
        for (const auto& d : discos) {
            if (d.activo) {
                totalStripes = d.bloques.size();
                break;
            }
        }

        for (size_t stripe = 0; stripe < totalStripes; ++stripe) {
            int parityDisk = (numDiscos - 1) - (stripe % numDiscos);

            for (int d = 0; d < numDiscos; ++d) {
                if (d == parityDisk) continue;

                if (d == discoCaidoId) {
                    std::vector<char> xorGroup;
                    for (int o = 0; o < numDiscos; ++o) {
                        if (o != discoCaidoId) {
                            xorGroup.push_back(discos[o].bloques[stripe].contenido[0]);
                        }
                    }
                    char recuperado = calcularXOR(xorGroup);
                    res += recuperado;
                } else {
                    res += discos[d].bloques[stripe].contenido;
                }
            }
        }
        return res;
    }

    bool reconstruirDisco(int discoId) override {
        if (discos[discoId].activo) {
            std::cout << "El disco ya esta funcional.\n";
            return false;
        }

        int discosInactivos = 0;
        for (const auto& d : discos) if (!d.activo) discosInactivos++;

        if (discosInactivos > 1) {
            std::cout << "[ERROR] No se puede reconstruir RAID 5 con mas de un disco caido.\n";
            return false;
        }

        size_t numBloques = 0;
        for (const auto& d : discos) {
            if (d.activo) {
                numBloques = d.bloques.size();
                break;
            }
        }

        discos[discoId].reemplazarYReconstruir(numBloques);

        for (size_t stripe = 0; stripe < numBloques; ++stripe) {
            int parityDisk = (numDiscos - 1) - (stripe % numDiscos);
            std::vector<char> xorGroup;

            for (int d = 0; d < numDiscos; ++d) {
                if (d != discoId) {
                    xorGroup.push_back(discos[d].bloques[stripe].contenido[0]);
                }
            }

            char contenidoRecuperado = calcularXOR(xorGroup);
            discos[discoId].bloques[stripe].contenido = std::string(1, contenidoRecuperado);
            discos[discoId].bloques[stripe].esParidad = (discoId == parityDisk);
        }

        std::cout << "[RAID 5] Disco " << discoId << " reconstruido exitosamente mediante calculo XOR.\n";
        return true;
    }

    void mostrarEstado() const override {
        std::cout << "\n=== ESTADO DEL ARREGLO RAID 5 ===\n";
        for (const auto& d : discos) {
            std::cout << "Disco " << d.id << " [" << (d.activo ? "OK" : "FALLO") << "]: ";
            if (!d.activo) {
                std::cout << "(Sin datos)\n";
                continue;
            }
            for (const auto& b : d.bloques) {
                if (b.esParidad) std::cout << "[P:" << (int)(unsigned char)b.contenido[0] << "] ";
                else std::cout << "[" << b.contenido << "] ";
            }
            std::cout << "\n";
        }
    }
};