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
#include "/Library/gurobi811/mac64/include/gurobi_c++.h" // GUROBI install path
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

    // Create PLI Program
    GRBVar x[employeeAmount][employeeAmount][numberOfGroups];
    GRBVar y[employeeAmount][employeeAmount];

    try {
        GRBEnv env = GRBEnv(true);
        env.set("LogFile", "mip1.log");
        env.start();

        GRBModel model = GRBModel(env);

        // Create GRB variables
        for (int i  = 0; i < employeeAmount; i++) {
            for (int j = 0; j < employeeAmount; j++) {

                for (int k = 0; k < numberOfGroups; k++) {
                    x[i][j][k] = model.addVar(0.0, 1.0, 0.0, GRB_BINARY, "x-" + std::to_string(i) + "-" + std::to_string(j) + "-" + std::to_string(k));
                }

                y[i][j] = model.addVar(0.0, 1.0, 0.0, GRB_BINARY, "y-" + std::to_string(i) + "-" + std::to_string(j));
            }
        }

        // Set objective
        GRBLinExpr objective = 0;
        int counter = 0;
        for (int i  = 0; i < employeeAmount; i++) {
            for (int j = 0; j < employeeAmount; j++) {
                objective += y[i][j] * matrix[i][j];
            }
        }
        model.setObjective(objective, GRB_MAXIMIZE);

        // Add constraints
        for (int k = 0; k < numberOfGroups; k++) {
            GRBLinExpr limitExpression = 0;
            counter = 0;

            for (int i  = 0; i < employeeAmount; i++) {
                for (int j = 0; j < employeeAmount; j++) {
                    limitExpression += x[i][j][k];
                }
            }

            model.addConstr(limitExpression, GRB_GREATER_EQUAL, min, "minimum group size for group " + std::to_string(k));
            model.addConstr(limitExpression, GRB_LESS_EQUAL,    max, "maximum group size for group " + std::to_string(k));
        }

        for (int i = 0; i < employeeAmount; i++) {
            GRBLinExpr groupBelong = 0;
            for (int k = 0; k < numberOfGroups; k++) {
                groupBelong += x[i][i][k];
            }

            model.addConstr(groupBelong, GRB_EQUAL, 1, "Person " + std::to_string(i) + " is in exactly 1 group");
        }

        for (int i = 0; i < employeeAmount; i++) {
            for (int j = 0; j < employeeAmount; j++) {
                for (int k = 0; k < numberOfGroups; k++) {
                    model.addConstr(y[i][j] <= (x[i][i][k] + x[j][j][k]    )/2,  "if " + std::to_string(i) + " and " + std::to_string(j) + "are in the same group " + std::to_string(k));
                    model.addConstr(y[i][j] >= (x[i][i][k] + x[j][j][k] - 1)/2, "2if " + std::to_string(i) + " and " + std::to_string(j) + "are in the same group " + std::to_string(k));
                }
            }
        }

        // Optimize model
        model.optimize();

        // Print output
        cout << model.get(GRB_DoubleAttr_ObjVal) << endl;

        for (int i = 0; i < employeeAmount; i++) {
            int k = 0;

            for (k = 0; k < numberOfGroups; k++) {
                if (x[i][i][k].get(GRB_DoubleAttr_X) > 0) { break; }
            }

            cout << "v" << i << " " << k << endl;
        }

    } catch (exception e) {
        cout << "Error during optimization" << endl;
    }

    return 0;
}

void checkOpeningParameters(int argc) {
    if (argc < 3) {
        cout << "Not enough parameters. Expected: ./lab <graph> <number of groups>" << endl;
        exit(1);
    }
}
