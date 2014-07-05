/* 
 * File:   projeto_software.cpp
 * Author: Otmar
 * 
 * Created on 1 de Julho de 2014, 15:41
 */

#include "projeto_software.h"
#include <sstream>
#include <ios>
#include <iterator>
#include <stdlib.h>
#include <ctime>
#include <time.h>
#include <stdexcept>
#include "helpers.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include<list>
using namespace std;

projeto_software::projeto_software(string linha_info_projetos) {
    stringstream stream_dados_projeto(linha_info_projetos);
    string campo_arquivo;

    // Cada linha do arquivo tem o formato: <ID>|<LINGUAGEM PROGRAMACAO>|<DESCRICAO>|<DATA ULTIMO COMMIT>|<LISTA MEMBROS PROJETO>
    const char DELIMITADOR_CAMPOS_ARQUIVO = '|';
    const int TOTAL_CAMPOS_ARQUIVO  = 5;
    int total_campos_linha = count(linha_info_projetos.begin(), linha_info_projetos.end(), DELIMITADOR_CAMPOS_ARQUIVO) +1;
    if ( total_campos_linha != TOTAL_CAMPOS_ARQUIVO) {
        string erro = "Foi recebida uma linha fora do formato de conversão, que é <ID>|<LINGUAGEM PROGRAMACAO>|<DESCRICAO>|<DATA ULTIMO COMMIT>|<LISTA MEMBROS PROJETO>.";;
        throw erro;
    }

    getline(stream_dados_projeto, campo_arquivo, DELIMITADOR_CAMPOS_ARQUIVO);

    if (!helpers::string_e_inteiro(campo_arquivo)) {
        string erro = "O campo <ID> recebido não é um inteiro válido.";
        throw erro;
    }

    istringstream ss(campo_arquivo);
    ss >> this->id;

    getline(stream_dados_projeto, campo_arquivo, DELIMITADOR_CAMPOS_ARQUIVO);
    this->linguagem = campo_arquivo;

    getline(stream_dados_projeto, campo_arquivo, DELIMITADOR_CAMPOS_ARQUIVO);
    this->descricao_projeto = campo_arquivo;

    getline(stream_dados_projeto, campo_arquivo, DELIMITADOR_CAMPOS_ARQUIVO);
    // data no formato: AAAA-MM-DD HH:MM:SS
    const string MASCARA_DATA_HORA_ARQUIVO = "AAAA-MM-DD HH:MM:SS";

    campo_arquivo = helpers::trim_string(campo_arquivo);

    if (!campo_arquivo.empty() && campo_arquivo.length() == MASCARA_DATA_HORA_ARQUIVO.length()) {
        time_t hora_corrente;
        time(&hora_corrente);

        tm momento_ultimo_commit;
        const char* formato_parse = "%Y-%m-%d %H:%M:%S";
        strptime(campo_arquivo.c_str(), formato_parse, &momento_ultimo_commit);
        time_t momento_commit = mktime(&momento_ultimo_commit);


        const double TOTAL_SEGUNDOS_EM_UM_ANO = 60 * 60 * 24 * 365;
        double segundos_decorridos_desde_ultimo_commit = difftime(hora_corrente, momento_commit);

        this->ultimo_commit_menos_um_ano = segundos_decorridos_desde_ultimo_commit <= TOTAL_SEGUNDOS_EM_UM_ANO;
    } else {
        this->ultimo_commit_menos_um_ano = false;
    }

    getline(stream_dados_projeto, campo_arquivo, DELIMITADOR_CAMPOS_ARQUIVO);

    campo_arquivo = helpers::trim_string(campo_arquivo);

    if (!campo_arquivo.empty()) {
        const char DELIMITADOR_LISTA_MEMBROS_PROJETO = ',';
        // A lista de membros do projeto é o último campo com os logins separados por vírgula
        stringstream stream_membros(campo_arquivo);

        string login;

        while (!stream_membros.eof()) {
            getline(stream_membros, login, DELIMITADOR_LISTA_MEMBROS_PROJETO);
            this->logins_membros.push_back(login);
        }
    }
}

string projeto_software::para_string() const {
    string infos_projeto;

    ostringstream formatador;

    formatador << "Id: " << this->id;
    formatador << " Linguagem_programacao: " << this->linguagem;
    formatador << " Descrição: " << this->descricao_projeto;
    formatador << " Ultimo commit há menos de um ano?: " << this->ultimo_commit_menos_um_ano;

    ostringstream formatador_membros;
    string buffer_login;
    copy(this->logins_membros.begin(), this->logins_membros.end(), ostream_iterator<string>(formatador_membros, ","));
    formatador << " Membros: " << formatador_membros.str();

    return formatador.str();
}

colecao_projetos_software projeto_software::carregar_lista_do_arquivo(string caminho_arquivo_info_projetos) {
    colecao_projetos_software m;

    list<string> linhas_arquivo = helpers::carregar_linhas_arquivo(caminho_arquivo_info_projetos);

    int indice_linha = 1;

    for (list<string>::iterator i = linhas_arquivo.begin(); i != linhas_arquivo.end(); ++i) {
        try {
            projeto_software *p = new projeto_software(*i);
            m[p->identificador()] = p;
        } catch (string mensagem_erro_construtor) {
            ostringstream oss;
            oss << "Ocorreu o seguinte erro ao tentar ler a linha " << indice_linha << " do arquivo. Erro: " << mensagem_erro_construtor << " Linha lida: " << *i;
            helpers::levantar_erro_execucao(oss.str());
        }
        indice_linha++;
    }

    return m;
}

void projeto_software::desalocar_lista_projeto_software(list<projeto_software*> lista) {
    while (!lista.empty()) {
        delete lista.front(), lista.pop_front();
    }
}

int projeto_software::identificador() const {
    return this->id;
}

string projeto_software::linguagem_programacao() const {
    return this->linguagem;
}

string projeto_software::descricao() const {
    return this->descricao_projeto;
}

bool projeto_software::modificado_ultimo_ano() const {
    return this->ultimo_commit_menos_um_ano;
}

const list<string>& projeto_software::membros() const {
    return this->logins_membros;
}

int projeto_software::calcular_similiaridade_com_outro_projeto(const projeto_software &outro_projeto, list<string> &stop_words) const {
    int similaridade = 0;


    string lp_outro_projeto = outro_projeto.linguagem;
    string lp_este_projeto = this->linguagem;

    if (helpers::strings_sao_equivalentes(lp_outro_projeto, lp_este_projeto)) {
        similaridade;
    }

    if (outro_projeto.ultimo_commit_menos_um_ano == this->ultimo_commit_menos_um_ano) {
        similaridade++;
    }


    list<string> palavras_chave_outro_projeto= outro_projeto.palavras_significativas_na_descricao(stop_words);
    list<string> palavras_chave_este_projeto = this->palavras_significativas_na_descricao(stop_words);

    int numero_palavras_em_comum_na_descricao = helpers::contar_numero_strings_em_comum(palavras_chave_outro_projeto, palavras_chave_este_projeto);
    similaridade += numero_palavras_em_comum_na_descricao;
    
    list<string> membros_outro_projeto = outro_projeto.membros();
    list<string> membros_este_projeto = this->membros();

    int numero_membros_em_comum_projeto = helpers::contar_numero_strings_em_comum(membros_este_projeto, membros_outro_projeto);
    similaridade += numero_membros_em_comum_projeto;

    return similaridade;
}

list<string> projeto_software::palavras_significativas_na_descricao(list<string> &stop_words) const {
    list<string> palavras_significativas;

    istringstream iss(this->descricao_projeto);
    string palavra_descricao;

    stop_words.sort();
    
    while (iss >> palavra_descricao) {
        bool palava_descricao_e_stop_word = binary_search(stop_words.begin(), stop_words.end(), palavra_descricao);

        if (!palava_descricao_e_stop_word) {
            palavras_significativas.push_back(palavra_descricao);
        }
    }

    return palavras_significativas;
}

