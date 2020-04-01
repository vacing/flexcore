#include <flexcore/pure/pure_ports.hpp>
#include <iostream>
#include <cassert>
#include <string>
using namespace fc;

std::string var;

int main()
{
    pure::state_source<std::string> src{[] { return var; }};
    pure::state_sink<std::string> sink;
    src >> sink;
    var = "Hello world!";
    assert(sink.get() == var);
    std::cout << sink.get() << std::endl;
}
