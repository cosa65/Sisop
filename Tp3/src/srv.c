#include "srv.h"
/*
 *  Ejemplo de servidor que tiene el "sí fácil" para con su
 *  cliente y no se lleva bien con los demás servidores.
 *
 */


void servidor(int mi_cliente)
{
    MPI_Status status; int origen, tag;
    int hay_pedido_local = FALSE;
    int listo_para_salir = FALSE;
    int rank;
    int cantidadDeServersOriginal;
    int cantidadDeServers;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &cantidadDeServers);
    cantidadDeServers = cantidadDeServersOriginal = cantidadDeServers/2;
    int maxSeqNum = 0;
    int seqNum = 0;
    int buf;
    int servEnEspera;
    char serverStatus[cantidadDeServers];
    int i;
    for (i = 0; i < cantidadDeServers; i++)
        serverStatus[i] = STAT_NORMAL;

    while( ! listo_para_salir ) {
        MPI_Recv(&buf, 1, MPI_INT, ANY_SOURCE, ANY_TAG, COMM_WORLD, &status);
        
        origen = status.MPI_SOURCE;
        tag = status.MPI_TAG;
        
        if (tag == TAG_PEDIDO) {
            assert(origen == mi_cliente);
            debug("Mi cliente solicita acceso exclusivo");
            assert(hay_pedido_local == FALSE);
            hay_pedido_local = TRUE;
            debug("Dándole permiso (frutesco por ahora)");
            servEnEspera = cantidadDeServers - 1;
            seqNum = ++maxSeqNum;
            printf("cso: %d\n", cantidadDeServers);
            if (cantidadDeServers != 1) { // si hay otros servidores, pido la seccion
                for(i = 0; i < cantidadDeServersOriginal*2; i = i + 2){
                    if(i != rank && serverStatus[i/2] != STAT_SE_FUE){
                        MPI_Send(&seqNum, 1, MPI_INT, i, TAG_ME_REQ, COMM_WORLD); //revisar que el send no espere
                    }
                }
            } else {
                MPI_Send(NULL, 0, MPI_INT, mi_cliente, TAG_OTORGADO, COMM_WORLD);
            }
            debug("Di permiso");
        }
        
        else if (tag == TAG_LIBERO) {
            assert(origen == mi_cliente);
            debug("Mi cliente libera su acceso exclusivo");
            assert(hay_pedido_local == TRUE);
            hay_pedido_local = FALSE;
            for (i=0; i<cantidadDeServersOriginal; i++) { // mando el OK a los que estaban esperando
                if (serverStatus[i] == STAT_WAITING) {
                    MPI_Send(&rank, 1, MPI_INT, i*2, TAG_ME_REP, COMM_WORLD);
                    serverStatus[i] = STAT_NORMAL;
                } 
            }
        }

        else if (tag == TAG_TERMINE) {
            assert(origen == mi_cliente);
            debug("Mi cliente avisa que terminó");
            listo_para_salir = TRUE;

            for (i=0; i<cantidadDeServersOriginal; i++) { // aviso a todos los servidores que me voy
                if (i != rank && serverStatus[i] != STAT_SE_FUE) {
                    MPI_Send(&rank, 1, MPI_INT, i*2, TAG_SRV_EXIT, COMM_WORLD);
                } 
            }
            return;
        }

        else if (tag == TAG_ME_REQ) {
            maxSeqNum = (maxSeqNum < buf)? buf : maxSeqNum;             // actualizo maxSeqNum
            if (hay_pedido_local && (seqNum < buf || (seqNum == buf && rank < origen))) { // si mi pedido tiene mayor prioridad, el otro espera
                serverStatus[origen/2] = STAT_WAITING;
            } else {
                MPI_Send(&rank, 1, MPI_INT, origen, TAG_ME_REP, COMM_WORLD); // si no, le doy el OK
            }
        }

        else if (tag == TAG_ME_REP) {
            servEnEspera--;
            if (servEnEspera == 0) {
                MPI_Send(NULL, 0, MPI_INT, mi_cliente, TAG_OTORGADO, COMM_WORLD);
            }
        }

        else if (tag == TAG_SRV_EXIT) {
            serverStatus[origen/2] = STAT_SE_FUE;
            cantidadDeServers--;
        }
        
    }
    
}

