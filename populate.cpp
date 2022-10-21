#include <chrono>
#include <errno.h>
#include <fcntl.h>
#include <iostream>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

int main(int argc, char** argv){
    std::chrono::time_point<std::chrono::system_clock> start, end;
    start = std::chrono::system_clock::now();
    srand(439); 

    // you need some file, you can change the name / permissions though
    // the return type of open is a bit different from our Shared<Node>
    // from p5. How can you possibly represent a file in an integer?
    //
    // This is called a file descriptor. You'll learn more about it later, 
    // but basically this number corresponds to the file that you care about.
    // The OS has a special map, so now if you call read / write / mmap using
    // this file descriptor, the OS knows which file you actually wanted to
    // read / write / mmap

    int fd = open("data", O_RDWR);
    struct stat statbuf;
    fstat(fd, &statbuf);
    int size = statbuf.st_size;
    printf("size is %d\n", size);
    
    void* addr = 0;

    // easy / hacky way to pass cmdline args
    if (argc == 1) {
        // do demand paging if no additional args
        printf("Demand paging\n");
        addr = mmap(addr, size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
    } else {
        // load all at once if additional args
        printf("Map populate\n");
        addr = mmap(addr, size, PROT_READ | PROT_WRITE, MAP_POPULATE | MAP_PRIVATE, fd, 0);
    }

    // Always check your return values!
    if (addr == MAP_FAILED) {
        printf("Oh dear, something went wrong with mmap()! %s\n", strerror(errno));    
    }

    // TODO: run benchmark here
    // printf("%s", (char *) addr);
    for (int i = 0; i < size; i++) {
        *((int *)(addr + i)) = i;
    }
    
    end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;
    std::time_t end_time = std::chrono::system_clock::to_time_t(end);

    std::cout << "finished computation at " << std::ctime(&end_time) << "elapsed time: " << elapsed_seconds.count() << "s\n";
    munmap(addr, 1);
}

