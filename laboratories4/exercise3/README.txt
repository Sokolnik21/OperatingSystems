Comparison of the three methods used in this exercise:
1. Send all SIGUSR1 signals and then SIGUSR2
  # lot of signals were lost
  - SIGUSR1 is non-realTime signal
    - they are not stored
    - when program receives the same signal that is currently handling
      it'll ignore new one (and all the rest that'll receive in that time)
  - STATISTICS: ~40/444 signals (10% success)
2. Send next signal when child ensure that signal is handled
  # not a single one signal is lost
  - SIGUSR1 is non-realTime signal
    - now it doesn't matter because main program provides that child will be
      handling one signal at a time
  - STATISTICS: 444/444 signals (100% success)
3. Use first method but this time instead of non-realTime signals use realTime
  # almost every signal were caught
  - SIGRTMIN + N is realTime signal
    - they are stored in queue
      Q: if they are stored in queue then why not every signal were caught?
        - at the end main program sends different signal to inform child to end
        - when child receives the same signals it handles them from the latest
          to the newest
        - when child receives different signals then that order doesn't matter
      A: so it must happened that child had has in his queue equivalent of
        SIGUSR1 signals when equivalent of SIGUSR2 occurs
  - STATISTICS: ~430/444 signals (95% success)
