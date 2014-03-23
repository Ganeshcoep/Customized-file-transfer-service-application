Customized-file-transfer-service-application
============================================

OVERVIEW

The application consists of two parts (i) Server Part and (ii) Client Part running possibly on different machines. Basically, the server hosts a number of files that are located in some folder called "data". A client can communicate with the server and request the server transfer the file to the client (i.e., download file). However, instead of receiving the file in whole, the client requests chunks of the file in parallel with multiple threads and those chunks are assembled together to form entire file.

SERVER PART

            The server basically provides two distinct services (i) file look-up service (ii) file transfer service. File look-up service is a UDP service. Clients send UDP packets containing a file name. As the server gets the message, it checks the list of the file that it hosts and sends the file size (in bytes) if it hosts the file otherwise sends zero. File transfer service is a is multi-threaded TCP service. Clients first initialize a TCP connection with the server before sending file transfer request. Whenever a TCP connection is created the server initializes a new thread (or possibly uses an idle thread in a thread pool) and transfer the rest of the communication to the new thread. Client sends a request consisting of three fields: file name, starting byte number (offset), and number of bytes to transfer. File name is the name of the file that the client wants to download. Starting byte number (s) is the byte number in the file at which the client wants to start the download (i.e. initial byte of a chunk). Number of bytes to transfer is the total number of the bytes that the client wants to download after an offset of starting byte number. When the server-side thread gets the message it sends the requested portion of the file and completes the file transfer.

 

CLIENT PART

            The client is single-threaded. Whenever it needs to download a file from a server first, it sends a UDP look-up message. If the response (file size) is positive it divides the obtained file size with k in order to determine the offset and the number of bytes needs to be received for each chunk. The client immediately creates k TCP sockets (each for a chunk) and uses "select(...)" function for asynchronous IO. That is, the client downloads the chunks in parallel and uses IO multiplexing instead of multi-threading. When all chunks are received it simply merges the chunks into a single file.

