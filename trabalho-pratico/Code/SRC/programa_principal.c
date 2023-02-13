#include "../Includes/catalog.h"


int main(int argc, char** argv){

    if (argc == 3)
    {
        run_Normal(argv);
    }

    else if (argc == 1){
        run_Interactive();
    }

    return 0;
}