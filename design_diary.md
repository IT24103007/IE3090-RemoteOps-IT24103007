# Design Diary

## 04 October 2026

Started the RemoteOps assignment.

Initial decisions:
- The project will be implemented in C using BSD sockets.
- The Agent will act as the server.
- The Controller will act as the client.
- TCP will be used for commands and file transfers.
- UDP will be used for periodic monitoring.
- Development and testing will be performed on Ubuntu through SSH from a Mac terminal.
- The Agent and Controller will initially be tested as separate processes on the same Ubuntu machine.

Initial personalised values were calculated from registration number IT24103007.

Personalised values:
- Student ID: IT24103007
- TCP port: 9410
- UDP port: 9411
- Authentication token: OPS-3007
- SID: 7003
- Storage directory: agentfiles/IT24103007

## Authentication and SYSINFO

Implemented Controller authentication using the personalised token OPS-3007 and SID 7003.

After successful authentication, the Controller can request SYSINFO from the Agent. SYSINFO returns Linux system load, used memory in MB, and system uptime in seconds.

The Agent and Controller communication was tested successfully on TCP port 9410.

## LISTPROC and EXEC

Implemented LISTPROC to provide process information from the Linux system.

For EXEC, a whitelist approach was selected instead of allowing arbitrary commands. This limits execution to approved commands and provides safer command handling.

Both successful and rejected EXEC cases were tested.

## File Transfer

Implemented PUT for uploading files from the Controller to the Agent and GET for downloading files from the Agent to the Controller.

A personalised storage directory was used for uploaded files:
agentfiles/IT24103007

File transfer validation was also tested, including invalid filename/path handling and oversized file handling.

During PUT implementation, TCP stream behaviour was considered because the PUT header and file data can arrive together in the same recv() call. The implementation therefore handles the initial file bytes separately before receiving the remaining file data.

## Concurrent Connections

The Controller and Agent were tested with multiple client connections.

A pthread-based approach was used so that each client connection could be handled independently. This allows multiple clients to communicate with the Agent without blocking other connections.

The implementation was tested using five concurrent clients.

## Graceful Disconnect and Logging

Graceful client disconnection was implemented so that a client can terminate the session cleanly.

Thread-safe logging was also added. A mutex is used to protect log writing when multiple threads may write to the log at the same time.

The log records command activity and monitoring events.

## UDP Monitoring

UDP was selected for periodic monitoring because the monitoring information does not require a TCP command-response connection for every update.

MONITOR START creates a separate monitoring thread. The thread periodically sends monitoring messages containing SID 7003 through UDP port 9411.

MONITOR STOP changes the monitoring state and allows the monitoring thread to stop cleanly.

The monitoring feature was tested with a UDP listener and repeated monitoring messages were successfully observed.

## Testing and Fixes

The implementation was compiled using the provided Makefile with:
make -f Makefile_007

Testing was performed for authentication, system information, process listing, command execution, file upload, file download, concurrent connections, graceful disconnect, logging, and UDP monitoring.

During UDP monitoring development, a string formatting error caused a compilation failure. The incorrect string was corrected and the project was successfully compiled again.

Final functional evidence was captured as screenshots for the required operations.

## Final Design

The final design uses:
- TCP port 9410 for Agent/Controller communication.
- UDP port 9411 for monitoring.
- pthreads for concurrent client handling.
- A separate monitoring thread for UDP status messages.
- A personalised authentication token and SID.
- A personalised storage directory for uploaded files.
- Thread-safe logging for concurrent operations.

The final implementation was tested successfully and pushed to the GitHub repository using meaningful incremental commits.
