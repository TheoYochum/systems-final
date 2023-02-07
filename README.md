# Divide and Conquer

# Theo Yochum
        
# A statement of the problem you are solving and/or a high level description of the project.

Creating a program to utilize the stuyCS computers to demonstrate potential uses of distributed computing. Current ideas include easily paralellizable algorithms such as Merge Sort, nth prime.
    
# A description as to how the project will be used (describe the user interface).

It will be a single main driver file where you can select the algorithm to use, it will prompt for relvant info such as how far to calculate, potentially able to display progress or other relevant statistics. Can compare running single threaded versus paralellized for comparison. 
  
# A description of your technical design. This should include:
   
How you will be using the topics covered in class in the project.

* Allocating memory is integral to every function in C and will be used for many different aspects.
* Files can be used to store constant information, such as the ip adresses of all computers that can be utilized.
* Forking will be used to create multiple concurrent processes on each system to utilize all cores, 6 per lab computer.
* Semaphores could be useful for tracking when the processes are done.
* Pipes will be used for communication between the forked processes to send and recieve data.
* Sockets will be used for communication between the computers to send and recieve data.
     
How you are breaking down the project and who is responsible for which parts.

Working solo, going to start with an easy to implement algorithm, write it regularly, then write it to use multithreading, then incorporate distributed computing. Then move on to potentially more advanced algorithms.
  
What data structures you will be using and how.

* Structs will be useful for sending data.
* Arrays are fundamental aspects of the algorithms.     
     
What algorithms and /or data structures you will be using, and how.

* Merge Sort is the most approachable and simple.
* Using Fermat's algorithms to guess primes, very large numbers potentially use GMP library.
* Password cracking, could use brute force or implement common word libraries.
* Other sorts, bogo sort would be fun. 
* Further ideas [here](https://www.cs.cmu.edu/~scandal/nesl/algorithms.html)
        
# A timeline with expected completion dates of parts of the project.
- [ ] 1/9 Single Threaded Merge Sort
- [ ] 1/9 Networking Infrastructure
- [ ] 1/11 Functional distributed merge sort
- [ ] 1/12 Better interface
- [ ] 1/13 Well defined product
- [ ] 1/17 Additional ideas to be added
