# Networking-in-CPP

This is a Asyncronous, low-latency implementation of a websocket server and client with thread safe data structures 
the purpose of the project was to connect multiple clients to the server and the server handels the requests sent by each client 
using multiple threads, the requests will be of a limited types eg. BUY,SELL,MODIFY
and the server then connects to some Remote API to complete the operation to return the results back to the clients


sources used while creating this project:
BOOST::BEAST documents 
BOOST::ASIO documents
Youtube tutorials:
