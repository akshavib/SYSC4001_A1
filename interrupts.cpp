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
    int isr_activity = 40; // default 40ms for interrupt activity

    /******************************************************************/

    //parse each line of the input trace file
    while(std::getline(input_file, trace)) {
        auto [activity, duration_intr] = parse_trace(trace); // duration_intr is the duration of the activity or the interrupt number

        /******************ADD YOUR SIMULATION CODE HERE*************************/
        if(activity == "CPU"){
            // CPU activity - string format: <time>, <duration>, CPU activity
            execution += std::to_string(curr_time) + ", " + std::to_string(duration_intr) + ", CPU burst\n";
            curr_time += duration_intr;
        
        } else if (activity == "SYSCALL"){
            // boilerplate for syscall interrupt
            // boilerplate wants the current tume, corrolated interrupt time, context save time, vector table
            auto [logging, upd_time] = intr_boilerplate(curr_time, duration_intr, cntxt_time, vectors);
            execution += logging;
            curr_time = upd_time;

            // ISR body (wait time for that device)
            int total_isr = delays[duration_intr];

            if (total_isr <= isr_activity){
                execution += std::to_string(curr_time) + ", " + std::to_string(total_isr) + ", SYSCALL: run ISR for device " + std::to_string(duration_intr) + "\n";
                curr_time += total_isr;
            } else if (total_isr <= 2 * isr_activity){
                execution += std::to_string(curr_time) + ", " + std::to_string(isr_activity) + ", SYSCALL: run ISR for device " + std::to_string(duration_intr) + "\n";
                curr_time += isr_activity;

                int remaining = total_isr - isr_activity;
                execution += std::to_string(curr_time) + ", " + std::to_string(remaining) + ", transfer device data to memory\n";
                curr_time += remaining;
            } else {
                execution += std::to_string(curr_time) + ", " + std::to_string(isr_activity) + ", SYSCALL: run ISR for device " + std::to_string(duration_intr) + "\n";
                curr_time += isr_activity;

                execution += std::to_string(curr_time) + ", " + std::to_string(isr_activity) + ", transfer device data to memory\n";
                curr_time += isr_activity;

                int remaining = total_isr - 2 * isr_activity;
                execution += std::to_string(curr_time) + ", " + std::to_string(remaining) + ", SYSCALL: finalize ISR for device, check errors\n";
                curr_time += remaining;
            }

            // conclude interrupt, return to user mode
            execution += std::to_string(curr_time) + ", " + std::to_string(1) + ", return from interrupt (IRET)\n";
            curr_time += 1;


        } else if(activity == "END_IO") {
            // boilerplate for endio
            auto[logging, upd_time] = intr_boilerplate(curr_time, duration_intr, cntxt_time, vectors);

            execution += logging;
            curr_time = upd_time;

            // ISR body (wait time for that device)
            int total_isr = delays[duration_intr];

            if(total_isr <= isr_activity){
                execution += std::to_string(curr_time) + ", " + std::to_string(total_isr) + ", ENDIO: run the ISR (device driver)\n";
                curr_time += total_isr;
            } else{
                execution += std::to_string(curr_time) + ", " + std::to_string(isr_activity) + ", ENDIO: run the ISR (device driver)\n";
                curr_time += isr_activity;

                int diff = total_isr - isr_activity;
                execution += std::to_string(curr_time) + ", " + std::to_string(diff) + ", check device status\n";

                curr_time += diff;
            }

            // conclude interrupt, return to user mode
            execution += std::to_string(curr_time) + ", " + std::to_string(1) + ", return from interrupt (IRET)\n";
            curr_time += 1;

        } else {
            // unknown activity
            execution += std::to_string(curr_time) + ", " + std::to_string(1) + ", UNKNOWN activity\n";
        }
        

        /************************************************************************/

    }

    input_file.close();

    write_output(execution);

    return 0;
}
