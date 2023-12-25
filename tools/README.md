This directory contains a number of very very simple scripts automating repetitive and time consuming tasks:

- *copy\_from.sh*: Simply copies 01.cpp, 02.cpp and Makefile from the given directory. Mostly useless in isolation, but called from init
- *fetch\_input.sh*: Downloads the problem input and stores it in a file appropriately named *input*. The default Makefile has targets that start a solution with this file piped as stdin. It checks the current working directories name to know which day you are working on. **DO NOT CALL THIS MORE THAN ONCE FOR EACH DAY!** The poor server really has to do enough without us spamming it with request. Better yet, don't ever call it manually.
- *init.sh*: Calls copy\_from with the default files (in *base*), followed by fetch\_input and then starts up [kakoune](https://github.com/mawww/kakoune) in an appropriately split [kitty](https://github.com/kovidgoyal/kitty) session with the newly created files open and ready to be worked on.
- *submit\_result.sh*: Submits the given result for the given part and filters the output just enough so you can see if it was correct or not. Is called from the default Makefile

As you can see, most of them are just building blocks. You, as a user, should normally just call init.sh once and everything else will be taken care of.
