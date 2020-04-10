#include <flexcore/pure/pure_ports.hpp>
#include <iostream>
#include <cassert>
#include <string>
using namespace fc;

std::string var;

void state_test()
{
    pure::state_source<std::string> src{[] { return "src1"; }};
    pure::state_source<std::string> src2{[] { return "src2"; }};
    pure::state_sink<std::string> sink;
    src >> sink;
    std::cout << "state_test: " << sink.get() << std::endl;
}

void event_test()
{
    pure::event_source<int> src;
    pure::event_sink<int> sink{[](int v) { 
        std::cout << "event_test: "
                  << v << std::endl;
    }};
    src >> sink;
    src.fire(42);
}

int main()
{
    state_test();
    event_test();
}
