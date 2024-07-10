#include "printing.hpp"
#include "../h/main.hpp"

#if USE_TEST == 1

#define LEVEL_1_IMPLEMENTED 0
#define LEVEL_2_IMPLEMENTED 1
#define LEVEL_3_IMPLEMENTED 1
#define LEVEL_4_IMPLEMENTED 1
#define MODIFICATION_20_SENDRECEIVE_IMPLEMENTED 1
#define MODIFICATION_30_TIMEDJOIN_IMPLEMENTED   1
#define ADDITIONAL_MOD_JOINALL_IMPLEMENTED      1
#define ADDITIONAL_MOD_JOIN_IMPLEMENTED         1
#define ADDITIONAL_MOD_TIMEDJOINALL_IMPLEMENTED 1

#if LEVEL_2_IMPLEMENTED == 1
// TEST 1 (zadatak 2, niti C API i sinhrona promena konteksta)
#include "../test/Threads_C_API_test.hpp"
// TEST 2 (zadatak 2., niti CPP API i sinhrona promena konteksta)
#include "../test/Threads_CPP_API_test.hpp"
// TEST 7 (zadatak 2., testiranje da li se korisnicki kod izvrsava u korisnickom rezimu)
#include "../test/System_Mode_test.hpp"
#endif

#if LEVEL_3_IMPLEMENTED == 1
// TEST 3 (zadatak 3., kompletan C API sa semaforima, sinhrona promena konteksta)
#include "../test/ConsumerProducer_C_API_test.hpp"
// TEST 4 (zadatak 3., kompletan CPP API sa semaforima, sinhrona promena konteksta)
#include "../test/ConsumerProducer_CPP_Sync_API_test.hpp"
#endif

#if LEVEL_4_IMPLEMENTED == 1
// TEST 5 (zadatak 4., thread_sleep test C API)
#include "../test/ThreadSleep_C_API_test.hpp"
// TEST 6 (zadatak 4. CPP API i asinhrona promena konteksta)
#include "../test/ConsumerProducer_CPP_API_test.hpp"
#include "System_Mode_test.hpp"
#endif

#if MODIFICATION_20_SENDRECEIVE_IMPLEMENTED == 1
// TEST 8 (zadatak 8. (modifikacija 20): send receive)
#include "../test/SendReceive_CPP_API_test.hpp"
#endif

#if MODIFICATION_30_TIMEDJOIN_IMPLEMENTED == 1
// TEST 9 (zadatak 9. (modifikacija 30): timedJoin)
#include "../test/TimedJoin_CPP_API_test.hpp"
#endif

#if ADDITIONAL_MOD_JOINALL_IMPLEMENTED == 1
// TEST 10 (dodatna modifikacija (jun 2024): joinAll)
#include "../test/JoinAll_CPP_API_test.hpp"
#endif

#if ADDITIONAL_MOD_JOIN_IMPLEMENTED == 1
// TEST 11 (dodatna modifikacija: join)
#include "../test/Join_CPP_API_test.hpp"
#endif

#if ADDITIONAL_MOD_TIMEDJOINALL_IMPLEMENTED == 1
// TEST 12 (dodatna modifikacija: timedJoinAll)
#include "../test/TimedJoinAll_CPP_API_test.hpp"
#endif

void userMain() {
    char input[30];
    printString("Unesite broj testa? [1-10]\n");
    getString(input, 30);
    int test = stringToInt(input);

    if ((test >= 1 && test <= 2) || test == 7) {
        if (LEVEL_2_IMPLEMENTED == 0) {
            printString("Nije navedeno da je zadatak 2 implementiran\n");
            return;
        }
    }

    if (test >= 3 && test <= 4) {
        if (LEVEL_3_IMPLEMENTED == 0) {
            printString("Nije navedeno da je zadatak 3 implementiran\n");
            return;
        }
    }

    if (test >= 5 && test <= 6) {
        if (LEVEL_4_IMPLEMENTED == 0) {
            printString("Nije navedeno da je zadatak 4 implementiran\n");
            return;
        }
    }

    if (test == 8) {
        if (MODIFICATION_20_SENDRECEIVE_IMPLEMENTED == 0) {
            printString("Nije navedeno da je modifikacija 20 (send & receive) implementirana\n");
            return;
        }
    }

    if (test == 9) {
        if (MODIFICATION_30_TIMEDJOIN_IMPLEMENTED == 0) {
            printString("Nije navedeno da je modifikacija 30 (timedJoin) implementirana\n");
            return;
        }
    }

    if (test == 10) {
        if (ADDITIONAL_MOD_JOINALL_IMPLEMENTED == 0) {
            printString("Nije navedeno da je modifikacija joinAll implementirana\n");
            return;
        }
    }

    if (test == 11) {
        if (ADDITIONAL_MOD_JOIN_IMPLEMENTED == 0) {
            printString("Nije navedeno da je modifikacija join implementirana\n");
            return;
        }
    }

    if (test == 12) {
        if (ADDITIONAL_MOD_TIMEDJOINALL_IMPLEMENTED == 0) {
            printString("Nije navedeno da je modifikacija timedJoinAll implementirana\n");
            return;
        }
    }

    switch (test) {
        case 1:
#if LEVEL_2_IMPLEMENTED == 1
            Threads_C_API_test();
            printString("TEST 1 (zadatak 2, niti C API i sinhrona promena konteksta)\n");
#endif
            break;
        case 2:
#if LEVEL_2_IMPLEMENTED == 1
            Threads_CPP_API_test();
            printString("TEST 2 (zadatak 2., niti CPP API i sinhrona promena konteksta)\n");
#endif
            break;
        case 3:
#if LEVEL_3_IMPLEMENTED == 1
            producerConsumer_C_API();
            printString("TEST 3 (zadatak 3., kompletan C API sa semaforima, sinhrona promena konteksta)\n");
#endif
            break;
        case 4:
#if LEVEL_3_IMPLEMENTED == 1
            producerConsumer_CPP_Sync_API();
            printString("TEST 4 (zadatak 3., kompletan CPP API sa semaforima, sinhrona promena konteksta)\n");
#endif
            break;
        case 5:
#if LEVEL_4_IMPLEMENTED == 1
            testSleeping();
            printString("TEST 5 (zadatak 4., thread_sleep test C API)\n");
#endif
            break;
        case 6:
#if LEVEL_4_IMPLEMENTED == 1
            testConsumerProducer();
            printString("TEST 6 (zadatak 4. CPP API i asinhrona promena konteksta)\n");
#endif
            break;
        case 7:
#if LEVEL_2_IMPLEMENTED == 1
            System_Mode_test();
            printString("Test se nije uspesno zavrsio\n");
            printString("TEST 7 (zadatak 2., testiranje da li se korisnicki kod izvrsava u korisnickom rezimu)\n");
#endif
            break;
        case 8:
#if MODIFICATION_20_SENDRECEIVE_IMPLEMENTED == 1
            testSendReceive();
            printString("TEST 8 (modifikacija 20: send receive)\n");
#endif
            break;
        case 9:
#if MODIFICATION_30_TIMEDJOIN_IMPLEMENTED == 1
            testTimedJoin();
            printString("TEST 9 (modifikacija 30: timedJoin)\n");
#endif
            break;
        case 10:
#if ADDITIONAL_MOD_JOINALL_IMPLEMENTED == 1
            testJoinAll();
            printString("TEST 10 (dodatna modifikacija (jun 2024): joinAll)\n");
#endif
            break;
        case 11:
#if ADDITIONAL_MOD_JOIN_IMPLEMENTED == 1
            testJoin();
            printString("TEST 11 (dodatna modifikacija: join)\n");
#endif
            break;
        case 12:
#if ADDITIONAL_MOD_TIMEDJOINALL_IMPLEMENTED == 1
            testTimedJoinAll();
            printString("TEST 12 (dodatna modifikacija: timedJoinAll)\n");
#endif
            break;
        default:
            printString("Niste uneli odgovarajuci broj za test\n");
    }
}

#endif