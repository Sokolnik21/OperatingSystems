To do:
  - CPU time  (seconds)
  - virtual memory usage  (MegaBytes)

Restrictions:
  - handling by child process (right before execvp())
  - use setrlimit() function

Reports:
  - for every child process save to file information about
    resources that were used (CPU time, virt. mem. usage)
    and time needed to execute process (USER and SYS time)
  - use getrusage() with RUSAGE_CHILDREN flag

(*) Bonus:
  - write program that exceed set limits
