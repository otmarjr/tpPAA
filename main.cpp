/* 
 * File:   main.cpp
 * Author: Otmar
 *
 * Created on 30 de Junho de 2014, 10:37
 */

#include "projeto_software.h"
#include "grafo.h"
#include "helpers.h"
#include <cstdlib>
#include <iostream>
#include <stdexcept>

using namespace std;

/*
 * 
 */

static void testar_linhas()
{
    projeto_software* p1 = new projeto_software("34707|Shell|19/x çalışma ortamı|2013-09-10 16:15:09| ");
    projeto_software* p2 = new projeto_software("3586|JavaScript|Rapid Interface Builder (RIB) is a browser-based design tool for quickly prototyping and creating the user interface for web applications. Layout your UI by dropping widgets onto a canvas. Run the UI in an interactive \"Preview mode\". Export the generated |2012-09-17 23:08:15|zhizhangchen, grgustaf, sbryan, bspencer, otcuser");
    projeto_software* p3 = new projeto_software("1431411|C||2014-01-31 00:39:50|bpeel, rib, dlespiau");
    
    cout<<"\nLeitura de p1: "<<p1->para_string();
    cout<<"\nLeitura de p2: "<<p2->para_string();
    cout<<"\nLeitura de p3: "<<p2->para_string();

}
static void testar_arquivo()
{
    try
    {
        colecao_projetos_software projetos = projeto_software::carregar_lista_do_arquivo("C:\\Users\\Otmar\\Google Drive\\Mestrado\\PAA\\TP1\\massa-testes\\projects_github_sorted_developers_info.csv");

        int i=0;
        for (colecao_projetos_software::const_iterator it = projetos.begin();it != projetos.end(); ++it) 
        {
            cout<<"Projeto na linha "<<++i<<" :"<<(*it->second).para_string();
        }
    }
    catch(runtime_error re)
    {
        cout<<"\nOcorreu um erro ao tentar gerar o grafo correspondente ao arquivo informado. Mensagem de erro: "<<re.what();
    }
}

static void testar_entradas()
{
    testar_arquivo();
}

static void testar_geracao_grafo(){
    try
    {
        colecao_projetos_software projetos = projeto_software::carregar_lista_do_arquivo("C:\\Users\\Otmar\\Google Drive\\Mestrado\\PAA\\TP1\\massa-testes\\projects_github_sorted_developers_info.csv");
        list<string> stop_words = helpers::carregar_linhas_arquivo("C:\\Users\\Otmar\\Google Drive\\Mestrado\\PAA\\TP1\\massa-testes\\glasgow-modified-stop-words-list.txt");
        
        grafo *g = grafo::construir_a_partir_colecao_projetos_e_stop_words(projetos, stop_words);
    }
    catch(runtime_error re)
    {
        cout<<"\nOcorreu um erro ao tentar gerar o grafo correspondente ao arquivo informado. Mensagem de erro: "<<re.what();
    }
}

int main(int argc, char** argv) {

    testar_geracao_grafo();
    return 0;
}

