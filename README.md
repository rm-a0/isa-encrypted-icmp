# Encrypted File Transfer
## Assignment Details

**Course:** ISA - Network Applications and Network Administration  
**Academic Year:** 2025/2026  
**Title:** File Transfer via Covert Channel (Dr. Grégr)  
**Supervisor:** Ing. Matěj Grégr, Ph.D.  

**Description:**  
Develop a client/server application to transfer a file through a covert channel using ICMP/ICMPv6 Echo-Request/Response messages. The file must be encrypted (using AES with your login as the key) before transmission to avoid plaintext transfer. The client sends the file to a specified IP/hostname, while the server (in listen mode) receives, decrypts, and saves it.  

**Execution:**  
`secret -r <file> -s <ip|hostname> [-l]`  
- `-r <file>`: Specifies the file to transfer (e.g., `secret -r ../test.file`).  
- `-s <ip|hostname>`: Target IP address or hostname.  
- `-l`: Runs the program as a server, listening for ICMP messages and saving the file in the current directory.  

**Details:**  
- Use only ICMP/ICMPv6 Echo-Request/Reply messages.  
- Define a protocol for data transfer (e.g., include filename, verify complete transfer). Document this protocol.  
- Handle files larger than 1500B by splitting them into multiple packets.  
- Assume no packet loss, but document any reliable transfer mechanisms if implemented.  
- Use standard libraries (e.g., netinet/*, sys/*, arpa/*, pthread, pcap, SSL) and C/C++ (ISO/ANSI, POSIX, STL). No other libraries allowed.  
- Handle invalid inputs and output clear error messages to stderr.  
- Ensure portability; compile on `merlin.fit.vutbr.cz` (GNU/Linux). Specify minimum library versions in documentation if needed.  

**Documentation:**  
- Replace the general Readme with `secret.1` in man page format (see https://liw.fi/manpages/).  
- Provide a PDF documentation per general assignment guidelines.  
- Highlight any extensions in the man page and documentation.  
- Justify and document any unspecified design choices.  
- Apply best practices for code formatting, comments, file naming, and user-friendliness.  

**Reference Environment:**  
- Compile and test on `merlin.fit.vutbr.cz`. Testing on other GNU/Linux systems is allowed, but document any specific requirements.  

**Reference:**  
- AES encryption: https://wiki.openssl.org/index.php/EVP_Symmetric_Encryption_and_Decryption