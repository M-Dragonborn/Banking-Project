#include "../include/flowcash.h"

static void init_directories(void) {
    // A simple hack to ensure files can be created if directories are missing
    // In actual C we'd use mkdir, but the setup script/Makefile handles it for us.
    // Ensure admin.cfg exists
    FILE *f = fopen("data/bank_data/admin.cfg", "r");
    if(!f) {
        set_admin_password_hash("admin");
    } else {
        fclose(f);
    }
    
    f = fopen("data/bank_data/system.cfg", "r");
    if(!f) {
        f = fopen("data/bank_data/system.cfg", "w");
        if(f) {
            fprintf(f, "9999999\n1\n");
            fclose(f);
        }
    } else {
        fclose(f);
    }
}

int main(int argc, char **argv) {
    if (argc > 1 && strcmp(argv[1], "--demo") == 0) {
        init_directories();
        run_demo();
        return 0;
    }
    
    init_directories();
    
    int choice;
    do {
        printf("\n=================================\n");
        printf("    FlowCash - Bank Manager      \n");
        printf("=================================\n");
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
