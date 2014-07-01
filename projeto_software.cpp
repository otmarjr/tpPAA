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
#include "helpers.h"

projeto_software::projeto_software(string linha_info_projetos)
{
    stringstream stream_dados_projeto(linha_info_projetos);
    string campo_arquivo;

    // Cada linha do arquivo tem o formato: <ID>|<LINGUAGEM PROGRAMACAO>|<DESCRICAO>|<DATA ULTIMO COMMIT>|<LISTA MEMBROS PROJETO>
    const char DELIMITADOR_CAMPOS_ARQUIVO = '|';
    
    getline(stream_dados_projeto, campo_arquivo, DELIMITADOR_CAMPOS_ARQUIVO);
    istringstream ss(campo_arquivo);
    ss >> this->id;
    
    getline(stream_dados_projeto, campo_arquivo, DELIMITADOR_CAMPOS_ARQUIVO);
    this->linguagem_programacao = campo_arquivo;
    
    getline(stream_dados_projeto, campo_arquivo, DELIMITADOR_CAMPOS_ARQUIVO);
    this->descricao_projeto = campo_arquivo;
    
    getline(stream_dados_projeto, campo_arquivo, DELIMITADOR_CAMPOS_ARQUIVO);
    // data no formato: AAAA-MM-DD HH:MM:SS
    const string MASCARA_DATA_HORA_ARQUIVO = "AAAA-MM-DD HH:MM:SS";

    campo_arquivo = helpers::trim_string(campo_arquivo);
       
    if (!campo_arquivo.empty() && campo_arquivo.length() == MASCARA_DATA_HORA_ARQUIVO.length())
    {
        time_t hora_corrente;
        time(&hora_corrente);
        
        tm momento_ultimo_commit;
        const char* formato_parse = "%Y-%m-%d %H:%M:%S";
        strptime(campo_arquivo.c_str(), formato_parse, &momento_ultimo_commit);
        time_t momento_commit = mktime(&momento_ultimo_commit);
        
        
        const double TOTAL_SEGUNDOS_EM_UM_ANO = 60*60*24*365;
        double segundos_decorridos_desde_ultimo_commit = difftime(hora_corrente, momento_commit);
        
        this->ultimo_commit_menos_um_ano = segundos_decorridos_desde_ultimo_commit <= TOTAL_SEGUNDOS_EM_UM_ANO;
    }
    else
    {
        this->ultimo_commit_menos_um_ano = false;
    }
            
    getline(stream_dados_projeto, campo_arquivo, DELIMITADOR_CAMPOS_ARQUIVO);
    
    campo_arquivo = helpers::trim_string(campo_arquivo);
    
    if (!campo_arquivo.empty())
    {
        const char DELIMITADOR_LISTA_MEMBROS_PROJETO = ',';    
        // A lista de membros do projeto é o último campo com os logins separados por vírgula
        stringstream stream_membros(campo_arquivo);
        
        string login;
        
        while (!stream_membros.eof())
        {
            getline(stream_membros, login, DELIMITADOR_LISTA_MEMBROS_PROJETO);
            this->logins_membros.push_back(login);
        }
    }
}

string projeto_software::para_string()
{
    string infos_projeto;
    
    ostringstream formatador;
    
    formatador<<"Id: "<<this->id;
    formatador<<" Linguagem_programacao: "<<this->linguagem_programacao;
    formatador<<" Descrição: "<<this->descricao_projeto;
    formatador<<" Ultimo commit há menos de um ano?: "<<this->ultimo_commit_menos_um_ano;
    
    ostringstream formatador_membros;
    string buffer_login;
    copy(this->logins_membros.begin(), this->logins_membros.end(), ostream_iterator<string>(formatador_membros,","));
    formatador<<" Membros: "<<formatador_membros.str();
    
    return formatador.str();
}

