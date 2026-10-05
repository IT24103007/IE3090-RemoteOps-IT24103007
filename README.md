# RemoteOps - IE3090 Network Programming

## Student Details
Registration Number: IT24103007

## Personalised Configuration
- Agent Port: 9410
- Session ID (SID): 7003
- Authentication Token: OPS-3007
- Agent Source: agent_007.c
- Controller Source: controller_007.c
- Makefile: Makefile_007
- Log File: remoteops_IT24103007.log
- Storage Path: ./agentfiles/IT24103007/
- Submission Archive: IE3090_IT24103007.zip

## Project Overview
RemoteOps is a client/server remote system monitoring and management tool implemented in C using BSD sockets.

The Agent acts as the server and the Controller acts as the client.

The project uses:
- TCP for control commands and file transfers.
- UDP for periodic system monitoring.

## Status
Implementation completed and tested.

Implemented features:
- Personalised TCP Agent/Controller communication.
- Authentication using the personalised token.
- SYSINFO and LISTPROC commands.
- Whitelisted EXEC command handling.
- PUT and GET file transfer.
- File transfer validation.
- Concurrent client handling using pthreads.
- Graceful client disconnect.
- Thread-safe logging.
- UDP monitoring using MONITOR START and MONITOR STOP.

Testing evidence and project documentation have been prepared for final submission.
