/* 
 * File:   grafo.cpp
 * Author: Otmar
 * 
 * Created on 2 de Julho de 2014, 09:41
 */

#include "grafo.h"
#include "helpers.h"
#include <iostream>
#include <fstream>
#include <sstream>

grafo::grafo(list<vertice*> vertices) {
    this->V = vertices;
}

grafo* grafo::construir_a_partir_colecao_projetos_e_stop_words(colecao_projetos_software &projetos, list<string> &stop_words) {
    list<vertice*> vertices;

    for (map<int, projeto_software*>::const_iterator it = projetos.begin(); it != projetos.end(); ++it) {
        projeto_software *p = it->second;
        vertices.push_back(new vertice(p->identificador()));
    }

    for (list<vertice*>::const_iterator i = vertices.begin(); i != vertices.end(); ++i) {
        int id_i = (*i)->identificador();

        vertice* x = (*i);
        projeto_software *proj_x = projetos[x->identificador()];

        list<vertice*>::const_iterator j = i;


        for (++j; j != vertices.end(); ++j) {
            vertice* y = (*j);
            projeto_software *proj_y = projetos[y->identificador()];
            int id_j = proj_y->identificador();
            int peso_i_j = proj_x->calcular_similiaridade_com_outro_projeto(*proj_y, stop_words);

            if (peso_i_j > 0) {
                x->conectar_a_outro_vertice(*y, peso_i_j);
            }
        }
    }
}

void grafo::salvar_representacao_lista_adjacencia_em_arquivo(string caminho_arquivo) {

    ofstream f_saida(caminho_arquivo.c_str());

    if (!f_saida.good() || !f_saida.is_open()){
        f_saida.close();
        ostringstream oss;
        oss << "Ocorreu um erro ao tentar abrir para escrita o arquivo de saída do grafo no caminho '" << caminho_arquivo << "'. Verifique se você tem permissão de escrita no caminho informado e se nehum outro processo está utilizando o arquivo.";
        helpers::levantar_erro_execucao(oss.str());
    } else {
        
        for (list<vertice*>::iterator i=this->V.begin();i!=this->V.end();++i){
            vertice *v = *i;
            f_saida<<v->para_string()<<std::endl;
        }

        f_saida.close();
        
        if (!f_saida.good() || !f_saida.is_open()){
            ostringstream oss;
            oss << "Ocorreu um erro durante a escrita do arquivo de saída do grafo no caminho '" << caminho_arquivo << "'. Verifique se você tem permissão de escrita no caminho informado.";
            helpers::levantar_erro_execucao(oss.str());
        }
    }
}
