#ifndef PROJETO_USERS_H
#define PROJETO_USERS_H

//Estrutura dos Users
typedef struct user User;
//Estrutura da Lista de Users
typedef struct catalog_users CATALOG_USERS;

//Cria uma Lista de Users através de um ficheiro de Input
CATALOG_USERS* create_Catalog_User(char* entry_files);
void delete_Catalog_User(CATALOG_USERS* cu);
void update_User(CATALOG_USERS *uc, char* username, double rating, int distance, double spent, int new_date);

void sort_DistanceArray_User(CATALOG_USERS *cu);

int get_AccountCreation_User(CATALOG_USERS *cu, char* username);
char* get_Name_User(CATALOG_USERS *cu, char* username);
int verify_GenderAndActive_User(CATALOG_USERS *cu, char *username, char *gender);
int verify_AccountAge_User(CATALOG_USERS *cu, char* username, int age);

char* perfil_User(CATALOG_USERS * cu, char* username);
char** highestDistance_Users(CATALOG_USERS* cu, int N);


#endif //PROJETO_USERS_H