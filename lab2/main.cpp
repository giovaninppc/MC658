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

int nonExistentEdge = -1;
void checkOpeningParameters(int argc);
void checkNodeIfValid(int nextNode);
void printNodeInPath(Node node, double *sum);

int main(int argc, char *argv[]) {

    checkOpeningParameters(argc);
    char *graph = argv[1];

    // DEBUG -----------
    // cout << graph << endl;
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
    s -= 1;
    t -= 1;

    // DEBUG -----------
    // cout << dispose << endl;
    // cout << nnodes << "\t" << nedges << "\t" << type << "\t" << capacity << "\t" << P << "\t" << s << "\t" << t << endl;
    // -----------------

    // Read Node values
    Node nodes[nnodes];

    getline(file, dispose); // line break
    getline(file, dispose); // second line
    for (int i = 0; i < nnodes; i++) {
        file >> nodes[i].nodename >> nodes[i].posx >> nodes[i].posy >> nodes[i].itemValue >> nodes[i].itemWeight;
    }

    // Read Edges
    int edges[nnodes][nnodes];

    for (int i = 0; i < nnodes; i++) {
        for (int j = 0; j < nnodes; j++) {
            edges[i][j] = nonExistentEdge;
        }
    }

    getline(file, dispose); // line break
    getline(file, dispose); // second line
    for (int i = 0; i < nedges; i++) {

        int end1, end2, weight;
        file >> end1 >> end2 >> weight;

        edges[end1 - 1][end2 - 1] = weight;
    }

    // Create PLI Env
    GRBVar x[nnodes];          // Vertexes
    GRBVar f[nnodes][nnodes];  // Weight Accumulator
    GRBVar y[nnodes][nnodes];  // Edges

    try {
        GRBEnv env = GRBEnv(true);
        env.start();

        GRBModel model = GRBModel(env);

        // ------- Create GRB Variables
        // x \in {0,1} | y \in {0,1} | f \in Z”
        for (int i = 0; i < nnodes; i++) {
            x[i] = model.addVar(0.0, 1.0, 0.0, GRB_BINARY, "X-" + std::to_string(i));

            for (int j = 0; j < nnodes; j++) {
                y[i][j] = model.addVar(0.0, 1.0, 0.0, GRB_BINARY, "Y-" + std::to_string(i) + "-" + std::to_string(j));
                f[i][j] = model.addVar(0.0, INT_MAX, 0.0, GRB_INTEGER, "F-" + std::to_string(i) + "-" + std::to_string(j));
            }
        }

        // --------- Optimization Rule

        // Income value
        GRBQuadExpr objective = 0;
        for (int i = 0; i < nnodes; i++) {
            objective += x[i] * nodes[i].itemValue;                // Sum collected items
            for (int j = 0; j < nnodes; j++) {
                objective -= (f[i][j] + P * y[i][j]) * edges[i][j]; // Subtract edge transport cost
            }
        }

        model.setObjective(objective, GRB_MAXIMIZE);

        // ------ Constraints

        // S restrictions
        GRBLinExpr sOutEdge = 0;
        GRBLinExpr sInEdge = 0;
        for (int i = 0; i < nnodes; i++) {
            sOutEdge += y[s][i];
            sInEdge  += y[i][s];
        }
        model.addConstr(sOutEdge, GRB_EQUAL, 1, "Node s can only have 1 edge out");
        model.addConstr(sInEdge, GRB_EQUAL, 0, "Node s cannot have edges in");

        // // T restrictions
        GRBLinExpr tOutEdge = 0;
        GRBLinExpr tInEdge = 0;
        for (int i = 0; i < nnodes; i++) {
            tOutEdge += y[t][i];
            tInEdge  += y[i][t];
        }
        model.addConstr(tInEdge, GRB_EQUAL, 1, "Node t can only have 1 edge in");
        model.addConstr(tOutEdge, GRB_EQUAL, 0, "Node t cannot have edges out");

        // Generic edges restrictions
        for (int i = 0; i < nnodes; i++) {
            GRBLinExpr outEdges = 0;
            GRBLinExpr inEdges = 0;

            if (i == s || i == t) { continue; } // Rules already created

            for (int j = 0; j < nnodes; j++) {

                if (edges[i][j] == nonExistentEdge) {

                    // The edge does not exits - it has to be 0
                    GRBLinExpr edge = y[i][j];
                    model.addConstr(edge, GRB_EQUAL, 0, "Non existent (" + std::to_string(i) + "," + std::to_string(j) + ") edge");

                    continue;
                }

                outEdges += y[i][j];
                inEdges  += y[j][i];
            }

            model.addConstr(outEdges, GRB_LESS_EQUAL, 1,  "Node " + std::to_string(i) + " can have max 1 edge in");
            // model.addConstr(inEdges, GRB_LESS_EQUAL, 1,  "Node " + std::to_string(i) + " can have max 1 edge out"); -- redundant
            model.addConstr(inEdges, GRB_EQUAL, outEdges, "Node " + std::to_string(i) + " must have the same number of edges in and out");
        }

        // Weight restrictions - eh aqui q o filho chora e a mae nao ve

        // Each vertex cost
        for (int i = 0; i < nnodes; i++) {
            GRBLinExpr edgeWeightIn = 0;
            GRBLinExpr edgeWeightOut = 0;

            for (int j = 0; j < nnodes; j++) {
                edgeWeightOut += f[i][j];
                edgeWeightIn += f[j][i];
            }

            model.addConstr(edgeWeightOut, GRB_EQUAL, edgeWeightIn + x[i] * nodes[i].itemWeight,  "Node " + std::to_string(i) + " edge out weight");
        }

        // Truck capacity restriction
        GRBLinExpr itemsBeingCarried = 0;
        itemsBeingCarried += P;
        for (int i = 0; i < nnodes; i++) {
            itemsBeingCarried += x[i] * nodes[i].itemWeight;
        }
        model.addConstr(itemsBeingCarried, GRB_LESS_EQUAL, capacity, "Truck capacity");

        // Node value
        for (int i = 0; i < nnodes; i++) {

            if (i == s || i == t) { continue; }

            GRBLinExpr nodeValue = 0;
            for (int j = 0; j < nnodes; j++) {
                nodeValue += y[j][i];
            }

            model.addConstr(x[i], GRB_EQUAL, nodeValue,  "Node " + std::to_string(i) + " exists");
        }

        model.addConstr(x[s], GRB_EQUAL, 1,  "Node s");
        model.addConstr(x[t], GRB_EQUAL, 1,  "Node t");

        // -------- Optimize model
        model.optimize();

        // Print output
        // Optimized value
        cout << model.get(GRB_DoubleAttr_ObjVal) << endl;

        // PATH
        cout << "s" << endl;
        int currentNode = s;
        double sum = 0;
        while (currentNode != t) {
            int nextNode = nonExistentEdge;

            for (int i = 0; i < nnodes; i++) {
                if (y[currentNode][i].get(GRB_DoubleAttr_X) > 0) {
                    nextNode = i;
                    break;
                }
            }

            checkNodeIfValid(nextNode);
            printNodeInPath(nodes[nextNode], &sum);

            currentNode = nextNode;
        }
        cout << "t" << endl;

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

void checkNodeIfValid(int nextNode) {
    if (nextNode == nonExistentEdge) {
        cout << "Unable to find s -> t path" << endl;
        exit(2);
    }
}

void printNodeInPath(Node node, double *sum) {
    cout << node.nodename;

    *sum += node.itemValue;
    cout << " coletou";

    cout << " " << *sum << endl;
}
