## Student Name: Zachary Force

# Assignment 3: Unix Shell

In this project, you'll build a simple Unix shell. The shell is the heart of the command-line interface, and thus is central to the Unix/C programming
environment. Mastering use of the shell is necessary to become proficient in this world; knowing how the shell itself is built is the focus of this project.
There are three specific objectives to this assignment:
- To further familiarize yourself with the Linux programming environment.
- To learn how processes are created, destroyed, and managed.
- To gain exposure to the necessary functionality in shells.

## Overview
In this assignment, you will implement a command line interpreter (CLI) or, as it is more commonly known, a shell. The shell should operate in this
basic way: when you type in a command (in response to its prompt), the shell creates a child process that executes the command you entered and then
prompts for more user input when it has finished. The shells you implement will be similar to, but simpler than, the one you run every day in Unix.
If you don't know what shell you are running, it's probably `bash`.

## Program Specifications

### Basic Shell: grsh

Your basic shell, called `grsh` (short for Golden Ram Shell), is basically an interactive loop: it repeatedly prints a prompt `grsh> `
(*note the space after the greater-than sign*), parses the input, executes the command specified on that line of input, and waits for
the command to finish. This is repeated until the user types `exit`. The name of your source code should be `grsh.c`.

The shell can be invoked with either no arguments or a single argument; anything else is an error. Here is the no-argument way:

```
prompt> ./grsh
grsh>
```

At this point, grsh is running, and ready to accept commands. Type away!

The mode above is called interactive mode, and allows the user to type commands directly. The shell also supports a batch mode, which instead
reads input from a batch file and executes commands from therein. Here is how you run the shell with a batch file named batch.txt:

```
prompt> ./grsh batch.txt
```

One difference between batch and interactive modes: in interactive mode, a prompt is printed (`grsh> `). In batch mode, no prompt should be printed.
You should structure your shell such that it creates a process for each new command (the exception are built-in commands, discussed below). Your basic
shell should be able to parse a command and run the program corresponding to the command. For example, if the user types `ls -la /tmp`, your shell
should run the program `/bin/ls` with the given arguments `-la` and `/tmp` (how does the shell know to run `/bin/ls`? It's something called the shell path;
more on this below).
