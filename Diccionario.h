//
// Created by jimmy on 21/08/18.
//

#ifndef PRUEBA_DICCIONARIO_H
#define PRUEBA_DICCIONARIO_H
#include <string>
#include <stdio.h>
#include <iostream>

using namespace std;

class Diccionario {
    struct Caja{
    public:
        string elem;
        int numApariciones;
        Caja* sgt;
        Caja(){
            sgt = nullptr;
            elem = "";
            numApariciones = 0;
        }
        Caja(string elmt){
            elem = elmt;
            numApariciones = 0;
            sgt = nullptr;
        }

        void setElem(string nE){
            elem = nE;
        }

    };
    
public:
        //Efecto: inicializa el diccionario e inicializa numElem
        //Requiere: -
        //Modifica: -
        void crear();

        //Efecto: Destruye el diccionario devolviendo la memoria al sistema.
        //Requiere: Diccionario inicializado
        //Modifica: el Diccionario
        void destruir();

        //Efecto: Agrega un elemento al diccionario si no existe. Si existe se aumenta el contador de apariciones del elemento.
        //Requiere: diccionario inicializado.
        //Modifica: el diccionario
        void agregar(string);


	    //Efecto: ordena alfabeticamente los elementos del diccionario
        //Requiere: diccionario inicializado y no vacio
        //Modifica: el diccionario
        void ordenar();

        //Efecto:
        //Requiere:
        //Modifica:
        string getEtq(int i);

        int getNumApar(int i);
        int numElementos();

    private:
        Caja *primero;
	    int numElem;
};

#endif //PRUEBA_DICCIONARIO_H
