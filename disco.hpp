#pragma once
#include <iostream>
#include <vector>
#include <string>
#include "bloque.hpp"

class Disco {
public:
    int id;
    bool activo;
    std::vector<Bloque> bloques;

    Disco(int id_) : id(id_), activo(true) {}

    void fallar() {
        activo = false;
        bloques.clear(); // Si el disco falla se pierden los datos
        std::cout << "[ALERTA] El Disco " << id << " ha fallado y se encuentra INACTIVO.\n";
    }

    void reemplazarYReconstruir(size_t numBloques) {
        activo = true;
        bloques.assign(numBloques, Bloque{"", false});
        std::cout << "[INFO] El Disco " << id << " ha sido reemplazado y preparado para reconstruccion.\n";
    }
};