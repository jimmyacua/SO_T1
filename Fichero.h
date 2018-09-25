//
// Created by jimmy on 21/08/18.
//

#ifndef PRUEBA_FICHERO_H
#define PRUEBA_FICHERO_H

#include <string>
#include <stdio.h>
#include <iostream>
#include <cstring>
#include <fstream>
#include "Diccionario.h"

using namespace std;

class Fichero {
public:

    // constructor
    Fichero();

    //Efecto: lee el archivo con nombre s y agrega las etiquetas al diccionario
    //Requiere: s valido 
    //Modifica: -
    void leerArchivo(string s);

    void opcional(string nom);
    string getEtq(int i);

    int getTimes(int i);

    int totalEtq();

private:
    string nArchivo;
    Diccionario diccionario;

};


#endif //PRUEBA_FICHERO_H
