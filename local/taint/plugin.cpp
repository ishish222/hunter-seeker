#include <plugin.h>

int Plugin::set_taint_eng(taint_x86* taint_eng)
{
    this->taint_eng = taint_eng;
    return 0x0;
}
