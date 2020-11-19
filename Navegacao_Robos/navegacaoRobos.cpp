
/* Navegação de Robôs - Trabalho 2 - IAR
    Dupla: Beatriz e Geremias
    
    Nele temos a implementação da busca BFS, de custo uniforme e A* (a última ainda falta)

    Realizamos ele por matriz adjacência, o que não prejudicou seus tempos para o dado ambiente de entrada

    A forma de execução é padrão C++, com dois parâmetros no cmd, vértice origem e fim da busca:
    $ g++ -Wall -O3 -o exec navegacaoRobos.cpp
    $ ./exec verticeOrigem verticeFim

    É RECOMENDADO DESCOMENTAR NO MAIN QUAL VOCÊ DESEJA EXECUTAR (APENAS 1 DELES) PARA OS RESULTADOS NÃO FICAREM CONFUSOS!
*/


/* O QUE FAZER - pode ir anotando o que já acha que precisaremos fazer - Cronológico (?):

    Implementação da função pro A* (com distancia euclidiana) - 

    Outros detalhes de dúvidas pro relatório podem ser vistos aqui: https://docs.google.com/document/d/1_eOm9wp0Lvr1vziPJPXfLiyYyfOJh48wYbwf6zp5t50/edit 
*/

/* O QUE FIZEMOS:
    Já no BFS{
        Algoritmos de BFS inicial posto na plataforma (ainda não adaptado)
        Matriz de leitura do ambiente pronta. Assim com o print dela.
        Retirada a quantidade de arestas do BFS (não era útil)
        Transmitimos a matriz ambiente para a adjacente
            Ela tá printando certo os valores, mas nossa ideia tá limitada, pois as ligações são direcionadas acho. Se não fica incoerente os custos.
            Inicia tudo em 0 agora (pois o valor real dos custos já está adaptado)
            Tinha um erro, agora tudo certo usando (i*42)+j nessa transmissão.
        Adjacencias agora estão biderecionadas (parece ser o correto) - custo de A p/ B != B p/ A, mas se um existe, o outro tbm
        O valor real dos custos está correto (a partir da legenda passada)
        Calculamos o caminho e seu custo, através de um vetor de pais.
        FUnciona independente da origem e destino agora a busca certinha (não tava, agora tá).
        Encerra quando acha destino, conta os nós expandidos e quantidade de vértices do caminho tbm.
    }
    
    No de custo uniforme{
        Adicionamos uma função para execução do dijkstra
        Encerra quando acha destino, conta os nós expandidos e quantidade de vértices do caminho tbm.
        Funfando tudo certo.

    }

    No de A*{
        Tentamos, mas ainda sem sucesso.
        O A*original ali da forma atual gera resultado daquele lá de antes (que é igual ao dijktra), mesmo com as listas. visando.
    }

    Realizo a expansão e de forma bonitinha e interativa até, tá razoável
    Problema é a tremedeira no terminal a cada iteração :/
    Pra saída dos resultados no relatório ele disse que tá ok a da expansão em vermelho com o caminho final
*/


#include <bits/stdc++.h> 
#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <unistd.h>

using namespace std; 
  
#define tamMat 42
int matriz[tamMat][tamMat];
int matrizExpansao[tamMat][tamMat];
int matrizExpansaoBFS[tamMat][tamMat];
int matrizExpansaoDijkstra[tamMat][tamMat];
//matriz adj e dos custos é [tamMat*tamMat][tamMat*tamMat];

//Função para ler o ambiente.txt e atribuir a uma matriz equivalente para uso
void leituraAmbiente(){
    FILE *arq;
    char c;
    int i = 0, j = 0;
    int auxx;
    arq = fopen("ambiente.txt", "r");

    if(arq == NULL){
        printf("Erro na abertura do arquivo!");
        exit(1);
    }else{
        while(!feof(arq)){
            fflush(stdin);
            c = getc(arq);//ler por caracter é mais facil acho
           // printf("c = %c\n", c);
            if(c != ' ' && c != '\n'){
                //Legenda do ambiente e valores para matriz adj:  0 = 1 // 1 = 5 // 2 = 10 // 3 = 15 //não adaptado ainda!
                auxx = c - '0'; //convertendo char pra int, gambiarra :)
                if(auxx == 0)
                    matriz[i][j] = 1;
                else if(auxx == 1)
                    matriz[i][j] = 5;
                else if(auxx == 2)
                    matriz[i][j] = 10;
                else if(auxx == 3)
                    matriz[i][j] = 15;
                j++; //contando as colunas
            }
            if(c == '\n'){
                i++; //contando as linhas
                j = 0; //zerando as colunas
            }

        }
    }
    fclose(arq);
}

//Função auxiliar para imprimir a matriz ambiente
void imprimirAmbiente(){
    int i,j;
    cout << "IMPRESSÃO DO AMBIENTE COM SEUS PESOS:" << endl;
    for(i = 0; i< tamMat; i++){
        for(j=0;j< tamMat;j++){
            if(matriz[i][j]>=10)
                printf("%d ", matriz[i][j]);
            else
                printf("%d  ", matriz[i][j]);
        }
        printf("\n");
    }
    cout << endl;
}

//Função para algoritmo A*: pode usar euclidiana, aqui foi usada a do taxi, porque
//ela nao considera as diagonais
int h(int atual, int destino){
    //ponto P1 com (x1,y1)
    int posInversaXAtual = floor(atual/42); 
    int posInversaYAtual = atual%42; 
    //ponto P2 com (x2,y2)
    int posInversaXDestino = floor(destino/42);
    int posInversaYDestino = destino%42;

    //printf("atual = %d destino = %d x1 = %d y1 = %d x2 = %d y2 = %d\n",atual, destino, posInversaXAtual, posInversaYAtual, posInversaXDestino, posInversaYDestino );
    
    //|x1 - x2| + |y1 - y2|. 
    return abs(posInversaXAtual - posInversaXDestino) + abs(posInversaYAtual - posInversaYDestino);
     
}

int distEuclidiana (int atual, int destino){
    //distancia euclidiana considera as diagonais, nesse caso o robo nao anda nas diagonais,
    //entao eu não recomendaria usar ela

        //ponto P1 com (x1,y1)
    int posInversaXAtual = floor(atual/42); 
    int posInversaYAtual = atual%42; 
    //ponto P2 com (x2,y2)
    int posInversaXDestino = floor(destino/42);
    int posInversaYDestino = destino%42;

    return sqrt(pow(abs(posInversaXAtual - posInversaXDestino), 2) + pow (abs(posInversaYAtual - posInversaYDestino), 2));
}

class Graph { 
    int v; //Número de nós
    int** adj; //Matriz adj
    int pais[1764];//matriz dos nós pais, pega a posição do matriz adj do nó que é o pai desse.

  
public:  
    //Ordenado as funções e construtores abaixo nessa mesma ordem:
    Graph(int v); //Criando a matriz adj 
    void addAresta(int v1, int v2); //Função para inserir um nó
    void BFS(int inicio, int destino); //Função de busca em largura
    void dijkstra(int inicio, int destino);
    void aEstrelaOriginal(int inicio, int destino);
    void aEstrela(int inicio, int destino);
    void aEstrelaFunfa(int inicio, int destino);
    void buscaCaminho(int origem, int destino);
    void printExpansao();
    void caminhoEscolhidoAmbiente(int origem, int destino);
    void imprimirAmbienteComCaminho(int origem, int destino);
    void imprimirAmbienteComExpansaoCaminho(int origem, int destino);
    void printAdj();
    void printMatrizAdjAmbiente(); //Ele tem o ambiente mas com as posições da matriz adjacente
    void printPais();
}; 

//Zerando a matriz adj, iniciando ela vazia
Graph::Graph(int v){ 
    this->v = v; 
    adj = new int*[v]; 
    for (int linha = 0; linha < v; linha++) { 
        pais[linha] = -1; //todos os pais são setados com -1 inicialmente 
        //Sim, tinha pensado como matriz, mas como vetor já basta, ele armazena o nó pai só, vai uma posição pra cada linha (nó).
        //Obs: no caso pega a row do pai, a parte da linha do adj[][] ok
        //bele
        adj[linha] = new int[v]; 
        for (int coluna = 0; coluna < v; coluna++) { 
            adj[linha][coluna] = 0; //melhor não, pq aí no uso do bfs dá pau sla, caso vamos usar no mesmo (precisaria arrumar o bfs só). Se precisar dps vemos
            //pais[row][column] = -1; //todos os pais são setados com -1 inicialmente. -não precisa ser matriz, pode ver vetor.
        } 
    } 
} 


  
// Função que adiciona a ligação entre dois nós (com um peso) no grafo: 
void Graph::addAresta(int i, int j){  //coordenadas da matriz ambiente.

    //Ligação direcionada dos vértices:
    //Isso pq [0][0] = 0 e [0][1]=2, o custo para ir de 00 a 01 é 2, mas para ir de [1][0] é 0! 
        //Essa lógica faz mais sentido, pois se não estaremos aplicando o custo de forma meio errada.
    //Ambiente não esférico!
    //Legenda do ambiente e valores para matriz adj:  0 = 1 // 1 = 5 // 2 = 10 // 3 = 15

    //Para v inicial genérico: 
    //Aqui com o for faço a verificação de todos os casos que extrapolam o limite a direita ou esquerda da matriz pela coluna

    //ir para cima - norte - OK
    if(i != 0){//se não é primeira linha 
        adj[(i*42)+j][(i*42)+j-42] = matriz[i-1][j]; //de A para B, o custo é B
    }

    //ir para direita - leste - OK
    if(j != 41){ //verificando se pode ir pra direita -- TÁ ERRADO, ARRUMAR ISSO AQUI
        adj[(i*42)+j][(i*42)+j+1] = matriz[i][j+1]; //de A para B, o custo é B
    }


    //ir para baixo - sul - OK
    if(i != 41){ //se não é limite na matriz ambiente e se existe vértice 42 posições a frente para ser adjacente. "v" é a qtd vertices
        adj[(i*42)+j][(i*42)+j+42] = matriz[i+1][j]; //de A para B, o custo é B
    }

    //Ir para esquerda - oeste. - OK
    if(j != 0){ //não está no limite de coluna esquerdo da matriz? Então posso ligar.
        adj[(i*42)+j][(i*42)+j-1] = matriz[i][j-1]; //de A para B, o custo é B
    }

    //Explicações do pq +1 e +42: dado a matriz ambiente, todo vizinho dela, na matriz adjacencia, estará uma posição a frente (caso à direita) ou 42 posições a frente (caso abaixo)
        //Além disso, só efetuamos as ligações para direita e para baixo, para não ficar repetindo arestas criadas.
        //Da atual forma acredita-se que satisfaça a ideia.
} 

// Função para realizar a busca em largura no grafo
void Graph::BFS(int inicio, int destino){ 
    vector<bool> visitados(v, false); 
    vector<int> filaProcessados; 
    vector<int> paisAux;
    filaProcessados.push_back(inicio); 

    int qtdNosExpandidos = 0;

    //Zerando matrix expansão. 0 igual a não expandido, 1 igual a expandido.
    for(int i =0;i<tamMat;i++){
        for(int j=0;j<tamMat; j++){
            matrizExpansaoBFS[i][j] = 0;
            matrizExpansao[i][j] = 0;
        }
    }
    
  
    // Atribui o vértice origem como visitado
    visitados[inicio] = true;

    //zero os pais, para não ter problema
    for(int i=0;i<v;i++){
        pais[i] = -1;
    }

    //Marcando o pai:
    pais[inicio] = -1; //já que não tem, fica como -1.

    //Obs: isso aqui é só pro nó início, precisa fazer esquema parecido no while das visitações.

    //essa matriz de expansao é pra printar todos os nos visitados dai
    //Isso, mas to com um probleminha, pq passamos as posições da matriz adj, mas nesse caso ele precisa receber valores de 0 a 42, se não dá seg fault
    //Mas dá pra adaptar a posição da adjacencia pra ele fazendo a fórmula do (i*42) + j de forma contrária (voltando)
    //entendi, vamos tentar a formula contraria entao   
    int visitadoAtual = filaProcessados[0]; //nó visitado atual
    //cout << "Expansão do BFS" << endl;
    while (!filaProcessados.empty() && visitadoAtual != destino) { //QUANDO ENCONTRA O DESTINO ELE PARA O LAÇO!

        visitadoAtual = filaProcessados[0]; //nó visitado atual.
        //cout << visitadoAtual << "->"; // printa o nó atual visitado.
        filaProcessados.erase(filaProcessados.begin()); //apago o nó inicial da lista de a ser visitados.

        //Print da expansão pelo BFS:
        int posInversaX = floor(visitadoAtual/42); //tem que pegar o inteiro de baixo. se tá na 34 será linha 0, se tá na 52 será linha 1.
        int posInversaY = visitadoAtual%42; // resto da divisão do adj[i][j] atual por 42.
        matrizExpansao[posInversaX][posInversaY] = 1;
        //usleep(500); //4000 tá bom, 300000 para mostrar bem lentamente o processo
        //cout << "Expansao BFS:" << endl;
        //printExpansao(); //cada novo nó printo a expansão;
        qtdNosExpandidos++; 

        int adjacentes;
        // Para cada nó adjacente(i) não visitado  do nó atual - lembrando do formato horário pedido.
        for (int i = 0; i < 4; i++) { //máximo de vizinhos é 4 mesmo! Assim obrigo a pegar a ordem Norte->leste->sul->oeste.
            //obs: se fosse geral e sem essa ordem bastava fazer um for de 0 ao numero max de vértices (testaria todos)
            if(i==0 && posInversaX != 0)
                adjacentes = visitadoAtual-42; //pega o norte primeiro
            else if(i == 1 && posInversaY != 41)
                adjacentes = visitadoAtual + 1; //pega o da direita por segundo
            else if(i == 2 && posInversaX != 41)
                adjacentes = visitadoAtual + 42; //pega o de baixo por terceiro
            else if(i == 3 && posInversaY != 0)
                adjacentes = visitadoAtual - 1; //pega o da esquerda por ultimo

            if (adj[visitadoAtual][adjacentes] > 0 && (!visitados[adjacentes])) { 
                //Antes era adj[vis][i] == 1, mas agora temos qlqr valor > -1 significa ligação! ok
                filaProcessados.push_back(adjacentes); //manda pro final, de forma inversa ao dijkstra, que manda pro começo, por isso fica em profundidade.
                pais[adjacentes] = visitadoAtual; //PENSEI ASSIM, ESSA LINHA
                //O pai do nó i (adjacente ao nó atual, que é o vis), é marcado como pai dele, se tudo estiver certo.
                visitados[adjacentes] = true; 
                //matrizExpansao[visitadoAtual][i] = 1;
                //printExpansao();
            } 
        } 
    } 
    //printAdj();
    cout << endl << "----------------------------------- SAÍDA DA EXECUÇÃO ------------------------------" << endl;
    cout << "Quantidade de nós expandidos pelo BFS: " << qtdNosExpandidos << endl;
    cout << endl << "Caminho do vértice origem "<<inicio<<" ao destino "<<destino<<" via BFS:";
    buscaCaminho(inicio, destino);
    

} 

//Função dijkstra/custo uniforme
void Graph::dijkstra(int inicio, int destino){
    //Obs: lembrando que v = tamMat*tamMat
	bool visitados[v]; //booleano pros visitados
	int menorcaminho[v];//vetor dos menores caminhos
	int contador=0; //contador de iterações
	int aux; //aux é auxiliar pra guardar menor caminho
	int proximo = -1; //proxima aresta (para trabalhar com o pai)

    int posInversaX; //tem que pegar o inteiro de baixo. se tá na 34 será linha 0, se tá na 52 será linha 1.
    int posInversaY; // resto da divisão do adj[i][j] atual por 42.
    int qtdNosExpandidos=0;

    //zero os pais, para não ter problema
    for(int i=0;i<v;i++){
        pais[i] = -1;
    }

    for(int i=0;i<tamMat;i++){
        for(int j=0;j<tamMat;j++){
            matrizExpansao[i][j]=0;
        }
    }

    //Arrumando os pesos 0 para um valor mto mto alto
    for(int i=0; i<v; i++){
        for(int j=0; j<v; j++){
            if(adj[i][j] == 0)
                adj[i][j] = 1000000;
        }
    }
	
	for(int i=0;i<v;i++){
		pais[i]=inicio;
        if(i!=inicio){
			visitados[i]=false;
			menorcaminho[i] = adj[inicio][i]; 
		}
		else{
			visitados[i]=true;
			menorcaminho[i]=0;

            posInversaX = floor(inicio/42); //tem que pegar o inteiro de baixo. se tá na 34 será linha 0, se tá na 52 será linha 1.
            posInversaY = inicio%42; // resto da divisão do adj[i][j] atual por 42.
            matrizExpansao[posInversaX][posInversaY] = 1;
            //usleep(500); //4000 tá bom, 2000 fica meio rapido
            //cout << "Expansao custo uniforme:" << endl;
            //printExpansao(); //cada novo nó printo a expansão;
		}
	}
	contador++;
    //cout << "Expansão do Custo Uniforme" << endl;
	//Algoritmo
	while(v-1 > contador && proximo != destino){ //Percorre enquanto não buscou todas posições ou não achou o nó destino
		aux=1000000;

		for(int i=0; i<v; i++){
			if(visitados[i]==false && menorcaminho[i]<aux){
				proximo=i;
				aux=menorcaminho[i];
			}
		}

		visitados[proximo]=true;
        
        //Printa da expansão pelo custo uniforme/Dijkstra:
        posInversaX = floor(proximo/42); //tem que pegar o inteiro de baixo. se tá na 34 será linha 0, se tá na 52 será linha 1.
        posInversaY = proximo%42; // resto da divisão do adj[i][j] atual por 42.
        matrizExpansao[posInversaX][posInversaY] = 1;
        //usleep(4000); //4000 tá bom, ,as meio rapido. 20k fica bom tbm
        //cout << "Expansao custo uniforme:" << endl;
        //printExpansao(); //cada novo nó printo a expansão;
        qtdNosExpandidos++;

        for(int i=0;i<v;i++){
			if(visitados[i]==false){
				if(aux+adj[proximo][i] < menorcaminho[i]){
					pais[i]=proximo;
					menorcaminho[i]=aux+adj[proximo][i];
				}
			}
		}

        /* TENTATIVA GARANTINDO SENTIDO HORÁRIO! DÁ RUIM PRA ALGUNS CASOS
        bool canTry = false; //para saber se tem um adjacente válido na iteração e pode tentar
        int adjacentes;
		for(int i=0;i<4;i++){ //máximo de 4 vizinhos no max e smepre no mesmo padrão, por isso vou direto buscar neles
            //Assim pego na ordem horária requisitada!
            //Caso quisesse geral era só fazer i<v, não ter esses if e else com adjacentes e pegar a posição i ao invés dos adjacentes!
            
            //aquele erro no dijkstra tá aqui!!!! porque o adjacentes vai com lixo e fica looping
            if(i==0 && posInversaX != 0){
                adjacentes = proximo-42; //pega o norte primeiro
                canTry = true;
            }
            else if(i == 1 && posInversaY != 41){
                adjacentes = proximo + 1; //pega o da direita por segundo
                canTry = true;
            }
            else if(i == 2 && posInversaX != 41){
                adjacentes = proximo + 42; //pega o de baixo por terceiro
                canTry = true;
            }
            else if(i == 3 && posInversaY != 0){
                adjacentes = proximo - 1; //pega o da esquerda por ultimo
                canTry = true;
            }

			if(visitados[i]==false && canTry == true){
				if(aux+adj[proximo][adjacentes] < menorcaminho[adjacentes]){
					pais[adjacentes]=proximo;
					menorcaminho[adjacentes]=aux+adj[proximo][adjacentes];
				}
			}
            
		}*/
		contador++;
	}

	//RESULTADOS - Após a conclusão do dijkstra pra todos, buscamos um caminho em específico dado pela entrada.
    cout << endl << "----------------------------------- SAÍDA DA EXECUÇÃO ------------------------------" << endl;
    cout << "Quantidade de nós expandidos pelo custo uniforme: " << qtdNosExpandidos << endl;
    cout << endl << "caminho do vértice origem " << inicio << " ao destino "<<destino<<" via custo uniforme (Dijkstra):";
    buscaCaminho(inicio,destino);
    
}

void Graph::aEstrelaOriginal(int inicio, int destino){
    //Obs: lembrando que v = tamMat*tamMat
	bool visitados[v]; //booleano pros visitados
	int menorcaminho[v];//vetor dos menores caminhos
	int contador=0; //contador de iterações
	int aux; //aux é auxiliar pra guardar menor caminho
	int proximo = -1; //proxima aresta (para trabalhar com o pai)

    list<int> fila;
    int visitadoAtual;
    //for(list<int>::iterator q=fila.begin(); q!=fila.end(); ++q)

    int peso = 0.2; //define o pesoda heurística 

    int posInversaX = floor(proximo/42); //tem que pegar o inteiro de baixo. se tá na 34 será linha 0, se tá na 52 será linha 1.
    int posInversaY = proximo%42; // resto da divisão do adj[i][j] atual por 42.
    int qtdNosExpandidos = 0;

    //Arrumando os pesos 0 para um valor mto mto alto
    for(int i=0; i<v; i++){
        for(int j=0; j<v; j++){
            if(adj[i][j] == 0)
                adj[i][j] = 1000000;
        }
    }
	
	for(int i=0;i<v;i++){
		pais[i]=inicio;
        if(i!=inicio){
			visitados[i]=false;
            
			menorcaminho[i] = adj[inicio][i] /*+ distEuclidiana(proximo, destino)*/; // + h()?
		}
		else{
			visitados[i]=true;
			menorcaminho[i]=0;
		}
	}
	contador++;
    fila.push_front(inicio);
    visitadoAtual = fila.front(); //pega o primeiro elemento da lista.

	//Algoritmo
	while(v-1 > contador && proximo != destino){
		aux=1000000;

        qtdNosExpandidos++;

        if(!fila.empty()){
            for(list<int>::iterator q=fila.begin(); q!=fila.end(); ++q){ //tendo essa remove o for abaixo, mas dá erro.
                if(visitados[*q] == false && menorcaminho[*q]<aux){
                    proximo = *q;
                    aux=menorcaminho[*q];
                }
            }
        }
        
		for(int i=0; i<v; i++){
			if(visitados[i]==false && menorcaminho[i]<aux){
				proximo=i;
				aux=menorcaminho[i]; // + h()?
                //fila.push_front(proximo); //assim a cada novo que encontrar toma a frente, pq é melhor, sem jogar fora os anteriores, só vão para trás.
			}
		}

        //fila.push_front(proximo);
        visitadoAtual = fila.front(); //pega o primeiro elemento da lista.
        fila.erase(fila.begin());
		visitados[proximo]=true;

        posInversaX = floor(proximo/42); //tem que pegar o inteiro de baixo. se tá na 34 será linha 0, se tá na 52 será linha 1.
        posInversaY = proximo%42; // resto da divisão do adj[i][j] atual por 42.
        matrizExpansao[posInversaX][posInversaY] = 1;
        //usleep(500); //4000 tá bom, ,as meio rapido. 20k fica bom tbm
        //cout << "Expansao A*:" << endl;
        //printExpansao(); //cada novo nó printo a expansão; para ficar mais rápido o rpocessamento, pois o caminho é longo. quando quiser descomentar.
        peso = 0.5;
		for(int i=0;i<v;i++){
			if(visitados[i]==false){
				if(aux + adj[proximo][i]+ (h(proximo,destino) * peso) < menorcaminho[i]){
					pais[i]=proximo;
                    
                    //erro aqui, comentei daí.
					//menorcaminho[i]=aux+/*adj[proximo][i] +*/ h(proximo, destino);
                    //menorcaminho[i]=aux+adj[proximo][i] + distEuclidiana(proximo, destino);

                    //Tentando fazer de acordo com o slide 9/44 de busca heurística do professor:
                    //Pegamos os até 4 vizinhos em volta do nó atual, verificamos a f1 a f4 de cada, o que tiver menor resultado será o próximo visitado.
                    //Sendo assim os nós tendem a se expandir de forma a ir direto para o destino (aproximadamente)
                    //Eu basicamente lá no início passei o menor caminho inicial  com h
                    menorcaminho[i] = aux + adj[proximo][i] + h(proximo, destino) * peso ;
                    fila.push_front(i);
                    //break; //não interfere
				}
			}
		}
		contador++;
	}
    //Sobre os testes do pesos - resultados retornados empiricamente - cenário 1: 43 a 1720:
    //original (dijkstra) - expande 1674 dos 1764 nós.
    //peso 1: retornou mesmo custo ótimo do dijkstra, mas expandiu mais nós (se aproximou do bfs) - dimnuir peso
    //peso 0.1, 0.3, 0.5 e 0.75: retornou mesmo custo e qtd de nós expandidos do dijkstra - mto proximo do dijkstra talvez, peso precisa aumentar.
    //peso 2: retornou custo ótimo, mas expandiu mais nós (1741).
    //peso 10: retornou custo ótimo, mas expandiu mais nós (1756)
    //peso 100: retornou custo ótimo, mas expandiu mais nós (1759)
    //problema: conforme aumenta o peso ele sempre visita mais nós (fica mais bfs)

	//RESULTADOS - Após a conclusão do dijkstra pra todos, buscamos um caminho em específico dado pela entrada.
    cout << endl << "----------------------------------- SAÍDA DA EXECUÇÃO ------------------------------" << endl;
    cout << "Quantidade de nós expandidos pelo A*: " << qtdNosExpandidos << endl;
    cout << endl << "Busca do vértice origem " << inicio << " ao destino "<<destino<<" via A*:";
    buscaCaminho(inicio,destino);
    
}

void Graph::aEstrela(int inicio, int destino){
    //Obs: lembrando que v = tamMat*tamMat
	bool visitados[v]; //booleano pros visitados
	int menorcaminho[v];//vetor dos menores caminhos
	int contador=0; //contador de iterações
	int aux; //aux é auxiliar pra guardar menor caminho
	int proximo = -1; //proxima aresta (para trabalhar com o pai)

    int peso = 0.2; //define o pesoda heurística 

    int posInversaX = floor(proximo/42); //tem que pegar o inteiro de baixo. se tá na 34 será linha 0, se tá na 52 será linha 1.
    int posInversaY = proximo%42; // resto da divisão do adj[i][j] atual por 42.
    int qtdNosExpandidos = 0;

    vector<int> filaAstar; 

    //Arrumando os pesos 0 para um valor mto mto alto
    for(int i=0; i<v; i++){
        for(int j=0; j<v; j++){
            if(adj[i][j] == 0)
                adj[i][j] = 1000000;
        }
    }
	
	for(int i=0;i<v;i++){
		pais[i]=inicio;
        if(i!=inicio){
			visitados[i]=false;
            
			menorcaminho[i] = adj[inicio][i] /*+ distEuclidiana(proximo, destino)*/; // + h()?
		}
		else{
			visitados[i]=true;
			menorcaminho[i]=0;
		}
	}
	contador++;

    filaAstar.push_back(inicio); 
    int visitadoAtual;
    int distVizinhosPossiveis[4];
    int contt = 0;

	//Algoritmo
	while(contador < v - 1 && proximo != destino){
		//aux=1000000;

        
        visitadoAtual = filaAstar[0];
        //aux = menorcaminho[filaAstar[0]] + h(proximo,destino) * peso;
        filaAstar.erase(filaAstar.begin()); //apago a pos 0 da fila.
        qtdNosExpandidos++;

        //Professor:
        //tendo o dijkstra tem o custo do nó atual ao raíz, considera o custo do terreno.
        //precisa reordenar os nós, talvez fosse isso que tava faltando na anterior (por isso expandia mto)
        //Deu a ideia de fazer o algoritmo só com em linha reta pra ver se tá ok.

        /*
		for(int i=0; i<v; i++){
			if(visitados[i]==false && menorcaminho[i] + h(i, destino) * peso <aux){
				proximo=i;
				aux=menorcaminho[i] + h(proximo, destino) * peso; // + h()?
			}
		}*/

        peso = 1;
        while(1){
            posInversaY = visitadoAtual%42;//para pegar verdadeiro y
            //if(contt == 0 && visitadoAtual-42 >= 0 && visitados[visitadoAtual-42]==false){//norte
                proximo=visitadoAtual-42;
				aux=menorcaminho[visitadoAtual] + adj[proximo][visitadoAtual] + h(proximo, destino) * peso; // + h()?
            //}
            if (contt == 1 && posInversaY != 0 && visitados[visitadoAtual+1]==false && menorcaminho[visitadoAtual]+adj[visitadoAtual+1][visitadoAtual]+h(visitadoAtual+1, destino) * peso < aux){//dirieta
                proximo = visitadoAtual + 1;
                aux = menorcaminho[visitadoAtual]+adj[proximo][visitadoAtual]+h(proximo, destino) * peso;
            }   
            else if (contt == 2 && visitadoAtual+42<1763 && visitados[visitadoAtual+42]==false && menorcaminho[visitadoAtual]+adj[visitadoAtual+42][visitadoAtual]+h(visitadoAtual+42, destino) * peso < aux){//sul
                proximo = visitadoAtual + 42;
                aux = menorcaminho[visitadoAtual]+adj[proximo][visitadoAtual]+h(proximo, destino) * peso;
            }   
            else if (contt == 3 && posInversaY != 41 && visitados[visitadoAtual-1]==false && menorcaminho[visitadoAtual]+adj[visitadoAtual-1][visitadoAtual]+h(visitadoAtual-1, destino) * peso < aux){//oeste
                proximo = visitadoAtual - 1;
                aux = menorcaminho[visitadoAtual]+adj[proximo][visitadoAtual]+h(proximo, destino) * peso;
            }   
            contt++;
            if(contt==4)
                break;
        }

        filaAstar.push_back(proximo); 
        menorcaminho[proximo] = menorcaminho[visitadoAtual] + adj[proximo][visitadoAtual];
        pais[visitadoAtual]=proximo;
        //aux é o último setado (assim como prox)
        //proximo = filaAstar[0];
        
        //for(int i=0; i<4; i++){ //Visitado os 4 vizinhos possiveis

		visitados[proximo]=true;
        posInversaX = floor(proximo/42); //tem que pegar o inteiro de baixo. se tá na 34 será linha 0, se tá na 52 será linha 1.
        posInversaY = proximo%42; // resto da divisão do adj[i][j] atual por 42.
        matrizExpansao[posInversaX][posInversaY] = 1;
        //usleep(500); //4000 tá bom, ,as meio rapido. 20k fica bom tbm
        //cout << "Expansao A*:" << endl;
        //printExpansao(); //cada novo nó printo a expansão; para ficar mais rápido o rpocessamento, pois o caminho é longo. quando quiser descomentar.
        
        
        //}
        
        
        /*
		for(int i=0;i<v;i++){
			if(visitados[i]==false){
				if(aux + adj[proximo][i] < menorcaminho[i]){
					pais[i]=proximo;
                    
                    //erro aqui, comentei daí.
					//menorcaminho[i]=aux+/*adj[proximo][i] +*/ /*h(proximo, destino);*/
                    //menorcaminho[i]=aux+adj[proximo][i] + distEuclidiana(proximo, destino);

                    //Tentando fazer de acordo com o slide 9/44 de busca heurística do professor:
                    //Pegamos os até 4 vizinhos em volta do nó atual, verificamos a f1 a f4 de cada, o que tiver menor resultado será o próximo visitado.
                    //Sendo assim os nós tendem a se expandir de forma a ir direto para o destino (aproximadamente)
                    //Eu basicamente lá no início passei o menor caminho inicial  com h
                    
                    /*menorcaminho[i] = aux + adj[proximo][i];
                    filaAstar.push_back(i);

				}
			}
		}*/
		contador++;
	}
    //Sobre os testes do pesos - resultados retornados empiricamente - cenário 1: 43 a 1720:
    //original (dijkstra) - expande 1674 dos 1764 nós.
    //peso 1: retornou mesmo custo ótimo do dijkstra, mas expandiu mais nós (se aproximou do bfs) - dimnuir peso
    //peso 0.1, 0.3, 0.5 e 0.75: retornou mesmo custo e qtd de nós expandidos do dijkstra - mto proximo do dijkstra talvez, peso precisa aumentar.
    //peso 2: retornou custo ótimo, mas expandiu mais nós (1741).
    //peso 10: retornou custo ótimo, mas expandiu mais nós (1756)
    //peso 100: retornou custo ótimo, mas expandiu mais nós (1759)
    //problema: conforme aumenta o peso ele sempre visita mais nós (fica mais bfs)

	//RESULTADOS - Após a conclusão do dijkstra pra todos, buscamos um caminho em específico dado pela entrada.
    cout << endl << "----------------------------------- SAÍDA DA EXECUÇÃO ------------------------------" << endl;
    cout << "Quantidade de nós expandidos pelo A*: " << qtdNosExpandidos << endl;
    cout << endl << "Busca do vértice origem " << inicio << " ao destino "<<destino<<" via A*:";
    buscaCaminho(inicio,destino);
    
    
}

void Graph::buscaCaminho(int origem, int destino){
    int custoTotal = 0;
    int qtdVertices=0;
    int pos = destino;
    int posInversaX = floor(pos/42); //tem que pegar o inteiro de baixo. se tá na 34 será linha 0, se tá na 52 será linha 1.
    int posInversaY = pos%42; // resto da divisão do adj[i][j] atual por 42.

    cout << endl << destino;
    while(pos != origem){
        //custoTotal += adj[pos][pais[pos]];
        posInversaX = floor(pos/42); //tem que pegar o inteiro de baixo. se tá na 34 será linha 0, se tá na 52 será linha 1.
        posInversaY = pos%42; // resto da divisão do adj[i][j] atual por 42.
        if(qtdVertices == 0)
            matrizExpansao[posInversaX][posInversaY] = 4; //final do caminho.     
        else
            matrizExpansao[posInversaX][posInversaY] = 3; //caminho percorrido é 3
        custoTotal += adj[pais[pos]][pos]; 
        cout << "<-" <<  pais[pos];
        pos = pais[pos];
        qtdVertices++;
    }
    posInversaX = floor(pos/42); //tem que pegar o inteiro de baixo. se tá na 34 será linha 0, se tá na 52 será linha 1.
    posInversaY = pos%42; // resto da divisão do adj[i][j] atual por 42.
    matrizExpansao[posInversaX][posInversaY] = 2; //origem do caminho percorrido é 2 
    cout << endl << endl << "Quantidade de vértices/saltos no caminho: " << qtdVertices << endl;
    cout << endl << "Custo total do caminho: " << custoTotal; 

    cout << endl << endl << "Expansão com caminho buscado de origem " << origem << " e destino " << destino << endl;
    printExpansao(); //cada novo nó printo a expansão;
    imprimirAmbienteComCaminho(origem, destino); //printa o ambiente com o caminho percorrido (cores ok)
    imprimirAmbienteComExpansaoCaminho(origem, destino);
    
}

void Graph::printExpansao(){
    printf ("   ");
    for(int i=0; i<42; i++){
        if(i<10)
            printf("0%d ",i);
        else
            printf("%d ",i);
    }
    printf ("\n");
    for(int i=0;i<42;i++){
        //Sobre as cores: https://stackoverflow.com/questions/2616906/how-do-i-output-coloured-text-to-a-linux-terminal
        if(i < 10)
            printf ("0%d ", i);
        else
            printf ("%d ", i);
        //printf ("%.0lf ", floor(i/10)); //caso fique tudo junto, sem espaço no \033[1;30mX\033[0m" etc, aí tirar o if else aqui
        for (int j=0; j<42; j++){
            if(matrizExpansao[i][j] == 0)
                cout << "\033[1;30mX\033[0m  "; //não vistado - 30 é cor preta
            else if (matrizExpansao[i][j] == 1)
                //cout << bold_off << 'V'; //visitado - 31 é cor vermelha
                cout << "\033[1;31mX\033[0m  "; //visitado - 31 é cor vermelha
            else if (matrizExpansao[i][j] == 2)
                cout << "\033[1;32mO\033[0m  "; //origem do caminho - 32 é verde
            else if (matrizExpansao[i][j] == 3){
                cout << "\033[1;32mX\033[0m  "; //faz parte do caminho - 32 é verde
            }
            else if (matrizExpansao[i][j] == 4)
                cout << "\033[1;32mF\033[0m  "; //final do caminho - 32 é verde
            //cout << matrizExpansao[i][j];
        }
        cout << endl;
    }
    cout <<  "Legenda: \033[1;30mX\033[0m = não visitado // \033[1;31mX\033[0m = visitado // \033[1;32mX\033[0m = caminho // \033[1;32mO\033[0m = origem do caminho // \033[1;32mF\033[0m = final do caminho" << endl;
    /*Legenda: na matriz de expansão:
        0 é não visitado
        1 é visitado
        2 é visitado e faz parte do caminho buscado.*/

}

void Graph::imprimirAmbienteComCaminho(int origem, int destino){
    int i,j;
    cout << endl << "IMPRESSÃO DO AMBIENTE COM CAMINHO REALIZADO:" << endl << endl;
    cout << endl;
    for(i = 0; i< tamMat; i++){
        for(j=0;j< tamMat;j++){
            if (matriz[i][j] == 1){//gramado
                if(matrizExpansao[i][j] == 0) //não expandido
                    cout << "\033[1;32m▢\033[0m ";
                else if(matrizExpansao[i][j] == 1) //expandido mas não é caminho.
                    cout << "\033[1;32m☑\033[0m "; //expandido
                else{ //2, 3 ou 4. Expandiu e é caminho.
                    if(matrizExpansao[i][j] == 2)//origem caminho
                        cout << "\033[1;32m▣\033[0m ";
                    else if(matrizExpansao[i][j] == 3)//Meio caminho
                        cout << "\033[1;32m▩\033[0m ";
                    else if(matrizExpansao[i][j] == 4)//fim caminho
                        cout << "\033[1;32m✪\033[0m ";
                }
            }
            if (matriz[i][j] == 5){//montanha
                if(matrizExpansao[i][j] == 0) //não expandido
                    cout << "\033[1;30m▢\033[0m ";
                else if(matrizExpansao[i][j] == 1) //expandido mas não é caminho.
                    cout << "\033[1;30m☑\033[0m "; //expandido
                else{ //2, 3 ou 4. Expandiu e é caminho.
                    if(matrizExpansao[i][j] == 2)//origem caminho
                        cout << "\033[1;30m▣\033[0m ";
                    else if(matrizExpansao[i][j] == 3)//Meio caminho
                        cout << "\033[1;30m▩\033[0m ";
                    else if(matrizExpansao[i][j] == 4)//fim caminho
                        cout << "\033[1;30m✪\033[0m ";
                }
            }
            if (matriz[i][j] == 10){//Pântano
                if(matrizExpansao[i][j] == 0) //não expandido
                    cout << "\033[1;34m▢\033[0m ";
                else if(matrizExpansao[i][j] == 1) //expandido mas não é caminho.
                    cout << "\033[1;34m☑\033[0m "; //expandido
                else{ //2, 3 ou 4. Expandiu e é caminho.
                    if(matrizExpansao[i][j] == 2)//origem caminho
                        cout << "\033[1;34m▣\033[0m ";
                    else if(matrizExpansao[i][j] == 3)//Meio caminho
                        cout << "\033[1;34m▩\033[0m ";
                    else if(matrizExpansao[i][j] == 4)//fim caminho
                        cout << "\033[1;34m✪\033[0m ";
                }
            }
            if (matriz[i][j] == 15){//fogo
                if(matrizExpansao[i][j] == 0) //não expandido
                    cout << "\033[1;31m▢\033[0m ";
                else if(matrizExpansao[i][j] == 1) //expandido mas não é caminho.
                    cout << "\033[1;31m☑\033[0m "; //expandido
                else{ //2, 3 ou 4. Expandiu e é caminho.
                    if(matrizExpansao[i][j] == 2)//origem caminho
                        cout << "\033[1;31m▣\033[0m ";
                    else if(matrizExpansao[i][j] == 3)//Meio caminho
                        cout << "\033[1;31m▩\033[0m ";
                    else if(matrizExpansao[i][j] == 4)//fim caminho
                        cout << "\033[1;31m✪\033[0m ";
                }
            }
        }
        printf("\n");
    }
    cout << "Legenda terreno: \033[1;32m●\033[0m -gramado // \033[1;30m●\033[0m - montanha // \033[1;34m●\033[0m -pântano // \033[1;31m●\033[0m -fogo" << endl;
    cout << "Legenda visitação: ▢ -nó não visitado // ☑ -nó visitado // ▣ -nó início do caminho" << endl<< "Legenda visitação: ▩ -nó do meio do caminho // ✪ -nó final do caminho";
    
    cout << endl;
}

void Graph::imprimirAmbienteComExpansaoCaminho(int origem, int destino){
    int i,j;
    cout << endl << "IMPRESSÃO DO AMBIENTE COM CAMINHO REALIZADO:" << endl;
    for(i = 0; i< tamMat; i++){
        for(j=0;j< tamMat;j++){
            if (matriz[i][j] == 1){//gramado
                if(matrizExpansao[i][j] == 3 || matrizExpansao[i][j] == 2 || matrizExpansao[i][j] == 4)
                    cout << "\033[1;32m.\033[0m ";
                else
                    cout << "\033[1;32mX\033[0m ";
            }
            else if(matriz[i][j] == 5){ //montanha
                if(matrizExpansao[i][j] == 3 || matrizExpansao[i][j] == 2 || matrizExpansao[i][j] == 4)
                    cout << "\033[1;30m.\033[0m ";
                else
                    cout << "\033[1;30mX\033[0m ";
            }
            else if (matriz[i][j] == 15){ //fogo
                if(matrizExpansao[i][j] == 3 || matrizExpansao[i][j] == 2 || matrizExpansao[i][j] == 4)
                    cout << "\033[1;31m.\033[0m ";
                else
                    cout << "\033[1;31mX\033[0m ";
            }
            else if (matriz[i][j] == 10){//agua - pantano
                if(matrizExpansao[i][j] == 3 || matrizExpansao[i][j] == 2 || matrizExpansao[i][j] == 4)
                    cout << "\033[1;34m.\033[0m ";
                else
                    cout << "\033[1;34mX\033[0m ";
            }
        }
        printf("\n");
    }
    cout << "Legenda: \033[1;32mX\033[0m = gramado // \033[1;30mX\033[0m = montanha // \033[1;34mX\033[0m = pântano // \033[1;31mX\033[0m = fogo //  . = caminho percorrido pelo agente" << endl;
    cout << endl;
}

void Graph::printAdj(){
    printf("\n\nPrint da matriz adjacencia com suas ligacoes e custos. Obs: Truncado em 45x45");
    //printf("  ");
    //for(int i=0;i<45;i++)
        //printf("%d ", i);
    printf("\n");
    for(int i = 0; i< 45; i++){
        //printf("%d ", i);
        for(int j=0;j< 45;j++){
            if(adj[i][j] >= 10)
                printf("%d ", adj[i][j]);
            else    
                printf("%d  ", adj[i][j]);
        }
        printf("\n");
    }
}

//matriz dos valroes da adjacencia no ambiente, pra facilitar a visualização
void Graph::printMatrizAdjAmbiente(){
    printf ("\nMATRIZ COM POSICOES DA ADJACENCIA NO AMBIENTE (TRUNCADO EM 42X29):\n");
    //obs trunquei em 29 e não o tam 42 pq minha tela fica pulando pra seguinte pq ela é pequena haha
    for(int i=0; i<42; i++){
        for(int j=0; j<29; j++){
            //vários if else ṕra arruamr o formato do print.
            if((i*42)+j >= 1000)
                printf ("%d ", (i*42)+j);
            else if((i*42)+j >= 100)
                printf ("%d  ", (i*42)+j);
            else if((i*42)+j >= 10)
                printf ("%d   ", (i*42)+j);
            else
                printf ("%d    ", (i*42)+j);
        }
        printf ("\n");
    }
    printf ("\n");
}

void Graph::printPais(){ //para print depois de teste.
        printf("\n");
    for(int i = 0; i< v; i++){
       printf ("Pai[%d]: %d\n", i, pais[i]);
    }
}
  
int main(int argc, char *argv[]) { 

    if( argc != 3 ){
	      printf( "Parametros incorretos!! %i", argc );
	      return 0;
	}

    leituraAmbiente();
    //imprimirAmbiente(); //imprime o ambiente original (com pesos, não legenda)
    
    int v = tamMat*tamMat; //42x42

    Graph grafo(v); //Criando o grafo, passando a quantidade de vértices no construtor

    //Criando a matriz adjacencia dado o ambiente:
    for(int i=0; i<tamMat; i++){
        for(int j=0; j<tamMat; j++){
            grafo.addAresta(i,j); //lança a célula matriz ambiente (vértice) para criar as relações. 
        }
    }

    int inicio = atoi(argv[1]); //vértice origem
    int destino = atoi(argv[2]); //vértice destino

    //grafo.printAdj(); //printa os valores (custos) de cada vértice na matriz adj)
    //grafo.printPais(); //printa os pais de cada vértice
    grafo.printMatrizAdjAmbiente(); //printa a matriz adj (posições) no ambiente

    //grafo.BFS(inicio, destino); //Execução do BFS com origem e destino

    //grafo.dijkstra(inicio, destino); //Execução do dijkstra/custo uniforme com origem e destino
    
    //grafo.aEstrela(inicio, destino);

    grafo.aEstrelaOriginal(inicio,destino);

} 
