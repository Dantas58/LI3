#include "../Includes/catalog.h"
#include "../Includes/rides.h"
#include "../Includes/print_outpus.h"

#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <ctype.h>

typedef struct catalog {
    CATALOG_DRIVERS *cd;
    CATALOG_USERS *cu;
    CATALOG_RIDES *cr;
} Catalog;


Catalog *create_Catalog(char *entry_files) {
    Catalog *c = malloc(sizeof(struct catalog));
    c->cd = create_Catalog_Driver(entry_files);
    c->cu = create_Catalog_User(entry_files);
    c->cr = create_Catalog_Ride(entry_files, c->cd, c->cu);
    return c;
}

void delete_Catalog(Catalog *c) {
    delete_Catalog_Driver(c->cd);
    delete_Catalog_User(c->cu);
    delete_Catalog_Ride(c->cr);
    free(c);
}

//função responsável por realizar a query1
char *query1(char *username_or_id, Catalog *c) {

    int user_or_driver = 0;
    for (int i = 0; username_or_id[i] != '\0'; i++) {
        if (isalpha(username_or_id[i])) {
            user_or_driver = 1;
            break;
        }  // if user == 1 is USER
    }

    char *output;
    if (user_or_driver == 1) output = perfil_User(c->cu, username_or_id);
    else {
        unsigned int id = (int) strtol(username_or_id, (char **) NULL, 10);
        output = perfil_Driver(c->cd, id);
    }

    return output;
}

//função responsável por realizar a query2
char **query2(char *number_of_drivers, Catalog *c, int *N) {

    *N = (int) strtol(number_of_drivers, (char **) NULL, 10);
    char **output = highestRated_Drivers(c->cd, *N);

    return output;
}

//função responsável por realizar a query3
char **query3(char *number_of_users, Catalog *c, int *N) {

    *N = (int) strtol(number_of_users, (char **) NULL, 10);
    char **output = highestDistance_Users(c->cu, *N);

    return output;
}

//função responsável por realizar a query4
char *query4(char *city, Catalog *c) {
    char *output = average_Price_City(c->cr, c->cd, city);
    return output;
}

//função responsável por realizar a query5
char *query5(char *dateA, char *dateB, Catalog *c) {
    char *output = average_Price_Timespan(c->cr, dateA, dateB);
    return output;
}

//função responsável por realizar a query6
char *query6(char *city, char *dateA, char *dateB, Catalog *c) {
    char *output = average_Distance_CityAndTimespan(c->cr, city, dateA, dateB);
    return output;
}

//função responsável por realizar a query7
char **query7(char *number_of_drivers, char *city, Catalog *c, int *N) {

    *N = (int) strtol(number_of_drivers, (char **) NULL, 10);
    char **output = highestRated_City_Drivers(c->cr, c->cd, *N, city);

    return output;
}

//função responsável por realizar a query8
char **query8(char *gender, char *age, Catalog *c, int *N) {

    int age_int = (int) strtol(age, (char **) NULL, 10);
    char **answer = list_by_GenderAndAccountAge(c->cr, c->cd, c->cu, gender, age_int, N);

    return answer;
}

//função responsável por realizar a query9
char **query9(char *dateA, char *dateB, Catalog *c, int *N) {
    char **answer = list_by_TipAndTimespan(c->cr, dateA, dateB, N);
    return answer;
}


//função responsável por dar parse a uma linha, chamando a query respetiva
void parseQueries(char *line, Catalog *c, int output_number) {

    int i = (int) strtol(strsep(&line, " "), (char **) NULL, 10);

    switch (i) {
        case 1: {
            char *username_or_id = strsep(&line, " ");
            printToFile_OneLine(query1(username_or_id, c), output_number);
            break;
        }
        case 2: {
            char *number_of_drivers = strsep(&line, " ");
            int N;
            char **answer = query2(number_of_drivers, c, &N);
            printToFile_MultipleLines(answer, N, output_number);
            break;
        }
        case 3: {
            char *number_of_users = strsep(&line, " ");
            int N;
            char **answer = query3(number_of_users, c, &N);
            printToFile_MultipleLines(answer, N, output_number);
            break;
        }
        case 4: {
            char *city = strsep(&line, " ");
            printToFile_OneLine(query4(city, c), output_number);
            break;
        }
        case 5: {
            char *dateA = strsep(&line, " ");
            char *dateB = strsep(&line, " ");
            printToFile_OneLine(query5(dateA, dateB, c), output_number);
            break;
        }
        case 6: {
            char *city = strsep(&line, " ");
            char *dateA = strsep(&line, " ");
            char *dateB = strsep(&line, " ");
            printToFile_OneLine(query6(city, dateA, dateB, c), output_number);
            break;
        }
        case 7: {
            char *number_of_drivers = strsep(&line, " ");
            char *city = strsep(&line, " ");
            int N;
            char **answer = query7(number_of_drivers, city, c, &N);
            printToFile_MultipleLines(answer, N, output_number);
            break;
        }
        case 8: {
            char *gender = strsep(&line, " ");
            char *age = strsep(&line, " ");
            int N;
            char **answer = query8(gender, age, c, &N);
            printToFile_MultipleLines(answer, N, output_number);
            break;
        }
        case 9: {
            char *dateA = strsep(&line, " ");
            char *dateB = strsep(&line, " ");
            int N;
            char **answer = query9(dateA, dateB, c, &N);
            printToFile_MultipleLines(answer, N, output_number);
            break;
        }
    }
}

/* função responsável por abrir o ficheiro de input com as queries
 * chama a função parseQueries para dar parse a cada linha */
int run_Normal(char **input_file) {

    Catalog *c = create_Catalog(input_file[1]);

    system("exec rm -rf Resultados/*");

    FILE *fp;
    fp = fopen(input_file[2], "r");
    if (!fp) {
        perror("Error");
        return -1;
    }

    char *line = NULL;
    size_t len = 0;
    int output_number = 1;

    while (getline(&line, &len, fp) != -1) {
        line[strcspn(line, "\n")] = 0;
        parseQueries(line, c, output_number);
        output_number++;
    }

    free(line);
    fclose(fp);

    delete_Catalog(c);

    return 0;
}


int compare(int output_number) {

    char produced_file[100];
    snprintf(produced_file, 100, "Resultados/command%d_output.txt", output_number);

    char expected_file[100];
    snprintf(expected_file, 100, "Resultados_Esperados/command%d_output.txt", output_number);

    FILE *fp_produced, *fp_expected;
    fp_produced = fopen(produced_file, "r");
    if (!fp_produced) {
        perror("Error");
        return -1;
    }
    fp_expected = fopen(expected_file, "r");
    if (!fp_expected) {
        perror("Error expected");
        return -1;
    }


    char *line_produced = NULL;
    size_t len_produced;
    char *line_expected = NULL;
    size_t len_expected;
    int verify = 1;

    while (getline(&line_produced, &len_produced, fp_produced) != -1) {
        getline(&line_expected, &len_expected, fp_expected);
        if (strcmp(line_produced, line_expected) != 0) {
            verify = 0;
            break;
        }
    }

    fclose(fp_produced);
    fclose(fp_expected);

    if (verify == 0)
        printf("O output do comando %d não é o output esperado!\n", output_number);
    else printf("O output do comando %d é igual ao output esperado!\n", output_number);

    return 0;
}

int run_Test(char **input_file) {

    Catalog *c = create_Catalog(input_file[1]);

    system("exec rm -rf Resultados/*");

    FILE *fp;
    fp = fopen(input_file[2], "r");
    if (!fp) {
        perror("Error");
        return -1;
    }

    char *line = NULL;
    size_t len = 0;
    int output_number = 1;

    clock_t start, end;
    double cpu_time_used;

    while (getline(&line, &len, fp) != -1) {
        line[strcspn(line, "\n")] = 0;
        start = clock();
        parseQueries(line, c, output_number);
        end = clock();
        cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
        printf("O comando %d demorou: %f segundos\n", output_number, cpu_time_used);
        compare(output_number);
        output_number++;
    }

    free(line);
    fclose(fp);

    delete_Catalog(c);

    return 0;
}


int run_Interactive() {

    printf("Indique o caminho para a pasta onde se encontram os ficheiros de entrada!\n");

    char input[20];

    fgets(input, sizeof(input), stdin);
    input[strcspn(input, "\n")] = 0;

    Catalog *c = create_Catalog(input);

    printf("\n");

    printf("  ___                   _         _       __     ___ _    _        _ \n"
           " | _ ) ___ _ __ _____ _(_)_ _  __| |___   \\_\\_  | _ (_)__| |___ __| |\n"
           " | _ \\/ -_) '  \\___\\ V / | ' \\/ _` / _ \\ / _` | |   / / _` / -_|_-<_|\n"
           " |___/\\___|_|_|_|   \\_/|_|_||_\\__,_\\___/ \\__,_| |_|_\\_\\__,_\\___/__(_)\n");


    int cont = 0;

    while (cont == 0) {
        printf("Selecione um nũmero de 1 a 9 para escolher a query que quer executar ou 0 para sair!\n");
        char query[10];
        fgets(query, sizeof(query), stdin);
        int choice = (int) strtol(query, (char **) NULL, 10);
        switch (choice) {
            case 1: {
                printf("Escolha o ID do Driver ou Username do User!\n");
                char username_or_id[30];
                fgets(username_or_id, sizeof(username_or_id), stdin);
                username_or_id[strcspn(username_or_id, "\n")] = 0;
                printToTerminal_OneLine(query1(username_or_id, c));
                break;
            }
            case 2: {
                printf("Escolha o tamanho da lista de drivers com melhor avaliação média!\n");
                char number_of_drivers[10];
                fgets(number_of_drivers, sizeof(number_of_drivers), stdin);

                int N;
                char **answer = query2(number_of_drivers, c, &N);
                paginacaoAnswer(answer, N);

                break;
            }
            case 3: {
                printf("Escolha o tamanho da lista de users com maior distância viajada!\n");
                char number_of_users[10];
                fgets(number_of_users, sizeof(number_of_users), stdin);

                int N;
                char **answer = query3(number_of_users, c, &N);
                paginacaoAnswer(answer, N);

                for (int i = 0; i < N; i++) free(answer[i]);
                free(answer);
                break;
            }
            case 4: {
                printf("Escolha a Cidade!\n");
                char city[20];
                fgets(city, sizeof(city), stdin);
                city[strcspn(city, "\n")] = 0;
                printToTerminal_OneLine(query4(city, c));
                break;
            }
            case 5: {
                printf("Escolha duas datas no formato dia/mês/ano!\n");
                char dateA[20], dateB[20];
                fgets(dateA, sizeof(dateA), stdin);
                fgets(dateB, sizeof(dateB), stdin);
                dateA[strcspn(dateA, "\n")] = 0;
                dateB[strcspn(dateB, "\n")] = 0;
                printToTerminal_OneLine(query5(dateA, dateB, c));
                break;
            }
            case 6: {
                printf("Escolha a Cidade!\n");
                char city[20];
                fgets(city, sizeof(city), stdin);
                printf("Escolha duas datas no formato dia/mês/ano!\n");
                char dateA[20], dateB[20];
                fgets(dateA, sizeof(dateA), stdin);
                fgets(dateB, sizeof(dateB), stdin);
                city[strcspn(city, "\n")] = 0;
                dateA[strcspn(dateA, "\n")] = 0;
                dateB[strcspn(dateB, "\n")] = 0;
                printToTerminal_OneLine(query6(city, dateA, dateB, c));
                break;
            }
            case 7: {
                printf("Escolha a Cidade!\n");
                char city[20];
                fgets(city, sizeof(city), stdin);
                city[strcspn(city, "\n")] = 0;

                printf("Escolha o tamanho da lista de drivers com melhor avaliação média nessa cidade!\n");
                char number_of_drivers[10];
                fgets(number_of_drivers, sizeof(number_of_drivers), stdin);

                int N;
                char **answer = query7(number_of_drivers, city, c, &N);
                paginacaoAnswer(answer, N);

                break;
            }
            case 8: {
                printf("Escolha o Género!\n");
                char gender[5];
                fgets(gender, sizeof(gender), stdin);
                gender[strcspn(gender, "\n")] = 0;
                printf("Escolha a idade mínima dos perfis!\n");
                char idade[5];
                fgets(idade, sizeof(idade), stdin);

                int N;
                char **answer = query8(gender, idade, c, &N);

                paginacaoAnswer(answer, N);
                break;
            }
            case 9: {
                printf("Escolha duas datas no formato dia/mês/ano!\n");
                char dateA[20], dateB[20];
                fgets(dateA, sizeof(dateA), stdin);
                fgets(dateB, sizeof(dateB), stdin);
                dateA[strcspn(dateA, "\n")] = 0;
                dateB[strcspn(dateB, "\n")] = 0;

                int N;
                char **answer = query9(dateA, dateB, c, &N);

                paginacaoAnswer(answer, N);
                break;
            }
            case 0: {
                cont = 1;
                break;
            }

        }

    }

    delete_Catalog(c);

    return 0;
}