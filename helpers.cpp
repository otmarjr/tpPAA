/* 
 * File:   helpers.cpp
 * Author: Otmar
 * 
 * Created on 1 de Julho de 2014, 18:32
 */

#include "helpers.h"
#include <algorithm>
#include <stdexcept>
#include <string>
#include <list>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <sstream>
using namespace std;

std::string &helpers::trim_string_a_esquerda(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
    return s;
}

std::string &helpers::trim_string_a_direita(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
    return s;
}

std::string &helpers::trim_string(std::string &s) {
    return trim_string_a_esquerda(trim_string_a_direita(s));
}

void helpers::levantar_erro_execucao(string mensagem) {
    throw std::runtime_error(mensagem.c_str());
}

string& helpers::normalizar_string_para_minusculas(string& texto) {
    texto = trim_string(texto);
    transform(texto.begin(), texto.end(), texto.begin(), ::tolower);
    return texto;
}

bool helpers::strings_sao_equivalentes(string& x, string& y) {
    return normalizar_string_para_minusculas(x).compare(normalizar_string_para_minusculas(y)) == 0;
}

int helpers::contar_numero_strings_em_comum(list<string> &A, list<string> &B) {
    vector<string> palavras_comuns;

    A.sort();
    B.sort();

    set_intersection(A.begin(), A.end(), B.begin(), B.end(), back_inserter(palavras_comuns));

    return palavras_comuns.size();
}

list<string> helpers::carregar_linhas_arquivo(string carminho_arquivo) {
    string linha;
    ifstream arquivo(carminho_arquivo.c_str());
    list<string> lista;

    if (arquivo.is_open()) {
        while (getline(arquivo, linha)) {
            // Remove o \r que pode ficar ao final, conforme o SO de origem do arquivo:
            helpers::retirar_ultimo_caractere_se_presente(linha,'\r');
            lista.push_back(linha);
        }
        arquivo.close();
    } else {
        stringstream buffer_mensagem;
        buffer_mensagem << "Não foi possível abrir o arquivo '" << carminho_arquivo << "'.  Verifique se o caminho do arquivo foi digitado corretamente e se você tem permissão de leitura do arquivo.";

        helpers::levantar_erro_execucao(buffer_mensagem.str());
    }
    return lista;
}

bool helpers::string_is_int(string texto) {

    if (texto.empty() || ((!isdigit(texto[0])) && (texto[0] != '-') && (texto[0] != '+'))) {
        return false;
    }

    char * p;
    strtol(texto.c_str(), &p, 10);

    return (*p == 0);
}

string& helpers::retirar_ultimo_caractere_se_presente(string& texto, char caractere) {
    if (!texto.empty() && texto[texto.size() - 1] == caractere) {
        texto.erase(texto.size() - 1);
    }
    return texto;
}
