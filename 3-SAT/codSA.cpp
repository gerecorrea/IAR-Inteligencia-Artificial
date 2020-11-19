#include <bits/stdc++.h>
#include <iostream>
#include <cstdio>
#include <cstring>
#include <cstdlib>


/* Anotações
	Dupla: Beatriz e Geremias
	
	Feito:
		Ambientação do problema, imposição da variáveis do problema
		Tudo principal por cmd até então
		Configuração aleatório do ambiente e da geração de vizinhos
		Taxa de decaimento conforme a fórmula de resfriamento optada
		Fazendo automatizadamente 10 testes, com impressão dos dados de média e desvio padrão
		Gerando um arquivo de saída, formato .xlsx, para cada teste executado.  Pode ser modificado na variável "formato"
		Está escalável para claúsulas diferentes de 3, pois lemos e criamos a quantidade de variáveis dentro da claúsula a partir do arquivo

	Compilação e execução exemplo:
	$ g++ -Wall -O3 -o e codSA.cpp
	$ ./e uf20-91-01.cnf saida 0.5 0.0001

*/

using namespace std;

//Variáveis globais default e da documentação
double temperatura_inicial = 5; //default
double temperatura_final = 0.0001; //default
double qtd_iteracoes = 250000; //quantidade descrita na documentação - 250k
int numero_execucoes = 10; //quantidade descrita na documentação

//Para geração de configurações inteiramente aleatorias (dar uma olhada melhor nisso aqui dps)
//motor de geração aleatoria - mersenne_twister_engine, este para 64 bits.
random_device randomm;
mt19937_64 engine{randomm()};
uniform_real_distribution<double> distribuicaoBool{0.0, 1.0}; //realiza uma distribuição uniforme de valores entre 0 a 1 (0.00 a 1.00).

//Struct que representa as clausulas, armazenando as variáveis e as cláusulas satisfeitas
struct Clausula {
    vector<int> variaveis_clausula; //representa as 3 variáveis de uma cláusula

	//Construtor de uma cláusula, cujo recebe um vetor de inteiros (com 3 variaveis)
    Clausula(vector<int> &vecVar){
        variaveis_clausula = vecVar;
    }

	//Função que dado a configuração atual de todas as variaveis  e as variaveis que pertencem a essa clausula, verifico se ao menos uma é satisfeita 
	int verifica_satisfatoriedade(vector<bool> &configuracao_atual){
		bool satisfeito = false;
		for(int i=0; i<variaveis_clausula.size(); i++){
			if(variaveis_clausula[i] > 0){  //caso a variavel de posição i seja positiva
				if(configuracao_atual[variaveis_clausula[i]-1] == 1) //verifico se a configuração atual tem ela positiva, se sim, satisfaz e retorno true
					return true;

			} else { //caso a variável de posição i seja negativa
				if(configuracao_atual[abs(variaveis_clausula[i]-1)] == 0) //verifoc se a configuração atual tem ela negativa, se sim, satisfaz e retorno false.
					return true;
			}
		}
		return false; //a configuração não é satisfeita, retorno false.
	}
};


//Fórmula de resfriamento (a optada foi a 7, dados os resultados etc):
double formulaResfrimento (int iteracao_atual){
	double T0_Tn = temperatura_inicial - temperatura_final;
	double cosh_10i_N = cosh((10.0 * iteracao_atual) / qtd_iteracoes);
	double Tn =  temperatura_final;
	return (T0_Tn/cosh_10i_N) + Tn;
}

//Função que verificar quantas claúsulas das totais dado a configuração atual são satisfeitas
int verificaQuantidadeClausulasSatisfeitas(vector<Clausula> &Clausulas, vector<bool> &configuracao_atual){
	int satisfeitas = 0;
	//Itero sobre cada clausula sat, verificando se ao menos uma das variáveis dessa clausula é satisfeita, se sim, qtd de satisfeitas (res) é incrementado
	for(Clausula i : Clausulas){
		if(i.verifica_satisfatoriedade(configuracao_atual)){
			satisfeitas++;
		}
	}
	return satisfeitas;
}

vector<bool> gerarNovaConfiguracaoVariveis(vector<bool> &configuracao_atual){
	vector<bool> vector_auxiliar;
	
	for(int i = 0; i < configuracao_atual.size(); i++){
		double aleatorio = distribuicaoBool(engine);
		if(aleatorio < 0.0005){ //chance de 0,5% de uma mudar a configuração de cada variável, basicamente
			vector_auxiliar.push_back(!configuracao_atual[i]);
		} else {
			vector_auxiliar.push_back(configuracao_atual[i]);
		}
	}
	return vector_auxiliar;
}

int main(int argc, char const *argv[]) {
    int numero_variaveis; //quantidade de variáveis do sistema (arquivo de entrada)
	int numero_clausulas; //quantidade de cláusulas do sistema (arquivo de entrada)
	ifstream arquivo_entrada; //arquivo de entrada
	ofstream arquivo_saida; // arquivo de saída
	string s;
	int i;

	//Variáveis de melhores soluções:
	int melhor_solucao_clausulas_satisfeitas;
	vector<bool> melhor_solucao_configuracao_variaveis;

	//variaveis de estatistica total:
	int vetor_solucoes[numero_execucoes];
	int vetor_qtd_iteracoes[numero_execucoes];
	double soma = 0;
	double media;
	double desvio_padrao;
	double somaIteracoes = 0;
	double mediaIteracoes;

	//Verificamos se todos os parâmetros foram devidamente informados, se não retorna a descirção correta
	if (argc != 5){
		printf("4 argumentos: <arquivo_entrada> <arquivo_saída_sem_formato> <temperatura_inicial> <temperatura_final> \n");
		return 1;
	}

	//temperatura_inicial = atof(argv[3]);
	//temperatura_final = atof(argv[4]);
	sscanf(argv[3], "%lf", &temperatura_inicial);
	sscanf(argv[4], "%lf", &temperatura_final);

	// Leitura da entrada:
	arquivo_entrada.open(argv[1]);
	if (!arquivo_entrada.is_open()){
		printf("Erro na abertura do arquivo \"%s\" para entrada\n", argv[1]);
	}

	//Dados para saída.
	string arqSaida;
	string formato = ".xlsx";

	arquivo_entrada >> s >> s; //pega o "p" e o "cnf" do arquivo de arquivo_entrada
	arquivo_entrada >> numero_variaveis >> numero_clausulas; //pega o número de variáveis e clausulas do arquivo de entrada.
	vector<Clausula> Clausulas; //Declaro um vector de cláusulas.
    
	//For que interage sobre todas as clasulas
	for(i = 0; i < numero_clausulas; i++){ 
        vector<int> vet; //vector de inteiros para inserção das variáveis e seu valores (pos ou negação tá no próprio numero ser pos ou neg)
        for(int j = 0; j < 3; j++){
            int x;
            arquivo_entrada >> x; //leio uma variável da clausula atual.
            vet.push_back(x); //insiro a variavel no meu vetor de inteiros
        }
		int t;
        arquivo_entrada >> t; // leio o 0 (serve como ^)
		Clausulas.push_back(Clausula(vet)); //Insiro esse meu vetor de 3 inteiros como uma claúsulas no meu vector de cláusulas
    }
	arquivo_entrada.close();
	
	//Variáveis de iteração comum e que variam constantemente
	double temperatura_atual; //A variável temperatura recebe inicialmente a temperatura inciial.
	vector<bool> configuracao_atual;
	int clausulas_satisfeitas;

	//Laço para geração de todos os resultados, baseado na quantidade de execuções para uma mesma entrada.
	for(int j=0; j<numero_execucoes; j++){
		
		//Reescrevo as variáveis com os valores de configurações iniciais
		temperatura_atual = temperatura_inicial;
		configuracao_atual.clear(); //limpo a configuração atual do vetor
		
		//Geração da configuração inicial aleatória
		// = init(numero_variaveis); //Crio um vector de configurações chamando uma função de início aleatorio. Obs: podemos deixar no main, sem função.
		for(i = 0; i < numero_variaveis; i++){
			double aleatorio = distribuicaoBool(engine); //gero um valor float entre 0 e 1.
			if(aleatorio >= 0.5) //Se for maior que 0.5, gravo como afirmação da variável (1)
				configuracao_atual.push_back(1);
			else //Se for menor, gravo como negação da variável (0)
				configuracao_atual.push_back(0);
		}
		i = 1;
		clausulas_satisfeitas = verificaQuantidadeClausulasSatisfeitas(Clausulas, configuracao_atual); //Verifica a quantidade de clausulas satisfeitas e atribui o valor a variavel
		melhor_solucao_clausulas_satisfeitas = clausulas_satisfeitas;
		melhor_solucao_configuracao_variaveis = configuracao_atual;
		//fim reinicialização dos valores das variáveis para uma nova execução
		
		arqSaida = "/0"; //garantir que fique vazia
		arqSaida = argv[2] + to_string(j) + formato; //altero o nome para saída.
		//cout << arqSaida << endl;
		arquivo_saida.open(arqSaida);

		// LAÇO PRINCIPAL, EXECUTA ENQUANTO A TEMPERATURA NÃO CHEGAR AO LIMITE (OU AS CLAÚSULAS NÃO ESTIVEREM TODAS SATISFEITAS)
		do {
			temperatura_atual = formulaResfrimento(i); //chamo o resfriamento,dada a iteração atual
			vector<bool> nova_configuracao = gerarNovaConfiguracaoVariveis(configuracao_atual); //gera uma nova configuração das variáveis e atribui ao vector nova_configuracao.
			int nova_qtd_clausulas_satisfeitas = verificaQuantidadeClausulasSatisfeitas(Clausulas, nova_configuracao); //Chamo a função que verifica a quantidade de clausulas satisfeitas com a atual configuração de variaveis, atribui a qtd a variavel ret
			double delta = nova_qtd_clausulas_satisfeitas - clausulas_satisfeitas; //delta de retorno de positivo ou negativo a nova configuração 
			if(delta > 0){ //Se a nova configuração é mais satisfatoria em clausula, atribuo ela
				configuracao_atual = nova_configuracao;
				clausulas_satisfeitas = nova_qtd_clausulas_satisfeitas;
			} else { //Caso não seja satisfatorio
				//Calculo da função de probabilidade. No pseucodódigo2222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222
				double prob = exp(delta / temperatura_atual); //e^(-delta/Temperatura atual). delta é obrigatoriamente negativo aqui, por isso não precisa -delta? Muda de acordo com o objetivo de max ou min
				//double prob = exp(-delta / temp); //ou assim?

				//gero um valor aleatorio de probabilidade de pegar ela ou não:
				double x = distribuicaoBool(engine); //x é o valor entre 0 a 1 gerado aleatoriamente.
				if(x <= prob){ //Se o valor gerado for menor que a probabilidade calculada, ele pega.
					configuracao_atual = nova_configuracao;
					clausulas_satisfeitas = nova_qtd_clausulas_satisfeitas;
				}
			}
		
			//Se a qtd de clausulas satisfeitas é maior que a grava na solução final atual, então troca.
			if(clausulas_satisfeitas > melhor_solucao_clausulas_satisfeitas){
				melhor_solucao_clausulas_satisfeitas = clausulas_satisfeitas;
				melhor_solucao_configuracao_variaveis = configuracao_atual;
			}
			
			//printf("%d %d\n", i, clausulas_satisfeitas); //printa a interação atual e qtd clausulas satisfeitas
			arquivo_saida << i << " " << clausulas_satisfeitas << "\n"; //salva no arquivo iteração atual e quantas cláusulas satisfeitas nela. Precisa salvar em outr formato

			if(clausulas_satisfeitas == numero_clausulas){//Caso esteja totalmente satisfeito, encerra a busca.
				cout << "Completamente satisfeito com " << i << " iterações" << endl;
				break; 
			}

			if(i == qtd_iteracoes){
				cout << "Limite de iterações atingido! Encerrando" << endl;
				break;
			}

			i++;
		}while(temperatura_atual > temperatura_final); 
		if(i == qtd_iteracoes)
			somaIteracoes += qtd_iteracoes;
		else
			somaIteracoes += i - 1;//pois nesse caso ele deu um i++ extra!

		//saida << "\n"; //pula duas linhas ao fim da adição de cada teste, para facilitar a identificação dos testes.
		arquivo_saida.close(); //fecho, no arquivo de saída bote apenas o do primeiro no caso.

		//GERAÇÃO DA SAÍDA DOS DADOS NO TERMINAL:
		cout << endl << "###### GERAÇÃO DE DADOS DE SAÍDA DA EXECUÇÃO " << j << " ######" << endl;
		cout << "Temperatura inicial: " << temperatura_inicial << endl << "Temperatura final limite: " << temperatura_final << endl; 
		cout << "Temperatura ao final da execução: " << temperatura_atual << endl;
		cout << "Limite de iterações: " << qtd_iteracoes << endl;
		cout << "Quantidade de iterações realizadas: " << i << endl;
		cout << "Satisfeitas " << melhor_solucao_clausulas_satisfeitas << " de " << numero_clausulas << " clausulas." << endl << endl;

		cout << "Configuração das variáveis final: " << endl;
		for (i = 0; i < melhor_solucao_configuracao_variaveis.size(); i++){
			if(i%7==0)
				cout << endl << " ";
			if(melhor_solucao_configuracao_variaveis[i] == 1)
				cout << " x" << i+1 << ";\t";
			else if(melhor_solucao_configuracao_variaveis[i] == 0)
				cout << "~x" << i+1 << ";\t";
		} 
		cout << endl << endl << "######################################################\n" << endl;
		cout << endl;

		vetor_solucoes[j] = melhor_solucao_clausulas_satisfeitas;
		soma += melhor_solucao_clausulas_satisfeitas;
		vetor_qtd_iteracoes[j] = qtd_iteracoes;

		
	}

	//Dados estatísticos:
	cout << "Saída dos dados estáticos para os " << numero_execucoes << " testes:" << endl;
	media = soma/numero_execucoes;
	cout <<  "media:" << media << endl;
	double somatorioDesvio = 0;
	int max = vetor_solucoes[0], indice = 0, qtd_iter; //para pegar o melhor teste
	for(i=0;i<numero_execucoes;i++){
		somatorioDesvio += pow(vetor_solucoes[i] - media, 2);
		//Para pegar a melor execução:
		if(vetor_solucoes[i] > max){
			max = vetor_solucoes[i];
			indice = i;
			qtd_iter = vetor_qtd_iteracoes[i];
		}
		if(vetor_solucoes[i] == max){
			if(vetor_qtd_iteracoes[i] < qtd_iter){
				max = vetor_solucoes[i];
				indice = i;
				qtd_iter = vetor_qtd_iteracoes[i];
			}
		}
	}
	desvio_padrao = somatorioDesvio / numero_execucoes;

	
	cout << "Desvio padrão: " << desvio_padrao << endl;

	mediaIteracoes = somaIteracoes / numero_execucoes;
	cout << "Média de iterações realizadas: " << mediaIteracoes << endl;
	
	cout << "Melhor teste é o teste " << indice << " com " << max << " de " << numero_clausulas << " cláusulas satisfeitas." << endl; 

	cout << endl << "Gerados " << numero_execucoes << " arquivos de saída no formato " << formato << " com as iterações atuais e a quantidades de cláusulas satisfeitas em cada iteração." << endl;
	
	cout << endl << "#####################################################\n" << endl;

    return 0;
}
