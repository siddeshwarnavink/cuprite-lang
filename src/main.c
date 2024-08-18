#include <stdbool.h>

#include "ui/repl.h"
#include "utils/memstk.h"

int main() {
    memstk_init();

    repl_run();

    memstk_clean();

    return 0;
}
