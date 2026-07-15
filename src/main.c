#include "../include/flowcash.h"

// this function creates the config files if they dont exist yet so the program doesnt crash on first run
static void init_directories(void) {
    FILE *f = fopen("data/bank_data/admin.txt", "r");
    if(!f) {
        set_admin_password_hash("admin");
    } else {
        fclose(f);
    }
    
    f = fopen("data/bank_data/system.txt", "r");
    if(!f) {
        f = fopen("data/bank_data/system.txt", "w");
        if(f) {
            fprintf(f, "9999999\n1\n");
            fclose(f);
        }
    } else {
        fclose(f);
    }
}

// the main entry point where the program actually starts running
int main(int argc, char **argv) {
    if (argc > 1 && strcmp(argv[1], "--demo") == 0) {
        init_directories();
        run_demo();
        return 0;
    }
    
    init_directories();
    
    int choice;
    do {
        clear_screen();
        printf("\n===========================================\n");
        printf("   _____ _               _____           _ \n");
        printf("  |  ___| |             /  __ \\         | |\n");
        printf("  | |_  | | _____      _| /  \\/ __ _ ___| |__ \n");
        printf("  |  _| | |/ _ \\ \\ /\\ / / |    / _` / __| '_ \\ \n");
        printf("  | |   | | (_) \\ V  V /| \\__/\\ (_| \\__ \\ | | |\n");
        printf("  \\_|   |_|\\___/ \\_/\\_/  \\____/\\__,_|___/_| |_|\n");
        printf("===========================================\n");
        printf("          Bank Management System           \n");
        printf("===========================================\n");
        printf("1) User Panel\n");
        printf("2) Admin Panel\n");
        printf("0) Exit\n");
        
        if (!get_int("Select option: ", &choice)) continue;
        
        switch (choice) {
            case 1: user_menu(); break;
            case 2: admin_menu(); break;
            case 0: printf("Exiting FlowCash. Goodbye!\n"); break;
            default: printf("Invalid option.\n");
        }
    } while (choice != 0);

    return 0;
}

/* TODO: migrate flat-file DB to SQLite for faster queries                */
/* TODO: replace plain-text UI with ncurses for a richer terminal UI      */
/* TODO: add AES-256 encryption for account and loan files                */
/* TODO: expose a tiny HTTP API via CGI for remote access (optional)      */
