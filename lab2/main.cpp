// Giovani Nascimento Pereira
// RA168609
// Lab1 MC658 2S2019 - FKM

#include <iostream>
#include <fstream>
#include "gurobi_c++.h"
using namespace std;

struct Node {
    int nodename, posx, posy, itemValue, itemWeight;
};

struct Edge {
    int end1, end2, weight;
};

void checkOpeningParameters(int argc);

int main(int argc, char *argv[]) {

    checkOpeningParameters(argc);
    char *graph = argv[1];

    // DEBUG -----------
    cout << graph << endl;
    // -----------------

    // Read instance information
    string dispose;
    int nnodes, nedges, capacity, P, s, t;
    string type;

    fstream file;
    file.open (graph);

    getline(file, dispose);
    file >> nnodes >> nedges;
    while (file >> type) { break; }
    file >> capacity >> P >> s >> t;

    nedges = (nedges * 2);

    // DEBUG -----------
    cout << dispose << endl;
    cout << nnodes << "\t" << nedges << "\t" << type << "\t" << capacity << "\t" << P << "\t" << s << "\t" << t << endl;
    // -----------------

    // Read Node values
    Node nodes[nnodes];

    getline(file, dispose); // line break
    getline(file, dispose); // second line
    for (int i = 0; i < nnodes; i++) {
        file >> nodes[i].nodename >> nodes[i].posx >> nodes[i].posy >> nodes[i].itemValue >> nodes[i].itemWeight;
    }

    // Read Edges
    Edge edges[nedges];

    getline(file, dispose); // line break
    getline(file, dispose); // second line
    for (int i = 0; i < nedges; i++) {
        file >> edges[i].end1 >> edges[i].end2 >> edges[i].weight;
    }

    // Create GUROBI Env
    // GRBVar x[employeeAmount][employeeAmount][numberOfGroups];

    try {
        GRBEnv env = GRBEnv(true);
        env.start();

        GRBModel model = GRBModel(env);

    } catch (exception e) {
        cout << "Error during optimization: " << endl;
    }

    return 0;
}

void checkOpeningParameters(int argc) {
    if (argc < 2) {
        cout << "Not enough parameters. Expected: ./lab <graph>" << endl;
        exit(1);
    }
}
