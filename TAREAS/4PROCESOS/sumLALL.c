#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <time.h>

#define TERMS (4e9)  // 4 billones de términos para la serie
#define NUM_PROCESSES 4  // Número de procesos a utilizar

// Estructura para definir rangos de trabajo
typedef struct {
    long long start;  // Índice inicial
    long long end;    // Índice final (no inclusive)
} Range;

// Función que calcula una parte de la serie de Leibniz
double partial_leibniz(Range range) {
    double sum = 0.0;
    for (long long i = range.start; i < range.end; i++) {
        if (i % 2 == 0) {
            sum += 1.0 / (2 * i + 1);  // Términos pares: suma
        } else {
            sum -= 1.0 / (2 * i + 1);  // Términos impares: resta
        }
    }
    return sum;
}

// Versión secuencial del cálculo de π
double leibniz_sequential() {
    double sum = 0.0;
    for (long long i = 0; i < TERMS; i++) {
        if (i % 2 == 0) {
            sum += 1.0 / (2 * i + 1);
        } else {
            sum -= 1.0 / (2 * i + 1);
        }
    }
    return 4 * sum;  // Multiplica por 4 para obtener π
}

// Versión paralela usando pipes (tuberías)
void leibniz_pipes() {
    int fd[NUM_PROCESSES][2];  // Descriptores de archivo para los pipes
    pid_t pids[NUM_PROCESSES]; // IDs de los procesos hijos
    Range ranges[NUM_PROCESSES]; // Rangos para cada proceso
    
    // Divide el trabajo entre los procesos
    long long terms_per_process = TERMS / NUM_PROCESSES;
    
    // Configura los pipes y los rangos
    for (int i = 0; i < NUM_PROCESSES; i++) {
        if (pipe(fd[i]) == -1) {  // Crea un pipe
            perror("pipe");
            exit(EXIT_FAILURE);
        }
        
        // Asigna el rango de trabajo para este proceso
        ranges[i].start = i * terms_per_process;
        ranges[i].end = (i == NUM_PROCESSES - 1) ? TERMS : (i + 1) * terms_per_process;
    }
    
    // Crea los procesos hijos
    for (int i = 0; i < NUM_PROCESSES; i++) {
        pids[i] = fork();  // Divide el proceso
        
        if (pids[i] < 0) {
            perror("Error al crear un proceso hijo (fork)");
            exit(EXIT_FAILURE);
        }
        
        if (pids[i] == 0) {  // Código del proceso hijo
            close(fd[i][0]);  // Cierra el extremo de lectura del pipe
            
            // Calcula su parte de la serie
            double partial_sum = partial_leibniz(ranges[i]);
            // Envía el resultado al padre
            write(fd[i][1], &partial_sum, sizeof(partial_sum));
            close(fd[i][1]);
            
            exit(EXIT_SUCCESS);  // Termina el hijo
        } else {  // Código del proceso padre
            close(fd[i][1]);  // Cierra el extremo de escritura
        }
    }
    
    // Proceso padre recolecta los resultados
    double total = 0.0;
    for (int i = 0; i < NUM_PROCESSES; i++) {
        double partial_sum;
        // Lee el resultado de cada hijo
        read(fd[i][0], &partial_sum, sizeof(partial_sum));
        total += partial_sum;
        close(fd[i][0]);
        
        // Espera a que el hijo termine
        waitpid(pids[i], NULL, 0);
    }
    
    printf("Pi con pipes o tuberias: %.15f\n", 4 * total);
}

// Versión paralela usando memoria compartida
void leibniz_shared_memory() {
    int shmid;  // ID del segmento de memoria compartida
    double *shared_sum;  // Puntero a la memoria compartida
    
    // Crea un segmento de memoria compartida
    shmid = shmget(IPC_PRIVATE, NUM_PROCESSES * sizeof(double), IPC_CREAT | 0666);
    if (shmid < 0) {
        perror("shmget no puedo crear un segmento de memoria ");
        exit(EXIT_FAILURE); 
    }
    
    // Adjunta el segmento al espacio de direcciones del proceso
    shared_sum = (double *)shmat(shmid, NULL, 0);//casteo explicito a double *
    if (shared_sum == NULL) {
        perror("shmat fallo");
        exit(EXIT_FAILURE);
    }
    
    // Configura los rangos de trabajo
    Range ranges[NUM_PROCESSES];
    long long terms_per_process = TERMS / NUM_PROCESSES;
    
    for (int i = 0; i < NUM_PROCESSES; i++) {
        ranges[i].start = i * terms_per_process;
        ranges[i].end = (i == NUM_PROCESSES - 1) ? TERMS : (i + 1) * terms_per_process;
    }
    
    // Crea los procesos hijos
    for (int i = 0; i < NUM_PROCESSES; i++) {
        pid_t pid = fork();
        
        if (pid < 0) {
            perror("fork");
            exit(EXIT_FAILURE);
        }
        
        if (pid == 0) {  // Código del hijo
            // Calcula su parte y guarda en memoria compartida
            shared_sum[i] = partial_leibniz(ranges[i]); //*(shared_sum + i), suma en este caso 8 bytes cada iteracion
            shmdt(shared_sum);  // Desvincula la memoria compartida
            exit(EXIT_SUCCESS);  // Termina
        }
    }
    
    // Espera a que todos los hijos terminen
    for (int i = 0; i < NUM_PROCESSES; i++) {
        wait(NULL);
    }
    
    // Suma todos los resultados parciales
    double total = 0.0;
    for (int i = 0; i < NUM_PROCESSES; i++) {
        total += shared_sum[i];
    }
    
    printf("Pi con memoria compartida: %.15f\n", 4 * total);
    
    // Limpieza
    shmdt(shared_sum);  // Desvincula
    shmctl(shmid, IPC_RMID, NULL);  // Elimina el segmento
}

int main() {
    clock_t start, end;
    double time_spent;
    
    printf("Calculando pi con 4 billones de terminos...\n");
    printf("Ticks por segundo: %ld\n", CLOCKS_PER_SEC);
    printf("--------------------------------------------\n");
    // Versión secuencial
    start = clock();
    double pi_seq = leibniz_sequential();
    end = clock();
    time_spent = (double)(end - start) / CLOCKS_PER_SEC;
    printf("Secuencial: Pi = %.15f, Tiempo = %.3f segundos\n", pi_seq, time_spent);
    
    printf("--------------------------------------------\n");
    // Versión con pipes
    start = clock();
    leibniz_pipes();
    end = clock();
    time_spent = (double)(end - start) / CLOCKS_PER_SEC;
    printf("Tiempo = %.3f segundos\n", time_spent);
    
    printf("--------------------------------------------\n");
    // Versión con memoria compartida
    start = clock();
    leibniz_shared_memory();
    end = clock();
    time_spent = (double)(end - start) / CLOCKS_PER_SEC;
    printf("Tiempo = %.3f segundos\n", time_spent);
    
    return 0;
}