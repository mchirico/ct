ct
==

ct (Connection Test) - simple tool for testing a connection, without blocking.



 Example Usage:

    $ ./ct gmail.com 80
    03-16-2013 12:59:32.20282,gmail.com,74.125.226.213,80,Connected,***   Good  ***

    $ ./ct gmail.com 81
    03-16-2013 13:00:10.79637,gmail.com,74.125.226.213,81,No Connection

  Or, you can but this in a quick script. It will never block, so your script
  can go about it's business.

    $ for i in $(seq 79 84); do ./ct gmail.com ${i}; done
    03-16-2013 13:15:35.20628,gmail.com,74.125.226.245,79,No Connection,timeout
    03-16-2013 13:15:36.24639,gmail.com,74.125.226.246,80,Connected,***   Good  ***
    03-16-2013 13:15:37.28678,gmail.com,74.125.226.245,81,No Connection,timeout
    03-16-2013 13:15:38.32927,gmail.com,74.125.226.246,82,No Connection,timeout
    03-16-2013 13:15:39.37058,gmail.com,74.125.226.245,83,No Connection,timeout
    03-16-2013 13:15:40.41139,gmail.com,74.125.226.246,84,No Connection,timeout


 Getting and Compiling the Program:

    wget https://raw.github.com/mchirico/ct/master/src/ct.c
    gcc ct.c -o ct -lpthread

