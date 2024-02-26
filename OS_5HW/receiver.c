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

pid_t sender_pid;
int received_bits[32];
int bit_index = 0;

// Обработчик сигналов SIGUSR1 и SIGUSR2
void receive_bit(int signum) {
    if (signum == SIGUSR1) {
        received_bits[bit_index] = 0;  // Устанавливаем 0, если получен сигнал SIGUSR1
    } else if (signum == SIGUSR2) {
        received_bits[bit_index] = 1;  // Устанавливаем 1, если получен сигнал SIGUSR2
    }

    ++bit_index;  // Увеличиваем индекс текущего бита

    if (bit_index == 32) {  // Проверяем, получены ли все 32 бита
        int number = 0;
        for (int i = 1; i < 32; i++) {
            number = (number << 1) | received_bits[i];  // Преобразуем последовательность битов в число
        }
        if (received_bits[0] == 1) {
            number *= -1;  // Если первый бит 1, делаем число отрицательным
        }
        printf("Get number: %d\n", number);  // Выводим полученное число
    }

    kill(sender_pid, SIGUSR1);  // Отправляем сигнал-подтверждение передатчику
}

// Основная функция программы
int main() {
    // Получаем PID текущего процесса (получателя)
    pid_t my_pid = getpid();
    printf("Receiver's PID: %d\n", my_pid);  // Выводим PID получателя

    // Запрашиваем у пользователя PID передатчика
    printf("Enter transmitter's PID: ");
    scanf("%d", &sender_pid);

    // Устанавливаем обработчики сигналов SIGUSR1 и SIGUSR2
    signal(SIGUSR1, receive_bit);
    signal(SIGUSR2, receive_bit);

    while (1) {
        pause();  // Ждем сигнала от передатчика
    }

    return 0;
}
