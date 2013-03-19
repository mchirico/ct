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

 Note that it runs pretty quick (adjust MAX_WORKER_THREADS beyond 30):

    real	   0m1.124s
    user	   0m0.003s
    sys	           0m0.007s



##Getting and Compiling the Program (It's just one source file):

    wget https://raw.github.com/mchirico/ct/master/src/ct.c
    gcc ct.c -o ct -lpthread



 Although nmap is often faster, there are situations (Linux on Mac OS X vm),
 where nmap is extremely slow:
  
    time nmap -PN -p 80 gmail.com

    Starting Nmap 5.51 ( http://nmap.org ) at 2013-03-19 02:51 PDT
    Nmap scan report for gmail.com (74.125.226.214)
    Host is up (0.026s latency).
    Other addresses for gmail.com (not scanned): 74.125.226.213
    rDNS record for 74.125.226.214: lga15s28-in-f22.1e100.net
    PORT   STATE SERVICE
    80/tcp open  http

      Nmap done: 1 IP address (1 host up) scanned in 6.60 seconds

    real 0m6.606s
    user 0m0.028s
    sys  0m0.014s


 Compare this to ct

    time ./ct gmail.com 80
    03-19-2013 02:52:08.333391,gmail.com,74.125.226.213,80,1,Connected,*** GOOD ***

    real   0m1.005s
    user   0m0.001s
    sys	   0m0.003s


 Note that ct also allows easy mixing of IP addresses and hostnames (no switches, just 
 simple commands).

    time ./ct openadsi.com,wsj.com,google.com,nyt.com,amazon.com,192.168.1.1,192.168.1.6 80
    03-19-2013 03:13:06.552708,openadsi.com,107.21.122.231,80,1,Connected,*** GOOD ***
    03-19-2013 03:13:06.553387,wsj.com,205.203.132.65,80,1,Connected,*** GOOD ***
    03-19-2013 03:13:06.554087,google.com,74.125.226.198,80,1,Connected,*** GOOD ***
    03-19-2013 03:13:06.554681,nyt.com,170.149.168.130,80,1,Connected,*** GOOD ***
    03-19-2013 03:13:06.555606,amazon.com,205.251.242.54,80,1,Connected,*** GOOD ***
    03-19-2013 03:13:06.555511,192.168.1.1,192.168.1.1,80,1,Connected,*** GOOD ***
    03-19-2013 03:13:06.555413,192.168.1.6,192.168.1.6,80,1,Connected,*** GOOD ***

    real   0m1.007s
    user   0m0.000s
    sys	   0m0.004s


 The program is still alpha, and the idea is to keep it all in one program, which
 is what SQLite is doing with it's amalgamation.
