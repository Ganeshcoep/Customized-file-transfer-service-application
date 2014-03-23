Customized-file-transfer-service-application
============================================

The application consists of two parts (i) Server Part and (ii) Client Part running possibly on different machines. Basically, the server hosts a number of files that are located in some folder called "data". A client can communicate with the server and request the server transfer the file to the client (i.e., download file). However, instead of receiving the file in whole, the client requests chunks of the file in parallel with multiple threads and those chunks are assembled together to form entire file.
