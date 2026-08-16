# Multi-Role Community Knowledge Sharing & Q&A System in C

A secure, modular, command-line Q&A platform written in ANSI C (C99) with dynamic memory management, credential hashing, and role-based permissions.

---

## Key Features

* **Authentication & RBAC:** Multi-tier permissions for **Student**, **Teacher**, and **Admin** profiles.
* **Security:** Non-reversible **djb2** password hashing.
* **Q&A Engine:** Post questions with tags, attach diagram image paths (`image_path`), and submit answers.
* **Gamification:** Earn **+5 Reputation** per answer upvote; dynamic `qsort()` leaderboard.
* **Media & Search:** Avatar links (`profile_pic`), keyword thread search (`strstr`).
* **Admin Control:** Account deletion, role promotion/demotion, and content moderation.
* **Persistence:** Dynamic heap scaling (`malloc`/`realloc`) with delimited text file storage.

---

## Tech Stack & Project Structure

* **Language/Compiler:** C99 / GCC or Clang
* **Storage:** `users.txt`, `questions.txt`, `answers.txt`

```text
├── src/
│   ├── main.c        # Entry point & CLI loop
│   ├── user.h/user.c # Auth, djb2 hashing, & user management
│   └── qa.h/qa.c     # Thread lifecycle, upvotes, qsort leaderboard
└── Makefile          # Build automation
