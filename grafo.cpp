/* 
 * File:   grafo.cpp
 * Author: Otmar
 * 
 * Created on 2 de Julho de 2014, 09:41
 */

#include "grafo.h"
#include "helpers.h"

grafo::grafo(list<vertice*> vertices) {
    this->V = vertices;
}

grafo* grafo::construir_a_partir_colecao_projetos(map<int, projeto_software*> projetos) {
    list<vertice*> vertices;
    
    for (map<int, projeto_software*>::const_iterator it = projetos.begin(); it != projetos.end(); ++it) {
        vertices.push_back(new vertice((*it->second).identificador()));
    }
    
    for (list<vertice*>::const_iterator i = vertices.begin(); i != vertices.end(); ++i) {
        vertice* x = (*i);
        projeto_software *proj_x = projetos[x->identificador()];
        list<vertice*>::const_iterator j = i;
        
        
        for (++j;j != vertices.end();++j){
            vertice* y = (*j);
            projeto_software *proj_y = projetos[y->identificador()];
            
            int peso_i_j=0;
            
            
            
            
            if (peso_i_j >0){
                x->conectar_a_outro_vertice(y,peso_i_j);
            }
        }
    }
}

