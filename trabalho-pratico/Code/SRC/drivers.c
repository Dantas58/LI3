#define SIZE 100000

#include "../Includes/drivers.h"
#include "../Includes/validator.h"
#include "../Includes/date.h"

#include <glib.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


//Estrutura dos Drivers
struct driver {
    int id;
    char *name;
    int birth_date;
    char *gender;
    enum car_class class; //basic, green ou premium
    char *license_plate;
    char *city;
    int account_creation;
    enum account_status account_status; //active ou inactive

    int most_recent_ride;
    double total_received;
    double total_rating;
    int total_rides;
};

//Estrutura do Catálogo de Drivers (HashTable)
struct catalog_drivers {
    GHashTable *drivers_hash;
    Driver** biggestRating_array;
    int size, used;
};


void delete_Driver(void* data){
    Driver *d = (Driver*) data;
    free(d->name);
    free(d->gender);
    free(d->license_plate);
    free(d->city);
    free(d);
}

/*Função responsável por criar um Driver
Dá parse a uma linha (retirada do ficheiro de drivers)
Ao dar parse garante a validade dos dados*/
Driver *create_Driver(char *line) {

    Driver *d = malloc(sizeof(struct driver));
    char *token;
    int i = 0;
    int verify = 1;

    while ((token = strsep(&line, ";")) != NULL) {
        switch (i++) {
            case 0:
                if (strlen(token) == 0) verify = 0;
                d->id = (int) strtol(token, (char **)NULL, 10);
                break;
            case 1:
                if (strlen(token) == 0) verify = 0;
                d->name = strdup(token);
                break;
            case 2:
                d->birth_date = verify_Date(token);
                if (d->birth_date == 0) verify = 0;
                break;
            case 3:
                if (strlen(token) == 0) verify = 0;
                d->gender = strdup(token);
                break;
            case 4:
                d->class = verify_CarClass(token);
                if (d->class == NoClass) verify = 0;
                break;
            case 5:
                if (strlen(token) == 0) verify = 0;
                d->license_plate = strdup(token);
                break;
            case 6:
                if (strlen(token) == 0) verify = 0;
                d->city = strdup(token);
                break;
            case 7:
                d->account_creation = verify_Date(token);
                if (d->account_creation == 0) verify = 0;
                break;
            case 8:
                d->account_status = verify_AccountStatus(token);
                if (d->account_status == NoStatus) verify = 0;
                break;
        }
    }

    if (verify == 0) {
        delete_Driver(d);
        return NULL;
    }

    d->most_recent_ride = 0;
    d->total_rating = 0;
    d->total_rides = 0;
    d->total_received = 0;

    return d;
}

void insert_Driver(CATALOG_DRIVERS *cd, Driver *d){
    g_hash_table_insert(cd->drivers_hash, &d->id, d);
    if (d->account_status==Active){
        if (cd->used == cd->size){
            cd->size += cd->size;
            cd->biggestRating_array = realloc(cd->biggestRating_array, cd->size * sizeof(struct driver));
        }
        cd->biggestRating_array[cd->used++] = d;
    }
}

//funcao responsavel por criar e preencher a hashtable dos drivers
CATALOG_DRIVERS *create_Catalog_Driver(char *entry_files) {

    FILE *fp;
    char open[50];
    strcpy(open, entry_files);
    fp = fopen(strcat(open, "/drivers.csv"), "r");
    if (!fp) {
        perror("Error");
        return NULL;
    }

    CATALOG_DRIVERS *cd = malloc(sizeof(struct catalog_drivers));
    cd->drivers_hash = g_hash_table_new_full(g_int_hash, g_int_equal,NULL,delete_Driver);
    cd->biggestRating_array = malloc(SIZE * sizeof (Driver *));
    cd->size = SIZE;
    cd->used = 0;

    char *line = NULL;
    size_t len = 0;

    clock_t start, end;
    double cpu_time_used;

    start = clock();
    while (getline(&line, &len, fp)>0) {
        line[strcspn(line, "\n")] = 0;
        Driver *d = create_Driver(line);
        if (d != NULL) insert_Driver(cd,d);
    }
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Drivers parsed em: %f segundos\n", cpu_time_used);


    free(line);
    fclose(fp);

    return cd;
}

void delete_Catalog_Driver(CATALOG_DRIVERS* cd){
    g_hash_table_destroy(cd->drivers_hash);
    free(cd->biggestRating_array);
    free(cd);
}

//funcao responsavel por atualizar os valores de um driver
void update_Driver(CATALOG_DRIVERS *cd, int id, double rating, double received, int new_date) {

    Driver *d = g_hash_table_lookup(cd->drivers_hash, &id);

    d->total_received += received;
    d->total_rating += rating;
    d->total_rides++;
    d->most_recent_ride = get_MostRecentDate(d->most_recent_ride, new_date);
}


int cmpHighestRating (const void * a, const void * b) {

    Driver *d1 = *(Driver**) a;
    Driver *d2 = *(Driver**) b;

    double rating_average_d1 = calculate_RatingAverage(d1->total_rating, d1->total_rides);
    double rating_average_d2 = calculate_RatingAverage(d2->total_rating, d2->total_rides);

    if (rating_average_d1 < rating_average_d2) return 1;
    else if (rating_average_d1 > rating_average_d2) return -1;
    else {
        int compare_dates = compare_Date(d1->most_recent_ride,
                                         d2->most_recent_ride);
        switch (compare_dates) {
            case 1:
                return -1;
            case 2:
                return 1;
            case 0:
                if (d1->id < d2->id) {
                    return -1;
                }
                else return 1;
        }
    }

    return 0;
}

void sort_RatingArrayDriver(CATALOG_DRIVERS *cd){
    qsort(cd->biggestRating_array, cd->used, sizeof(Driver*), cmpHighestRating);
}


//funcao devolve a car_class de um driver
enum car_class get_CarClass_Driver(CATALOG_DRIVERS *cd, int id) {
    Driver *d = g_hash_table_lookup(cd->drivers_hash, &id);
    return d->class;
}

int get_AccountCreation_Driver(CATALOG_DRIVERS *cd, int id){
    Driver *d = g_hash_table_lookup(cd->drivers_hash, &id);
    return d->account_creation;
}

char* get_Name_Driver(CATALOG_DRIVERS *cd, int id){
    Driver *d = g_hash_table_lookup(cd->drivers_hash,&id);
    return d->name;
}

enum account_status get_AccountStatus_Driver(CATALOG_DRIVERS *cd, int id){
    Driver *d = g_hash_table_lookup(cd->drivers_hash,&id);
    return d->account_status;
}

int verify_GenderActive_Driver(CATALOG_DRIVERS *cd, int id, char* gender){
    Driver *d = g_hash_table_lookup(cd->drivers_hash,&id);
    if (d->account_status == Inactive) return -1;
    char* verify_gender = d->gender;
    return strcmp(verify_gender,gender) ;
}

int verify_AccountAge_Driver(CATALOG_DRIVERS *cd, int id, int age){
    Driver *d = g_hash_table_lookup(cd->drivers_hash,&id);
    int verify_age = calculate_Age(d->account_creation);
    if (verify_age>=age) return 0;
    return 1;
}


char *perfil_Driver(CATALOG_DRIVERS *cd, int id) {

    Driver *d = g_hash_table_lookup(cd->drivers_hash, &id);

    if (d == NULL || d->account_status == Inactive)  return "";

    char *name = d->name;
    char *gender = d->gender;
    int age = calculate_Age(d->birth_date);
    int number_of_rides = d->total_rides;
    double average_rating = calculate_RatingAverage(d->total_rating, number_of_rides);
    double total_received = d->total_received;

    char *answer = malloc(snprintf(NULL, 0, "%s;%s;%d;%.3lf;%d;%.3lf\n", name, gender, age, average_rating, number_of_rides,
                                   total_received) + 1);
    snprintf(answer, 100, "%s;%s;%d;%.3lf;%d;%.3lf\n", name, gender, age, average_rating, number_of_rides,
             total_received);
    return answer;
}


char** highestRated_Drivers(CATALOG_DRIVERS *cd, int N) {

    char** answer = malloc(N * sizeof (char*));
    for (size_t a = 0; a < N; a++) {
        Driver *d = cd->biggestRating_array[a];
        answer[a] = malloc(snprintf(NULL, 0, "%012d;%s;%.3lf\n", d->id, d->name,
                                    calculate_RatingAverage(d->total_rating, d->total_rides)) + 1);
        snprintf(answer[a], 100, "%012d;%s;%.3lf\n", d->id, d->name,
                 calculate_RatingAverage(d->total_rating, d->total_rides));
    }
    return answer;
}

