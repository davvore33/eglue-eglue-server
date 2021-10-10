#include <iostream>
#include "library.h"

int main() {
    Loader l = Loader();
    short err = 0;
    err = l.load_resource("/home/matteo/Documents/eGlue/eglue-server/try.ini");
    if (err) exit(1);
    err = l.get_value("Section1.Value1");
    if (err) exit(err);
    err = l.set_value("Section1.Value1", "15");
    if (err) exit(err);
    err = l.get_value("Section1.Value1");
    if (err) exit(err);
    return 0;
}
