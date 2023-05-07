// Serial Fetcher Requirements:
// The serial fetcher shall be invoked as ./serial and fetch locations one by one. After the parent
// process has read and parsed an entry from input.txt file, it shall proceed as follows:

// The parent process forks off a child process.

// The child will print it’s process ID (you can use getpid() system call to retrieve process’s
// ID) and the process ID of it’s parent (you can use getppid() system call to do this).

// The child and parent should also print their UID using the getuid() and GID using the
// getgid() system calls. Each process is assigned a UID and GID when it is launched.

// Typically, these are the same as the UID of the user who has launched the process and
// the GID of that user’s default group. When the process accesses a file/directory, the OS
// checks these process UID/GID values against the permissions of the file/directory to
// enforce permissions.

// Based on this, do children inherit the user and group from the parent?

// The child uses execlp("/usr/bin/curl", "curl",... other arguments NULL) system
// call in order to replace its program with curl program that will fetch the weather
// information for the latitude/longitude location and save it in fileX.json where X represents
// the location number. The first location will be saved in file named file1.json,
// second in file2.json etc. You can check the file called numberedfilegen.cpp to see how
// to dynamically generate names of files.

// The parent executes a wait() system call until the child exits.
// The parent forks off another child process which downloads the next location specified in
// input.txt.

// Repeat the above steps until all files are downloaded.