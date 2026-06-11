*This project has been created as part of the 42 curriculum by mait-mou.*

# Codexion

## Description

Codexion is a concurrency simulation inspired by the Dining Philosophers problem. Multiple coders sit in a circle, each needing two USB dongles at the same time to compile their quantum code. Dongles are shared between neighbors, so coders must coordinate access fairly without deadlocking or starving.

Each coder runs as a POSIX thread and cycles through three phases: **compile → debug → refactor**. The simulation ends either when every coder has compiled the required number of times, or when one burns out from waiting too long.

Key features:
- Dongle cooldown: after a dongle is released, it cannot be used again for `dongle_cooldown` ms
- Two scheduling policies: **fifo** (first come, first served) and **edf** (earliest deadline first)
- A monitor thread detects burnout within 10 ms and stops the simulation

## Instructions

```bash
make          # build
make clean    # remove objects
make fclean   # remove objects + binary
make re       # rebuild
```

```bash
./codexion <nb_coders> <time_to_burnout> <time_to_compile> <time_to_debug> \
           <time_to_refactor> <nb_compiles_required> <dongle_cooldown> <scheduler>
```

All arguments are mandatory. The program rejects negative numbers, non-integers, and unknown schedulers.

| Argument | Description |
|---|---|
| `nb_coders` | Number of coders (= number of dongles) |
| `time_to_burnout` | ms before a coder burns out if it hasn't started compiling |
| `time_to_compile` | ms spent compiling (holds 2 dongles) |
| `time_to_debug` | ms spent debugging |
| `time_to_refactor` | ms spent refactoring |
| `nb_compiles_required` | simulation stops when all coders reach this count |
| `dongle_cooldown` | ms a dongle is unavailable after being released |
| `scheduler` | `fifo` or `edf` |

**Examples:**

```bash
./codexion 5 800 200 200 200 3 50 fifo
./codexion 4 1000 200 200 200 5 100 edf
```

**Log format:**

```
0 1 has taken a dongle
1 1 has taken a dongle
1 1 is compiling
201 1 is debugging
401 1 is refactoring
1505 4 burned out
```

## Blocking Cases Handled

**Deadlock prevention** — The classic deadlock (each coder holds one dongle and waits for the other) is broken by never partially acquiring dongles. Each dongle has its own waiting queue (the heap). A coder enqueues on both dongles before waiting, so no coder holds one dongle while blocking on the other. Circular wait cannot form.

**Starvation prevention** — Under `fifo`, requests are served strictly in arrival order. Under `edf`, the coder whose deadline (`last_compile_start + time_to_burnout`) is earliest is served first, so the most at-risk coder always gets priority.

**Cooldown handling** — `released_at` is stamped per dongle on release. When a coder wins the queue but the cooldown has not expired yet, it sleeps exactly the remaining time (`precise_sleep`) then re-checks. This avoids busy-waiting while still respecting the cooldown window precisely.

**Burnout detection** — A dedicated monitor thread polls every ~1 ms. It computes `now - last_compile_start` for each coder and logs `X burned out` within 10 ms of the actual deadline. On burnout it sets `info->stop` and broadcasts on all dongle condition variables so blocked threads wake up and exit cleanly.

**Log serialization** — Every call to `log_state()` acquires `info->print_mutex` before writing, so two messages can never be interleaved on the same line.

## Thread Synchronization Mechanisms

**Mutexes** (`pthread_mutex_t`):
- `dongle->mutex` — guards `taken`, `released_at`, and the waiter heap for each dongle
- `coder->mutex` — guards `last_compile_start` and `compile_count`, read by the monitor and written by the coder thread
- `info->stop_mutex` — guards the global `stop` flag
- `info->print_mutex` — serializes all stdout output
- `info->finish_mutex` — guards `finished_coders`, incremented when a coder hits its required compile count

**Condition variables** (`pthread_cond_t`): Each dongle has a `cond`. When a coder cannot take a dongle it enqueues itself in the heap and calls `pthread_cond_wait`. On `drop_dongle`, `pthread_cond_broadcast` wakes all waiters. Each waiter re-checks: only the head of the heap proceeds, others go back to waiting. This broadcast + re-check loop prevents lost wakeups and spurious wakeup issues.

**Odd/even ordering** — In `coder_routine`, odd-numbered coders take `right` then `left`; even-numbered take `left` then `right`. Combined with the heap-based serialization, this further reduces contention and eliminates the symmetric deadlock scenario.

**Monitor** — Runs as a separate `pthread_t`. Reads `last_compile_start` under `coder->mutex` to avoid a data race with the coder thread writing it. Once `stop` is set it broadcasts on all dongle `cond` variables so every waiting thread unblocks, checks the flag, and returns — allowing `pthread_join` in main to complete cleanly.

## Resources
- What is a thread — https://hpc-tutorials.llnl.gov/posix/what_is_a_thread/#pthreads-overview-what-is-a-thread
- Mutex Overview — https://hpc-tutorials.llnl.gov/posix/mutex_variables/
- Condition Variables Overview — https://hpc-tutorials.llnl.gov/posix/condition_variables/

- Dining Philosophers problem — https://en.wikipedia.org/wiki/Dining_philosophers_problem
- Deadlock — https://www.geeksforgeeks.org/operating-systems/introduction-of-deadlock-in-operating-system/
- Race condition — https://www.geeksforgeeks.org/operating-systems/race-condition-in-operating-systems/
- Context Switching — https://www.geeksforgeeks.org/operating-systems/context-switch-in-operating-system/
