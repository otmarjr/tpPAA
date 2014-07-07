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
#include <algorithm>
#include <tr1/regex>
grafo::grafo(list<vertice*> vertices) {
    copy(vertices.begin(),vertices.end(), back_inserter(this->V));
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
    
    return new grafo(vertices);
}

void grafo::salvar_em_formato_pajek(string caminho_arquivo) {

    ofstream f_saida(caminho_arquivo.c_str());

    if (!f_saida.good() || !f_saida.is_open()){
        f_saida.close();
        ostringstream oss;
        oss << "Ocorreu um erro ao tentar abrir para escrita o arquivo de saída do grafo no caminho '" << caminho_arquivo << "'. Verifique se você tem permissão de escrita no caminho informado e se nehum outro processo está utilizando o arquivo.";
        helpers::levantar_erro_execucao(oss.str());
    } else {
        
        int quantidade_vertices = this->V.size();
        
        f_saida<<"*Vertices "<<quantidade_vertices<<std::endl;
        
        map<int, int> indices_pajek;
        int linha_atual = 1;
        
        for (list<vertice*>::iterator i=this->V.begin();i!=this->V.end();++i){
            vertice *v = *i;
            indices_pajek[v->identificador()] = linha_atual;
            f_saida<<linha_atual<<" \""<<v->identificador()<<"\""<<std::endl;
            linha_atual++;
        }
        
        f_saida<<"*Edges"<<std::endl;
        
        
        for (list<vertice*>::iterator i=this->V.begin();i!=this->V.end();++i){
            vertice *v = *i;
            int id_pajek_vertice_i = indices_pajek[v->identificador()];
            
            list<aresta*> l = v->lista_adjacencia();
            
            for (list<aresta*>::const_iterator j = l.begin();j!= l.end();++j){
                aresta *a = *j;
                int id_pajek_vertice_j = indices_pajek[a->obter_vertice_da_outra_extremidade(*v).identificador()];
                
                int peso_aresta = a->peso();
                
                f_saida<<id_pajek_vertice_i<<" "<<id_pajek_vertice_j<<" "<<peso_aresta<<std::endl;
            }
            
        }
        

        f_saida.close();
        
        if (!f_saida.good()){
            ostringstream oss;
            oss << "Ocorreu um erro durante a escrita do arquivo de saída do grafo no caminho '" << caminho_arquivo << "'. Verifique se você tem permissão de escrita no caminho informado.";
            helpers::levantar_erro_execucao(oss.str());
        }
    }
}

grafo* grafo::construir_a_partir_de_arquivo_pajek(string caminho_arquivo_pajek) {
    
    list<string> linhas_arquivo = helpers::carregar_linhas_arquivo(caminho_arquivo_pajek);
    
    list<string>::iterator i = linhas_arquivo.begin();
    istringstream iss(*i);

    string string_atual_arquivo;
    iss >> string_atual_arquivo; // Ignora a parte da 1a linha, que possui apenas o texto *Vertices
    
    int quantidade_vertices;
    iss >> quantidade_vertices; 
    
    list<vertice*> l;
    
    map<int, vertice*> vertices_pajek;
    
    for (++i;distance(linhas_arquivo.begin(),i)<=quantidade_vertices;++i){
        string linha = *i;
        stringstream ss(linha);
        
        int id_pajek;
        ss>>id_pajek;
        
        int pos = linha.find(' ')+2;
        
        string id_projeto_software = linha.substr(pos, linha.size()-pos-1);
        
        vertice *v = new vertice(helpers::string_para_inteiro(id_projeto_software));
        
        vertices_pajek[id_pajek] = v;
        l.push_back(v);
    }
    
    for (++i;i!=linhas_arquivo.end();++i){
        string linha = *i;
        stringstream ss(linha);
        int id_origem, id_destino, peso;
        
        ss>>id_origem;
        vertice* x = vertices_pajek[id_origem];
        
        ss>>id_destino;
        vertice* y = vertices_pajek[id_destino];
        
        ss>>peso;
        x->conectar_a_outro_vertice(*y,peso);
        
    }
    grafo* g = new grafo(l);
    
    return g;
}

void grafo::salvar_clusters_projetos_em_arquivo(int quantidade_clusters, string caminho_arquivo_clusters) {

}
