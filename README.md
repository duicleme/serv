# serv

It's client/server model service project on the raspberry pi 4

Language : C++ ( g++ )

If you want to compile :->

g++ serv.cpp -pthread

then result a.out 

run serv  ( It is a Web Server ) :->

./a.out port_number
  
ex)  ./a.out 1234

after use web browser.

Address :->

http:// ip_address : port_number /index.html

ex) http://192.168.0.13:1234/index.html

will be connected SERV.
