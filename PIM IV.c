#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <string.h>
#include <time.h>

// Estruturas para guardar variáveis de usuário e paciente

struct dados_paciente { //dados
    char nome[250];
    char telefone[21];
    char cpf[15];
    char email[50];
    int diaNas, mesNas, anoNas; // Data de Nascimento
    int diaDiag, mesDiag, anoDiag; // Data do Diagnóstico
};

struct endereco_paciente { //endereço
    char rua[100];
    char numero[10];
    char bairro[50];
    char cidade[50];
    char estado[4];
    char cep [10];
};

struct comorbidade_paciente { //comorbidade
    int diabete, hipertensao, obesidade, tuberculose, outro;
    char tipoComorbidade[100];
};

struct paciente { //reúne todas as estruturas anteriores em uma uníca
    struct dados_paciente dados;
    struct endereco_paciente endereco;
    struct comorbidade_paciente comorbidade;
};

struct usuario {
    char user[50], senha[9];
};

// Definição de variáveis
int calc_dataNascimento(int diaAtual, int mesAtual, int anoAtual, int diaNas, int mesNas, int anoNas); // Calcula idade
int validarData(int dia, int mes, int ano, int anoAtual, int mesAtual, int diaAtual); //Validador de data (para data de nascimento e diagnóstico)
void limparBuffer(); // limpa o buffer para evitar espaços vazios causados pelo scanf
void lerStringNaoVazia(const char* prompt, char* destino, int tamanho); // para não permitir dados vazios (os inputs feito pelo usuário)
int lerOpcaoComorbidade(const char* prompt); // leitura da comorbidade do paciente

// início do programa
int main (){
    setlocale(LC_ALL, "Portuguese");

    // Definição da data atual
    time_t t = time(NULL);
    struct tm *data_atual = localtime(&t);
    int anoAtual = data_atual->tm_year + 1900;
    int mesAtual = data_atual->tm_mon + 1; // tm_mon é de 0-11
    int diaAtual = data_atual->tm_mday;

    // variáveis locais para funcionamento do programa
    int opcao = 0, idade;
    struct paciente p1;
    struct usuario u1; // Para o log
    char entradaMenu[20]; // Para o menu

    // Arquivos que serão utilizados
    FILE *users; // lista para log dos usuários
    FILE *lista; // lista de pacientes com seus dados
    FILE *risco; // lista de pacientes que foram identificados com risco

    // menu principal
    printf("\t\t\t\t  ===== UNNE-TRACK (PIM IV) =====\n");
    printf("\t\t\t   sistema de cadastro - pacientes com covid-19 \n\n");
    printf("\t\t\t\t\t##### LOGIN #####\n");
    printf("\t\t\t  (Usuário Padrão: admin | Senha Padrão: admin)\n");

    /*------------------------------------------------------------------------------------------------------------------------*/
                                                            // Usuário e Senha //

    const char* USER_CORRETO = "admin";
    const char* SENHA_CORRETA = "admin";
    char usuarioLogin[50];
    char senhaLogin[50];

    int loginSucesso = 0;

    do {
        // Usa a nova função para garantir que o login não seja vazio
        lerStringNaoVazia("\nUsuário: ", usuarioLogin, sizeof(usuarioLogin));
        lerStringNaoVazia("Senha: ", senhaLogin, sizeof(senhaLogin));

        // Compara o usuário e senha digitados com os corretos
        if (strcmp(usuarioLogin, USER_CORRETO) == 0 && strcmp(senhaLogin, SENHA_CORRETA) == 0) {
            printf("\nLogado com sucesso!\n");

            users = fopen("Log_Usuarios.txt", "a");
            if (users != NULL) {
                fprintf(users,"===========================================\n");
                fprintf(users,"Usuário: %s (Login em %02d/%02d/%d)\n", usuarioLogin, diaAtual, mesAtual, anoAtual); // caso o login dê certo, ele grava no log txt
                fclose(users);
            }
            loginSucesso = 1;
        } else {
            printf("[ERRO] Usuário ou senha inválidos. Tente novamente.\n"); // caso digite algo inválido
        }

    } while (loginSucesso == 0);

    /*------------------------------------------------------------------------------------------------------------------------*/

    // Loop do menu principal
    do{
        printf("\n\t\t\t\t    ==== LOBBY ====\n");
        printf("[1] Cadastro de Paciente\n");
        printf("[2] Visualizar Lista de Pacientes\n");
        printf("[3] Sair\n");
        printf("Escolha uma opção: ");

        if (fgets(entradaMenu, sizeof(entradaMenu), stdin) != NULL) {
            if (entradaMenu[0] == '\n') {
                opcao = 0;
            } else {
                opcao = atoi(entradaMenu);
            }
        }

        switch (opcao){
        case 1:
            lista = fopen("Lista_Pacientes.txt", "a");

            if (lista == NULL) {
                printf("[ERRO] Não foi possível abrir o banco de dados de pacientes!\n"); // caso o arquivo esteja vazio
                break;
            }

            /*------------------------------------------------------------------------------------------------------------------------*/
                                                                    //Nome//
            printf("\nSelecionado -> Cadastro de Paciente\n");
            // validação de campo
            lerStringNaoVazia("\nNome: ", p1.dados.nome, sizeof(p1.dados.nome));

            fprintf(lista,"===========================================\n");
            fprintf(lista,"Nome: %s\n", p1.dados.nome);
            /*------------------------------------------------------------------------------------------------------------------------*/
                                                              //Data de Nascimento//
            do {
                printf("Data de Nascimento (formato dd/mm/aaaa): ");
                // Validação de data
                if (scanf("%d/%d/%d", &p1.dados.diaNas, &p1.dados.mesNas, &p1.dados.anoNas) != 3) {
                    printf("Formato inválido! Use dd/mm/aaaa.\n");
                    limparBuffer();
                    continue; // Pula para a próxima iteração do loop
                }
                limparBuffer(); // Limpa o \n depois do scanf

                if (!validarData(p1.dados.diaNas, p1.dados.mesNas, p1.dados.anoNas, anoAtual, mesAtual, diaAtual)) {
                        printf("Digite uma data válida!\n");
                    }
                    else {
                        break;
                    }
                } while(1);

            idade = calc_dataNascimento(diaAtual, mesAtual, anoAtual, p1.dados.diaNas, p1.dados.mesNas, p1.dados.anoNas);

            fprintf(lista,"Data de Nascimento: %02d/%02d/%d // Idade: %d anos\n",
            p1.dados.diaNas, p1.dados.mesNas, p1.dados.anoNas, idade);

            /*------------------------------------------------------------------------------------------------------------------------*/
                                                                   //CPF//
            //validação
            lerStringNaoVazia("Cpf (apenas números): ", p1.dados.cpf, sizeof(p1.dados.cpf));
            fprintf(lista,"Cpf: %s\n", p1.dados.cpf);

            /*------------------------------------------------------------------------------------------------------------------------*/
                                                                 //Telefone//
            //validação
            lerStringNaoVazia("Telefone (formato 99 999999999): ", p1.dados.telefone, sizeof(p1.dados.telefone));
            fprintf(lista,"Telefone: %s\n", p1.dados.telefone);

            /*------------------------------------------------------------------------------------------------------------------------*/
                                                                 //E-Mail//
            // *** VALIDAÇÃO DE CAMPO NÃO VAZIO ***
            lerStringNaoVazia("E-mail: ", p1.dados.email, sizeof(p1.dados.email));
            fprintf(lista,"Email: %s\n", p1.dados.email);

            /*------------------------------------------------------------------------------------------------------------------------*/
                                                                //Endereço//
            printf("\n* Endereço\n");
            //validação
            lerStringNaoVazia("Cep: ", p1.endereco.cep, sizeof(p1.endereco.cep));
            lerStringNaoVazia("Rua: ", p1.endereco.rua, sizeof(p1.endereco.rua));
            lerStringNaoVazia("Bairro: ", p1.endereco.bairro, sizeof(p1.endereco.bairro));
            lerStringNaoVazia("Cidade: ", p1.endereco.cidade, sizeof(p1.endereco.cidade));
            lerStringNaoVazia("Estado (somente sigla, ex: PA): ", p1.endereco.estado, sizeof(p1.endereco.estado));
            lerStringNaoVazia("Número: ", p1.endereco.numero, sizeof(p1.endereco.numero));

            fprintf(lista,"Cep: %s\n", p1.endereco.cep);
            fprintf(lista,"Bairro: %s\n", p1.endereco.bairro);
            fprintf(lista,"Cidade: %s\n", p1.endereco.cidade);
            fprintf(lista,"Estado: %s\n", p1.endereco.estado);
            fprintf(lista,"Rua: %s\n", p1.endereco.rua);
            fprintf(lista,"Nº: %s\n", p1.endereco.numero);

            /*------------------------------------------------------------------------------------------------------------------------*/
                                                                //Comorbidade//
            printf("\n\nComorbidade\n");

            //validação
            p1.comorbidade.diabete = lerOpcaoComorbidade("Possui Diabetes? \n[1] Sim | [0] Não: ");
            p1.comorbidade.hipertensao = lerOpcaoComorbidade("Possui Hipertensão? \n[1] Sim | [0] Não: ");
            p1.comorbidade.obesidade = lerOpcaoComorbidade("Possui Obesidade? \n[1] Sim | [0] Não: ");
            p1.comorbidade.tuberculose = lerOpcaoComorbidade("Possui Tuberculose? \n[1] Sim | [0] Não: ");
            p1.comorbidade.outro = lerOpcaoComorbidade("Possui outro tipo de comorbidade?\n[1] Sim | [0] Não: ");

            // zera o campo caso não haja outro tipo de comorbidade
            strcpy(p1.comorbidade.tipoComorbidade, "");

            if (p1.comorbidade.outro == 1){
                //validação
                lerStringNaoVazia("Digite a comorbidade do paciente: ", p1.comorbidade.tipoComorbidade, sizeof(p1.comorbidade.tipoComorbidade));
            }

            fprintf(lista, "Comorbidades: ");
            char *nomes[] = {"Diabetes", "Hipertensão", "Obesidade", "Tuberculose"};
            int valores[] = {p1.comorbidade.diabete, p1.comorbidade.hipertensao,
                             p1.comorbidade.obesidade, p1.comorbidade.tuberculose};
            int primeira = 1;
            int temComorbidade = 0;

            for (int i = 0; i < 4; i++) {
                if (valores[i] == 1) {
                    if (!primeira) fprintf(lista, ", ");
                    fprintf(lista, "%s", nomes[i]);
                    primeira = 0;
                    temComorbidade = 1;
                }
            }

            //corrige erro do input do usuário
            if (p1.comorbidade.outro == 1 && strlen(p1.comorbidade.tipoComorbidade) > 0) {
                if (!primeira) fprintf(lista, ", ");
                fprintf(lista, "%s", p1.comorbidade.tipoComorbidade);
                primeira = 0;
                temComorbidade = 1;
            }

            if (primeira) fprintf(lista, "Nenhuma");
            fprintf(lista, "\n");

            /*------------------------------------------------------------------------------------------------------------------------*/
                                                                //Diagnóstico//
            do {
                printf("Data do diagnóstico (formato dd/mm/aaaa): ");
                if (scanf("%d/%d/%d", &p1.dados.diaDiag, &p1.dados.mesDiag, &p1.dados.anoDiag) != 3) {
                    printf("Formato inválido! Use dd/mm/aaaa.\n");
                    limparBuffer();
                    continue;
                }
                limparBuffer();

                if (!validarData(p1.dados.diaDiag, p1.dados.mesDiag, p1.dados.anoDiag, anoAtual, mesAtual, diaAtual)) {
                        printf("Digite uma data válida!\n");
                    }
                    else {
                        break;
                    }
                } while(1);

            fprintf(lista,"Data do Diagnóstico: %02d/%02d/%d\n", p1.dados.diaDiag, p1.dados.mesDiag, p1.dados.anoDiag);

            /*------------------------------------------------------------------------------------------------------------------------*/
                                                                // Grupo de risco//

            if (idade > 65 || temComorbidade == 1) {
                printf("\nPaciente identificado como GRUPO DE RISCO.\n");
                risco = fopen("Pacientes_Risco.txt", "a");

                if (risco == NULL) {
                    printf("[ERRO] Não foi possível abrir o arquivo de risco!\n");
                } else {
                    fprintf(risco, "===========================================\n");
                    fprintf(risco, "PACIENTE EM GRUPO DE RISCO\n");
                    fprintf(risco, "Nome: %s\n", p1.dados.nome);
                    fprintf(risco, "Idade: %d\n", idade);
                    fprintf(risco, "CEP: %s\n", p1.endereco.cep);

                    if (idade > 65) {
                        fprintf(risco, "Motivo(s): Idade (> 65 anos)\n");
                    }
                    if (temComorbidade == 1) {
                         fprintf(risco, "Motivo(s): Comorbidade(s)\n");
                    }
                    fclose(risco);
                    printf("Dados de CEP e Idade salvos em Pacientes_Risco.txt.\n");
                }
            }
            printf("\nPaciente cadastrado com sucesso!\n");
            fclose(lista); // Fecha o arquivo principal
            break;

        case 2: // Oopção de visualizar a lista dde pacientes no próprio programa (sem ter a necessidade de abrir o txt)
            lista = fopen("Lista_Pacientes.txt", "r");
                if (lista == NULL) {
                    printf("\nNenhum paciente cadastrado!\n");
                } else {
                    printf("\n--- LISTA DE PACIENTES CADASTRADOS ---\n");
                    char linha[300];
                    while (fgets(linha, sizeof(linha), lista)) {
                        printf("%s", linha);
                    }
                    fclose(lista);
                }
                break;
        case 3:
            break;

        default:
            printf("\n[ERRO] Escolha uma opção válida (1, 2 ou 3)!\n");
            break;
        }

    } while (opcao != 3);
        printf("\nVocê escolheu sair...");
    return 0;
}

// Funções
int calc_dataNascimento(int diaAtual, int mesAtual, int anoAtual, int diaNas, int mesNas, int anoNas) {
    int idade = anoAtual - anoNas;
    if (mesAtual < mesNas || (mesAtual == mesNas && diaAtual < diaNas)) {
        idade--;
    }
    return idade;
}

int validarData(int dia, int mes, int ano, int anoAtual, int mesAtual, int diaAtual) { // validação de data
    if (ano < 1900 || ano > anoAtual) return 0;
    if (mes < 1 || mes > 12) return 0;
    if (dia < 1 || dia > 31) return 0;

    if (ano == anoAtual) {
        if (mes > mesAtual) return 0;
        if (mes == mesAtual && dia > diaAtual) return 0;
    }

    int diasNoMes[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}; // define a data máxima para cada mês
    if ((ano % 4 == 0 && ano % 100 != 0) || (ano % 400 == 0)) {
        diasNoMes[2] = 29; // fevereiro caso seja ano bissexto
    }
    if (dia > diasNoMes[mes]) return 0;

    return 1; // Data é válida
}

void limparBuffer(){ // remove \n do scanf
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void lerStringNaoVazia(const char* prompt, char* destino, int tamanho) {
    while (1) {
        printf("%s", prompt);
        if (fgets(destino, tamanho, stdin) != NULL) {
            destino[strcspn(destino, "\n")] = '\0';
            if (strlen(destino) > 0) {
                break; // Sucesso, sai do loop
            }
        }
        printf("[ERRO] Este campo não pode ficar em branco. Tente novamente.\n");
    }
}

int lerOpcaoComorbidade(const char* prompt) {
    char entrada[10];
    while (1) {
        printf("%s", prompt);
        if (fgets(entrada, sizeof(entrada), stdin) != NULL) {
            entrada[strcspn(entrada, "\n")] = '\0'; // Remove o \n

            if (strcmp(entrada, "0") == 0) {
                return 0; // Entrada válida
            }
            if (strcmp(entrada, "1") == 0) {
                return 1; // Entrada válida
            }
        }
        printf("[ERRO] Digite apenas 0 (Não) ou 1 (Sim). Tente novamente.\n");
    }
}
