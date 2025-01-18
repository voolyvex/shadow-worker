#ifndef EXAMPLE_H
#define EXAMPLE_H

#include <godot_cpp/classes/node.hpp>

namespace shadow_worker {

class Example : public godot::Node {
    GDCLASS(Example, Node)

private:
    static void _bind_methods();

public:
    Example();
    ~Example();

    void hello_world();
};

}  // namespace shadow_worker

#endif // EXAMPLE_H 