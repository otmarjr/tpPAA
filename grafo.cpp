/* 
 * File:   grafo.cpp
 * Author: Otmar
 * 
 * Created on 2 de Julho de 2014, 09:41
 */

#include "grafo.h"
#include "helpers.h"
#include <iostream>

grafo::grafo(list<vertice*> vertices) {
    this->V = vertices;
}

grafo* grafo::construir_a_partir_colecao_projetos_e_stop_words(colecao_projetos_software &projetos, list<string> &stop_words) {
    list<vertice*> vertices;
    
    for (map<int, projeto_software*>::const_iterator it = projetos.begin(); it != projetos.end(); ++it) {
        projeto_software *p = projetos[it->first];
        vertices.push_back(new vertice(p->identificador()));
    }
    
    for (list<vertice*>::const_iterator i = vertices.begin(); i != vertices.end(); ++i) {
        int id_i = (*i)->identificador();
        
        vertice* x = (*i);
        projeto_software *proj_x = projetos[x->identificador()];
        
        list<vertice*>::const_iterator j = i;
        
        
        for (++j;j != vertices.end();++j){
            vertice* y = (*j);
            projeto_software *proj_y = projetos[y->identificador()];
            int id_j = proj_y->identificador();
            int peso_i_j = proj_x->calcular_similiaridade_com_outro_projeto(*proj_y, stop_words);
            
            if (peso_i_j >0){
                x->conectar_a_outro_vertice(*y,peso_i_j);
            }
        }
    }
}

