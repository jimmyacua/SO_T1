//
// Created by jimmy on 21/08/18.
//

#include "Diccionario.h"

void Diccionario::crear() {
    primero = NULL;
    numElem = 0;
}

void Diccionario::destruir() {
    Caja *anterior;
    Caja *temp = primero;
    while(temp != NULL){
        anterior = temp;
        temp = temp->sgt;
        delete anterior;
    }
}


void Diccionario::agregar(string elem) {
    Caja *nuevo = new Caja(elem);
    if(primero == NULL){
        primero = nuevo;
        primero->numApariciones++;
        numElem++;
    }else{
        Caja* aux = primero;
        Caja *anterior;
        bool existe = false;
        while(aux != NULL && !existe){
            if(aux->elem.compare(elem) == 0){
                existe = true;
                aux->numApariciones++;
            } else{
                anterior = aux;
                aux = aux->sgt;
            }
        }
        if(!existe){
            anterior->sgt = nuevo;
            nuevo->numApariciones++;
            numElem++;
        }
    }

}


void Diccionario::ordenar() {
    Caja* min;
    Caja* p = primero;
    int cont1 = 1;
    while(p != 0 && cont1 < numElem){
        min = p;
        Caja* aux = p->sgt;
        int cont2 = cont1+1;
        while(aux != 0 && cont2 <= numElem){
            if(min->elem.compare(aux->elem) > 0){ //l.recuperar(min) > l.recuperar(aux)
                min = aux;
            }
            aux = aux->sgt;
            cont2++;
        }
        if(min->elem.compare(p->elem) < 0){ //l.recuperar(min) < l.recuperar(p
            string inter = min->elem;
            int interc = min->numApariciones;
            min->elem = p->elem;
            min->numApariciones = p->numApariciones;
            p->elem = inter;
            p->numApariciones = interc;
        }
        p = p->sgt;
        cont1++;
    }
}

string Diccionario::getEtq(int i) {
    Caja* aux = primero;
    int num = 1;
    while(aux != NULL && num < i){
        //cout << aux->elem << ": " << aux->numApariciones << endl;
        aux = aux->sgt;
        num++;
    }
    return aux->elem;
}

int Diccionario::getNumApar(int i) {
    Caja* aux = primero;
    int num = 1;
    while(aux != NULL && num<i){
        aux = aux->sgt;
        num++;
    }
    return aux->numApariciones;
}

int Diccionario::numElementos() {
    return numElem;
}