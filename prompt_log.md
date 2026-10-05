# AI Prompt Log

## 04 October 2026

### Tool
ChatGPT

### Prompt / Topic
Requested step-by-step guidance for starting the IE3090 RemoteOps assignment, including Ubuntu SSH setup, personalised assignment values, Git/GitHub setup, and initial project structure.

### How the output was used
The guidance was used to configure SSH access to Ubuntu, install GCC/Git/Make, calculate the personalised RemoteOps values, create the GitHub repository, and create the initial project structure. Commands and results were checked during each stage before continuing.

---

### Tool
ChatGPT

### Prompt / Topic
Requested step-by-step guidance to implement and test AUTH and SYSINFO in the RemoteOps Agent and Controller.

### How the output was used
The guidance was used to implement personalised authentication with token OPS-3007 and SID 7003, then replace temporary SYSINFO values with real Linux system load, memory usage, and uptime. The implementation was compiled and tested successfully.

---

### Tool
ChatGPT

### Prompt / Topic
Requested guidance for implementing LISTPROC and a whitelisted EXEC command.

### How the output was used
The guidance was used to implement process listing and restrict EXEC to approved commands instead of allowing arbitrary command execution. Both successful and rejected command cases were tested.

---

### Tool
ChatGPT

### Prompt / Topic
Requested guidance for implementing PUT and GET file transfer between the Controller and Agent.

### How the output was used
The guidance was used to implement file upload and download using the personalised storage directory. File transfer was tested with sample files and the transferred content was checked.

---

### Tool
ChatGPT

### Prompt / Topic
Requested help with TCP file-transfer handling and validation.

### How the output was used
The guidance was used to handle the case where the PUT command header and initial file bytes can arrive in the same TCP recv() call. Additional validation was also implemented for invalid filenames/paths and oversized files.

---

### Tool
ChatGPT

### Prompt / Topic
Requested guidance for handling multiple clients using pthreads.

### How the output was used
The guidance was used to implement thread-based client handling so multiple Controller connections could be served concurrently. The implementation was tested using five clients.

---

### Tool
ChatGPT

### Prompt / Topic
Requested guidance for graceful client disconnect and thread-safe logging.

### How the output was used
The guidance was used to implement clean client disconnection and protect log writing with a mutex so multiple threads could write safely. The resulting log file was checked as part of the evidence.

---

### Tool
ChatGPT

### Prompt / Topic
Requested guidance for implementing UDP monitoring with MONITOR START and MONITOR STOP.

### How the output was used
The guidance was used to implement a separate UDP monitoring thread. Monitoring messages containing SID 7003 were sent through UDP port 9411 at regular intervals. START and STOP behaviour was tested using a UDP listener.

---

### Tool
ChatGPT

### Prompt / Topic
Requested debugging help after a compilation error occurred during UDP monitoring development.

### How the output was used
The error was investigated and the incorrect string formatting was corrected. The Agent and Controller were then successfully compiled using Makefile_007.

---

### Tool
ChatGPT

### Prompt / Topic
Requested guidance for final testing, evidence screenshots, Git commits, and project documentation.

### How the output was used
The guidance was used to organise functional testing evidence, save screenshots for the required operations, maintain meaningful incremental Git commits, and prepare the design diary and documentation for final submission.
