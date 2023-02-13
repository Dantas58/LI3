#ifndef PROJETO_RIDES_H
#define PROJETO_RIDES_H

#include "drivers.h"
#include "users.h"

//Estrutura das Rides
typedef struct ride Ride;
//Estrutura da Lista de Rides
typedef struct catalog_rides CATALOG_RIDES;


//Cria uma lista de Rides através de um ficheiro de Input
CATALOG_RIDES * create_Catalog_Ride(char* entry_files, CATALOG_DRIVERS* cd, CATALOG_USERS* cu);
void delete_Catalog_Ride(CATALOG_RIDES *cr);

char* average_Distance_CityAndTimespan(CATALOG_RIDES *cr, char* city, char* dateA, char* dateB);
char* average_Price_City(CATALOG_RIDES *cr, CATALOG_DRIVERS* cd, char* city);
char *average_Price_Timespan(CATALOG_RIDES *cr, char *dateA, char *dateB);
char** list_by_TipAndTimespan(CATALOG_RIDES* cr, char* dateA, char* dateB, int *N);
char** list_by_GenderAndAccountAge(CATALOG_RIDES *cr, CATALOG_DRIVERS *cd, CATALOG_USERS *cu, char* gender, int age, int *N);
char **highestRated_City_Drivers(CATALOG_RIDES *cr, CATALOG_DRIVERS *cd, int N, char *city);

#endif //PROJETO_RIDES_H
