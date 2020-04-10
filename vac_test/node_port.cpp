#include <functional>
#include <fstream>
#include <iostream>
#include <chrono>
#include <iomanip>
#include <ctime>
#include <thread>

#include <boost/scope_exit.hpp>

#include "flexcore/extended/base_node.hpp"
#include "flexcore/ports.hpp"
#include "flexcore/infrastructure.hpp"

using namespace fc;

struct Op: tree_base_node
{
    explicit Op(const node_args& node): 
        tree_base_node(node),
        ein_{this, [this](int o) {this->add(o);}},
        eout_{this}
    {}
    int add(int o)
    {
        eout_.fire(o + 1);
    }
    event_sink<int> ein_;
    event_source<int> eout_;
};

int main()
{
    fc::infrastructure infrastructure;
    auto first_region = infrastructure.add_region(
            "region 1st",
            fc::thread::cycle_control::slow_tick);

    auto& child_a = infrastructure.node_owner().make_child_named<Op>(first_region, "source_a");
    auto& child_b = infrastructure.node_owner().make_child_named<Op>(first_region, "source_b");

    fc::pure::event_source<int> source;
    first_region->ticks.work_tick()
            >>  [&source]() { source.fire(3); };
    source >> child_a.ein_;
    child_a.eout_ >> child_b.ein_;
    child_b.eout_ >> [](int v) { std::cout << "abc " << v << std::endl; };

    infrastructure.start_scheduler();
    BOOST_SCOPE_EXIT(&infrastructure) {
        infrastructure.stop_scheduler();
    } BOOST_SCOPE_EXIT_END

    int iterations = 3;
    while (iterations--)
    {
        infrastructure.iterate_main_loop();
    }

    return 0;
}

