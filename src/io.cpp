#include "../h/io.hpp"


CharBuffer* IO::inputBuffer = nullptr;
CharBuffer* IO::outputBuffer = nullptr;
//kSemaphore* IO::inputSem = nullptr;
//kSemaphore* IO::outputSem = nullptr;
bool IO::inputPass = 0;
bool IO::outputPass = 0;
TCB* IO::inputTCB = nullptr;
TCB* IO::outputTCB = nullptr;

void IO::init()
{
    inputBuffer = new CharBuffer();
    outputBuffer = new CharBuffer();
//    inputSem = kSemaphore::kSemaphoreCreate(0);
//    outputSem = kSemaphore::kSemaphoreCreate(0);
    inputTCB = TCB::threadCreate(&inputRoutine, nullptr, TCB::Mode::SYSTEM, nullptr);
    outputTCB = TCB::threadCreate(&outputRoutine, nullptr, TCB::Mode::SYSTEM, nullptr);
}


void IO::inputRoutine(void* p)
{
    while (1)
    {
        while (!inputPass) thread_dispatch();

        while ((inputPass = (*((char*)CONSOLE_STATUS) & CONSOLE_RX_STATUS_BIT)))
        {
            char c = *((char*)CONSOLE_RX_DATA);
            inputBuffer->uPut(c);
        }
    }

    (void)p;
}

void IO::outputRoutine(void* p)
{
    while (1)
    {
        while (!outputPass) thread_dispatch();
        
        while ((outputPass = (*((char*)CONSOLE_STATUS) & CONSOLE_TX_STATUS_BIT)))
        {
            char c = outputBuffer->uGet();
            *((char*)CONSOLE_TX_DATA) = c;
        }
    }

    (void)p;
}