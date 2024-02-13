/**
 * @file Millis.cpp
 * 
 * @author
 * Angelo Elias Dalzotto (150633@upf.br)
 * GEPID - Grupo de Pesquisa em Cultura Digital (http://gepid.upf.br/)
 * Universidade de Passo Fundo (http://www.upf.br/)
 * 
 * @copyright
 * Copyright (c) 2018 Angelo Elias Dalzotto & Gabriel Boni Vicari
 * 
 * @brief The Millis class manages the milliseconds timer.
 */

#include <lib/Millis.h>

bool Millis::initialized = false;
uint32_t Millis::counter = 0;

ISR(TIMER2_COMP_vect)
{
    Millis::counter++;
}

void Millis::init()
{
    if(initialized)
        return;
    initialized = true;

    TCCR2 |= (1 << WGM21) | (1 << CS22); // Timer 2 CTC
        
    TCNT2 = 0;
    OCR2 = 115; // 1KHz or 1ms
    TIMSK |= (1 << OCIE2); // Interrupt to OCR2A
}

uint32_t Millis::get()
{
    uint32_t temp;
    ATOMIC_BLOCK(ATOMIC_FORCEON){
        temp = counter;
    }
    return temp;
}