#pragma once
#include <iostream>
#include <vector>
#include <string>
#include "disco.hpp"

class RAIDSystem {
protected:
    std::vector<Disco> discos;
    int numDiscos;

public:
    RAIDSystem(int nDiscos) : numDiscos(nDiscos) {
        for (int i = 0; i < nDiscos; ++i) {
            discos.emplace_back(i);
        }
    }

    virtual ~RAIDSystem() = default;
    virtual void escribirCadena(const std::string& datos) = 0;
    virtual std::string leerCadena() = 0;
    virtual bool reconstruirDisco(int discoId) = 0;
    virtual void mostrarEstado() const = 0;

    void simularFalloDisco(int discoId) {
        if (discoId >= 0 && discoId < numDiscos) {
            discos[discoId].fallar();
        } else {
            std::cout << "ID de disco invalido.\n";
        }
    }

    int getNumDiscos() const { return numDiscos; }
};