#define SIZE 1000000

#include "../Includes/users.h"
#include "../Includes/validator.h"
#include "../Includes/date.h"

#include <glib.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

// Estrutura dos Users
struct user {
    char *username;
    char *name;
    char *gender;
    int birth_date;
    int account_creation;
    char *pay_method;
    enum account_status account_status; //active ou inactive

    double rating_total;
    int rides_total;
    double spent_total;
    int distance_total;
    int most_recent_ride;
};

/* Estrutura do Catálogo de Users
 * Hashtable e Array (ordenado pela distãncia percorrida pelos users) */
struct catalog_users {
    GHashTable *users_hash;
    User** biggestDistance_array;
    int size, used;
};


// Função que dá free a um User e às suas variáveis
void delete_User(void *data) {
    User *u = (User *) data;
    free(u->username);
    free(u->name);
    free(u->gender);
    free(u->pay_method);
    free(u);
}

/* Função responsável por criar um User
 * Dá parse a uma linha (retirada do ficheiro de users)
 * Ao dar parse garante a validade dos dados */
User *create_User(char *line) {

    User *u = malloc(sizeof(struct user));
    char *token;
    int i = 0, verify = 1;

    while ((token = strsep(&line, ";")) != NULL) {
        switch (i++) {
            case 0:
                if (strlen(token) == 0) verify = 0;
                u->username = strdup(token);
                break;
            case 1:
                if (strlen(token) == 0) verify = 0;
                u->name = strdup(token);
                break;
            case 2:
                if (strlen(token) == 0) verify = 0;
                u->gender = strdup(token);
                break;
            case 3:
                u->birth_date = verify_Date(token);
                if (u->birth_date == 0) verify = 0;
                break;
            case 4:
                u->account_creation = verify_Date(token);
                if (u->account_creation == 0) verify = 0;
                break;
            case 5:
                if (strlen(token) == 0) verify = 0;
                u->pay_method = strdup(token);
                break;
            case 6:
                u->account_status = verify_AccountStatus(token);
                if (u->account_status == NoStatus) verify = 0;
                break;
        }
    }

    if (verify == 0) {
        delete_User(u);
        return NULL;
    }

    u->rating_total = 0;
    u->rides_total = 0;
    u->spent_total = 0;
    u->distance_total = 0;
    u->most_recent_ride = 0;
    return u;
}


void insert_User(CATALOG_USERS *cu, User* u){
    g_hash_table_insert(cu->users_hash, u->username, u);
    if (u->account_status==Active){
        if (cu->used == cu->size){
            cu->size += cu->size;
            cu->biggestDistance_array = realloc(cu->biggestDistance_array, cu->size * sizeof(struct user));
        }
        cu->biggestDistance_array[cu->used++] = u;
    }
}

// Função responsável por criar e preencher a HashTable dos Users
CATALOG_USERS *create_Catalog_User(char *entry_files) {

    FILE *fp;
    char open[50];
    strcpy(open, entry_files);
    fp = fopen(strcat(open, "/users.csv"), "r");
    if (!fp) {
        perror("Error opening users.csv");
        return NULL;
    }

    CATALOG_USERS *cu = malloc(sizeof(struct catalog_users));
    cu->users_hash = g_hash_table_new_full(g_str_hash, g_str_equal, NULL, delete_User);
    cu->biggestDistance_array = malloc(SIZE * sizeof (User*));
    cu->size = SIZE;
    cu->used = 0;


    char *line = NULL;
    size_t len = 0;

    clock_t start, end;
    double cpu_time_used;

    start = clock();
    while (getline(&line, &len, fp) > 0) {
        line[strcspn(line, "\n")] = 0;
        User *u = create_User(line);
        if (u != NULL) insert_User(cu, u);
    }
    end = clock();

    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Users parsed em: %f segundos\n", cpu_time_used);


    free(line);
    fclose(fp);

    return cu;
}

// Função responsável por dar free ao catálogo de Users
void delete_Catalog_User(CATALOG_USERS *cu) {
    g_hash_table_destroy(cu->users_hash);
    free(cu->biggestDistance_array);
    free(cu);
}

// Função responsável por atualizar valores de um User
void update_User(CATALOG_USERS *uc, char *username, double rating, int distance, double spent, int new_date) {

    User *u = g_hash_table_lookup(uc->users_hash, username);
    u->rating_total += rating;
    u->rides_total++;
    u->distance_total += distance;
    u->spent_total += spent;
    u->most_recent_ride = get_MostRecentDate(u->most_recent_ride, new_date);
}


int cmp_HighestDistance (const void * a, const void * b) {

    User *u1 = *(User **) a;
    User *u2 = *(User**) b;

    if (u1->distance_total < u2->distance_total) return 1;
    else if (u1->distance_total > u2->distance_total) return -1;
    else {
        int compare_dates = compare_Date(u1->most_recent_ride,
                                         u2->most_recent_ride);
        switch (compare_dates) {
            case 1:
                return -1;
            case 2:
                return 1;
            case 0:
                return strcmp(u1->username,u2->username);
        }
    }

    return 0;
}

void sort_DistanceArray_User(CATALOG_USERS *cu){
    qsort(cu->biggestDistance_array, cu->used, sizeof(User *), cmp_HighestDistance);
}


int get_AccountCreation_User(CATALOG_USERS *cu, char *username) {
    User *u = g_hash_table_lookup(cu->users_hash, username);
    return u->account_creation;
}

char *get_Name_User(CATALOG_USERS *cu, char *username) {
    User *u = g_hash_table_lookup(cu->users_hash, username);
    return u->name;
}

int verify_GenderAndActive_User(CATALOG_USERS *cu, char *username, char *gender) {
    User *u = g_hash_table_lookup(cu->users_hash, username);
    if (u->account_status == Inactive) return -1;
    char *verify_gender = u->gender;
    return strcmp(verify_gender, gender);
}

int verify_AccountAge_User(CATALOG_USERS *cu, char* username, int age){
    User *u = g_hash_table_lookup(cu->users_hash,username);
    int verify_age = calculate_Age(u->account_creation);
    if (verify_age>=age) return 0;
    else return 1;
}


// Função que encontra na HashTable o User pedido e dá return a uma string com os dados necessários para responder à Query1
char *perfil_User(CATALOG_USERS *cu, char *username) {

    User *u = g_hash_table_lookup(cu->users_hash, username);

    if (u == NULL || u->account_status == Inactive) return "";

    char *name = u->name;
    char *gender = u->gender;
    int age = calculate_Age(u->birth_date);
    int number_of_rides = u->rides_total;
    double average_rating = calculate_RatingAverage(u->rating_total, number_of_rides);
    double total_spent = u->spent_total;

    char *answer = malloc(
            snprintf(NULL, 0, "%s;%s;%d;%.3lf;%d;%.3lf\n", name, gender, age, average_rating, number_of_rides,
                     total_spent) + 1);
    snprintf(answer, 100, "%s;%s;%d;%.3lf;%d;%.3lf\n", name, gender, age, average_rating, number_of_rides, total_spent);
    return answer;
}


char **highestDistance_Users(CATALOG_USERS *cu, int N) {

    char **answer = malloc(N * sizeof(char *));
    for (size_t a = 0; a < N; a++) {
        User* u = cu->biggestDistance_array[a];
        answer[a] = malloc(snprintf(NULL, 0, "%s;%s;%d\n", u->username, u->name, u->distance_total) + 1);
        snprintf(answer[a], 100, "%s;%s;%d\n", u->username, u->name, u->distance_total);
    }
    return answer;
}
