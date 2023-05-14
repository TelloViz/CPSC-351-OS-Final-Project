 # Time Analysis of Parallel vs. Serial Fetchers
 
> In your submission, please include the answers to the following questions:
 
### 1. In the output of time, what is the difference between real, user, and sys times?

``` real ``` : measure of the processing time, from start until completion, that has passed in real-time.

``` user ``` : measure of the processing time spent by the CPU executing the user's code and its various computational tasks.

``` sys ``` : measure of the processing time spent by the CPU executing system calls/IO tasks.

### 2. Which is longer: user time or sys time? Use your knowledge to explain why.

```
time ./serial
----------------
real    0m2.063s
user    0m0.340s        <- slower
sys     0m0.008s        <- faster
----------------
```
```
time ./parallel
----------------
real    0m0.464s
user    0m0.345s        <- slower
sys     0m0.016s        <- faster
----------------
```

My results show that ```system``` time is generally faster than ```user``` time. There are multiple reasons why this is generally the case. Some of these reasons include, but may not be limited to:

- The system is highly scrutinized, revised, optimized, and maintained to be as efficient as possible.
- The system is making low level calls that are processed on registers and other low level hardware that are inherently much faster than much of the user-level code.
- User code is making network calls, printing console output, and other time consuming tasks that the system-level time is not representative of.

### 3. When fetching all of the locations in the file, which fetcher finishes faster? Why do you think that is?

The parallel fetcher finishes the file I/O operations in less real-time overall. This is likely because, while the serial fetcher is stuck waiting for each child to finish before reading subsequent lines, the parallel fetcher is already moving on to the next line, and the next fork operation.

However, the implementations of the file I/O code is identical between both programs so the amount of processing time spent on those calls alone should be the same on average.

### 4. Suppose one of the fetcher children crashes. Will this affect other children? Explain.

The answer(S) to this question are very circumstantial in nature. It depends on how the errors are caught/handled/analyzed. It depends on how the program logic is structured. It depends on how you define "affect other children" as well. On the surface, the children aren't sharing any pools of data. The children are indpendent processes and their actions in this instance are not affected by each other's successes or failures in the direct sense. 

The way they can affect each other though is through the success of the forking entity that is their shared parent. If the parent is terminated due to the failure of a particular child, then any subsequent children left to be forked may never get forked. If the parent handled the child's failure in a way that allows it to move on to the next forking operation, then the subsequent children would be unharmed, given that the data they are initially given to do their task is not also corrupted. The forks done prior to that failed child would also remain unbothered potentially.

If any time a child fails in its operations and returns a value indicating its failure, if read at surface value, then the parent may terminate itself or not based on that. This is where the children may or may not "affect" each other...

It also depends on weather(lol pun) or not the children are compiling data that the parent, or other forks will later need for the successful processing of the potential calculation using said data to be achieved. In this programs case, the children are indpendently fetching api data and writing it to a file, the parent lets them free, then just waits for each one (serial) or all of them (parallel) blindly and without a care for the files they are writing or the data within them.

-------------

```Serial``` :

The way my serial fetcher is implemented, the parent simply exits upon discovery of a wait error. If we ignored this error, we could potentially move onto the next fork. In a safer way, we could dive deeper into the nature of the failed child and process accordingly, which would also allow us to keep forking the subsequent children potentially. In my code, as i mentioned, the parent simply exits on the discovery of a wait error. In that sense, if there is a wait error in fork 1, then forks 2-5 would never happen. In that sense, then 1 child is causing the forkign of subsequent children to never happen. However, none of the children come into contact with one another, and are distinct instances of processes doing their own thing.

-------------

-------------

```Parallel``` : 

I utilized the macro ECHILD to differentiate between "end of children" and some other potential wait error. In the case of any other wait error, my program will terminate due to the error as it currently is implemented. If I simply commented out the line ```exit(1)``` in this error catchign code, my program would effectively ignore the error, while the ECHILD check saves us from the endless loop caused by ignorning -1 values altogether. So, in the case of "children affecting other children", commenting out the ```exit(1)``` or handling it in more detail, would save the other children from potentially having their parent axed prematurely. This is one way that children might "affect" one another in the parallel program case.

```
    do                                                    // parent loops on wait
    {   
        waitReturn = wait(&childEventInfo);
        if (waitReturn < 0                               // if there's a wait error
            && errno != ECHILD)                          // and its not a "No More Children" error...
        {
            perror("Waiting error!");                    // signal the error
            exit(1);                                     // terminate with error code
        }
        else if(waitReturn < 0 && errno == ECHILD) std::cerr << "All children have been waited for...\n";
        
        else if (WIFEXITED(childEventInfo))                    // If child terminated normally
        {
            // Print the terminated child's process id
            fprintf(stderr, "Parent: The child with pid=%d has terminated\n", waitReturn);

            // WEXITSTATUS extracts the child's exit code from the childEventInfo
            fprintf(stderr, "Parent: The child's exit code is %d\n", WEXITSTATUS(childEventInfo));
        }

    }while (waitReturn > 0);
```
-------------


> Summary: The potentiality of one child's failure and premature termination having an affect on 
> another child's success, is completely subjective to the implementation details of the program, and 
> the problem it's trying to solve.



### 5. What is your conclusion regarding the benefits or disadvantages of isolating each fetch task within a different process?

``` Benefits``` :

- Separation of concerns is one way to orgnaize and modularize logic that makes it more digestible, thus easier to understand and potentially debug.
- Smaller parts can be done quickly without being hung up on other parts they have no connection to otherwise.
- The OS and/or system is leveraged, with its robust, well written structure. This will beat any user-written code any day. This can be seen in the time results.
- Results can be sped up when using multi-process design methodologies (circumstantial).

- Many more...

```Disadvantages``` : 

- Can easily become a chaotic and tangled rats nest, which can be hellish to debug.
- More moving parts, or workers to deligate doesn't always mean easier task. (The phrase "Too many hands in the pot")
- Synchronization can get messy when data is needed between processes, or there is a need for inter-proc comms.
- Results can be slowed down when using multi-process design methodologies (circumstantial).
- Many more...

Overall, like with ANYTHING in the world of computers, it really depends on the needs of the problem statement, the limits of the implementation, and the specific system that the solution is being implemented or run on.