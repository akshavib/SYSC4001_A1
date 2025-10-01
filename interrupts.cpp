/**
 *
 * @file interrupts.cpp
 * @author Sasisekhar Govind
 *
 * @author Akshavi Baskaran
 * 
 * @author Liam Addie
 */

#include <interrupts.hpp>

int main(int argc, char** argv) {

    //vectors is a C++ std::vector of strings that contain the address of the ISR
    //delays  is a C++ std::vector of ints that contain the delays of each device
    //the index of these elemens is the device number, starting from 0
    auto [vectors, delays] = parse_args(argc, argv);
    std::ifstream input_file(argv[1]);

    std::string trace;      //!< string to store single line of trace file
    std::string execution;  //!< string to accumulate the execution output

    /******************ADD YOUR VARIABLES HERE*************************/
    int curr_time = 0; // current time in the simulation
    int cntxt_time = 10; // default 10ms for saving context

    /******************************************************************/

    //parse each line of the input trace file
    while(std::getline(input_file, trace)) {
        auto [activity, duration_intr] = parse_trace(trace); // duration_intr is the duration of the activity or the interrupt number

        /******************ADD YOUR SIMULATION CODE HERE*************************/
        if(activity == "CPU"){
            // CPU activity - string format: <time>, <duration>, CPU activity
            execution += std::to_string(curr_time) + ", " + std::to_string(duration_intr) + ", CPU burst\n";
        
        } else if (activity == "SYSCALL"){
            // boilerplate for syscall interrupt
            auto [logging, upd_time] = intr_boilerplate(curr_time, duration_intr, cntxt_time, vectors);
            curr_time = upd_time;
            execution += logging;

            // ISR body (wait time for that device)
            int wait = delays[duration_intr];
            curr_time += wait;
            execution += std::to_string(curr_time) + ", " + std::to_string(wait) + ", ISR for device " + std::to_string(duration_intr) + "\n";

            // return from interrupt boilerplate
            curr_time++;
            execution += std::to_string(curr_time) + ", " + std::to_string(1) + ", return from interrupt\n";


        } else if(activity == "END_IO") {
        }
        

        /************************************************************************/

    }

    input_file.close();

    write_output(execution);

    return 0;
}
