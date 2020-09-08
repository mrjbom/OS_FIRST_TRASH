#include "sync.h"
#include "scheduler.h"

/*
//semaphore
semaphore_t* semaphore_create(uint32_t max_value) {
    semaphore_t* semaphore = pm_malloc(sizeof(semaphore_t));
    if(!semaphore) {
        serial_printf("ERROR! semaphore_create failed!\n");
        return NULL;
    }
    semaphore->max_value = max_value;
    semaphore->current_value = max_value;
    return semaphore;
}

void semaphore_lock(semaphore_t* semaphore) {
    //если ещё есть место
    if(semaphore->current_value > 0) {
        //занимаем семафор
        //serial_write_str("lock success\n");
        --semaphore->current_value;
    }
    //если места нет
    else {
        //ждём
        //while(semaphore->current_value == 0) { }
        //переключаем задачу
        //serial_write_str("lock failed, switch\n");
        scheduler_switch();
    }
}

void semaphore_unlock(semaphore_t* semaphore) {
    //если можем разблокировать семафор
    if((semaphore->current_value + 1) <= semaphore->max_value) {
        //serial_write_str("unlock success\n");
        ++semaphore->current_value;
        __asm__ volatile ("sti");
        return;
    }
    //serial_write_str("unlock failed\n");
}

void semaphore_delete(semaphore_t* semaphore) {
    pm_free(semaphore);
}
*/
