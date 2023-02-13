#define SIZE_CITY_RIDES 1000000
#define SIZE_DATE_RIDES 5000


#include "../Includes/rides.h"
#include "../Includes/validator.h"
#include "../Includes/date.h"

#include <glib.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

//Estrutura das rides
struct ride {
    int id;
    int date;
    int driver_id;
    char *user_username;
    char *city;
    int distance;
    double score_user;
    double score_driver;
    double tip;
    char *comment;
};

void delete_Ride(Ride *r) {
    free(r->user_username);
    free(r->city);
    free(r->comment);
    free(r);
}

typedef struct drivers_in_city {
    double total_rating;
    int total_rides;
    int id;
} DriversInCity;

//Estrutura do Catálogo de Drivers (HashTable) e variáveis associadas para facilitar querys
typedef struct city_rides {
    double price_notip_total, price_total;
    int rides_total;
    int size;
    int flag_ridesSorted;
    Ride **rides;
    GHashTable *drivers;
    GList *drivers_sorted;
} CityRides;

void delete_CityRides(void *data) {

    CityRides *cityRides = (CityRides *) data;

    for (int i = 0; i < cityRides->rides_total; i++) delete_Ride(cityRides->rides[i]);
    free(cityRides->rides);

    g_hash_table_destroy(cityRides->drivers);
    g_list_free(cityRides->drivers_sorted);

    free(cityRides);
}

typedef struct date_rides {
    double price_notip_total;
    int rides_total;
    int size;
    int rides_tip;
    Ride **rides;
} DateRides;

void delete_DateRides(void *data) {
    DateRides *dateRides = (DateRides *) data;
    free(dateRides->rides);
    free(dateRides);
}


struct catalog_rides {
    GHashTable *city_rides;
    GHashTable *date_rides;

    GSList *list_feminino;
    GSList *list_masculino;
};


//funcao responsavel por criar uma ride
//da parse a uma linha (retirada do ficheiro de rides)
//ao dar parse garante a validade dos dados
Ride *create_Ride(char *line) {

    Ride *r = malloc(sizeof(struct ride));
    char *token;
    int i = 0;
    int verify = 1;

    while ((token = strsep(&line, ";")) != NULL) {
        switch (i++) {
            case 0:
                if (strlen(token) == 0) verify = 0;
                r->id = (int) strtol(token, (char **) NULL, 10);
                break;
            case 1:
                r->date = verify_Date(token);
                if (r->date == 0) verify = 0;
                break;
            case 2:
                if (strlen(token) == 0) verify = 0;
                r->driver_id = (int) strtol(token, (char **) NULL, 10);
                break;
            case 3:
                if (strlen(token) == 0) verify = 0;
                r->user_username = strdup(token);
                break;
            case 4:
                if (strlen(token) == 0) verify = 0;
                r->city = strdup(token);
                break;
            case 5:
                r->distance = verify_Distance(token);
                if (r->distance == 0) verify = 0;
            case 6:
                r->score_user = verify_Score(token);
                if (r->score_user == -1) verify = 0;
                break;
            case 7:
                r->score_driver = verify_Score(token);
                if (r->score_driver == -1) verify = 0;
                break;
            case 8:
                r->tip = verify_Tip(token);
                if (r->tip == -1) verify = 0;
                break;
            case 9:
                r->comment = strdup(token);
                break;
        }
    }


    if (verify == 0) {
        delete_Ride(r);
        return NULL;
    }


    return r;
}

//funcao responsavel por inicializar o rides_array de rides
CATALOG_RIDES *init_CatalogRides() {
    CATALOG_RIDES *cr;
    cr = malloc(sizeof(struct catalog_rides));
    cr->city_rides = g_hash_table_new_full(g_str_hash, g_str_equal, NULL, delete_CityRides);
    cr->date_rides = g_hash_table_new_full(g_int_hash, g_int_equal, NULL, delete_DateRides);
    cr->list_feminino = NULL;
    cr->list_masculino = NULL;
    return cr;
}

//funcao responsavel por calcular o custo de uma ride com gorjetas
double calculate_CostRide(Ride *r, enum car_class car_class) {
    double cost = 0;
    if (car_class == Basic) {
        cost = 3.25 + 0.62 * r->distance + r->tip;
    } else if (car_class == Green) {
        cost = 4 + 0.79 * r->distance + r->tip;
    } else if (car_class == Premium) {
        cost = 5.20 + 0.94 * r->distance + r->tip;
    }
    return cost;
}

//funcao responsavel por calcular o custo de uma ride sem gorjetas
double calculate_CostRide_NoTips(Ride *r, enum car_class car_class) {
    double cost = 0;
    if (car_class == Basic) {
        cost = 3.25 + 0.62 * r->distance;
    } else if (car_class == Green) {
        cost = 4 + 0.79 * r->distance;
    } else if (car_class == Premium) {
        cost = 5.20 + 0.94 * r->distance;
    }
    return cost;
}

//funcao responsavel por inserir uma ride no rides_array de rides
void insert_Ride(CATALOG_RIDES *cr, Ride *r, CATALOG_DRIVERS *cd, CATALOG_USERS *cu) {


    double money = calculate_CostRide(r, get_CarClass_Driver(cd, r->driver_id));
    double money_notip = calculate_CostRide_NoTips(r, get_CarClass_Driver(cd, r->driver_id));


    CityRides *cityRides = g_hash_table_lookup(cr->city_rides, r->city);
    if (cityRides == NULL) {

        cityRides = malloc(sizeof(CityRides));
        cityRides->price_total = money;
        cityRides->price_notip_total = money_notip;
        cityRides->rides = malloc(SIZE_DATE_RIDES * sizeof(Ride *));
        cityRides->size = SIZE_DATE_RIDES;
        cityRides->rides[0] = r;
        cityRides->rides_total = 1;
        cityRides->flag_ridesSorted = 0;
        cityRides->drivers_sorted = NULL;

        cityRides->drivers = g_hash_table_new_full(g_int_hash, g_int_equal,NULL,free);

        if (get_AccountStatus_Driver(cd, r->driver_id) == Active) {
            DriversInCity *cityDrivers = malloc(sizeof(DriversInCity));
            cityDrivers->total_rides = 1;
            cityDrivers->total_rating = r->score_driver;
            cityDrivers->id = r->driver_id;
            g_hash_table_insert(cityRides->drivers, &r->driver_id, cityDrivers);
        }

        g_hash_table_insert(cr->city_rides, r->city, cityRides);
    } else {
        cityRides->price_total += money;
        cityRides->price_notip_total += money_notip;
        if (cityRides->rides_total == cityRides->size) {
            cityRides->size += cityRides->size;
            cityRides->rides = realloc(cityRides->rides, cityRides->size * sizeof(struct ride));
        }
        cityRides->rides[cityRides->rides_total++] = r;
        if (get_AccountStatus_Driver(cd, r->driver_id) == Active) {
            DriversInCity *cityDrivers = g_hash_table_lookup(cityRides->drivers, &r->driver_id);
            if (cityDrivers == NULL) {
                cityDrivers = malloc(sizeof(DriversInCity));
                cityDrivers->total_rides = 1;
                cityDrivers->total_rating = r->score_driver;
                cityDrivers->id = r->driver_id;
                g_hash_table_insert(cityRides->drivers, &r->driver_id, cityDrivers);
            } else {
                cityDrivers->total_rides++;
                cityDrivers->total_rating += r->score_driver;
            }
        }
    }

    DateRides *dayRides = g_hash_table_lookup(cr->date_rides, &r->date);
    if (dayRides == NULL) {
        dayRides = malloc(sizeof(DateRides));
        dayRides->price_notip_total = money_notip;
        dayRides->rides = malloc(SIZE_CITY_RIDES * sizeof(Ride *));
        dayRides->rides_total = 1;
        dayRides->size = SIZE_CITY_RIDES;
        if (r->tip > 0) {
            dayRides->rides[0] = r;
            dayRides->rides_tip = 1;
        } else dayRides->rides_tip = 0;
        g_hash_table_insert(cr->date_rides, &r->date, dayRides);
    } else {
        dayRides->price_notip_total += money_notip;
        dayRides->rides_total++;
        if (r->tip > 0) {
            if (dayRides->rides_tip == dayRides->size) {
                dayRides->size += dayRides->size;
                dayRides->rides = realloc(dayRides->rides, dayRides->size * sizeof(struct ride));
            }
            dayRides->rides[dayRides->rides_tip++] = r;
        }
    }

    if (verify_GenderActive_Driver(cd, r->driver_id, "F") == 0 &&
        verify_GenderAndActive_User(cu, r->user_username, "F") == 0) {
        cr->list_feminino = g_slist_prepend(cr->list_feminino, r);
    } else if (verify_GenderActive_Driver(cd, r->driver_id, "M") == 0 &&
               verify_GenderAndActive_User(cu, r->user_username, "M") == 0) {
        cr->list_masculino = g_slist_prepend(cr->list_masculino, r);
    }

    update_Driver(cd, r->driver_id, r->score_driver, money, r->date);
    update_User(cu, r->user_username, r->score_user, r->distance, money, r->date);

}


//mais antigo para mais recente
int cmpDates(const void *a, const void *b) {

    Ride *r1 = *(Ride **) a;
    Ride *r2 = *(Ride **) b;

    int compare_dates = compare_Date(r1->date, r2->date);

    if (compare_dates == 1) return 1;
    else if (compare_dates == 2) return -1;
    else return 0;
}

typedef struct catalog_user_driver {
    CATALOG_DRIVERS *cd;
    CATALOG_USERS *cu;
} Catalog_User_Driver;

gint comparator_List_GenderAndAge(gconstpointer item1, gconstpointer item2, gpointer user_data) {

    Ride *ride_one = (Ride *) item1;
    Ride *ride_two = (Ride *) item2;
    Catalog_User_Driver *catalog = (Catalog_User_Driver *) user_data;

    int r_AccountCreation = get_AccountCreation_Driver(catalog->cd, ride_one->driver_id);
    int i_AccountCreation = get_AccountCreation_Driver(catalog->cd, ride_two->driver_id);

    if (compare_Date(r_AccountCreation, i_AccountCreation) == 2) return -1;
    else if (compare_Date(r_AccountCreation, i_AccountCreation) == 0) {
        r_AccountCreation = get_AccountCreation_User(catalog->cu, ride_one->user_username);
        i_AccountCreation = get_AccountCreation_User(catalog->cu, ride_two->user_username);
        if (compare_Date(r_AccountCreation, i_AccountCreation) == 2) return -1;
        else if (compare_Date(r_AccountCreation, i_AccountCreation) == 0) {
            if (ride_one->id < ride_two->id) return -1;
        }
    }
    return 1;
}


void delete_Catalog_Ride(CATALOG_RIDES *cr) {
    g_hash_table_destroy(cr->city_rides);
    g_hash_table_destroy(cr->date_rides);
    g_slist_free(cr->list_feminino);
    g_slist_free(cr->list_masculino);
    free(cr);
}

//funcao responsavel por criar e preencher o rides_array de rides
CATALOG_RIDES *create_Catalog_Ride(char *entry_files, CATALOG_DRIVERS *cd, CATALOG_USERS *cu) {

    FILE *fp;
    char open[50];
    strcpy(open, entry_files);
    fp = fopen(strcat(open, "/rides.csv"), "r");
    if (!fp) {
        perror("Error");
        return NULL;
    }

    CATALOG_RIDES *cr = init_CatalogRides();

    char *line = NULL;
    size_t len = 0;

    clock_t start, end;
    double cpu_time_used;

    start = clock();
    while (getline(&line, &len, fp) != -1) {
        line[strcspn(line, "\n")] = 0;
        Ride *r = create_Ride(line);
        if (r != NULL) insert_Ride(cr, r, cd, cu);
    }
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Rides parsed e Drivers e Users updated em: %f segundos\n", cpu_time_used);

    start = clock();
    sort_RatingArrayDriver(cd);
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Array de Drivers sorted em: %f segundos\n", cpu_time_used);

    start = clock();
    sort_DistanceArray_User(cu);
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Array de Users sorted em: %f segundos\n", cpu_time_used);

    struct catalog_user_driver *catalog_data = malloc(sizeof(struct catalog_user_driver));
    catalog_data->cu = cu;
    catalog_data->cd = cd;

    start = clock();
    cr->list_feminino = g_slist_sort_with_data(cr->list_feminino, comparator_List_GenderAndAge, catalog_data);
    cr->list_masculino = g_slist_sort_with_data(cr->list_masculino, comparator_List_GenderAndAge, catalog_data);
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Arrays Femininos e Masculinos de Rides sorted em: %f segundos\n", cpu_time_used);

    free(catalog_data);

    free(line);
    fclose(fp);

    return cr;
}

//funcao que calcula o preco medio de uma cidade 
char *average_Price_City(CATALOG_RIDES *cr, CATALOG_DRIVERS *cd, char *city) {

    CityRides *cityRides = g_hash_table_lookup(cr->city_rides, city);

    if (cityRides == NULL) return "";

    double price = cityRides->price_notip_total / cityRides->rides_total;

    char *answer = malloc(snprintf(NULL, 0, "%.3f\n", price) + 1);
    snprintf(answer, 100, "%.3f\n", price);
    return answer;
}


int binarySearchTimespan(Ride **sortedDate_array, int left, int right, int fst_date, int snd_date) {
    if (right >= left) {
        int mid = left + (right - left) / 2;

        if (date_InBetween(sortedDate_array[mid]->date, fst_date, snd_date))
            return mid;

       if (compare_Date(sortedDate_array[mid]->date, snd_date) == 1)
            return binarySearchTimespan(sortedDate_array, left, mid - 1, fst_date, snd_date);

        return binarySearchTimespan(sortedDate_array, mid + 1, right, fst_date, snd_date);
    }

  return -1;
}

//funcao que calcula a distancia media percorrida, entre duas datas
char *average_Distance_CityAndTimespan(CATALOG_RIDES *cr, char *city, char *dateA, char *dateB) {

    CityRides *cityRides = g_hash_table_lookup(cr->city_rides, city);

    if (cityRides == NULL) return "";

    int date_A = create_Date(dateA), date_B = create_Date(dateB);
    int first_date, second_date;
    if (compare_Date(date_A, date_B) == 1) {
        first_date = date_B;
        second_date = date_A;
    } else {
        first_date = date_A;
        second_date = date_B;
    }


    if (cityRides->flag_ridesSorted == 0) {
        cityRides->flag_ridesSorted = 1;
        qsort(cityRides->rides, cityRides->rides_total, sizeof(Ride *), cmpDates);
    }


    int mid = binarySearchTimespan(cityRides->rides, 0, cityRides->rides_total - 1, first_date, second_date);

    if (mid == -1) return "";

    double distance = 0;
    int number_of_rides = 0;

    Ride *r_mid = cityRides->rides[mid];
    distance += r_mid->distance;
    number_of_rides++;

    int before = mid - 1;
    int next = mid + 1;
    while (before >= 0) {
        Ride *r_before = cityRides->rides[before];
        if (date_InBetween(r_before->date, first_date, second_date) == 1) {
            distance += r_before->distance;
            number_of_rides++;
            before--;
        } else break;
    }
    while (next < cityRides->rides_total) {
        Ride *r_next = cityRides->rides[next];
        if (date_InBetween(r_next->date, first_date, second_date) == 1) {
            distance += r_next->distance;
            number_of_rides++;
            next++;
        } else break;
    }

    distance /= number_of_rides;

    char *answer = malloc(snprintf(NULL, 0, "%.3f\n", distance) + 1);
    snprintf(answer, 100, "%.3f\n", distance);
    return answer;
}

//funcao que calcula o preco medio de viagens, entre duas datas
char *average_Price_Timespan(CATALOG_RIDES *cr, char *dateA, char *dateB) {

    int date_A = create_Date(dateA), date_B = create_Date(dateB);
    int first_date, second_date;
    if (compare_Date(date_A, date_B) == 1) {
        first_date = date_B;
        second_date = date_A;
    } else {
        first_date = date_A;
        second_date = date_B;
    }

    double price = 0;
    int number_of_rides = 0;

    while (compare_Date(first_date, second_date) != 1) {

        DateRides *dateRides = g_hash_table_lookup(cr->date_rides, &first_date);

        if (dateRides != NULL) {
            price += dateRides->price_notip_total;
            number_of_rides += dateRides->rides_total;
        }

        first_date = nextDay(first_date);
    }

    if (number_of_rides == 0) return "";

    price /= number_of_rides;


    char *answer = malloc(snprintf(NULL, 0, "%.3f\n", price) + 1);
    snprintf(answer, 100, "%.3f\n", price);
    return answer;
}


gint comparator_List_TipAndTimespan(gconstpointer item1, gconstpointer item2) {

    Ride *ride_one = (Ride *) item1;
    Ride *ride_two = (Ride *) item2;
    int distance_diff = ride_one->distance - ride_two->distance;
    if (distance_diff) return -distance_diff;
    if (compare_Date(ride_one->date, ride_two->date) == 1) return -1;
    else if (compare_Date(ride_one->date, ride_two->date) == 0) {
        if (ride_one->id > ride_two->id) return -1;
    }

    return 1;
}

//funcao que calcula uma lista de gorjetas, entre duas datas
char **list_by_TipAndTimespan(CATALOG_RIDES *cr, char *dateA, char *dateB, int *N) {

    int date_A = create_Date(dateA), date_B = create_Date(dateB);
    int first_date, second_date;

    if (compare_Date(date_A, date_B) == 1) {
        first_date = date_B;
        second_date = date_A;
    } else {
        first_date = date_A;
        second_date = date_B;
    }

    GSList *list = NULL;


    while (compare_Date(first_date, second_date) != 1) {

        DateRides *dateRides = g_hash_table_lookup(cr->date_rides, &first_date);

        if (dateRides != NULL) {
            for (int i = 0; i < dateRides->rides_tip; i++)
                list = g_slist_prepend(list, dateRides->rides[i]);
        }

        first_date = nextDay(first_date);
    }


    list = g_slist_sort(list, comparator_List_TipAndTimespan);

    *N = (int) g_slist_length(list);

    if (*N == 0) {
        g_slist_free(list);
        return NULL;
    }

    GSList *iterator = NULL;

    char **answer = malloc(*N * sizeof(char *));
    int a = 0;
    for (iterator = list; iterator; iterator = iterator->next, a++) {
        Ride *r = (Ride *) iterator->data;
        char *date = date_ToString(r->date);
        answer[a] = malloc(
                snprintf(NULL, 0, "%012d ;%s;%d;%s;%.3f\n", r->id, date, r->distance, r->city, r->tip) + 1);
        snprintf(answer[a], 100, "%012d;%s;%d;%s;%.3f\n", r->id, date, r->distance, r->city, r->tip);
        free(date);
    }

    g_slist_free(list);
    return answer;
}


gint comparator_List_Rating(gconstpointer item1, gconstpointer item2) {

    DriversInCity *driver_one = (DriversInCity *) item1;
    DriversInCity *driver_two = (DriversInCity *) item2;

    double driverone_rating = driver_one->total_rating / driver_one->total_rides;
    double drivertwo_rating = driver_two->total_rating / driver_two->total_rides;

    if (driverone_rating > drivertwo_rating) return -1;
    if (driverone_rating < drivertwo_rating) return 1;

    return driver_two->id - driver_one->id;
}

//funcao que calcula os condutores com maior classificação, numa cidade
char **highestRated_City_Drivers(CATALOG_RIDES *cr, CATALOG_DRIVERS *cd, int N, char *city) {

    CityRides *cityRides = g_hash_table_lookup(cr->city_rides, city);
    if (cityRides == NULL) return NULL;

    if (cityRides->drivers_sorted == NULL) {
        if (cityRides->drivers == NULL) return NULL;
        cityRides->drivers_sorted = g_list_sort(g_hash_table_get_values(cityRides->drivers), comparator_List_Rating);
    }

    GList *iterator = NULL;

    char **answer = malloc(N * sizeof(char *));
    int a = 0;
    for (iterator = cityRides->drivers_sorted; iterator && a < N; iterator = iterator->next, a++) {
        DriversInCity *cityDrivers = iterator->data;
        double rating_average = calculate_RatingAverage(cityDrivers->total_rating, cityDrivers->total_rides);
        answer[a] = malloc(snprintf(NULL, 0, "%012d;%s;%.3lf\n", cityDrivers->id, get_Name_Driver(cd, cityDrivers->id),
                                    rating_average) + 1);
        snprintf(answer[a], 100, "%012d;%s;%.3lf\n", cityDrivers->id, get_Name_Driver(cd, cityDrivers->id),
                 rating_average);
    }
    return answer;
}

char **
list_by_GenderAndAccountAge(CATALOG_RIDES *cr, CATALOG_DRIVERS *cd, CATALOG_USERS *cu, char *gender, int age, int *N) {

    GSList *aux = NULL;

    if (strcmp(gender, "F") == 0) {

        GSList *iterator_aux = NULL;

        for (iterator_aux = cr->list_feminino; iterator_aux; iterator_aux = iterator_aux->next) {
            Ride *r = (Ride *) iterator_aux->data;
            if (verify_AccountAge_Driver(cd, r->driver_id, age) != 0) break;
            else if (verify_AccountAge_User(cu, r->user_username, age) == 0) {
                aux = g_slist_append(aux, r);
            }
        }
    }

    else if (strcmp(gender, "M") == 0) {

        GSList *iterator_aux = NULL;

        for (iterator_aux = cr->list_masculino; iterator_aux; iterator_aux = iterator_aux->next) {
            Ride *r = (Ride *) iterator_aux->data;
            if (verify_AccountAge_Driver(cd, r->driver_id, age) != 0) break;
            else if (verify_AccountAge_User(cu, r->user_username, age) == 0) {
                aux = g_slist_append(aux, r);
            }
        }
    }

    if (aux == NULL) return NULL;

    *N = (int) g_slist_length(aux);

    GSList *iterator = NULL;

    char **answer = malloc(*N * sizeof(char *));
    int a = 0;
    for (iterator = aux; iterator; iterator = iterator->next, a++) {
        Ride *r = (Ride *) iterator->data;
        answer[a] = malloc(
                snprintf(NULL, 0, "%012d;%s;%s;%s\n", r->driver_id, get_Name_Driver(cd, r->driver_id), r->user_username, get_Name_User(cu, r->user_username)) +
                1);
        snprintf(answer[a], 100, "%012d;%s;%s;%s\n", r->driver_id, get_Name_Driver(cd, r->driver_id), r->user_username, get_Name_User(cu, r->user_username));
    }

    g_slist_free(aux);
    return answer;
}
