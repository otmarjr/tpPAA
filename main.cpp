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
#include <unistd.h>
#include <list>
#include <fstream>
using namespace std;

typedef struct {
    bool gerar_grafo;
    string caminho_arquivo_entrada;
    string caminho_arquivo_saida;
    bool gerar_clusters;
    string caminho_arquivo_stop_words;
} parametros_execucao_programa;

typedef struct {
    bool possui_erros;
    list<string> mensagens_erro;
} resultado_validacao_parametros_execucao;

/*
 * 
 */

parametros_execucao_programa processar_argumentos_entrada(int argc, char** argv) {
    parametros_execucao_programa params;

    params.gerar_clusters = false;
    params.gerar_grafo = false;
    
    // Processa os argumentos de entrada do programa que seguem um dos formatos:
    // ./tpPAA -[g,c] -e <ARQUIVO_ENTRADA> -s <ARQUIVO_SAIDA> -w

    char opcao;

    while ((opcao = getopt(argc, argv, "gce:s:w:")) != -1) {
        switch (opcao) {
            case 'g':
                params.gerar_grafo = true;
                break;
            case 'c':
                params.gerar_clusters = true;
                break;
            case 'e':
                params.caminho_arquivo_entrada = string(optarg);
                break;
            case 's':
                params.caminho_arquivo_saida = string(optarg);
                break;
            case 'w':
                params.caminho_arquivo_stop_words = string(optarg);
                break;
        }
    }

    return params;
}

resultado_validacao_parametros_execucao validar_parametros_execucao(parametros_execucao_programa &params_exec) {
    resultado_validacao_parametros_execucao r;

    bool arquivo_entrada_ok;
    ifstream f_entrada(params_exec.caminho_arquivo_entrada.c_str());
    arquivo_entrada_ok = f_entrada.good() && f_entrada.is_open();
    f_entrada.close();

    if (params_exec.gerar_clusters && params_exec.gerar_grafo) {
        r.mensagens_erro.push_back("\nPara executar o programa, você deve optar entre gerar o grafo dos projetos do software ou os clusters de um grafo. Não é suportado o procsesamento simultâneo de ambas funcionalidades. Execute o programa novamente com apenas um dos argumentos (-g para grafo ou -c para clusters).");
    } else {
        if (params_exec.gerar_grafo) {
            if (!arquivo_entrada_ok) {
                r.mensagens_erro.push_back("Ocorreu um erro ao tentar verificar o arquivo com informações dos projetos de software para geração do grafo. Verifique se o caminho deste arquivo de entrada foi digitado corretamente.");
            }

            // Verifica se o aruqivo de stop words foi fornecido.
            ifstream f_stop_words(params_exec.caminho_arquivo_stop_words.c_str());

            if (!f_stop_words.good() || !f_stop_words.is_open()) {
                r.mensagens_erro.push_back("Não foi especificado um caminho válido para o arquivo de stop words em inglês. Verifique o caminho digitado e tente novamente.");
            }

        } else {
            if (params_exec.gerar_clusters) {
                if (!arquivo_entrada_ok) {
                    r.mensagens_erro.push_back("Ocorreu um erro ao tentar verificar o arquivo de grafo informado para localização de clusters. Verifique se o caminho deste arquivo de entrada foi digitado corretamente.");
                }
            } else {

                r.mensagens_erro.push_back("\nEste programa tem dois modos de execução: geração de grafos ou clusters de um grafo. Você não selecionou nnehum dos 2 modos. Execute novamente o programa com o argumento -g para gerar o arquivo de grafo dos projetos de software ou -c para encontrar clusters no grafo de entrada.");
            }
        }
    }

    r.possui_erros = r.mensagens_erro.size() >= 1;

    return r;
}

static void exibir_erros_parametros_execucao(resultado_validacao_parametros_execucao val_entrada) {
    cout << "\nForam encontrados os seguintes problemas nos argumentos de entrada do programa:";
    for (list<string>::iterator i = val_entrada.mensagens_erro.begin(); i != val_entrada.mensagens_erro.end(); ++i) {
        cout << "\n " << *i;
    }

    cout << "\nO programa possui dois modos de execução: geração de grafo e geração de clusters. Para gerar um arquivo de grafo, execute o programa como ./tpPAA -g -e <CAMINHO_ARQUIVO_INFORMACOES_PROJETOS_SOFTWARE> -s <CAMINHO_ARQUIVO_GRAFO_GERADO> -w <CAMINHO_ARQUIVO_STOP_WORDS>.";
    cout << "\nPara gerar um arquivo com os clusters encontrados, execute o programa como ./tpPAA -c -e <CAMINHO_ARQUIVO_GRAFO_GERADO> -s <CAMINHO_ARQUIVO_CLUSTERS_ENCONTRADOS>.";
}

static void gerar_grafo(parametros_execucao_programa &params) {
    colecao_projetos_software projetos = projeto_software::carregar_lista_do_arquivo(params.caminho_arquivo_entrada);
    list<string> stop_words = helpers::carregar_linhas_arquivo(params.caminho_arquivo_stop_words);

    grafo *g = grafo::construir_a_partir_colecao_projetos_e_stop_words(projetos, stop_words);
    g->salvar_representacao_lista_adjacencia_em_arquivo(params.caminho_arquivo_saida);
    cout<<"Grafo gerado com sucesso e salvo em "<<params.caminho_arquivo_saida;
}

int main(int argc, char** argv) {

    parametros_execucao_programa params = processar_argumentos_entrada(argc, argv);

    resultado_validacao_parametros_execucao val_entrada = validar_parametros_execucao(params);

    if (val_entrada.possui_erros) {
        exibir_erros_parametros_execucao(val_entrada);
    } else {
        if (params.gerar_grafo) {
            try {
                gerar_grafo(params);
            } catch (runtime_error re) {
                cout << "\nOcorreu um erro ao tentar gerar o grafo correspondente ao arquivo informado. Mensagem de erro: " << re.what();
            }
        } else {

        }
    }

    return 0;
}

