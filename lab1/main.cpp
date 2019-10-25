// Giovani Nascimento Pereira
// RA168609
// Lab1 MC658 2S2019 - FKM

// * Problema da formacao de equipes *
//
// Seja
// x{ij}{k} variavel que indica que os funcionarios i e j estão na equipe k
// y{ij} indica se a aresta {i,j} pertence à solução
// v(i,j) valor da aresta {i,j}
//
// MAXIMIZAR
// ∑ y{ij} * v(i,j)
//
//
// Sujeito a,
//
// |S_k| -> ∑ x{ij}{k} <= b, ∀ {i,j}
// |S_k| -> ∑ x{ij}{k} >= a, ∀ {i,j}  - Tamanhos limites dos grupos (max e min)
//
// ∑ x{ii}{k} = 1, ∀ k                - Cada vertice (pessoa) pertence a 1 grupo apenas
//
// y{ij} <= (x{ii}{k} + x{jj}{k})/2   - Se 2 pessoas, i e j, estão no grupo k
// y{ij} >= (x{ii}{k} + x{jj}{k} - 1)/2 a aresta {i,j} pertence à solução
//
//

#include <iostream>
#include <fstream>
#include "gurobi_c++.h"
using namespace std;

// Functions
void checkOpeningParameters(int argc);

int main(int argc, char *argv[]) {

    checkOpeningParameters(argc);

    char *graph = argv[1];
    int numberOfGroups = atoi(argv[2]);

    // DEBUG ---
    // cout << "graph: " << graph << "\n";
    // cout << "k: " << numberOfGroups << "\n";
    // --------

    // Extract graph file information
    char n, a, b;
    int employeeAmount, min, max;
    fstream file;
    file.open (graph);

    file >> n >> a >> b;
    file >> employeeAmount >> min >> max;

    // DEBUG ---
    // cout << n << " " << a << " " << b << "\n";
    // cout << employeeAmount << " " << min << " " << max << "\n";
    // --------

    int matrix[employeeAmount][employeeAmount];
    for (int i = 0; i < employeeAmount; i++) {
        for (int j = 0; j < employeeAmount; j++) {
            file >> matrix[i][j];
        }
    }

    file.close();

    // Use the graph information

    return 0;
}

void checkOpeningParameters(int argc) {
    if (argc < 3) {
        printf("Not enough parameters. Expected: ./lab <graph> <number of groups>\n");
        exit(1);
    }
}
