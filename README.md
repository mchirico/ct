ct
==

ct (Connection Test) - simple tool for testing a connection, without blocking.



 Example Usage:

    $ ./ct gmail.com 80
    03-16-2013 12:59:32.20282,gmail.com,74.125.226.213,80,Connected,***   Good  ***

    $ ./ct gmail.com 81
    03-16-2013 13:00:10.79637,gmail.com,74.125.226.213,81,No Connection

  You can also do a range of ports with multiple hosts:

    $ ./ct gmail.com,google.com 80,440-444
    03-17-2013 18:30:43.75353,gmail.com,74.125.226.213,80,1,Connected,*** GOOD ***
    03-17-2013 18:30:43.75884,gmail.com,74.125.226.213,440,0,No Connection
    03-17-2013 18:30:43.76059,gmail.com,74.125.226.213,441,0,No Connection
    03-17-2013 18:30:43.76340,gmail.com,74.125.226.213,442,0,No Connection
    03-17-2013 18:30:43.76632,gmail.com,74.125.226.213,443,1,Connected,*** GOOD ***
    03-17-2013 18:30:43.76902,gmail.com,74.125.226.213,444,0,No Connection
    03-17-2013 18:30:43.77197,google.com,74.125.226.201,80,1,Connected,*** GOOD ***
    03-17-2013 18:30:43.77539,google.com,74.125.226.201,440,0,No Connection
    03-17-2013 18:30:43.77866,google.com,74.125.226.201,441,0,No Connection
    03-17-2013 18:30:43.78133,google.com,74.125.226.201,442,0,No Connection
    03-17-2013 18:30:43.78508,google.com,74.125.226.201,443,1,Connected,*** GOOD ***
    03-17-2013 18:30:43.78798,google.com,74.125.226.201,444,0,No Connection


 Getting and Compiling the Program:

    wget https://raw.github.com/mchirico/ct/master/src/ct.c
    gcc ct.c -o ct -lpthread

