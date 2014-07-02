/* 
 * File:   vertice.h
 * Author: Otmar
 *
 * Created on 2 de Julho de 2014, 09:06
 */

#ifndef VERTICE_H
#define	VERTICE_H

#include <string>
#include <map>
#include <list>
class aresta; // decleração atencipada para evitar dependência circular.

using namespace std;

class vertice {
public:
    int identificador();
    vertice(int id);
    string para_string();
    void conectar_a_outro_vertice(vertice &y, int peso_aresta);
    const list<aresta*>& lista_adjacencia() const;
    bool adjacente_a(vertice &outro);
    
private:
    int id;
    map<int,aresta*> adjacencia;
};

#endif	/* VERTICE_H */

