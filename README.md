# 2021 NUAA Computer Network Experiment
 
An SMTP client written in C++.

Running on Windows.

Use this command to compile:

> g++ -std=c++11 main.cpp -o main.exe -lwsock32

Use client.py to run the GUI.(above python 3.7.6,use tkinter)

main.exe reads mail's subject and content from file 'subject' and 'content'.