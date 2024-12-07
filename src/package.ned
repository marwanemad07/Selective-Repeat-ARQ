package project;

import ned.IdealChannel;


@license(LGPL);
//
// TODO documentation
//
network Network
{
    @display("bgb=878,318");
    submodules:
        coordinator: Coordinator {
            @display("p=310.905,242.55");
        }
        Node0: Node {
            @display("p=181.9125,128.99251");
        }
        Node1: Node {
            @display("p=450.92252,128.99251");
        }
    connections:
        Node0.out --> IdealChannel --> Node1.in[0];
        Node1.out --> IdealChannel --> Node0.in[0];
        coordinator.out[1] --> IdealChannel --> Node1.in[1];
        coordinator.out[0] --> IdealChannel --> Node0.in[1];
}

