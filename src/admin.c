#include "../include/flowcash.h"

static int admin_logged_in = 0;

static void list_and_manage_accounts(void) {
    FILE *idx = fopen("data/bank_data/index.idx", "r");
    if (!idx) {
        printf("No accounts found.\n");
        return;
    }
    
    char ids[1000][20];
    char paths[1000][100];
    int count = 0;
    while (fscanf(idx, "%s %s", ids[count], paths[count]) == 2) {
        count++;
    }
    fclose(idx);
    
    if (count == 0) {
        printf("No accounts found.\n");
        return;
    }
    
    printf("\n--- All Accounts ---\n");
    for (int i = 0; i < count; i++) {
        Account acc;
        if (read_account(ids[i], &acc)) {
            printf("%d) %s - %s\n", i + 1, acc.id, acc.name);
        }
    }
    
    int choice;
    if (!get_int("\nSelect an account number to manage (or 0 to go back): ", &choice)) return;
    if (choice < 1 || choice > count) return;
    
    int acc_idx = choice - 1;
    Account acc;
    if (!read_account(ids[acc_idx], &acc)) {
        printf("Failed to load account.\n");
        return;
    }
    
    int sub_choice;
    do {
        printf("\n--- Manage Account: %s ---\n", acc.id);
        printf("1) View full info\n");
        printf("2) Delete account\n");
        printf("3) Adjust balance\n");
        printf("4) Reset PIN\n");
        printf("5) Toggle loan eligibility\n");
        printf("0) Back\n");
        if (!get_int("Select option: ", &sub_choice)) continue;
        
        if (sub_choice == 1) {
            printf("\nID: %s\nName: %s\nBalance: BDT %.2lf\nLoans: %d\nLoan Eligible: %s\n",
                   acc.id, acc.name, acc.balance, acc.loan_count, acc.loan_enabled ? "Yes" : "No");
        } else if (sub_choice == 2) {
            if (ask_confirm("Are you sure you want to delete this account?")) {
                delete_account_files(acc.id);
                printf("Account deleted.\n");
                char msg[100];
                sprintf(msg, "Admin: deleted account %s", acc.id);
                log_admin(msg);
                return;
            }
        } else if (sub_choice == 3) {
            double amt;
            if (get_double("Enter amount to ADD (negative to subtract): BDT ", &amt)) {
                if (ask_confirm("Confirm balance adjustment?")) {
                    acc.balance += amt;
                    write_account(&acc);
                    log_transaction(acc.id, "ADMIN_ADJ", amt, "Admin balance adjustment");
                    printf("Balance updated to BDT %.2lf\n", acc.balance);
                    char msg[100];
                    sprintf(msg, "Admin: adjusted balance for %s by BDT %.2lf", acc.id, amt);
                    log_admin(msg);
                }
            }
        } else if (sub_choice == 4) {
            char new_pin[10];
            if (get_string("Enter new 4-digit PIN: ", new_pin, sizeof(new_pin)) && PIN_LEN(new_pin)) {
                if (ask_confirm("Confirm PIN reset?")) {
                    strcpy(acc.pin, new_pin);
                    write_account(&acc);
                    printf("PIN reset.\n");
                    char msg[100];
                    sprintf(msg, "Admin: reset PIN for %s", acc.id);
                    log_admin(msg);
                }
            } else {
                printf("Invalid PIN.\n");
            }
        } else if (sub_choice == 5) {
            if (ask_confirm("Toggle loan eligibility?")) {
                acc.loan_enabled = !acc.loan_enabled;
                write_account(&acc);
                printf("Loan eligibility set to: %s\n", acc.loan_enabled ? "Yes" : "No");
                char msg[100];
                sprintf(msg, "Admin: toggled loan eligibility for %s", acc.id);
                log_admin(msg);
            }
        } else if (sub_choice == 0) {
            break;
        } else {
            printf("Invalid option.\n");
        }
    } while (1);
}

static void search_accounts(void) {
    char query[50];
    if (!get_string("Enter ID or name to search: ", query, sizeof(query))) return;
    
    FILE *idx = fopen("data/bank_data/index.idx", "r");
    if (!idx) return;
    char id[20], path[100];
    int found = 0;
    while (fscanf(idx, "%s %s", id, path) == 2) {
        Account acc;
        if (read_account(id, &acc)) {
            if (strstr(acc.id, query) || strstr(acc.name, query)) {
                printf("Found: %s - %s (Balance: BDT %.2lf)\n", acc.id, acc.name, acc.balance);
                found++;
            }
        }
    }
    fclose(idx);
    if (!found) printf("No matching accounts found.\n");
}

static void view_statistics(void) {
    FILE *idx = fopen("data/bank_data/index.idx", "r");
    if (!idx) return;
    char id[20], path[100];
    int total_accs = 0;
    int total_loans = 0;
    double total_deposits = 0;
    
    while (fscanf(idx, "%s %s", id, path) == 2) {
        Account acc;
        if (read_account(id, &acc)) {
            total_accs++;
            total_deposits += acc.balance;
            total_loans += acc.loan_count;
        }
    }
    fclose(idx);
    
    printf("\n--- System Statistics ---\n");
    printf("Total Accounts: %d\n", total_accs);
    printf("Total Deposits: BDT %.2lf\n", total_deposits);
    printf("Total Loans Active: %d\n", total_loans);
    printf("Global Loan Feature: %s\n", get_system_loan_status() ? "ENABLED" : "DISABLED");
}

void admin_menu(void) {
    int choice;
    do {
        printf("\n=== ADMIN MENU ===\n");
        if (!admin_logged_in) {
            printf("1) Login as Admin\n");
        } else {
            printf("1) Logout\n");
            printf("2) Approve / Deny pending loan requests\n");
            printf("3) List all accounts / Manage\n");
            printf("4) Enable / Disable global loan feature\n");
            printf("5) View system statistics\n");
            printf("6) Search accounts\n");
        }
        printf("0) Help / Back\n");
        
        if (!get_int("Select option: ", &choice)) continue;
        
        if (!admin_logged_in) {
            if (choice == 1) {
                char pwd[50];
                if (get_string("Enter admin password: ", pwd, sizeof(pwd))) {
                    char hash[65];
                    get_admin_password_hash(hash);
                    if (strcmp(pwd, hash) == 0 || strcmp(pwd, "admin") == 0) { // admin is default
                        admin_logged_in = 1;
                        printf("Admin logged in.\n");
                        log_admin("Admin: logged in");
                    } else {
                        printf("Incorrect password.\n");
                    }
                }
            } else if (choice == 0) {
                return;
            } else {
                printf("Invalid option.\n");
            }
            continue;
        }
        
        switch (choice) {
            case 1: admin_logged_in = 0; printf("Logged out.\n"); break;
            case 2: approve_deny_loans(); break;
            case 3: list_and_manage_accounts(); break;
            case 4: {
                int st = get_system_loan_status();
                printf("Current status: %s\n", st ? "Enabled" : "Disabled");
                if (ask_confirm("Toggle global loan feature?")) {
                    set_system_loan_status(!st);
                    printf("Feature toggled.\n");
                    log_admin("Admin: toggled global loan feature");
                }
                break;
            }
            case 5: view_statistics(); break;
            case 6: search_accounts(); break;
            case 0: return;
            default: printf("Invalid option.\n");
        }
    } while (1);
}
