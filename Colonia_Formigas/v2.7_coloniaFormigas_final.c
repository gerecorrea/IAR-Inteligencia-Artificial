/*  Implementação básica colônia de formigas
    Criadores: Beatriz e Geremias
    Disciplina: Inteligência Artificial
    BCC - UDESC*/


/* versão 2.7:
    Criamos as funções de pegar e largar baseada no artigo 1, baseadas na aleatoriedade (verificar se isso mesmo)
        Retiramos as funcoesObjetivos anteriores, para usar essa nova, ela usa a relação lá dentro. (2.5)
        Havia um erro para estar gerando os resultados errados, agora está tudo ok
        Tentar melhorar os resultados alterando os valores de k1 e k2 nas funções respectivas.
    Função andarSoltar alterada para andar aleatoriamente para qualquer lugar (2.5)
    Valores de parametros testado e resultados ok
    Não deixamos mais um agente voltar para sua célula antiga (pedido do professor)
    Caso a probabilidade P seja extrema, 0 ou 1, dependendo do objetivo, fica deterministico se pega ou larga (pq pela função é só probabildiadade)
        Foi pedido do professor.
    
    Obs: os testes desse serão só pra raio1. Testar com k e qtdIterações diferentes

    Executar com: $ gcc -Wall -O3 -pthread -o exec v2.7... && ./exec
        Sem o -pthread vez ou outra dá ref não definida para pthreads. */

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include <string.h>
#include <math.h>

//TUDO CERTO
//FAZER TESTES DE R = 1 PARA 4 E 15 GRUPOS
//FAZER 50 AGENTES COM 1KK DE ITERAÇÕES, TÁ MASSA ACHO

//Aqui a declaração dos mutexes:
pthread_mutex_t modificaMatriz = PTHREAD_MUTEX_INITIALIZER;
//Dados de entrada:
//#define tamMat 28 //Deixei 28 pq dá 784 células, e estamos inserindo 400 corpos, aí fica +- 50%
#define tamMat 34 //Para arquivo com 15 grupos (600 corpos), posi assim temos 1156 células (+- 50%)

int raioVisao = 5; //Raio de visao dos agentes quanto à vizinhança
#define qtdAgentes 50 //quantidade de agentes (formigas vivas) - não muda mto, mas to pensando por padrão deixar 50 sla.
//Com 10 agentes o resultado tá bem bom até, sim. Até com 1 agente fica bom kkk
#define porcentagemCorpos 0.5 //para fazer a função objetivo se adaptar - ainda não usado
int qtdCorpos = tamMat * tamMat * porcentagemCorpos; //60% da matriz ocupada por corpos
unsigned int qtdIteracoes = 100000;//qtd de iterações para cada thread/agente. 
int matriz[tamMat][tamMat]; //A matriz que compoe os corpos e agentes.

//Variáveis para entrada de dados diferentes.
int qtdGrupos = 15; //Para o a qtd de grupos vindas do arquivo.
#define qtdCorposDados 600 //qtd corpos dos dados (heterogêneos)
char nomeArq[50] = "arquivo15grupos.txt";

//Struct dos agentes para armazenamento de dados essenciais
struct Agentes{
    int linhaAtual; //linha inicial e atual do agente
    int colunaAtual; //coluna inicial e atual do agente
    int comCorpo; //Se carrega algo, inicializa como 0 (n carrega). 1 ele carrega.
    int idCorpo; //identificador do corpo que pegou!
    int linhaAntiga;
    int colunaAntiga;
};

struct Agentes agentes[qtdAgentes]; 

//Struct dos vizinhos quando for dar rand para andar na matriz.
struct Vizinhos{
    int linha;
    int coluna;
    int flagPode; //0 não pode ir, 1 pode ir.
}; 
struct Vizinhos vizinhos[qtdAgentes][100]; //10 é a qtd de threads por enquanto

//struct para armazenar os dados lidos do arquivo - parte v2.0+
struct Dados{
    int id; //identificador do item, até o momento sem uso
    //Preciso usar o identiicar para saer facilmente qual item está na matriz. Sendo que a posição dessedado é por i-1 (pq i inicia em 1 pois 0 é celula vazia)
    double x; //valor ref 1 do arquivo de entrada
    double y; //valor ref 2 do arquivo de entrada
    int label; //label do arquivo de entrada (classe)
    int linhaAtual; //linha atual onde o dado se encontra
    int colunaAtual; //coluna atual onde o dado se encontra
};

//o arquivo com 15 grupos tem 600 dados
//e o arquivo com 4 grupos tem 400 dados
struct Dados dados[qtdCorposDados]; //limite pelo maior arquivo ter 600 corpos.

//Leitura do arquivo com os dados heterogêneo para a simulação (v2.0+)
void leituraArquivo(){
    int i = 0;
    char n1[20], n2[20];
    char *v;
    
    FILE *arq;
    //arq = fopen("arquivo4grupos.txt", "r");
    arq = fopen(nomeArq, "r");
    //testando se o arquivo foi realmente criado
    if(arq == NULL){
        printf("Erro na abertura do arquivo!");
        exit(1);
    }else{
        while(!feof(arq)){
            /*Lendo os dados*/
            fflush(stdin);
            fscanf(arq,"%s %s %d", n1, n2, &dados[i].label);
                
            //Substituindo a vírgula por ponto
            v = strchr(n1, ','); // encontra a primeira vírgula
            if (v) *v = '.'; // substitui por ponto
            dados[i].x = strtod(n1, NULL);   // devolve valor double

            v = strchr(n2, ','); // encontra a primeira vírgula
            if (v) *v = '.'; // substitui por ponto
            dados[i].y = strtod(n2, NULL);   // devolve valor double
                
            //Fim da substituição da vírgula
 
            dados[i].id = i; //recebe um identificador
            //printf("%d %lf %lf %d\n",dados[i].id,dados[i].x,dados[i].y, dados[i].label); //tudo ok
            i++;
            //if(i == 400) //para caso queira pegar até x grupos, basta arrumar o if aqui
                //break
        }
    }
    
    fclose(arq);
}

//BACKUP VAZIA = 0

void popularMatrizDados(){
    /* A princípio pensei em um jeito bem básico de popular, eu zerei uma matriz, as casas com 0 são vazias. Ai como são 400 dados, eu vou sorteando aleatoriamente uma linha e uma coluna da matriz para colocar cada dado. Cada dado tem um id que vai de 1 até 400, e é esse id q eu coloco na matriz.
    */
    int i,j, x=0;

    time_t t;
    srand((unsigned) time(&t)); //para gerar valores randomicos na população inicial

    //zerar a matriz
    for(i=0; i<tamMat; i++){
        for(j=0; j<tamMat; j++){
            matriz[i][j] = -1; //-1 representa casa vazia, qlqr outro valor tem corpo
        }
    }

    //Populaciona dados
    if((tamMat * tamMat) > qtdCorposDados){ //verificando se cabe todos os dados na matriz
        while(x != qtdCorposDados){ //colocando todos os dados na matriz
            i = (rand() % tamMat); //linha aleatória da matriz entre [0, tamMat -1]
            j = (rand() % tamMat); //coluna aleatória da matriz entre [0, tamMat -1]
            
            if(matriz[i][j] == -1){ //condicional para evitar popular células repetidas
                dados[x].linhaAtual = i;
                dados[x].colunaAtual = j;
                matriz[i][j] = dados[x].id; // 
                //matriz[i][j] = 1; // 
                x++;
            }
        }
    }
    else{
        printf ("Matriz muito pequena para a quantidade de dados recebida!");
    }

    x = 0;
    //Populando os agentes
    while(x != qtdAgentes){
        i = (rand() % tamMat); //linha aleatória da matriz entre [0, tamMat -1]
        j = (rand() % tamMat); //coluna aleatória da matriz entre [0, tamMat -1]
        agentes[x].linhaAtual = i;
        agentes[x].colunaAtual = j;
        agentes[x].comCorpo = 0; //0 pq não carrega nenhum corpo. 
        agentes[x].idCorpo = -1; //sem corpo
        x++;
    }
}



void popularMatriz(){
    int x = 0;
    int i,j;

    time_t t;
    srand((unsigned) time(&t)); //para gerar valores randomicos na população inicial

    //zerar a matriz
    for(i=0; i<tamMat; i++){
        for(j=0; j<tamMat; j++){
            matriz[i][j] = 0; //0 representa a casa vazia
        }
    }
    
    //Populaciona corpos
    while(x != qtdCorpos){
        i = (rand() % tamMat); //linha aleatória da matriz entre [0, tamMat -1]
        j = (rand() % tamMat); //coluna aleatória da matriz entre [0, tamMat -1]
        
        if(matriz[i][j] == 0){ //condicional para evitar popular células repetidas
            matriz[i][j] = 1; //1 representa a casa com uma formiga morta
            x++;
        }
    }

    x = 0;
    while(x != qtdAgentes){
        i = (rand() % tamMat); //linha aleatória da matriz entre [0, tamMat -1]
        j = (rand() % tamMat); //coluna aleatória da matriz entre [0, tamMat -1]
        agentes[x].linhaAtual = i;
        agentes[x].colunaAtual = j;
        agentes[x].comCorpo = 0; //0 pq não carrega nenhum corpo. 
        x++;
    }
}

void printMatrizOriginal(){
    int i,j;
    for(i=0;i<tamMat; i++){
        for(j=0;j<tamMat;j++){
            //Deixei assim para pode ver melhor a distribuição.
            if(matriz[i][j] != -1)
                printf ("x");
            else
                printf (" ");
            //printf (" %d", matriz[i][j]);
        }
        printf("\n");
    }
}

void printMatriz(){
    int i,j;
    //int cont = 0;
    for(i=0; i<tamMat; i++){
        if(i==0)
            printf("  ");
        printf ("___");
    }
    printf ("\n");
    for(i=0;i<tamMat; i++){
        printf ("  |");
        for(j=0;j<tamMat;j++){
            //Deixei assim para pode ver melhor a distribuição.
            if(matriz[i][j] != -1){ //se tiver uma id
                if(dados[matriz[i][j]].label <= 9)
                    printf ("%d |", dados[matriz[i][j]].label);
                else
                    printf ("%d|", dados[matriz[i][j]].label);
            }else{
                printf ("  |");
            }
            //printf (" %d", matriz[i][j]);
        }
        printf("\n");
    }
    for(i=0;i<tamMat;i++){
        if(i==0)
            printf("  ");
        printf("¯¯¯");
    }
    printf("\n");

    //printf("cont = %d", cont);
}

//Função que representa o agente buscando soltar um corpo
void andarSoltar(int linha, int coluna, int tid){
    int qtdPossivel = 0; //qtd de possiblidades lógicas para dar o rand em cima.
    int qtdTotal = 0; //qtd Total de vizinhos
    int celulaEscolhida; //variável que recebe para onde irei
    int i, j;
    int auxX, auxY;

    for(i=0;i<8;i++){ //garanto que inicialmente nenhum pode.
        vizinhos[tid][i].flagPode = 0;
    }

    //Itero sobre a vizinhança adjacente do agente atual.
    pthread_mutex_lock(&modificaMatriz);
    for(i = linha - 1; i<= linha + 1; i++){
        for(j = coluna - 1; j<= coluna + 1; j++){

            if(i < 0) //caso k ultrapassa a borda esquerda
                auxX = tamMat + i; //assim vai para a direita.
            else if (i>= tamMat) //caso k ultrapassa a borda direita
                auxX = i - tamMat; //volta para a esquerda, posição inicial 0.
            else
                auxX = i;
            if(j < 0) //caso k ultrapasse a borda esquerda
                auxY = tamMat + j; //volta para baixo
            else if (j >= tamMat)//caso ultrapasse a borda de baixo
                auxY = j - tamMat; //volta para cima, posição inicial 0
            else
                auxY = j;
                

            //verificar se os valores i e j estão dentro da matriz
            if((i != linha && j != coluna)){ //se está dentro do limites da matriz.
                vizinhos[tid][qtdTotal].linha = auxX;
                vizinhos[tid][qtdTotal].coluna = auxY;
                if(agentes[tid].linhaAntiga != i && agentes[tid].colunaAntiga != j){ //Se não é a célula anterior
                    vizinhos[tid][qtdTotal].flagPode = 1;
                    qtdPossivel++;
                }else
                    vizinhos[tid][qtdTotal].flagPode = 0;
                qtdTotal++;
            }
            //OBS: PARA MAIS DE UM GRUPO, A IDEIA DO ANDARSOLTAR PARA ONDE A CÉLULA ESTÁ VAZIA PODE NÃO ESTAR DANDO BOM
            //OS TESTES RÁPIDOS COM RAND TOTAL EM CIMA DOS 8 VIZINHOS SE MOSTROU POSSIVELMENTE MELHOR
            //TESTAR MELHOR DEPOIS ISSO!

            /* FORMA ANTES DE SER TOTALMENTE ALEATORIO:
            if((i != linha && j != coluna)){ //se está dentro do limites da matriz.
                vizinhos[tid][qtdTotal].linha = auxX;
                vizinhos[tid][qtdTotal].coluna = auxY;
                if(matriz[auxX][auxY] == -1){ //Se não há um corpo
                    vizinhos[tid][qtdTotal].flagPode = 1;
                    qtdPossivel++;
                }else
                    vizinhos[tid][qtdTotal].flagPode = 0;
                qtdTotal++;
            }*/
        }
    }
    pthread_mutex_unlock(&modificaMatriz);
    if(qtdPossivel == 0){ //Caso não haja o objetivo requerido
        celulaEscolhida = rand() % qtdTotal;
    }
    else{ //Caso haja ao menos uma célula com o objetivo requerido. Só paro quando dou rand em um dos possíveis
        while(1){
            celulaEscolhida = rand() % qtdTotal;
            if(vizinhos[tid][celulaEscolhida].flagPode == 1){
                break;
            }
        }
    }
    
    //Agora, escolhida a nova célula, salvo a antiga (para não permitir voltar) e vou para a nova.
    agentes[tid].linhaAntiga = agentes[tid].linhaAtual;
    agentes[tid].colunaAntiga = agentes[tid].colunaAtual;
    agentes[tid].linhaAtual = vizinhos[tid][celulaEscolhida].linha;
    agentes[tid].colunaAtual = vizinhos[tid][celulaEscolhida].coluna;
    /*Detalhe: essa função simplesmente leva o agente a uma célula vazia. Buscando a aleatoriedade
    Poderia ter sido feito e buscar uma vazia com bastante corpos na vizinhança, porém consideramos que tira a aleatoriedade*/

}

//Função para onde devo andar quando sou um agente que ainda busca um corpo.
//Essa não se altera, pois apenas quero buscar um item/corpo, e tanto faz  de qual grupo ele é.
void andarCarregar(int linhaAtual, int colunaAtual, int tid){
    int qtdPossivel = 0; //qtd de possiblidades lógicas para dar o rand em cima.
    int qtdTotal = 0; //qtd Total de vizinhos
    int celulaEscolhida; //variável que recebe para onde irei
    int i, j;
    int auxX,auxY;

    //Itero sobre a vizinhança adjacente do agente atual.
    pthread_mutex_lock(&modificaMatriz);
    for(i = linhaAtual - 1; i<= linhaAtual + 1; i++){
        for(j = colunaAtual - 1; j<= colunaAtual + 1; j++){
            //verificar se os valores i e j estão dentro da matriz
            
            if(i < 0) //caso k ultrapassa a borda esquerda
                auxX = tamMat + i; //assim vai para a direita.
            else if (i>= tamMat) //caso k ultrapassa a borda direita
                auxX = i - tamMat; //volta para a esquerda, posição inicial 0.
            else
                auxX = i;
            if(j < 0) //caso k ultrapasse a borda esquerda
                auxY = tamMat + j; //volta para baixo
            else if (j >= tamMat)//caso ultrapasse a borda de baixo
                auxY = j - tamMat; //volta para cima, posição inicial 0
            else
                auxY = j;
            
            if((i != linhaAtual && j != colunaAtual)){ //se não é o agente
                vizinhos[tid][qtdTotal].linha = auxX;
                vizinhos[tid][qtdTotal].coluna = auxY;
                if(matriz[auxX][auxY] != -1 ||  (agentes[tid].linhaAntiga != i && agentes[tid].colunaAntiga != j)){ //Se há um corpo, pois estamos buscando um
                    //Além que verifico para não permtir ele ir pra que ele estava antes.
                    vizinhos[tid][qtdTotal].flagPode = 1;
                    qtdPossivel++;
                }else
                    vizinhos[tid][qtdTotal].flagPode = 0;
                qtdTotal++;
            }
        }
    }
    pthread_mutex_unlock(&modificaMatriz);
    if(qtdPossivel == 0){ //Caso não haja o objetivo requerido
        celulaEscolhida = rand() % qtdTotal;
    }
    else{ //Caso haja ao menos uma célula com o objetivo requerido. Só paro quando dou rand em um dos possíveis
        while(1){
            celulaEscolhida = rand() % qtdTotal;
            if(vizinhos[tid][celulaEscolhida].flagPode == 1){
                break;
            }
        }
    }

    //O agente então vai para outra célula, e salvamos a atual como antiga para não permitir voltar..
    agentes[tid].linhaAntiga = agentes[tid].linhaAtual;
    agentes[tid].colunaAntiga = agentes[tid].colunaAtual;
    agentes[tid].linhaAtual = vizinhos[tid][celulaEscolhida].linha;
    agentes[tid].colunaAtual = vizinhos[tid][celulaEscolhida].coluna;
}

//Função que busca a relação dos corpos e vizinhos com ponderamento na distância das células.
//Nossa nova função relação vai buscar apenas itens similares (por distância euclidiana)
//Muito provavelmente serão simialres/estarão no mesmo quadrante de valor do dado se o valor euclidiano der < 800 (pq 20² + 20², limite dos valores basicamente)
float relacao(int tid, int objetivo){ 
    //objetivo 1 é pegar e 2 é largar
    int j,k;
    float soma = 0; //soma ponderada dos valores baseado na proximidade das celulas
    float somaMax = 0;
    float max;
    int auxX,auxY;
    int minX, minY;
    float itemAlvoX, itemAlvoY, corpoX, corpoY, distancia; //para buscar relação por euclidiana (ou outra)

    for(k = agentes[tid].linhaAtual - raioVisao; k<= agentes[tid].linhaAtual + raioVisao; k++){
        for(j = agentes[tid].colunaAtual - raioVisao; j<= agentes[tid].colunaAtual + raioVisao; j++){

            //verificar se os valores i e j estão dentro da matriz e se não estiverem ir para o lado oposto (esfera):
            if(k < 0) //caso k ultrapassa a borda esquerda
                auxX = tamMat + k; //assim vai para a direita.
            else if (k>= tamMat) //caso k ultrapassa a borda direita
                auxX = k - tamMat; //volta para a esquerda, posição inicial 0.
            else
                auxX = k;
            if(j < 0) //caso k ultrapasse a borda esquerda
                auxY = tamMat + j; //volta para baixo
            else if (j >= tamMat)//caso ultrapasse a borda de baixo
                auxY = j - tamMat; //volta para cima, posição inicial 0
            else
                auxY = j;

            //Aplicando função euclidiana para encontrar similaridade: 
			if(objetivo == 2){
				itemAlvoX = dados[agentes[tid].idCorpo].x; //item que o agente tem em mãos
            	itemAlvoY = dados[agentes[tid].idCorpo].y; //item que o agente tem em mãos
			}
			else if(objetivo == 1){
				itemAlvoX = dados[matriz[agentes[tid].linhaAtual][agentes[tid].colunaAtual]].x; //item que o agente está
            	itemAlvoY = dados[matriz[agentes[tid].linhaAtual][agentes[tid].colunaAtual]].y;
			}
            if(matriz[auxX][auxY] != -1){
                corpoX = dados[matriz[auxX][auxY]].x; //item do for
                corpoY = dados[matriz[auxX][auxY]].y; //item do for
                distancia = pow(fabs(itemAlvoX-corpoX), 2) + pow(fabs(itemAlvoY-corpoY), 2); //distancia euclidiana com 2 atributos.
                //printf ("itemX: %.2f, itemY: %.2f, corpoX: %.2f, corpoY: %.2f, dist: %.2f\n",itemAlvoX, itemAlvoY, corpoX, corpoY, distancia);
                //sqrt não tá funfando, não tá encontrando mesmo com a flag na lib, então tirei :/
            }
            else    
                distancia = 9999;
            
            //Tem sqrt por fora, tirei pq não tá encontrando (ref ñ definida), então dobrei o valor da distancia (virou 30*30), dps ver isso
            //se for abaixo de 30 já encaixa no que queremos, pois é maior que o caso sqrt(20²+20²)


            if(auxX != agentes[tid].linhaAtual || auxY != agentes[tid].colunaAtual ){
                //Pegando o mínimo da distância entre o agente e a célula para caso a célula dando a volta fique mais próxima do agente que no original.
                if(abs(agentes[tid].linhaAtual - k) < abs(agentes[tid].linhaAtual - auxX))
                    minX = abs(agentes[tid].linhaAtual - k);
                else
                    minX = abs(agentes[tid].linhaAtual - auxX);
                if(abs(agentes[tid].colunaAtual - j) < abs(agentes[tid].colunaAtual - auxY))
                    minY = abs(agentes[tid].colunaAtual - j);
                else
                    minY = abs(agentes[tid].colunaAtual - auxY);

                if(minX > minY) //aqui pode mantém k e j, posi são distâncias mesmo.
                    max = minX;
                else
                    max = minY;
                if(matriz[auxX][auxY] != -1){ //Se há item e são similares. Considerei caso 12²+12² para minimo de similaridade, por isso 300
                    if(qtdGrupos == 4 && distancia < 300){    //caso pro arquivo 1
                        soma += (raioVisao*2) * (pow(1/max,2)); 
                    }
                    else if (qtdGrupos == 15 && distancia < 0.5){ ////caso pro arquivo 2. Valor dist tá ok. Validado empíricamente.
                        soma += (raioVisao*2) * (pow(1/max,2)); 
                    }
                }
                somaMax += (raioVisao*2) * (pow(1/max,2)); //caso maximo de células com corpos.
            }
                
        }
    }
    //printf ("Relacao dentro: %.2f\n", soma/somaMax);
    return soma/somaMax;
}

// Aqui o protótipo. Escolher duas para implementar aqui dos 2 artigos.
int funcaoObjetivoPegar(int tid){
    //P = (k1 / k1 + f)^2
    /*Conclusões por enquanto:
        f = fração percebida de itens na vizinhança da formiga (considerar apenas itens similares como vizinhança do alvo)
        k1 = é uma constante
        ARTIGO ORIGINAL: where f is an estimation of the fraction of nearby points occupied by objects of the same type, and k1 is a constant.
            The probability thus decreases with f, from 1 (when f=0), to 1/4 (when f= k1), and less as f tends to 1.
        
        Ver com ele ser coparamos o valor daqui com um rand entre 0 e 1 gerado ou a relacao() mesmo - ver com Parpinelli

        Artigo original: https://www.researchgate.net/profile/Nigel_Franks/publication/235362107_The_dynamics_of_collective_sorting_robot-like_ants_and_ant-like_robots/links/02e7e52b98ed20ebab000000/The-dynamics-of-collective-sorting-robot-like-ants-and-ant-like-robots.pdf 
    */
    float const f = relacao(tid, 1); //verifico a relação da vizinhana (itens similares x não similares ou vazio) com o objetivo de pegar (1)
    if (relacao(tid,1) == 0) //caso extremo fazemos determinista, ele pega se relação é 0
        return 1;
    float const k1 = 0.1; //valor da constante - empírico

    //tipo: quando f = 0 -> P = 1, quando P= 1/ 4 -> f = k1, quando f = 1 -> P = menor que 1/4
    float const P = pow(k1/(k1 + f), 2);
    //Se k1 = 1: quando f = 0, P = 1; quando f = 0.25, P = 0.64; qdo f = 0.5, P = 0.44, qdo f = 0.75, P = 0.32, qdo f = 1, P = 0.25 (não faz sentido os dois ultimos) acho que para f = 1, pode resultar em P menor ou igual a 1/4, e o penultimo faz sentido porque f é menor que 1 entao P vai ser maior que 0.25
    //Se k1 = 0.2: quando f = 0, P = 1; qdo f = 0.25, P = 0.19; qndo f 0.75, P = 0.044; qdo f = 1, P = 0.02 (queremos mais alto)
    //Se k1 = 0.5: qndo f = 0, P = 1; qdo f = 0.25, P = 0.44; qdo f = 0.75, P=0.32; f=1, P = 0.11 (tá melhor, mas quero mais baixo acho)
    //Se k1 = 0.4: qndo f=0,p=1; qdo f = 0.25, p = 0.38; qdo f = 0.75, p = 0,12; qdo f = 1, p = 0.08;
    //VALOR DO ARTIGO K1 = 0.1

    int const auxGerado = rand()%101; //gera inteiro entre 0 e 100
    float const valorGerado = (float) auxGerado/100;
    //Quanto maior P, maior a probabilidade de pegar, eele varia de 0 a 1 (float). Então qualquer valor gerado menor que ele (gera de 0a 1 tbm), ele precisa pegar.
    if(valorGerado < P){ 
        //pega
        return 1;
    }else{
        //senão, não pega
        return 0;
    }
}

int funcaoObjetivoLargar(int tid){
    //P = (f / k2 + f)^2
    float const f = relacao(tid, 2); //verifico a relação da vizinhana (itens similares x não similares ou vazio) com o objetivo de pegar (1)
    if (relacao(tid,2) == 1) //caso extremo fazemos determinista, ele larga se relacao = 1
        return 1;
    float const k2 = 0.3; //valor da constante - empírico 
    //VALOR DO ARTIGO K2 = 0.3
    float const P = pow(f/(k2 + f), 2);
    //k = 0.4. Se f = 1, P = 0,71, se f = 0, P = 0; se f = 0.5, P  = 0.31
    //k = 0.3. Se f = 1, P = 0,59, se f = 0, P = 0; se f = 0.5, P = 0,39
    int const auxGerado = rand()%101; //gera inteiro entre 0 e 100
    float const valorGerado = (float) auxGerado/100; 
    //Quanto maior P, maior a probabilidade de pegar, eele varia de 0 a 1 (float). Então qualquer valor gerado menor que ele (gera de 0a 1 tbm), ele precisa pegar.
    if(valorGerado < P){ //pega
        return 1;
    }else{//não pega
        return 0;
    }
    //from 0 (when f=0), to lp $ = k), andmore as f tends to 1
}

void *executaAgentes(void * argp){ //tid é o número da thread/do agente, pois precisaremos para saber ql struct trabalhar

    int tid = (long)argp;
    int i = 0; //variável de auxílio para iteração.

    while(i<qtdIteracoes || agentes[tid].comCorpo == 1){ //enquanto estiver abaixo do limite ou com corpo. Quando as duas forem falsas, aí fechou.

        pthread_mutex_lock(&modificaMatriz);
        //Se estourou o limite, carrega um corpo e não tem corpo na célula atual:
        if( i >= qtdIteracoes && matriz[agentes[tid].linhaAtual][agentes[tid].colunaAtual] == -1){ 
            matriz[agentes[tid].linhaAtual][agentes[tid].colunaAtual] = agentes[tid].idCorpo;
            pthread_mutex_unlock(&modificaMatriz);
            agentes[tid].comCorpo = 0;
            agentes[tid].idCorpo = -1;
            break;
        }
        
        //Se estourou o limite, carrega um corpo e tem corpo na célula atual
        else if(i >= qtdIteracoes){
            pthread_mutex_unlock(&modificaMatriz);
            andarSoltar(agentes[tid].linhaAtual, agentes[tid].colunaAtual, tid);
        }
        //Condição normal, sem estourar o limite.
        else{
            pthread_mutex_unlock(&modificaMatriz);
            //Se o agente não está carregando um corpo
            if(agentes[tid].comCorpo == 0){
            
                //Se a casa atual tem um corpo
                pthread_mutex_lock(&modificaMatriz);
                if(matriz[agentes[tid].linhaAtual][agentes[tid].colunaAtual] != -1){     //agora qqlr valor diferente da matriz como 0 é porque há um corpo                   
                    //Aqui buscamos fazer uma ponderação baseada na distancia da células vizinhas, onde quanto mais perto mais importante ter ou não ter corpo.
                    if(funcaoObjetivoPegar(tid) == 1){ //se pego o item
                        agentes[tid].comCorpo = 1; //pegou o corpo
                        agentes[tid].idCorpo = matriz[agentes[tid].linhaAtual][agentes[tid].colunaAtual]; //pega o id do corpo e recebe no agente!
                        matriz[agentes[tid].linhaAtual][agentes[tid].colunaAtual] = -1; //célula da matriz agora está vazia                    
                        pthread_mutex_unlock(&modificaMatriz);
                    }else{ //não pego o item
                        pthread_mutex_unlock(&modificaMatriz);
                        andarCarregar(agentes[tid].linhaAtual, agentes[tid].colunaAtual, tid);         
                    }
                    
                }else{//Caso a célula atual não tenha um corpo. 
                    pthread_mutex_unlock(&modificaMatriz);
                    andarCarregar(agentes[tid].linhaAtual, agentes[tid].colunaAtual, tid);
                }
            }

            else{//Se o agente carrega um corpo
                pthread_mutex_lock(&modificaMatriz);
                if(matriz[agentes[tid].linhaAtual][agentes[tid].colunaAtual] == -1){//Se a casa atual está vazia
                    //Aqui buscamos fazer uma ponderação baseada na distancia da células vizinhas, onde quanto mais perto mais importante ter ou não ter corpo.
                    if(funcaoObjetivoLargar(tid) == 1){ //se larguei o corpo.
                        agentes[tid].comCorpo = 0; //largou o corpo
                        //aqui viria o lock do mudar matriz
                        //pthread_mutex_lock(&modificaMatriz);
                        matriz[agentes[tid].linhaAtual][agentes[tid].colunaAtual] = agentes[tid].idCorpo; //célula da matriz agora está com corpo  
                        agentes[tid].idCorpo = -1; //não carrega ninguém
                        //Assim passo o id do corpo (segurando no meu agente!)

                        pthread_mutex_unlock(&modificaMatriz); 
                        
                        //Tirei o andarCarregar para ele não andar na iteração que largou um corpo, para evitar pegar um já na sequencia e que pode ser ele mesmo.
                        //andarCarregar(agentes[tid].linhaAtual, agentes[tid].colunaAtual, tid);
                        
                    }else{  //não pôde largar o corpo, então pode andar
                        pthread_mutex_unlock(&modificaMatriz);
                        andarSoltar(agentes[tid].linhaAtual, agentes[tid].colunaAtual, tid);
                    }
                }else{//A casa não está vazia, então pode andar
                    pthread_mutex_unlock(&modificaMatriz);
                    andarSoltar(agentes[tid].linhaAtual, agentes[tid].colunaAtual, tid);
                }
            }
        }
        i++; //iterador
    }
    pthread_exit(NULL);
}

int main(int argc, char *argv[]){
    int i;

    //popularMatriz(); //função para colocar as formigas mortas na matriz
    printf("Corpos inicialmente dispostos na matriz:\n\n");
    
    //Para randomizar as escolhas durante a execução
    time_t t;
    srand((unsigned) time(&t));

    //Para fazer a leitura do arquivo
    leituraArquivo();
    popularMatrizDados();
    printMatriz();

    //Para calcular tempo de execução
    clock_t Ticks[2];
    double Tempo;
    Ticks[0] = clock();

    //iniciando threads:
    int rc;
    long d; 

    pthread_t *threads;
	threads = (pthread_t *)malloc(sizeof(pthread_t) * (qtdAgentes));
    if(threads==NULL){
            printf ("Erro na alocação das threads.\n");
            exit(1);
    }

    //Verificação da inicialização correta dos mutexes.
    if (pthread_mutex_init(&modificaMatriz,NULL)!=0){
            printf("Erro inicializando mutex t\n");
    }

    //Inicialização das threads
    for (i = 0; i < qtdAgentes; i++){
        d = i; //id thread;
            rc = pthread_create(&threads[i], NULL, executaAgentes, (void *) d); //tá gerando erro na executa agentes lá, talvez por não ter nada ou sla, mas não entendi
            if(rc != 0){
                printf ("ERRO - nao fatal = %d\n", rc);
            }
    }

    for (i = 0; i < qtdAgentes; i++){
		rc = pthread_join(threads[i], NULL);
	}

    //Finaliza tempos:
    Ticks[1] = clock();
    Tempo = (Ticks[1] - Ticks[0]) * 1.0/CLOCKS_PER_SEC;

    printf("\nCorpos dispostos na matriz ao final da execucao:\n\n");
    printMatriz();
    printf ("\nTempo de execucao: %.6lf segundos\n\n",Tempo);
}