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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <time.h>
#include <ctype.h>

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

string helpers::para_caixa_alta(string& texto) {
    string s = texto;
    
    std::transform(s.begin(), s.end(), s.begin(), ::toupper);
    
    return s;
}

string helpers::para_caixa_baixa(string& texto) {
    string s = texto;
    
    std::transform(s.begin(), s.end(), s.begin(), ::tolower);
    
    return s;
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
            linha = helpers::retirar_ultimo_caractere_se_presente(linha, '\r');
            linha = helpers::retirar_ultimo_caractere_se_presente(linha, '\r');
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

bool helpers::string_e_inteiro(string texto) {

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

int helpers::string_para_inteiro(string& texto) {
    istringstream iss(texto);
    int valor_inteiro;
    iss>>valor_inteiro;
    return valor_inteiro;
}




/*
 * We do not implement alternate representations. However, we always
 * check whether a given modifier is allowed for a certain conversion.
 */
#define ALT_E          0x01
#define ALT_O          0x02
//#define LEGAL_ALT(x)       { if (alt_format & ~(x)) return (0); }
#define LEGAL_ALT(x)       { ; }
#define TM_YEAR_BASE   (1970)

static int conv_num(const char **, int *, int, int);
static int strncasecmp(char *s1, char *s2, size_t n);

static const char *day[7] = {
    "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday",
    "Friday", "Saturday"
};
static const char *abday[7] = {
    "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
};
static const char *mon[12] = {
    "January", "February", "March", "April", "May", "June", "July",
    "August", "September", "October", "November", "December"
};
static const char *abmon[12] = {
    "Jan", "Feb", "Mar", "Apr", "May", "Jun",
    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};
static const char *am_pm[2] = {
    "AM", "PM"
};

char * helpers::strptime_port_windows(const char *buf, const char *fmt, struct tm *tm) {
    char c;
    const char *bp;
    size_t len = 0;
    int alt_format, i, split_year = 0;

    bp = buf;

    while ((c = *fmt) != '\0') {
        /* Clear `alternate' modifier prior to new conversion. */
        alt_format = 0;

        /* Eat up white-space. */
        if (isspace(c)) {
            while (isspace(*bp))
                bp++;

            fmt++;
            continue;
        }

        if ((c = *fmt++) != '%')
            goto literal;


again:
        switch (c = *fmt++) {
            case '%': /* "%%" is converted to "%". */
                literal :
                if (c != *bp++)
                    return (0);
                break;

                /* 
                 * "Alternative" modifiers. Just set the appropriate flag
                 * and start over again.
                 */
            case 'E': /* "%E?" alternative conversion modifier. */
                LEGAL_ALT(0);
                alt_format |= ALT_E;
                goto again;

            case 'O': /* "%O?" alternative conversion modifier. */
                LEGAL_ALT(0);
                alt_format |= ALT_O;
                goto again;

                /*
                 * "Complex" conversion rules, implemented through recursion.
                 */
            case 'c': /* Date and time, using the locale's format. */
                LEGAL_ALT(ALT_E);
                if (!(bp = strptime_port_windows(bp, "%x %X", tm)))
                    return (0);
                break;

            case 'D': /* The date as "%m/%d/%y". */
                LEGAL_ALT(0);
                if (!(bp = strptime_port_windows(bp, "%m/%d/%y", tm)))
                    return (0);
                break;

            case 'R': /* The time as "%H:%M". */
                LEGAL_ALT(0);
                if (!(bp = strptime_port_windows(bp, "%H:%M", tm)))
                    return (0);
                break;

            case 'r': /* The time in 12-hour clock representation. */
                LEGAL_ALT(0);
                if (!(bp = strptime_port_windows(bp, "%I:%M:%S %p", tm)))
                    return (0);
                break;

            case 'T': /* The time as "%H:%M:%S". */
                LEGAL_ALT(0);
                if (!(bp = strptime_port_windows(bp, "%H:%M:%S", tm)))
                    return (0);
                break;

            case 'X': /* The time, using the locale's format. */
                LEGAL_ALT(ALT_E);
                if (!(bp = strptime_port_windows(bp, "%H:%M:%S", tm)))
                    return (0);
                break;

            case 'x': /* The date, using the locale's format. */
                LEGAL_ALT(ALT_E);
                if (!(bp = strptime_port_windows(bp, "%m/%d/%y", tm)))
                    return (0);
                break;

                /*
                 * "Elementary" conversion rules.
                 */
            case 'A': /* The day of week, using the locale's form. */
            case 'a':
                LEGAL_ALT(0);
                for (i = 0; i < 7; i++) {
                    /* Full name. */
                    len = strlen(day[i]);
                    if (strncasecmp((char *) (day[i]), (char *) bp, len) == 0)
                        break;

                    /* Abbreviated name. */
                    len = strlen(abday[i]);
                    if (strncasecmp((char *) (abday[i]), (char *) bp, len) == 0)
                        break;
                }

                /* Nothing matched. */
                if (i == 7)
                    return (0);

                tm->tm_wday = i;
                bp += len;
                break;

            case 'B': /* The month, using the locale's form. */
            case 'b':
            case 'h':
                LEGAL_ALT(0);
                for (i = 0; i < 12; i++) {
                    /* Full name. */

                    len = strlen(mon[i]);
                    if (strncasecmp((char *) (mon[i]), (char *) bp, len) == 0)
                        break;

                    /* Abbreviated name. */
                    len = strlen(abmon[i]);
                    if (strncasecmp((char *) (abmon[i]), (char *) bp, len) == 0)
                        break;
                }

                /* Nothing matched. */
                if (i == 12)
                    return (0);

                tm->tm_mon = i;
                bp += len;
                break;

            case 'C': /* The century number. */
                LEGAL_ALT(ALT_E);
                if (!(conv_num(&bp, &i, 0, 99)))
                    return (0);

                if (split_year) {
                    tm->tm_year = (tm->tm_year % 100) + (i * 100);
                } else {
                    tm->tm_year = i * 100;
                    split_year = 1;
                }
                break;

            case 'd': /* The day of month. */
            case 'e':
                LEGAL_ALT(ALT_O);
                if (!(conv_num(&bp, &tm->tm_mday, 1, 31)))
                    return (0);
                break;

            case 'k': /* The hour (24-hour clock representation). */
                LEGAL_ALT(0);
                /* FALLTHROUGH */
            case 'H':
                LEGAL_ALT(ALT_O);
                if (!(conv_num(&bp, &tm->tm_hour, 0, 23)))
                    return (0);
                break;

            case 'l': /* The hour (12-hour clock representation). */
                LEGAL_ALT(0);
                /* FALLTHROUGH */
            case 'I':
                LEGAL_ALT(ALT_O);
                if (!(conv_num(&bp, &tm->tm_hour, 1, 12)))
                    return (0);
                if (tm->tm_hour == 12)
                    tm->tm_hour = 0;
                break;

            case 'j': /* The day of year. */
                LEGAL_ALT(0);
                if (!(conv_num(&bp, &i, 1, 366)))
                    return (0);
                tm->tm_yday = i - 1;
                break;

            case 'M': /* The minute. */
                LEGAL_ALT(ALT_O);
                if (!(conv_num(&bp, &tm->tm_min, 0, 59)))
                    return (0);
                break;

            case 'm': /* The month. */
                LEGAL_ALT(ALT_O);
                if (!(conv_num(&bp, &i, 1, 12)))
                    return (0);
                tm->tm_mon = i - 1;
                break;

                //            case 'p': /* The locale's equivalent of AM/PM. */
                //                LEGAL_ALT(0);
                //                /* AM? */
                //                if (strcasecmp(am_pm[0], bp) == 0) 
                //                {
                //                    if (tm->tm_hour > 11)
                //                        return (0);
                //
                //                    bp += strlen(am_pm[0]);
                //                    break;
                //                }
                //                /* PM? */
                //                else if (strcasecmp(am_pm[1], bp) == 0) 
                //                {
                //                    if (tm->tm_hour > 11)
                //                        return (0);
                //
                //                    tm->tm_hour += 12;
                //                    bp += strlen(am_pm[1]);
                //                    break;
                //                }
                //
                //                /* Nothing matched. */
                //                return (0);

            case 'S': /* The seconds. */
                LEGAL_ALT(ALT_O);
                if (!(conv_num(&bp, &tm->tm_sec, 0, 61)))
                    return (0);
                break;

            case 'U': /* The week of year, beginning on sunday. */
            case 'W': /* The week of year, beginning on monday. */
                LEGAL_ALT(ALT_O);
                /*
                 * XXX This is bogus, as we can not assume any valid
                 * information present in the tm structure at this
                 * point to calculate a real value, so just check the
                 * range for now.
                 */
                if (!(conv_num(&bp, &i, 0, 53)))
                    return (0);
                break;

            case 'w': /* The day of week, beginning on sunday. */
                LEGAL_ALT(ALT_O);
                if (!(conv_num(&bp, &tm->tm_wday, 0, 6)))
                    return (0);
                break;

            case 'Y': /* The year. */
                LEGAL_ALT(ALT_E);
                if (!(conv_num(&bp, &i, 0, 9999)))
                    return (0);

                tm->tm_year = i - TM_YEAR_BASE;
                break;

            case 'y': /* The year within 100 years of the epoch. */
                LEGAL_ALT(ALT_E | ALT_O);
                if (!(conv_num(&bp, &i, 0, 99)))
                    return (0);

                if (split_year) {
                    tm->tm_year = ((tm->tm_year / 100) * 100) + i;
                    break;
                }
                split_year = 1;
                if (i <= 68)
                    tm->tm_year = i + 2000 - TM_YEAR_BASE;
                else
                    tm->tm_year = i + 1900 - TM_YEAR_BASE;
                break;

                /*
                 * Miscellaneous conversions.
                 */
            case 'n': /* Any kind of white-space. */
            case 't':
                LEGAL_ALT(0);
                while (isspace(*bp))
                    bp++;
                break;


            default: /* Unknown/unsupported conversion. */
                return (0);
        }


    }

    /* LINTED functional specification */
    return ((char *) bp);
}

static int conv_num(const char **buf, int *dest, int llim, int ulim) {
    int result = 0;

    /* The limit also determines the number of valid digits. */
    int rulim = ulim;

    if (**buf < '0' || **buf > '9')
        return (0);

    do {
        result *= 10;
        result += *(*buf)++ -'0';
        rulim /= 10;
    } while ((result * 10 <= ulim) && rulim && **buf >= '0' && **buf <= '9');

    if (result < llim || result > ulim)
        return (0);

    *dest = result;
    return (1);
}

int strncasecmp(char *s1, char *s2, size_t n) {
    if (n == 0)
        return 0;

    while (n-- != 0 && tolower(*s1) == tolower(*s2)) {
        if (n == 0 || *s1 == '\0' || *s2 == '\0')
            break;
        s1++;
        s2++;
    }

    return tolower(*(unsigned char *) s1) - tolower(*(unsigned char *) s2);
}


template <typename T1, typename T2> struct ordenar_mapa_por_valor_decrescente {
    typedef pair<T1, T2> tipo;
    inline bool operator ()(tipo const& a, tipo const& b) const {
        return a.second > b.second;
    }
};

string helpers::sumarizar_entradas_dicionario(map<string, int> dicionario)  {
    stringstream ss;

    typedef pair<string, int> entrada_map;

    vector<entrada_map> entradas_maps(dicionario.begin(), dicionario.end());

    std::sort(entradas_maps.begin(), entradas_maps.end(), ordenar_mapa_por_valor_decrescente<string, int>());

    int cont = 0;
    for (vector<entrada_map>::iterator j = entradas_maps.begin(); j != entradas_maps.end(); ++j) {
        string chave = (*j).first;
        ss << ++cont << ") " << chave << ": " << (*j).second << " ";
    }

    return ss.str();
}


string helpers::sumarizar_entradas_dicionario(map<bool, int> dicionario)  {
    stringstream ss;

    typedef pair<bool, int> entrada_map;

    vector<entrada_map> entradas_maps(dicionario.begin(), dicionario.end());

    std::sort(entradas_maps.begin(), entradas_maps.end(), ordenar_mapa_por_valor_decrescente<bool, int>());

    int cont = 0;
    for (vector<entrada_map>::iterator j = entradas_maps.begin(); j != entradas_maps.end(); ++j) {
        bool chave = (*j).first;
        ss << ++cont << ") " << (chave? "Menos de um ano" : "Mais de um ano") << ": " << (*j).second << " ";
    }

    return ss.str();
}

void helpers::escrever_alerta(string mensagem) {
    cout<<"ALERTA DE EXECUÇÃO: "<<mensagem<<std::endl;
}

long helpers::segundos_entre_dois_clocks(clock_t& antes, clock_t& depois) {
    return (depois-antes)/CLOCKS_PER_SEC;
}

long helpers::milissegundos_entre_dois_clocks(clock_t& antes, clock_t& depois) {
    const int CLOCKS_PER_MSEC = CLOCKS_PER_SEC*1000;
        return (depois-antes)/CLOCKS_PER_MSEC;
}
