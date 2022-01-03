#include<iostream>
#include<stdio.h>
#include <sstream>
#include <stdlib.h>

#include <ilcplex/ilocplex.h>


typedef IloArray<IloNumVarArray> IloNumVarMatrix;
typedef IloArray<IloNumVarMatrix> IloNumVar3Matrix;
typedef IloArray<IloNumVar3Matrix> IloNumVar4Matrix;

using namespace std;

int main(int argc, char* argv[])
{

    // Declarando conjuntos de entrada
    int A; // Ampolas 
    int S; // Dias Ramp-up
    int T; // Tempo
    // Dados de entrada dependentes dos conjuntos
    float* Custo; // custo por produto a
    //int** Demanda; // Demanda de a em periodo t
    float* IEstoque; // Estoque inicial do produto  a
    float* SEstoque; // Estoque de segurança
    //float** TRamp; // Taxa de ramp-up de a em dia s
    float* QuantidadeE; // Quantidade de extração de a 
    //float** Refulgo; // Refulgo do produto a em periodo t
    // Leitura do Arquivo
    
    FILE* fp;
    fopen_s(&fp,argv[1], "r");

    if (fp == NULL)
    {
        printf("Erro ao abrir o arquivo!\n");
        exit(1);
    }

    // Leitura dos conjuntos
    fscanf_s(fp, "%d", &A);
    fscanf_s(fp, "%d", &S);
    fscanf_s(fp, "%d", &T);


   

    // Definindo os dados de entrada de acordo com o tamanho dos conjuntos lidos
    Custo = new float[A];
    //Demanda = new int[A][T];
    float** Demanda = (float**)malloc(A * sizeof(float*));
    for (int a = 0; a < A; a++) {
        Demanda[a] = (float*)malloc(T*sizeof(float*));
    }
    IEstoque = new float[A];
    SEstoque = new float[A];
    //TRamp = new float[A][S];
    float** TRamp = (float**)malloc(A * sizeof(float*));
    for (int a = 0; a < A; a++) {
        TRamp[a] = (float*)malloc(S * sizeof(float*));
    }
    QuantidadeE = new float[A];
    //U = new int[A];
    //Refulgo = new float[A][T];
    float** Refulgo = (float**)malloc(A * sizeof(float*));
    for (int a = 0; a < A; a++) {
        Refulgo[a] = (float*)malloc(T * sizeof(float*));
    }
    

    

    // Após a declaração dos tamanhos dos dados de entrada, realizando a leitura

    printf( "Ampolas:%d\n", A);
    printf("Rampup:%d\n", S);
    printf("Tempo:%d\n", T);

    for (int a = 0; a < A; a++) {
        fscanf_s(fp, "%f", &Custo[a]);
    }

    for (int a = 0; a < A; a++) {
        for (int t = 0; t < T; t++) {
            fscanf_s(fp,"%f",&Demanda[a][t]);
          
        }
    }

    for (int a = 0; a < A; a++) {
        fscanf_s(fp, "%f", &SEstoque[a]);
    }

    for (int a = 0; a < A; a++) {
        fscanf_s(fp, "%f", &IEstoque[a]);
    }
    
    for (int a = 0; a < A; a++) {
        for (int s= 0; s < S; s++) {
            fscanf_s(fp, "%f", &TRamp[a][s]);
        }
    }

    for (int a = 0; a < A; a++) {
        for (int t = 0; t < T; t++) {
            fscanf_s(fp, "%f", &Refulgo[a][t]);
        }
    }

    for (int a = 0; a < A; a++) {
        fscanf_s(fp, "%f", &QuantidadeE[a]);
    }

    //fscanf_s(fp, "%d", &U[0]);

    
    // Impressão para Verificação dos dados
    printf("custo\n");
    for (int a = 0; a < A; a++) {
        printf("%f", Custo[a]);
        printf("\n");
    }
    printf("Demanda\n");
    for (int a = 0; a < A; a++) {
        for (int t = 0; t < T; t++) {
            printf("%f\t", Demanda[a][t]);

        }
        printf("\n");
    }
    printf("\n");
    printf("estoque segurança\n");
    for (int a = 0; a < A; a++) {
        printf("%f", SEstoque[a]);
        printf("\n");
    }
    printf("\n");
    printf("Inicial Estoque\n");
    for (int a = 0; a < A; a++) {
        printf("%f", IEstoque[a]);
        printf("\n");
    }
    printf("\n");
    printf("Ramp\n");
    for (int a = 0; a < A; a++) {
        for (int s = 0; s < S; s++) {
            printf("%f\t", TRamp[a][s]);
        }
        printf("\n");
    }
    printf("\n");
    printf("Refulgo\n");
    for (int a = 0; a < A; a++) {
        for (int t = 0; t < T; t++) {
            printf("%f\t", Refulgo[a][t]);
        }
        printf("\n");
    }
    printf("\n");
    printf("Estraida\n");
    for (int a = 0; a < A; a++) {
        printf("%f", QuantidadeE[a]);
        printf("\n");
    }
    printf("\n");
    //printf("U");
   // printf("%d", U[0]);
    printf("\n");
    printf("\n");


    // DECLARANDO O AMBIENTE E O MODELO MATEMATICO
    IloEnv env;
    IloModel modelo;
    // iniciando o modelo
    modelo = IloModel(env);


    IloNumVar U(env);
   
   
    // ---------------  DECLARAÇÃO DE VARIÁVEIS DE DUAS DIMENSÕES
    
    IloNumVarMatrix Estoque(env, A); // Estoque
    for (int a = 0; a < A; a++)
    {
        Estoque[a] = IloNumVarArray(env, T, 0, IloInfinity, ILOFLOAT);
    }
    // adicionar as variáveis no modelo
    for (int a = 0; a < A; a++)
    {
        for (int t = 0; t < T; t++)
        {
            stringstream var;
            var << "Estoque[" << a << "][" << t << "]";
            Estoque[a][t].setName(var.str().c_str());
            modelo.add(Estoque[a][t]);
        }
    }


   


    

    IloNumVarMatrix ProdL(env, A); //Produção Liquida
    for (int a = 0; a < A; a++)
    {
        ProdL[a] = IloNumVarArray(env, T, 0, IloInfinity, ILOFLOAT);
    }
    // adicionar as variáveis no modelo
    for (int a = 0; a < A; a++)
    {
        for (int t = 0; t < T; t++)
        {
            stringstream var;
            var << "ProdL[" << a << "][" << t << "]";
            ProdL[a][t].setName(var.str().c_str());
            modelo.add(ProdL[a][t]);
        }
    }

    IloNumVarMatrix ProdT(env, A); //Produção total
    for (int a = 0; a < A; a++)
    {
        ProdT[a] = IloNumVarArray(env, T, 0, IloInfinity, ILOFLOAT);
    }
    // adicionar as variáveis no modelo
    for (int a = 0; a < A; a++)
    {
        for (int t = 0; t < T; t++)
        {
            stringstream var;
            var << "ProdT[" << a << "][" << t << "]";
            ProdT[a][t].setName(var.str().c_str());
            modelo.add(ProdT[a][t]);
        }
    }

    IloNumVarMatrix ProdOn(env, A); //Produção On ou OFF
    for (int a = 0; a < A; a++)
    {
        ProdOn[a] = IloNumVarArray(env, T, 0, 1, ILOBOOL);
    }
    // adicionar as variáveis no modelo
    for (int a = 0; a < A; a++)
    {
        for (int t = 0; t < T; t++)
        {
            stringstream var;
            var << "ProdOn[" << a << "][" << t << "]";
            ProdOn[a][t].setName(var.str().c_str());
            modelo.add(ProdOn[a][t]);
        }
    }


    IloNumVarMatrix PP(env, A); // Produção de algum produto on ou off
    for (int a = 0; a < A; a++)
    {
        PP[a] = IloNumVarArray(env, T, 0, 1, ILOBOOL);
    }
    // adicionar as variáveis no modelo
    for (int a = 0; a < A; a++)
    {
        for (int t = 0; t < T; t++)
        {
            stringstream var;
            var << "PP[" << a << "][" << t << "]";
            PP[a][t].setName(var.str().c_str());
            modelo.add(PP[a][t]);
        }
    }
    // ---------------  DECLARAÇÃO DE VARIÁVEIS DE TRES DIMENSÕES

     IloNumVar3Matrix Linear(env, A); //Variavel auxiliar de Lineartização
    for (int a = 0; a < A; a++)
    {
        Linear[a] = IloNumVarMatrix(env, T);
        for (int t = 0; t < T; t++) {
            Linear[a][t] = IloNumVarArray(env, S, 0, IloInfinity, ILOFLOAT);
        }
    }
    // adicionar as variáveis no modelo
    for (int a = 0; a < A; a++)
    {
        for (int t = 0; t < T; t++)
        {
            for (int s = 0; s < S; s++) {
                stringstream var;
                var << "Linear[" << a << "][" << t << "][" << s << "]";
                Linear[a][t][s].setName(var.str().c_str());
                modelo.add(Linear[a][t][s]);
            }
        }
    }

    IloNumVar3Matrix Set(env, A); // Set-up
    for (int a = 0; a < A; a++)
    {
        Set[a] = IloNumVarMatrix(env, T);
        for (int t = 0; t < T; t++) {
            Set[a][t] = IloNumVarArray(env, S, 0, 1, ILOBOOL);
        }
    }
    // adicionar as variáveis no modelo
    for (int a = 0; a < A; a++)
    {
        for (int t = 0; t < T; t++)
        {
            for (int s = 0; s < S; s++) {
                stringstream var;
                var << "Set[" << a << "][" << t << "]["<< s << "]";
                Set[a][t][s].setName(var.str().c_str());
                modelo.add(Set[a][t][s]);
            }
        }
    }
 


    // DECLARAÇÃO DA FUNÇÃO OBJETIVO
    //(1)
    IloExpr fo(env);
    IloExpr obj(env);
    IloNum x;
    x = 1;
    for (int a = 0; a < A; a++) {
        for (int t = 0; t < T; t++) {
            for (int s= 0; s < S; s++) {
                obj += Set[a][t][s] * QuantidadeE[a] * (x - TRamp[a][s]) * Custo[a];
                fo += 0.001 * U * SEstoque[a] * Custo[a];
            }
        }
    }
    fo = fo + obj;
    //IloMinimize e IloMaximize
    modelo.add(IloMinimize(env, fo));


    // DECLARAÇÃO DAS RESTRIÇÕES DO PROBLEMA
    // (2)&(3)Restrição Balanceamento de estoque
    if (T > 1) {
        for (int a = 1; a < A; a++) {
            for (int t = 1; t < T; t++) {
                IloExpr r2(env);
                r2 = ProdL[a][t] - Demanda[a][t] + Estoque[a][t-x];
                IloConstraint Balanc = Estoque[a][t] == r2;
                stringstream var;
                var << "Balanceamento[" << a << "][" << t << "]";
                Balanc.setName(var.str().c_str());
                modelo.add( Balanc );
            }
        }
    }
    //(3)
    else if (T == 1) {
        for (int a = 0; a < A; a++) {
            for (int t = 0; t < T; t++) {
                IloExpr r3(env);
                r3 = ProdL[a][t] - Demanda[a][t] + IEstoque[a];
                IloConstraint Balanc1 = Estoque[a][t] == r3;
                stringstream var;
                var << "Balanceamento[" << a << "][" << t << "]";
                Balanc1.setName(var.str().c_str());
                modelo.add(Balanc1);
            }
        }
    }
    // (4)Restrição equilibrio de estoque de segurança
    for (int a = 0; a < A; a++ ) {
        for (int t = 0; t < T; t++) {
            IloExpr r4(env);
            r4 = Estoque[a][t] / SEstoque[a];
            IloConstraint Equi = U >= r4;
            stringstream var;
            var << "Equilibrio[" << a << "][" << t << "]";
            Equi.setName(var.str().c_str());
            modelo.add(Equi);
            
        }
    }
    //(5)(6)(7)(8) Linearização
    //(5)
    for (int a = 0; a < A; a++) {
        for (int t = 0; t < T; t++) {
            for (int s = 0; s < S ; s++) {
                IloExpr r5(env);
                r5 = ProdT[a][t] - 1000000 * (x - Set[a][t][s]);
                IloConstraint Linear1 = Linear[a][t][s] >= r5;
                stringstream var;
                var << "Linear1[" << a << "][" << t << "][" << s << "]";
                Linear1.setName(var.str().c_str());
                modelo.add(Linear1);
            }
        }
    }
    //(6)
    for (int a = 0; a < A; a++) {
        for (int t = 0; t < T; t++) {
            for (int s = 0; s < S; s++) {
                IloExpr r6(env);
                r6 = ProdT[a][t] + 1000000 * (1 - Set[a][t][s]);
                IloConstraint Linear2 = Linear[a][t][s] <= r6;
                stringstream var;
                var << "Linear2[" << a << "][" << t << "][" << s << "]";
                Linear2.setName(var.str().c_str());
                modelo.add(Linear2);
            }
        }
    }
    //(7)
    for (int a = 0; a < A; a++) {
        for (int t = 0; t < T; t++) {
            for (int s = 0; s < S; s++) {
                IloExpr r7(env);
                r7 = 1000000 * Set[a][t][s];
                IloConstraint Linear3 = Linear[a][t][s] <= r7;
                stringstream var;
                var << "Linear3[" << a << "][" << t << "][" << s << "]";
                Linear3.setName(var.str().c_str());
                modelo.add(Linear3);
            }
        }
    }
    //(8)
    for (int a = 0; a < A; a++) {
        for (int t = 0; t < T; t++) {
            for (int s = 0; s < S; s++) {
                IloExpr r8(env);
                r8 = (-Set[a][t][s]) * 1000000 ;
                stringstream var;
                IloConstraint Linear4 = Linear[a][t][s] >= r8;
                var << "Linear4[" << a << "][" << t << "]["<< s << "]";
                Linear4.setName(var.str().c_str());
                modelo.add(Linear4);
            }
        }
    }
    //(9)Restrição Produção Liquida
    for (int a = 0; a < A; a++) {
        for (int t = 0; t < T; t++) {
            IloExpr PD(env);
            IloExpr PD1(env);
           // IloExpr PD2(env);
            for (int s = 0; s < S; s++) {
                PD1 += Linear[a][t][s] * TRamp[a][s];
                PD += (ProdT[a][t] - Linear[a][t][s]) * (x - Refulgo[a][t]);
                PD = PD1 + PD;
                IloConstraint ProdLi = PD == ProdL[a][t];
                stringstream var;
                var << "ProdLiquida[" << a << "][" << t << "][" << s << "]";
                ProdLi.setName(var.str().c_str());
                modelo.add(ProdLi);
            }
        }
    }
    
    //(10)Restrição Produto Total
    for (int a = 0; a < A; a++) {
        for (int t = 0; t < T; t++) {
           IloExpr r10(env);
           r10 = QuantidadeE[a] * PP[a][t];
           IloConstraint ProdTt = r10 == ProdT[a][t];
           stringstream var;
           var << "ProdTotal[" << a << "][" << t << "]";
           ProdTt.setName(var.str().c_str());
            modelo.add(ProdTt);
        }
    }
    //(11)Restrição um produto por maquina
    for (int t = 0; t < T; t++) {
        IloExpr r11(env);
        for (int a = 0; a < A; a++) {
            r11 += PP[a][t];
            IloConstraint Prod1 = r11 == 1;
            stringstream var;
            var << "UmProduto[" << a << "][" << t << "]";
            Prod1.setName(var.str().c_str());
            modelo.add(Prod1);
        }
    }
    //(12)&(13) Restrição set-up
    if (T > 1) {
        for (int a = 1; a < A; a++) {
            for (int t = 1; t < T; t++) {
                IloExpr r12(env);
                r12 = PP[a][t] - PP[a][t-x];
                IloConstraint SetU = r12 <= ProdOn[a][t];
                stringstream var;
                var << "SetUp2[" << a << "][" << t << "]";
                SetU.setName(var.str().c_str());
                modelo.add(SetU);
            }
        }
    }
    //(13)
    else if (T == 1) {
        for (int a = 0; a < A; a++) {
            for (int t = 0; t < T; t++) {
                IloExpr r13(env);
                r13 = PP[a][t];
                IloConstraint SetU1 = r13 == ProdOn[a][t];
                stringstream var;
                var << "SetUp1[" << a << "][" << t << "]";
                SetU1.setName(var.str().c_str());
                modelo.add(SetU1);
            }
        }

    }


    //(14) Ramp-up
    for (int a = 0; a < A; a++) {
        for (int s = 0; s < S; s++) {
            for (int t = 0; t < T - s; t++) {
                IloExpr r14(env);
                IloExpr r15(env);
                r14 = Set[a][t+s][s];
                IloConstraint Ramp= PP[a][t] == r14;
                stringstream var;
                var << "RampUp[" << a << "][" << t << "]";
                Ramp.setName(var.str().c_str());
                modelo.add(Ramp);
            }
        }
    }

    
    // IloRange parametros: ambiente, valor min, expressão, valor maximo


    // RESOLVENDO O MODELO

    // Carregando o módulo do Cplex
    IloCplex cplex(modelo);
    // exportando o lp
    cplex.exportModel("Ampola.lp");
    // Executando o modelo
    cplex.solve();
    if (cplex.getCplexStatus() == IloCplex::Optimal)
    {
        cout << "Ótimo encontrado! " << endl;
    }
    else
    {
        cout << "Nao foi dessa vez =(" << endl;
    }

    // PEGAR OS VALORES DAS VARIÁVEIS 
    //cplex.getValue(NOME_VAR)    


    return 0;
}





