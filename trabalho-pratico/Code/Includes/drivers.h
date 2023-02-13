#ifndef PROJETO_DRIVERS_H
#define PROJETO_DRIVERS_H

#include <time.h>

//Estrutura dos Drivers
typedef struct driver Driver;
//Catálogo dos Drivers
typedef struct catalog_drivers CATALOG_DRIVERS;

//Cria Cátalogo de Drivers através do ficheiro de input drivers.cvs
CATALOG_DRIVERS* create_Catalog_Driver(char* entry_files);
void delete_Catalog_Driver(CATALOG_DRIVERS* cd);
//Atualiza o Driver quando é adicionada uma Ride ao Catálago de Rides
void update_Driver(CATALOG_DRIVERS* cd, int id, double rating, double received, int new_date);

//Retorna a CarClass de um Driver
enum car_class get_CarClass_Driver(CATALOG_DRIVERS* cd, int id);
//Retorna nome de um Driver
char* get_Name_Driver(CATALOG_DRIVERS *cd, int id);
//Retorna AccountStatus de um Driver
enum account_status get_AccountStatus_Driver(CATALOG_DRIVERS *cd, int id);
//Retorna AccountCreation de um Driver
int get_AccountCreation_Driver(CATALOG_DRIVERS *cd, int id);

//Verifica se gender do Driver é igual ao gender dado como argumento
int verify_GenderActive_Driver(CATALOG_DRIVERS *cd, int id, char* gender);
//Verifica se idade da conta do Driver é maior ou igual à idade dada como argumento
int verify_AccountAge_Driver(CATALOG_DRIVERS *cd, int id, int age);

//Retorna uma string com o perfil do Driver
char* perfil_Driver(CATALOG_DRIVERS* cd, int id);
//Retorna uma lista de strings dos Drivers com maior avaliação média do Catálogo
char** highestRated_Drivers(CATALOG_DRIVERS* cd, int N);

void sort_RatingArrayDriver(CATALOG_DRIVERS *cd);



#endif //PROJETO_DRIVERS_H
