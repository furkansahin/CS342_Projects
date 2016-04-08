# Project1
The project is created for CS 342 course. It includes 2 different applications.

## Index Generator
Index generator simply takes 3 parameters.
* Number of child processes
* Input file name
* Output file name

It takes a file and creates child processes. It finds the indexes of the words in input file and sorts both lexicographycally and according to the line they occured. Creates and index file with the specified name and outputs to there.
The application uses POSIX message queues for parent-child intercommunication.

### Usage
* go to project directory
* run "make"
* run "./indexgen <# of processes> < input file name > < output file name >"
* check output file

## Kernel Module
Written kernel module outputs the processes that are in idle state. 

### Usage

* go to project directory,
* run "make"
* run "sudo insmod simplemodule.ko" to insert the created module
* run "dmesg" to see the output of the module.
* run "sudo rmmod simplemodule.ko" to remove the module


