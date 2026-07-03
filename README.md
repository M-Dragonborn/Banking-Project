# FlowCash – Bank Management System

A lightweight, terminal-only banking demonstration tool written entirely in plain C (C99/C11 standard). FlowCash was developed as an academic project to replace slow, error-prone paper ledgers without the heavy overhead of full database servers (like MySQL or PostgreSQL).

## Quick start
```sh
curl -sL https://raw.githubusercontent.com/M-Dragonborn/Banking-Project/main/run.sh | bash
```
*The script fetches the source, compiles it with GCC, and runs the program.*

## Why FlowCash? (Needs & Goals)
Traditional banking systems are often monolithic, requiring dedicated administration, costly proprietary licensing, and months of setup. FlowCash aims to provide a completely self-contained, lightweight alternative:
- **Zero External Dependencies:** Compiles out of the box with standard C libraries.
- **File System Database:** Uses the local file system as a flat-file database, saving data into structured `.txt` files instead of a heavy DB engine.
- **Fast Search & Retrieval:** Uses an `O(1)` index file lookup approach for rapid account access.

## Core Features
### User Panel
- **Account Management:** Secure login via a 4-digit numeric PIN, check balance, view information, and change PIN.
- **Transactions:** Deposit and withdraw funds with strict positive-amount and sufficient-funds validation.
- **Transfers:** Transfer BDT to other user accounts.
- **Loan Subsystem:** Request up to 2 loans per account, view loan status, and pay monthly installments (calculated with a fixed 5% interest).

### Admin Panel
- **Role-Based Access Control:** Secure admin login with exclusive system privileges.
- **Account Controls:** View a numbered list of all accounts, search accounts by ID/Name, safely adjust user balances, reset user PINs, and delete accounts.
- **Loan Management:** Approve or deny pending user loan requests, and dynamically toggle the loan feature system-wide.
- **Statistics:** View total active accounts, total bank deposits, and total active loans.

## Database & File Structure
Because FlowCash is self-contained, data is organized neatly within the `data/bank_data/` directory:
- `accounts/`: Individual files storing account states.
- `transactions/`: Append-only transaction logs for every account.
- `loans/`: Detailed records for active and pending user loans.
- `index.idx`: Maps account numbers to file paths for instant lookups.
- `user.log` / `admin.log`: Plain-sentence audit trails for actions taken by users and administrators.

## System Requirements
- **Hardware:** x86/x64 compatible processor (1 GHz+), 512 MB RAM (2 GB recommended), minimum 50 MB free storage space.
- **Software:** Linux, macOS, or Windows (via WSL/MinGW), GCC v5.0 or higher, and any standard terminal emulator (80x24 characters minimum).

## Limitations & Future Works
### Current Limitations
- Terminal-only interface (No GUI).
- Single-machine use without concurrent or network access.
- Plain-text data storage (No data encryption).
- Limited search by exact account number or name prefix.

### Future Roadmap
- Replace the plain-text UI with an `ncurses` or GTK interface.
- Implement network support via TCP/IP sockets for remote client access.
- Migrate the flat-file database structure to SQLite.
- Add AES-256 encryption for account profiles and loan files.
- Introduce an immutable audit trail for compliance and multi-currency support.

## Development Team
This system was proudly developed by:
- **Musfiqur Rahman** — Project Co-ordination & Management
- **G.M Entisar Hoque** — Admin Panel
- **Ratul Saha Anu** — User Panel
- **Anower Hossain** — Admin Panel
- **Manuel Tudu** — Database

*Submitted to Meher Afroj, Assistant Professor, Dept. of CSE, BUBT.*
