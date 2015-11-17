#WakeOnWAN
This program waits until it receives an incoming TCP connection attempt on the 
given port. It then close sends a WakeOnLAN magic packet to the LAN it is 
connected to, causing the waking on of the computers with WakeOnLAN feature 
enabled.

##Dependencies
This software uses the wakeonlan utility by Pedro Oliveira to send magic
packets on the LAN segment. Wakeonlan can is available in the repository of
the majority of Linux distribution.

For example, on Debian and its derivates, it can be installed with:
	sudo apt-get update
	sudo apt-get install wakeonlan

##How to use this software
Simply start the server with:
	./server <mac-address> <listening-port>

##How it works
This is a pre-fork() server that listens for incoming TCP connection on the
given port. Once a connection is established, one of the process is woke up and
it executes the wakeonlan tool (through an execl() call).
