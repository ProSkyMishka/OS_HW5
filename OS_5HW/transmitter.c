//
//  main.c
//  transmitter
//
//  Created by Михаил Прозорский on 26.02.2024.
//

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

pid_t receiver_pid;
int bit_to_send = 0;
int total_bits_sent = 0;
int max_bits_to_send = 32; // Максимальное количество битов для передачи
int number;
char bits[32];
int i = 31;

// Функция обработчика сигнала
void bit_sent_handler(int sig) {
    if (sig == SIGUSR1) {
        // Если был получен SIGUSR1
        ++total_bits_sent;
        if (total_bits_sent >= max_bits_to_send) {
            // Достигнуто максимальное количество переданных битов, завершаем передачу
            printf("Number sent successfully\n");
            exit(0);
        } else {
            // Продолжаем передачу следующего бита
            if (i != 32) {
                bit_to_send = bits[i++];
            } else {
                bit_to_send = 0;
            }
            kill(receiver_pid, bit_to_send == 0 ? SIGUSR1 : SIGUSR2);
        }
    }
}

int main() {
    // Вывод PID текущего процесса (трансмиттера)
    printf("Transmitter PID: %d\n", getpid());
    
    // Запрос PID процесса-получателя
    printf("Enter receiver's PID: ");
    scanf("%d", &receiver_pid);
    
    // Установка обработчиков сигналов SIGUSR1 и SIGUSR2
    signal(SIGUSR1, bit_sent_handler);
    signal(SIGUSR2, bit_sent_handler);
    
    // Ввод числа и его перевод в бинарный вид
    printf("Enter the number: ");
    scanf("%d", &number);
    
    // Инициализация массива битов
    for (int j = 0; j < 32; ++j) {
        bits[j] = 0;
    }
    
    // Перекодирование отрицательного числа
    if (number < 0) {
        number *= -1;
        bits[0] = 1;
    }
    
    // Перевод числа в бинарный вид
    while (number != 0) {
        bits[i--] = number % 2;
        number /= 2;
    }
    
    i = 0;
    bit_to_send = bits[i++];
    
    // Отправка первого бита получателю
    kill(receiver_pid, bit_to_send == 0 ? SIGUSR1 : SIGUSR2);
    
    // Бесконечный цикл для ожидания сигналов
    while (1) {
        pause();
    }
    
    return 0;
}
