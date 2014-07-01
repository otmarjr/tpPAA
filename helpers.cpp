/* 
 * File:   helpers.cpp
 * Author: Otmar
 * 
 * Created on 1 de Julho de 2014, 18:32
 */

#include "helpers.h"
#include <algorithm>
#include <stdexcept>

using namespace std;

std::string &helpers::trim_string_a_esquerda(std::string &s) 
{
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
        return s;
}

std::string &helpers::trim_string_a_direita(std::string &s) 
{
        s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
        return s;
}

std::string &helpers::trim_string(std::string &s) 
{
        return trim_string_a_esquerda(trim_string_a_direita(s));
}

void helpers::levantar_erro_execucao(string mensagem)
{
    throw std::runtime_error(mensagem.c_str());
}