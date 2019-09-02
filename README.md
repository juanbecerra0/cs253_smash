#Name

Smash - $() - Basic command shell, similar to the Linux bash shell.

#Synopsis

./smash... LAUNCH
$ [OPTION] [ARGS*]... COMMAND

#Description

Basic command shell built for a CS253 (Systems Programming) course.
Has a set of commands to emulate the functionality of a regular
unix-like command shell. Some commands, like cd, only simulate
expected functionality.

cd [directory]
	Change directory if directory argument exists
	
	**Options:
		'~'  -- home
		'..' -- previous directory (up)

history
	Prints history of commands since launch
	NOTE: I/O redirection tokens are not displayed in history log

[commandPath] [commandArgs]...[commandArgs]
	Executes an external command if it exists

	**Options:
		[CTRL + C] -- Stops the current external process, but not Smash

	**Redirector Usage (< and >):
		Ensure that redirectors are their own token.
		For example:
		
		VALID
			ls -a < a.txt > b.txt

		INVALID
			ls -a< a.txt> b.txt
	

exit
	Exits Smash

#P4 Implemented Functions and Known Bugs

Note: All functionality for Smash 1, 2, and 3 was implemented in this iteration

Implementation for Smash 4:
1. Smash redirects stdin.
	1.1. NOTE: This requires a seperate token (see external command usage)
2. Smash redirects stdout
	2.1. NOTE: This requires a seperate token (see external command usage)
3. Commands can be pipelined.
	3.1. NOTE: Only one pipe can be used. Additional pipes result in an error message.
	3.2. BUG: Results sent to stdout are only displayed after the 'exit' command is used.
4. Ctrl+C only terminates the executing command, not the smash shell.
	4.1. BUG: Terminating an executing command makes it so the "$ " in stderr no longer appears
		  after subsequent external commands, though it does show up in subsequent external
		  commands
5. NOTE: Threads have not been implemented.

#Author

Written by Juan Becerra. Base code and instructions written by Boise
State University's Department of Computer Science.
