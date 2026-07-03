#include "../include/flowcash.h"

static Account current_user;
static int is_logged_in = 0;

void create_account(void) {
    Account new_acc;
    memset(&new_acc, 0, sizeof(Account));
    
    printf("\n--- Create Account ---\n");
    if (!get_string("Enter full name: ", new_acc.name, sizeof(new_acc.name))) return;
    
    char pin[10];
    if (!get_string("Enter 4-digit PIN: ", pin, sizeof(pin)) || !PIN_LEN(pin)) {
        printf("Invalid PIN. Must be exactly 4 digits.\n");
        return;
    }
    
    double initial_deposit;
    if (!get_double("Enter initial deposit amount: BDT ", &initial_deposit) || !POSITIVE(initial_deposit) || !AMOUNT_RANGE(initial_deposit)) {
        printf("Invalid deposit amount.\n");
        return;
    }
    
    get_next_account_id(new_acc.id);
    strcpy(new_acc.pin, pin);
    new_acc.balance = initial_deposit;
    new_acc.loan_count = 0;
    new_acc.loan_enabled = 1;
    
    if (write_account(&new_acc)) {
        add_to_index(new_acc.id);
        log_transaction(new_acc.id, "DEPOSIT", initial_deposit, "Initial deposit");
        
        char msg[100];
        sprintf(msg, "System: created account %s for %s", new_acc.id, new_acc.name);
        log_user(msg);
        
        printf("\nAccount created successfully!\n");
        printf("Your Account ID is: %s\n", new_acc.id);
        printf("Please remember this ID and your PIN to login.\n");
    } else {
        printf("Failed to create account.\n");
    }
}

void login_user(void) {
    if (is_logged_in) {
        printf("Already logged in as %s.\n", current_user.id);
        is_logged_in = 0;
        printf("Logged out.\n");
        return;
    }
    
    char id[10], pin[10];
    if (!get_string("Enter Account ID: ", id, sizeof(id))) return;
    if (!get_string("Enter PIN: ", pin, sizeof(pin))) return;
    
    if (read_account(id, &current_user)) {
        if (strcmp(current_user.pin, pin) == 0) {
            is_logged_in = 1;
            printf("Login successful. Welcome, %s!\n", current_user.name);
            
            char msg[100];
            sprintf(msg, "User %s: logged in", current_user.id);
            log_user(msg);
            return;
        }
    }
    printf("Invalid ID or PIN.\n");
}

static void user_deposit(void) {
    double amount;
    if (!get_double("Enter deposit amount: BDT ", &amount) || !POSITIVE(amount) || !AMOUNT_RANGE(amount)) {
        printf("Invalid amount.\n");
        return;
    }
    
    current_user.balance += amount;
    if (write_account(&current_user)) {
        log_transaction(current_user.id, "DEPOSIT", amount, "Cash deposit");
        char msg[100];
        sprintf(msg, "User %s: deposited BDT %.2lf", current_user.id, amount);
        log_user(msg);
        printf("Deposit successful. New balance: BDT %.2lf\n", current_user.balance);
    }
}

static void user_withdraw(void) {
    double amount;
    if (!get_double("Enter withdrawal amount: BDT ", &amount) || !POSITIVE(amount) || !AMOUNT_RANGE(amount)) {
        printf("Invalid amount.\n");
        return;
    }
    
    if (amount > current_user.balance) {
        printf("Insufficient funds. Current balance: BDT %.2lf\n", current_user.balance);
        return;
    }
    
    current_user.balance -= amount;
    if (write_account(&current_user)) {
        log_transaction(current_user.id, "WITHDRAW", amount, "Cash withdrawal");
        char msg[100];
        sprintf(msg, "User %s: withdrew BDT %.2lf", current_user.id, amount);
        log_user(msg);
        printf("Withdrawal successful. New balance: BDT %.2lf\n", current_user.balance);
    }
}

static void user_transfer(void) {
    char target_id[10];
    if (!get_string("Enter recipient Account ID: ", target_id, sizeof(target_id))) return;
    
    if (strcmp(target_id, current_user.id) == 0) {
        printf("Cannot transfer to yourself.\n");
        return;
    }
    
    Account target;
    if (!read_account(target_id, &target)) {
        printf("Recipient account not found.\n");
        return;
    }
    
    double amount;
    if (!get_double("Enter transfer amount: BDT ", &amount) || !POSITIVE(amount) || !AMOUNT_RANGE(amount)) {
        printf("Invalid amount.\n");
        return;
    }
    
    if (amount > current_user.balance) {
        printf("Insufficient funds.\n");
        return;
    }
    
    current_user.balance -= amount;
    target.balance += amount;
    
    if (write_account(&current_user) && write_account(&target)) {
        log_transaction(current_user.id, "TRANSFER", amount, "Transfer out");
        log_transaction(target.id, "TRANSFER", amount, "Transfer in");
        
        char msg[100];
        sprintf(msg, "User %s: transferred BDT %.2lf to %s", current_user.id, amount, target.id);
        log_user(msg);
        printf("Transfer successful.\n");
    }
}

static void change_pin(void) {
    char pin[10];
    if (!get_string("Enter new 4-digit PIN: ", pin, sizeof(pin)) || !PIN_LEN(pin)) {
        printf("Invalid PIN format.\n");
        return;
    }
    
    strcpy(current_user.pin, pin);
    if (write_account(&current_user)) {
        printf("PIN changed successfully.\n");
        char msg[100];
        sprintf(msg, "User %s: changed PIN", current_user.id);
        log_user(msg);
    }
}

void user_menu(void) {
    int choice;
    do {
        printf("\n=== USER MENU ===\n");
        if (!is_logged_in) {
            printf("1) Create account\n");
            printf("2) Login\n");
        } else {
            printf("2) Logout\n");
            printf("3) Check balance\n");
            printf("4) Withdraw\n");
            printf("5) Deposit\n");
            printf("6) Request loan\n");
            printf("7) Pay loan installment\n");
            printf("8) View loan status\n");
            printf("9) View account information\n");
            printf("10) Transfer money\n");
            printf("11) Change PIN\n");
        }
        printf("0) Help / Back\n");
        
        if (!get_int("Select option: ", &choice)) continue;
        
        if (!is_logged_in) {
            if (choice == 1) create_account();
            else if (choice == 2) login_user();
            else if (choice == 0) return;
            else printf("Invalid option.\n");
            continue;
        }
        
        switch (choice) {
            case 2: login_user(); break;
            case 3: printf("Current Balance: BDT %.2lf\n", current_user.balance); break;
            case 4: user_withdraw(); break;
            case 5: user_deposit(); break;
            case 6: request_loan(&current_user); break;
            case 7: pay_loan(&current_user); break;
            case 8: view_loan_status(&current_user); break;
            case 9:
                printf("\n--- Account Info ---\nID: %s\nName: %s\nBalance: BDT %.2lf\nLoans: %d\n",
                       current_user.id, current_user.name, current_user.balance, current_user.loan_count);
                break;
            case 10: user_transfer(); break;
            case 11: change_pin(); break;
            case 0: return;
            default: printf("Invalid option.\n");
        }
    } while (1);
}
