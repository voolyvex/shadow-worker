#include "example.h"
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

namespace shadow_worker {

void Example::_bind_methods() {
    ClassDB::bind_method(D_METHOD("hello_world"), &Example::hello_world);
}

Example::Example() {
    // Constructor
}

Example::~Example() {
    // Destructor
}

void Example::hello_world() {
    // Add your custom logic here
}
} 