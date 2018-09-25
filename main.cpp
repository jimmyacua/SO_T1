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

#include "Buzon.h"
#include "Semaforo.h"

int main(int argc, char** argv) {
    Fichero f;
    Buzon b;
    string param = argv[1];
    if(param.compare("-t") == 0) {
        string nombre = argv[2];
        f.opcional(nombre);
    }
    else {
        string archivo = argv[1];
        int i = fork();
        if(i) {
            cout << i << " Empieza a leer " << endl;
            f.leerArchivo(archivo);
            int cont = 1;
            while(cont <= f.totalEtq()){
                b.Enviar(f.getEtq(cont).c_str(), f.getTimes(cont), 1);
                cont++;
            }
            struct msgbuf A;
            int st = b.Recibir(1);
            while(st > 1) {
                b.Recibir(1);
            }
            _exit(0);
        }
        else{
            exit(0);
        }
    }

    //PARTE 2
    /*
    int id;
    id = shmget(KEY, 1024, 0600 | IPC_CREAT );
    char * area = (char *) shmat( id, NULL, 0 );
    Buzon b;
    if ( fork() ) {
        Fichero f;
        string archivo = argv[1];
        f.leerArchivo(archivo);
        int cont = 1;
        while(cont <= f.totalEtq()){
            b.Enviar(f.getEtq(cont).c_str(), f.getTimes(cont), 1);
            cont++;
        }
        _exit(0);
    }
    else {
        int st = b.Recibir(1);
        while(st > 1) {
            b.Recibir(1);
        }
    }
    shmdt( area );
    shmctl( id, IPC_RMID, NULL );
    _exit(0);
*/
}