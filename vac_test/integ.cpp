#include <fstream>
#include <iostream>
#include <chrono>
#include <iomanip>
#include <ctime>
#include <thread>

#include <flexcore/extended/base_node.hpp>
#include <flexcore/ports.hpp>
#include <flexcore/infrastructure.hpp>

#include <boost/scope_exit.hpp>

using namespace fc;

struct MyNode : tree_base_node
{
    explicit MyNode(const std::string &name, const node_args& node) : tree_base_node(node) 
    {}
    std::string name_;
};

int main()
{

    std::cout << "Starting Dummy Solution\n";
    std::cout << "build up infrastructure \n";
    fc::infrastructure infrastructure;

    auto first_region = infrastructure.add_region(
            "region 1st",
            fc::thread::cycle_control::medium_tick);

    std::cout << "start building connections\n";

    using clock = fc::virtual_clock::system;
    using time_point = clock::time_point;

    fc::pure::event_source<time_point> source;
    first_region->ticks.work_tick()
            >> [&source](){ source.fire(clock::now()); };

    source >> [](time_point t){ return clock::to_time_t(t); }
           >> [](time_t t) { std::cout << std::localtime(&t)->tm_sec << " --abc \n"; };

    first_region->ticks.work_tick()
            >> [count = 0]() mutable {return count++;}
            >> [](int i) { std::cout << "counted ticks: " << i << "\n"; };


    //create a connection with region transition
    auto second_region = infrastructure.add_region(
            "region 2nd",
            fc::thread::cycle_control::slow_tick);

    second_region->ticks.work_tick() >> [](){ std::cout << "Zonk!\n"; };

    auto& child_a = infrastructure.node_owner().
            make_child_named<MyNode>(first_region, "source_a", "abc"); // MyNode 构造参数放最后
    auto& child_b = infrastructure.node_owner().
            make_child_named<MyNode>(second_region, "sink_b", "def");
    auto& child_c = infrastructure.node_owner().
            make_child_named<MyNode>(second_region, "source_c", "gh");
    auto& child_d = infrastructure.node_owner().
            make_child_named<MyNode>(second_region, "sink_d", "ddd");

    event_source<std::string> string_source_11(&child_a);
    event_source<std::string> string_source_12(&child_a);

    event_source<int> string_source_21(&child_b);
    event_sink<std::string> string_sink(&child_b,
            [second_region](std::string in){std::cout << second_region->get_id().key << " received: " << in << "\n";});

    string_source_11 >> string_sink;
    string_source_12 >> string_sink;
    first_region->ticks.work_tick()
            >>  [&string_source_11, id = first_region->get_id().key]() mutable
                {
                    string_source_11.fire("a magic string from " + id);
                };
    first_region->ticks.work_tick()
            >>  [&string_source_21, id = first_region->get_id().key]() mutable
                {
                    string_source_21.fire(100);
                };

    event_source<std::string> string_source_3(&child_c);
    string_source_3 >> string_sink;
    second_region->ticks.work_tick()
            >>  [&string_source_3, id = second_region->get_id().key]() mutable
                {
                    string_source_3.fire("a magic string from " + id);
                };

    event_sink<int> string_sink_4(&child_d, [](int i) { std::cout << i << std::endl;});
    string_source_21 >> string_sink_4;

    {
        // raw graph
        std::ofstream out1{"./out_graph.dot"};
        infrastructure.get_graph().print(out1);
        // graph with color(same color of the same region)
        std::ofstream out{"./out.dot"};
        infrastructure.visualize(out);
    }

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

