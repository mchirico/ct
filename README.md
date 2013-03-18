ct
==

ct (Connection Test) - simple tool for testing a connection, without blocking.



 Example Usage:

    $ ./ct gmail.com 80
    03-16-2013 12:59:32.20282,gmail.com,74.125.226.213,80,Connected,***   Good  ***

    $ ./ct gmail.com 81
    03-16-2013 13:00:10.79637,gmail.com,74.125.226.213,81,No Connection

 You can also do a range of ports with multiple hosts:

    $ time ./ct gmail.com,google.com,wsj.com 80,442-443
    03-17-2013 20:46:53.729206,gmail.com,74.125.226.214,80,1,Connected,*** GOOD ***
    03-17-2013 20:46:53.729728,gmail.com,74.125.226.214,442,0,No Connection
    03-17-2013 20:46:53.730118,gmail.com,74.125.226.214,443,1,Connected,*** GOOD ***
    03-17-2013 20:46:53.757564,google.com,74.125.226.231,80,1,Connected,*** GOOD ***
    03-17-2013 20:46:53.758006,google.com,74.125.226.231,442,0,No Connection
    03-17-2013 20:46:53.758449,google.com,74.125.226.231,443,1,Connected,*** GOOD ***
    03-17-2013 20:46:53.785137,wsj.com,205.203.140.1,80,1,Connected,*** GOOD ***
    03-17-2013 20:46:53.785592,wsj.com,205.203.140.1,442,0,No Connection
    03-17-2013 20:46:53.785968,wsj.com,205.203.140.1,443,1,Connected,*** GOOD ***

 Note that is runs pretty quick:

    real	   0m1.124s
    user	   0m0.003s
    sys	           0m0.007s


 Getting and Compiling the Program (It's just one source file):

    wget https://raw.github.com/mchirico/ct/master/src/ct.c
    gcc ct.c -o ct -lpthread


 The program is still alpha.

