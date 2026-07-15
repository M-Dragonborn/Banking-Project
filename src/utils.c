#include "../include/flowcash.h"

// writes a normal user action to the log file
void log_user(const char *msg) {
    FILE *f = fopen("data/bank_data/user.txt", "a");
    if (f) {
        fprintf(f, "%s\n", msg);
        fclose(f);
    }
}

// writes an admin action to the log file
void log_admin(const char *msg) {
    FILE *f = fopen("data/bank_data/admin.txt", "a");
    if (f) {
        fprintf(f, "%s\n", msg);
        fclose(f);
    }
}

// loads an account from a text file into memory
int read_account(const char *id, Account *acc) {
    char filepath[100];
    sprintf(filepath, "data/bank_data/accounts/%s.txt", id);
    FILE *f = fopen(filepath, "r");
    if (!f) return 0;
    
    int res = fscanf(f, "%7s\n%49[^\n]\n%9s\n%lf\n%d\n%d\n", 
           acc->id, acc->name, acc->pin, &acc->balance, &acc->loan_count, &acc->loan_enabled);
    fclose(f);
    return res == 6;
}

// saves an account struct back into its text file
int write_account(const Account *acc) {
    char filepath[100];
    sprintf(filepath, "data/bank_data/accounts/%s.txt", acc->id);
    FILE *f = fopen(filepath, "w");
    if (!f) {
        fprintf(stderr, "Error opening %s\n", filepath);
        log_user("failed to open file");
        return 0;
    }
    fprintf(f, "%s\n%s\n%s\n%.2lf\n%d\n%d\n", 
           acc->id, acc->name, acc->pin, acc->balance, acc->loan_count, acc->loan_enabled);
    fclose(f);
    return 1;
}

// deletes all files related to an account when an admin removes them
int delete_account_files(const char *id) {
    char path[100];
    sprintf(path, "data/bank_data/accounts/%s.txt", id);
    remove(path);
    sprintf(path, "data/bank_data/transactions/%s.txt", id);
    remove(path);
    sprintf(path, "data/bank_data/loans/%s.txt", id);
    remove(path);
    remove_from_index(id);
    return 1;
}

// adds a new account to the master phonebook file
void add_to_index(const char *id) {
    FILE *f = fopen("data/bank_data/index.txt", "a");
    if (f) {
        fprintf(f, "%s data/bank_data/accounts/%s.txt\n", id, id);
        fclose(f);
    }
}

// removes an account from the master phonebook file by rewriting it
void remove_from_index(const char *id) {
    FILE *f = fopen("data/bank_data/index.txt", "r");
    if (!f) return;
    char lines[1000][100];
    int count = 0;
    char cur_id[20], path[100];
    while(fscanf(f, "%s %s", cur_id, path) == 2) {
        if(strcmp(cur_id, id) != 0) {
            sprintf(lines[count++], "%s %s\n", cur_id, path);
        }
    }
    fclose(f);
    f = fopen("data/bank_data/index.txt", "w");
    if(f) {
        for(int i = 0; i < count; i++) {
            fprintf(f, "%s", lines[i]);
        }
        fclose(f);
    }
}

// logs a money movement with a timestamp
void log_transaction(const char *id, const char *type, double amount, const char *note) {
    char filepath[100];
    sprintf(filepath, "data/bank_data/transactions/%s.txt", id);
    FILE *f = fopen(filepath, "a");
    if (f) {
        fprintf(f, "%s | %s | %.2lf | %s\n", get_current_date(), type, amount, note);
        fclose(f);
    }
}

// gets a unique 7 digit ID for a new account from the system file
int get_next_account_id(char *out_id) {
    int next_id = 9999999;
    FILE *f = fopen("data/bank_data/system.txt", "r");
    if (f) {
        fscanf(f, "%d", &next_id);
        fclose(f);
    }
    sprintf(out_id, "%07d", next_id);
    
    f = fopen("data/bank_data/system.txt", "w");
    if(f) {
        fprintf(f, "%d\n", next_id - 1);
        fclose(f);
    }
    return 1;
}

// checks if the admin turned loans on or off for the whole bank
int get_system_loan_status(void) {
    int status = 1;
    FILE *f = fopen("data/bank_data/system.txt", "r");
    if(f) {
        int dummy;
        if(fscanf(f, "%d\n%d", &dummy, &status) != 2) {
            status = 1;
        }
        fclose(f);
    }
    return status;
}

// lets the admin turn loans on or off for the whole bank
void set_system_loan_status(int status) {
    int next_id = 9999999;
    FILE *f = fopen("data/bank_data/system.txt", "r");
    if(f) {
        fscanf(f, "%d", &next_id);
        fclose(f);
    }
    f = fopen("data/bank_data/system.txt", "w");
    if(f) {
        fprintf(f, "%d\n%d\n", next_id, status);
        fclose(f);
    }
}

// gets the admin password from config
int get_admin_password_hash(char *hash_out) {
    FILE *f = fopen("data/bank_data/admin.txt", "r");
    if(f) {
        if(fscanf(f, "%64s", hash_out) == 1) {
            fclose(f);
            return 1;
        }
        fclose(f);
    }
    strcpy(hash_out, "admin");
    return 0;
}

// saves a new admin password to config
void set_admin_password_hash(const char *hash) {
    FILE *f = fopen("data/bank_data/admin.txt", "w");
    if(f) {
        fprintf(f, "%s\n", hash);
        fclose(f);
    }
}

// cleans up leftover characters when reading input so it doesn't skip
void clear_input_buffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// safely reads text input without crashing the program
int get_string(const char *prompt, char *buffer, int max_len) {
    printf("%s", prompt);
    if (!fgets(buffer, max_len, stdin)) return 0;
    buffer[strcspn(buffer, "\n")] = 0;
    return 1;
}

// safely reads a money amount without crashing
int get_double(const char *prompt, double *val) {
    char buf[50];
    if (get_string(prompt, buf, sizeof(buf))) {
        if(sscanf(buf, "%lf", val) == 1) return 1;
    }
    return 0;
}

// safely reads a number without crashing
int get_int(const char *prompt, int *val) {
    char buf[50];
    if (get_string(prompt, buf, sizeof(buf))) {
        if(sscanf(buf, "%d", val) == 1) return 1;
    }
    return 0;
}

// asks a simple yes or no question and returns 1 for yes
int ask_confirm(const char *prompt) {
    char buf[10];
    printf("%s (y/n): ", prompt);
    if(fgets(buf, sizeof(buf), stdin)) {
        if(tolower(buf[0]) == 'y') return 1;
    }
    return 0;
}

// wipes the terminal screen clean
void clear_screen(void) {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

// gets the real time date and time as a string
char* get_current_date(void) {
    static char date[22];
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    sprintf(date, "%04d-%02d-%02d %02d:%02d:%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
    return date;
}
