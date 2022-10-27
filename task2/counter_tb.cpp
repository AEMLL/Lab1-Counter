#include "Vcounter.h"
#include "verilated.h"
#include "verilated_vcd_c.h"
#include "vbuddy.cpp"

int main(int argc, char **argv, char **env) {
    int i;
    int clk;

    Verilated::commandArgs(argc, argv);
    //init verilog instance
    Vcounter* top = new Vcounter;
    //init trace dump
    Verilated::traceEverOn(true);
    VerilatedVcdC* tfp = new VerilatedVcdC;
    top->trace (tfp, 99);
    tfp->open("counter.vcd");

    //init vbuddy
    if (vbdOpen()!=1) return(-1);
    vbdHeader("Lab 1: Counter");

    //intialize simulation inputs
    top->clk = 1;
    top->rst = 1;
    top->en = 0;
    top->order = 1;
    bool special_en = 1;
    int count;
    int stop = 0;

    //run simulation for many clock cycles
    for (i=0; i<300; i++) {

        //dump variables into VCD file and toggle clock
        for (clk = 0; clk<2; clk++) {
            tfp->dump (2*i+clk);            //unit is in ps!!!
            top->clk = !top->clk;
            top->eval ();
            count = top->count;
        }

        // ++++++ Send count value to Vbuddy
        vbdHex(4, (int(top->count) >> 16) & 0xF);
        vbdHex(3, (int(top->count) >> 8) & 0xF);
        vbdHex(2, (int(top->count) >> 4) & 0xF);
        vbdHex(1, int(top->count) & 0xF);
        vbdPlot(int(top->count), 0, 255);
        vbdCycle(i+1);
        // ----- end of Vbuddy section

        if (special_en) {
            special_en = !(count == 9);
        } else {
            if (stop >= 3) {
                special_en = 1;
            } else {
                stop++;
            }
        }

        //Change of input stimuli
        top->order = vbdFlag(); //TEST YOURSELF CHALLENGE
        top->rst = (i<2) | (i == 15);
        top->en = (i>4) && special_en /*&& vbdFlag()*/; //challenge 1

        if (Verilated::gotFinish()) exit(0);
    }

    vbdClose();
    tfp->close();
    exit(0);
}