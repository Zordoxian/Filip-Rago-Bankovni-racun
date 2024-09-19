#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include "racun.h"

int main() {
    int choice;  // 1. Odabir konkretnih primitivnih tipova podataka za rad s cjelobrojnim konstantama.

    printf("\tF E R I T B A N K A\n");

    do {
        printf("\n---------------------------------");
        printf("\nOdaberite opciju:\n");
        printf("1. Kreiranje novog racuna\n");
        printf("2. Prijava\n");
        printf("3. Brisanje racuna\n");
        printf("4. Sortiranje racuna\n");
        printf("5. Pretraga racuna\n");
        printf("6. Izlaz\n");
        printf("Vas izbor: ");
        scanf("%d", &choice);  

        switch (choice) { 
        case CREATE_ACCOUNT_OPTION:
            create_account();
            break;
        case LOGIN_OPTION:
            login();
            break;
        case DELETE_ACCOUNT_OPTION:
            delete_account();
            break;
        case SORT_OPTION:
            sort_accounts();
            break;
        case SEARCH_OPTION:
            search_accounts();
            break;
        case EXIT_OPTION:
            printf("\n- Hvala na koristenju nase usluge -\n");
            break;
        default:
            printf("\nNepostojeca opcija. Molimo unesite broj od 1 do 6 na izborniku.\n");
            while (getchar() != '\n');
            break;
        }
    } while (choice != EXIT_OPTION);  

    return 0;
}
