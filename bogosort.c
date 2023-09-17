#include "bogosort.h"
#define READ 0
#define WRITE 1
#define KEY 200001
#define PORT "15497"

int main(int argc, char * argv[]) {
  int iterations;
  int n;
  double average = 0;
  if(argc != 0 && !(strcasecmp(argv[1], "client") == 0 || strcasecmp(argv[1], "server") == 0)) 
  {
    iterations = 10;
    printf("How many iterations should be used for the average?\n");
    scanf("%d", &iterations);

    n = 11;
    printf("How long should each array be?\n");
    scanf("%d", &n);

  }
  if ( argc != 0 && strcasecmp(argv[1], "single") == 0) {
    int * array = calloc(n, sizeof(int));
    clock_t start, end;
    for (int i = 0; i < iterations; i++) {
      printf("Iteration %d\n", i);
      start = 0;
      end = 0;
      array = calloc(n, sizeof(int));
      for (int i = 0; i < n; i++) {
        array[i] = rand() % 50;
      }
      start = clock();
      bogosort(array, n);
      end = clock();
      average += ((double)(end - start)) / (CLOCKS_PER_SEC);
      free(array);
    }
    average /= iterations;
    printf("Running a single threaded random sort on %d iterations of length %d took an average of %f seconds\n", iterations, n, average);
    exit(0);
  }
  if ( argc != 0 && strcasecmp(argv[1], "local") == 0) {
    average = multithreaded_1PC(iterations, n);
    printf("Running a multi threaded random sort with 12 subprocesses on %d iterations of length %d took an average of %f seconds\n", iterations, n, average);
    exit(0);
  }
  if (argc != 0 && strcasecmp(argv[1], "server") == 0) {
    server();
    exit(0);
  }
  if (argc != 0 && strcasecmp(argv[1], "client") == 0) {
    client();
    exit(0);
  }

}

void server() {
  struct addrinfo * hints, * results;
  hints = calloc(1, sizeof(struct addrinfo));


  hints->ai_family = AF_INET;
  hints->ai_socktype = SOCK_STREAM;
  hints->ai_flags = AI_PASSIVE;
  getaddrinfo(NULL, PORT, hints, &results);

  int sockid = socket(results->ai_family, results->ai_socktype, results->ai_protocol);

  int yes = 1;
  if ( setsockopt(sockid, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1 ) {
      printf("sockopt  error\n");
      printf("%s\n",strerror(errno));
      exit(-1);
  }

  int err = bind(sockid, results->ai_addr, results->ai_addrlen);
  if(err == -1){
        printf("Error binding: %s",strerror(errno));
        exit(1);
    }

  listen(sockid, 1);
  printf("Listening on port %s\n",PORT);

  socklen_t sock_size;
  struct sockaddr * client_address;
  sock_size = sizeof(*client_address);
  int client_socket = accept(sockid, client_address, &sock_size);
  printf("\nConnected!\n");
  int success = 1;
  write(client_socket, &success, sizeof(int));

  int len;
  read(client_socket, &len, sizeof(int));
  int halt = 1;
  int iteration = 1;
  while(halt == 1) {
    printf("\nIteration: %d\n", iteration);
    iteration++;
    read(client_socket, &halt, sizeof(int));
    int * array = calloc(len, sizeof(int));
    read(client_socket, array, sizeof(int) * len);
    printf("[");
    for (int i = 0; i < len; i++) {
      printf("%d, ",array[i]);
    }
    printf("]\n");

    int * out;
    out = multi_bogo_server(len, array, client_socket);
    if (out == -1) {
      printf("Solved by another computer\n");
      continue;
    }
    printf("[");
    for (int i = 0; i < len; i++) {
      printf("%d, ",out[i]);
    }
    printf("]\n");
    success = 1;
    write(client_socket, &success, sizeof(int));
  }
  return;
}

void client() {
  srand(time(NULL) + getpid());
  char * ip_base = ".cs.swarthmore.edu";
  char * ip;
  char * cpu_num = calloc(3, sizeof(char));
  int connections = 0;
  size_t len = 0;
  struct addrinfo * hints, * results;
  hints = calloc(1, sizeof(struct addrinfo));
  hints->ai_family = AF_INET;
  hints->ai_socktype = SOCK_STREAM;
  int err;
  int * sd = calloc(35, sizeof(int));
  for (int i = 0; i < 35; i++) {
    sd[i] = -1;
  }
  int maxsd = 0;
  int i = 0;
  char ** machines = calloc(2, sizeof(char *));
  machines[0] = "pimento";
  machines[1] = "owl";
  while (i < 2) {
    ip = calloc(25, sizeof(char));
    strcpy(ip, machines[i]);
    strcat(ip, ip_base);
    printf("ip: %s when i = %d", ip, i);
    // if(i < 10 ) {
    //   sprintf(cpu_num, "0%d", i);
    // } else {
    //   sprintf(cpu_num, "%d", i);
    // }

    printf("\nAttempting to connect to %s on port: %s\n", ip, PORT);

    if ((err = getaddrinfo(ip, PORT, hints, &results)) != 0) {
        printf("Unable to getaddrinfo for %s, error %d : %s\n", ip, err, strerror(err));
      sd[i] = -1;
        i++;
        continue;
    }
    sd[i] = socket(results->ai_family, results->ai_socktype, results->ai_protocol);
    if(sd[i] < 0){
        printf("Unable to open socket for %s, %s\n", ip, strerror(errno));
        sd[i] = -1;
        i++;
        continue;
    }
    fcntl(sd[i], F_SETFL, O_NONBLOCK);
    struct pollfd connect_poll[1];
    connect_poll[0].fd = sd[i];
    connect_poll[0].events = POLLIN;
    int connecterr = connect(sd[i], results->ai_addr, results->ai_addrlen);
    
    int pollerr = poll(connect_poll, 1, 2 * 1000);
    if (pollerr == 0) {
      printf("Connection attempt to %s timed out\n", ip);
      sd[i] = -1;
      i++;
      continue;
    }
    if (pollerr == -1) {
      printf("Poll error %s\n", strerror(errno));
      sd[i] = -1;
      i++;
      continue;
    }
    if (connect_poll[0].revents != 1) {
        printf("Failed to connect to %s\n", ip);
        sd[i] = -1;
        i++;
        continue;
    }
    int success = 0;
    read(sd[i], &success, sizeof(int));
    printf("Connected successfully to %s\n", ip);

    maxsd = sd[i];
    connections++;
    i++;
    free(ip);
  }

  printf("\nEstablished a connection with %d computers\n", connections);
  if (connections == 0) {
    printf("No successful connections were established\n");
    return;
  }
  int iterations = 10;
  printf("\nHow many iterations should be used to test the sort?\n");
  scanf("%d", &iterations);

  int length = 11;
  printf("\nHow long should each array be?\n");
  scanf("%d", &length);
  for (int i = 1; i <= 34; i++) {
    if (sd[i] != -1) {
      write(sd[i], &length, sizeof(int));
    }
  }

  printf("\nStarting the sorting\n\n");
  struct pollfd * servers = calloc(connections, sizeof(struct pollfd));
  int connection_count = 0;
  for (int i = 1; i <= 34; i++) {
    if (sd[i] != -1) {
      // printf("Connection count %d\n",connection_count);
      servers[connection_count].fd = sd[i];
      servers[connection_count].events = POLLIN;
      connection_count++;
    }
  }
  int * array;
  struct timeval stop, start;
  double time_spent;
  int halt = 1;
  for (int i = 0; i < iterations; i++) {
    printf("Iteration %d - ", i);
    if (i >= iterations - 1) {
      halt = -1;
    }
    for (int ii = 0; ii < connections; ii++) {
      // printf("Writing to %d\n", servers[ii].fd);
      write(servers[ii].fd, &halt, sizeof(int));
    }

    array = calloc(length, sizeof(int));
    for (int ii = 0; ii < length; ii++) {
      array[ii] = rand() % 50;
    }
    // printf("Writing: ");
    // printf("[");
    // for (int ii = 0; ii < length; ii++) {
    //   printf("%d, ",array[ii]);
    // }
    // printf("]\n");
    for (int ii = 0; ii < connections; ii++) {
      write(servers[ii].fd, array, sizeof(int) * length);
    }
    // free(array);

    gettimeofday(&start, NULL);

    int pollerr = poll(servers, connections, -1);
    gettimeofday(&stop, NULL);
    if (pollerr <= 0) {
      printf("Poll errored with value %d meaning %s", pollerr, strerror(errno));
    }
    int success = -1;
    for (int ii = 0; ii < connections; ii++) {
      if (servers[ii].revents == 1) {
        read(servers[ii].fd, &success, sizeof(int));
        printf("by computer %d\n", ii);
        for (int iii = 0; iii < connections; iii++) {
          if (iii != ii) {
            int done = 1;
            write(servers[iii].fd, &done, sizeof(int));
          }
        }
        break;
      }
    }
    time_spent += ((double) (stop.tv_sec - start.tv_sec)) + (((double) (stop.tv_usec - start.tv_usec)) / 1000000);
  }

  time_spent = time_spent / iterations;

  printf("\nUsing %d computers with 12 subprocesses per computer it took %f seconds on average to randomly sort a %d length array using %d iterations \n", connections, time_spent, length, iterations);

  return;
}

int * multi_bogo_server(int n, int * array, int client_socket) {
  int semd = semget(KEY, 1, IPC_EXCL | 0644);
  int semval = semctl(semd, 0, GETVAL);
  union semun us;
  us.val = 1;
  semctl(semd, 0, SETVAL, us);
  struct sembuf * semaphore = calloc(sizeof(struct sembuf), 1);
  semaphore->sem_op = -1;
  semaphore->sem_num = 0;
  semaphore->sem_flg = SEM_UNDO;

  int processCount = 12;

  int * return_pipe = calloc(1, sizeof(int));
  pipe(return_pipe);

  int counter = 0;
  int * pid = calloc(processCount + 1, sizeof(int));
  for (int i = 0; i < processCount; i++) {
    pid[i] = -1;
  }

  while (counter < processCount) {
    if (pid[counter] != 0) {
      counter++;
      pid[counter] = fork();
    } else {
      break;
    }
  }

  int * out = calloc(n, sizeof(int));
  if (pid[counter] == 0) {
    srand(time(NULL) + getpid());
    close(return_pipe[READ]);
    out = bogosort(array, n);
    semop(semd, semaphore, 1);
    write(return_pipe[WRITE], out, sizeof(int) * n);
    exit(0);
  } else {
    int done = -1;
    struct pollfd * sort_response = calloc(2, sizeof(struct pollfd));
    sort_response[0].fd = client_socket;
    sort_response[0].events = POLLIN;
    sort_response[1].fd = return_pipe[READ];
    sort_response[1].events = POLLIN;
    int poll_value = poll(sort_response, 2, -1);
    if (sort_response[0].revents == 1) {
      read(sort_response[0].fd, &done, sizeof(int));
      if (done == 1) {
        for (int i = 1; i < processCount + 1; i++) {
          kill(pid[i], SIGKILL);
        }
        return -1;
      }
    } else if (sort_response[1].revents == 1) {
      read(return_pipe[READ], out, sizeof(int) * n);
      for (int i = 1; i < processCount + 1; i++) {
        kill(pid[i], SIGKILL);
      }
      return out;
    }
    printf("Error with poll, value %d, %s\n", poll_value, strerror(errno));
    return -2;
  }
}

double multi_bogo(int n, int * array) {
  int semd = semget(KEY, 1, IPC_EXCL | 0644);
  int semval = semctl(semd, 0, GETVAL);
  union semun us;
  us.val = 1;
  semctl(semd, 0, SETVAL, us);
  struct sembuf * semaphore = calloc(sizeof(struct sembuf), 1);
  semaphore->sem_op = -1;
  semaphore->sem_num = 0;
  semaphore->sem_flg = SEM_UNDO;

  int processCount = 12;

  if (array == NULL) {
    array = calloc(n, sizeof(int));
    for (int i = 0; i < n; i++) {
      array[i] = rand() % 50;
    }
  }

  int * return_pipe = calloc(1, sizeof(int));
  pipe(return_pipe);

  // int ** pipes = calloc(processCount, sizeof(int *));
  int counter = 0;
  int * pid = calloc(processCount + 1, sizeof(int));
  for (int i = 0; i < processCount; i++) {
    pid[i] = -1;
  }

  while (counter < processCount) {
    if (pid[counter] != 0) {
      counter++;
      pid[counter] = fork();
      // pipes[counter] = calloc(2, sizeof(int));
      // pipe(pipes[counter]);
    } else {
      break;
    }
  }

  int * out = calloc(n, sizeof(int));
  if (pid[counter] == 0) {
    srand(time(NULL) + getpid());
    close(return_pipe[READ]);
    out = bogosort(array, n);
    semop(semd, semaphore, 1);
    write(return_pipe[WRITE], out, sizeof(int) * n);
    exit(0);
  } else {
    struct timeval stop, start;
    gettimeofday(&start, NULL);
    close(return_pipe[WRITE]);
    read(return_pipe[READ], out, sizeof(int) * n);
    gettimeofday(&stop, NULL);
    double time_spent = ((double) (stop.tv_sec - start.tv_sec)) + (((double) (stop.tv_usec - start.tv_usec)) / 1000000);

    for (int i = 1; i < processCount + 1; i++) {
      kill(pid[i], SIGKILL);
    }
    return time_spent;
  }
}

double multithreaded_1PC(int iterations, int n) {
  srand(time(NULL));
  double sum = 0;
  for (int i = 0; i < iterations; i++) {
    printf("Iteration %d\n", i);
    sum += multi_bogo(n, NULL);
  }
  return sum / iterations;
}


int * bogosort(int * array, int n) {

  int unsorted = 1;
  for (int i = 1; i < n; i++) {
    if (array[i] < array[i - 1]) {
      break;
    }
    if (i == n - 1) {
      exit(0);
    }
  }

  while (unsorted) {
    
    for (int i = 0; i < n; i++) {
      int rindex = rand() % (n - i);
      int temp = array[i];
      array[i] = array[rindex + i];
      array[rindex + i] = temp;
    }

    for (int i = 1; i < n; i++) {
      if (array[i] < array[i - 1]) {
          break;
      }
      if (i == n - 1) {
        unsorted = 0;
      }
    }
  }

  return array;
}
