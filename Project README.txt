
************ README File To Execute All The C Program Files From - Ganesh Salvi				    ************

The Extracted folder has TWO C program files and 1 README file and one folder named "Data" containing the abc.txt file which can be used for the download from Client Side.

C Program Files
		1. Client.c		
		2. Server.c

Please, follow the sequence of instructions given.

******* INSTRUCTIONS *******

1. Open a command pompt window and browse up to this extracted folder.

2. Compilation of all the files ->
	2.1 $ gcc Client.c -o Client
	2.2 $ gcc Server.c -o Server -lpthread

3. Open 2 or more different command prompt windows of different or same machines.
Consider these as Net01, Net02, Net03, Net04, Net05.

4. In every command prompt windows, browse up to this extracted folder.
  
5. Execution of compiled files ->
	5.1 In Net01 -> $ ./Server <server-udp-port> <server-tcp-port>
	5.2 In Net01 or Net02 -> $ ./Client <udp-port_of_server> <tcp-port_of_server>

6.Use "lookup" and "download" commands from the user until "quit" command has been entered. On entering the "quit" command the client quits.


Please Note:
1. To run the Server Program correctly, the "Data " foder has be pplaced in the same directory where "Server " Object file is located.
2. Client program will download file ( lets say abc.txt) in the same folder/location where  "Client " Object file is located.
3. To download a file successfully, user has to lookup the file first. Program takes the output of the lookup into consideration so it it mandatory to run lookup first for successful download.

