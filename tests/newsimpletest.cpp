/* 
 * File:   newsimpletest.cpp
 * Author: Otmar
 *
 * Created on 02/07/2014, 20:30:57
 */

#include <stdlib.h>
#include <iostream>
#include "projeto_software.h"

/*
 * Simple C++ Test Suite
 */

void testar_criacao_projeto_software_de_linha_arquivo() {
    projeto_software* p1 = new projeto_software("34707|Shell|19/x çalışma ortamı|2013-09-10 16:15:09| ");
    projeto_software* p2 = new projeto_software("3586|JavaScript|Rapid Interface Builder (RIB) is a browser-based design tool for quickly prototyping and creating the user interface for web applications. Layout your UI by dropping widgets onto a canvas. Run the UI in an interactive \"Preview mode\". Export the generated |2012-09-17 23:08:15|zhizhangchen, grgustaf, sbryan, bspencer, otcuser");
    projeto_software* p3 = new projeto_software("1431411|C||2014-01-31 00:39:50|bpeel, rib, dlespiau");
    
    cout<<"\nLeitura de p1: "<<p1->para_string();
    cout<<"\nLeitura de p2: "<<p2->para_string();
    cout<<"\nLeitura de p3: "<<p2->para_string();
    std::cout << "newsimpletest test 1" << std::endl;
}

void test2() {
    std::cout << "newsimpletest test 2" << std::endl;
     //std::cout << "%TEST_FAILED% time=0 testname=test2 (newsimpletest) message=error message sample" << std::endl;
}

int main(int argc, char** argv) {
    std::cout << "%SUITE_STARTING% newsimpletest" << std::endl;
    std::cout << "%SUITE_STARTED%" << std::endl;

    std::cout << "%TEST_STARTED% test1 (newsimpletest)" << std::endl;
    testar_criacao_projeto_software_de_linha_arquivo();
    std::cout << "%TEST_FINISHED% time=0 test1 (newsimpletest)" << std::endl;

    std::cout << "%TEST_STARTED% test2 (newsimpletest)\n" << std::endl;
    test2();
    std::cout << "%TEST_FINISHED% time=0 test2 (newsimpletest)" << std::endl;

    std::cout << "%SUITE_FINISHED% time=0" << std::endl;

    return (EXIT_SUCCESS);
}

