#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>
#include <string.h>
#include "Diccionario.h"
#include "Fichero.h"
#include <wait.h>
#include <sys/sem.h>

#include "Buzon.h"
#include "Semaforo.h"

struct AreaCompartida {
    int numEtq;
    struct Etiqueta {
        char etq[MSGSIZE];
        int Veces;
    }
    Etiquetas[50];
};

typedef struct AreaCompartida AC;

int main(int argc, char** argv) {

    //--------Memoria compartida----------------
    AC * area;
    int id = shmget(KEY,1024,0600|IPC_CREAT);
    area = (AC *) shmat(id, NULL, 0);
    //------------------------------------------
    Semaforo sem;
    Fichero f;
    Buzon b;
    if(!fork()) { //proceso hijo
        string archivo = "/home/jimmy/GitHub/SO_T1/ejemploXML.xml";
        f.leerArchivo(archivo);
        int cont = 1;
        while (cont < f.totalEtq()) {
            area->numEtq++;
            strncpy(area->Etiquetas[cont].etq ,f.getEtq(cont).c_str(),MSGSIZE);
            area->Etiquetas[cont].Veces = f.getTimes(cont);
            b.Enviar(f.getEtq(cont).c_str(), f.getTimes(cont), 1);
            cont++;
        }
        sem.Signal();
        exit(0);
    }

    cout << "Proceso padre:" << endl;
    sem.Wait();
    int cont = 1;
    while(cont < area->numEtq){
        int st = b.Recibir(1);
        if(st < 1){
            cont = area->numEtq;
        } else{
            cont++;
        }
    }
    shmdt( area );
    shmctl( id, IPC_RMID, NULL );
    _exit(0);

}