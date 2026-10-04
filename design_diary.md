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
### Authentication and SYSINFO
Implemented Controller authentication using the personalised token OPS-3007 and SID 7003. After successful authentication, the Controller can request SYSINFO from the Agent. SYSINFO now returns the Linux system load, used memory in MB, and system uptime in seconds. The Agent and Controller communication was tested successfully on TCP port 9410.
