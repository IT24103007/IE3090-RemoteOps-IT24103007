# Reflection

This project helped me understand how a remote operations system works using network programming in C. Before starting the implementation, I had to understand the difference between the Agent and Controller roles, TCP client-server communication, authentication, file transfer, concurrency, and UDP monitoring.

One of the main challenges was understanding TCP as a stream. During the PUT implementation, the command header and file data could arrive together in the same recv() call. I learned that the program cannot always assume that one recv() contains only one logical message. Handling the initial file bytes separately helped make the file transfer more reliable.

Another important challenge was supporting multiple clients. Using pthreads allowed the Agent to handle different client connections independently. This helped me understand why concurrency is important in a remote operations system. I also learned that concurrent threads can create problems when they access shared resources. Therefore, a mutex was used for thread-safe logging.

The project also improved my understanding of security. Instead of allowing arbitrary commands through EXEC, a whitelist was used to restrict execution to approved commands. Personalised authentication values, SID values, ports, and the storage directory were also used according to the assignment requirements.

I used ChatGPT mainly as a learning and development assistant. I used it for step-by-step explanations, debugging, implementation guidance, testing ideas, and documentation. I did not rely only on the generated suggestions. I tested the commands and code on my Ubuntu environment, checked compilation results, and corrected errors when they occurred. For example, a string formatting error during UDP monitoring caused a compilation failure, which was then corrected and tested again.

The project also taught me the importance of incremental development. Instead of implementing everything at once, features were added and tested separately, followed by meaningful Git commits. Overall, this assignment improved my practical understanding of socket programming, TCP and UDP communication, concurrency, file transfer, logging, testing, Git, and responsible use of AI during software development.
