# master-slave-socket
Master-Slave / Worker-Supervisor Architecture Implemented using Socket Programming in C. 

### About
- The code distributes the workload on the available servers equally and the extra workload that remains (if equal divisions are not possible) is assigned in a round-robin fashion. 
- It implements a basic addition of consecutive numbers in an array. 

### To Run
- First compile and run the server files. Replicate the server files as many as you want and do the required changes in client file and socket paths for each server.
- Compile and run the client file.

## Possible Improvements
- Use multi-threading at each server for faster calculations.
- Use of single file to run multiple servers (I got lazy and left it with multiple files 🤐)
