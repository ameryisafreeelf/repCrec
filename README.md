# repCrec

(Replicated Concurrency Control and Recovery)

A simulation of a distributed database system. Important features are
* Deadlock detection (constructs a "waits-for" graph and checks for cycles
* Concurrency control (strict 2-phase locking- this also avoids cascading aborts)
* Site failure (uses an "available copies" approach to recover data items at failed sites

Please see repCrec_diagram.png for high-level design of classes. Note that the "manager" classes implement methods to control the lower-level classes (dependency injection). 

The text file repCrec_detailed_desc.txt describes important functions and design decisions.
