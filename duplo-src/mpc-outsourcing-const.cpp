#include "mains/outsourcing-mains.h"

int main(int argc, const char* argv[]) {
    ezOptionParser opt;

    opt.overview = "MPCOutsourcingConstructor Passing Parameters Guide.";
    opt.syntax = "MPCOutsourcingConstructor first second third forth fifth sixth";
    opt.example = "MPCOutsourcingConstructor -c outsc -e 1,1,1 -ip localhost -p 28001 -a 1 -t 1\n\n";
    opt.footer = "ezOptionParser 0.1.4  Copyright (C) 2011 Remik Ziemlinski\nThis program is free and without warranty.\n";

    opt.add(
            "", // Default.
            0, // Required?
            0, // Number of args expected.
            0, // Delimiter if expecting multiple args.
            "Display usage instructions.", // Help description.
            "-h",     // Flag token.
            "-help",  // Flag token.
            "--help", // Flag token.
            "--usage" // Flag token.
    );

    opt.add(
            default_circuit_name.c_str(), // Default.
            0, // Required?
            1, // Number of args expected.
            0, // Delimiter if expecting multiple args.
            "Circuit name. Can be either aes, sha-1, sha-256 or cbc.", // Help description.
            "-c" // Flag token.
    );

    opt.add(
            default_execs.c_str(), // Default.
            0, // Required?
            3, // Number of args expected.
            ',', // Delimiter if expecting multiple args.
            "Number of parallel executions for each phase. Preprocessing, Offline and Online.", // Help description.
            "-e"
    );

    opt.add(
            default_optimize_online.c_str(), // Default.
            0, // Required?
            1, // Number of args expected.
            0, // Delimiter if expecting multiple args.
            "Optimize for online or overall efficiency", // Help description.
            "-o"
    );

    opt.add(
            default_ip_address.c_str(), // Default.
            0, // Required?
            1, // Number of args expected.
            0, // Delimiter if expecting multiple args.
            "IP Address of Machine running TinyConst", // Help description.
            "-ip"
    );

    opt.add(
            default_port.c_str(), // Default.
            0, // Required?
            1, // Number of args expected.
            0, // Delimiter if expecting multiple args.
            "Port to listen on/connect to", // Help description.
            "-p"
    );

    opt.add(
            "0", // Default.
            0, // Required?
            1, // Number of args expected.
            0, // Delimiter if expecting multiple args.
            "Number of aes circuits in composed.", // Help description.
            "-a"
    );

    opt.add(
            "0", // Default.
            0, // Required?
            1, // Number of args expected.
            0, // Delimiter if expecting multiple args.
            "Number of tag circuits in composed 0 or 1.", // Help description.
            "-t"
    );


    //Attempt to parse input
    opt.parse(argc, argv);

    //Check if help was requested and do some basic validation
    if (opt.isSet("-h")) {
        Usage(opt);
        return 1;
    }
    std::vector<std::string> badOptions;
    if (!opt.gotExpected(badOptions)) {
        for (int i = 0; i < badOptions.size(); ++i)
            std::cerr << "ERROR: Got unexpected number of arguments for option " << badOptions[i] << ".\n\n";
        Usage(opt);
        return 1;
    }

    //Copy inputs into the right variables
    int num_execs_components, num_execs_auths, num_execs_online, optimize_online, port, num_aes_in_composed, num_tag_in_composed;
    std::vector<int> num_execs;
    std::string circuit_name, circuit_name_id, circuit_name_tag, circuit_name_aes, circuit_name_composed, ip_address, exec_name;
    Circuit circuit_id, circuit_tag, circuit_aes;
    ComposedCircuit composed_circuit;

    opt.get("-c")->getString(circuit_name);
    circuit_name_id = "const_ " + circuit_name + "_id";
    circuit_name_tag = "const_ " + circuit_name + "_tag";
    circuit_name_aes = "const_ " + circuit_name + "_aes";

    opt.get("-e")->getInts(num_execs);
    num_execs_components = num_execs[0];
    num_execs_auths = num_execs[1];
    num_execs_online = num_execs[2];

    opt.get("-o")->getInt(optimize_online);
    opt.get("-ip")->getString(ip_address);
    opt.get("-p")->getInt(port);
    opt.get("-a")->getInt(num_aes_in_composed);
    opt.get("-t")->getInt(num_tag_in_composed);

    //Set the circuit variables according to circuit_name
    if (circuit_name.find("outsc") != std::string::npos) {
        circuit_id = read_text_circuit("../test/data/idopt_i128_k128_s128.txt");
        circuit_tag = read_text_circuit("../test/data/tag_i128_k128_s128.txt");
        circuit_aes = read_text_circuit("../test/data/AES-non-expanded.txt");
    } else {
        std::cout << "No such circuit" << std::endl;
        return 1;
    }

//    Compute the required number of common_tools that are to be created. We create one main param and one for each sub-thread that will be spawned later on. Need to know this at this point to timing_file context properly
    int max_num_parallel_execs = max_element(num_execs.begin(), num_execs.end())[0];
    zmq::context_t context(NUM_IO_THREADS, 2 * (max_num_parallel_execs + 1)); //We need two sockets pr. channel

//    Setup the main common_tools object
    CommonTools common_tools(constant_seeds[0], ip_address, (uint16_t) port, 0, context, GLOBAL_PARAMS_CHAN);

    uint8_t* dummy_val = new uint8_t[network_dummy_size]; //50 MB
    uint8_t* dummy_val2 = new uint8_t[network_dummy_size]; //50 MB
    common_tools.chan.ReceiveBlocking(dummy_val2, network_dummy_size);
    common_tools.chan.Send(dummy_val, network_dummy_size);

    mr_init_threading(); //Needed for Miracl library to work with threading.
    DuploConstructor duplo_const(common_tools, max_num_parallel_execs);

//    Run initial Setup (BaseOT) phase
    auto setup_begin = GET_TIME();
    duplo_const.Setup();
    auto setup_end = GET_TIME();

    mr_end_threading();

//    Run Preprocessing phase
    auto preprocess_begin = GET_TIME();
    duplo_const.PreprocessComponentType(circuit_name_id, circuit_id, 1, num_execs_components);
    if (num_tag_in_composed != 0)
        num_tag_in_composed = 1;
        duplo_const.PreprocessComponentType(circuit_name_tag, circuit_tag, 1, num_execs_components);
    if (num_aes_in_composed != 0)
        duplo_const.PreprocessComponentType(circuit_name_aes, circuit_aes, num_aes_in_composed, num_execs_components);
    auto preprocess_end = GET_TIME();

//    Prepare the composed circuit
    solderIdTagAesToOne(composed_circuit, circuit_id, circuit_name_id, circuit_tag, circuit_name_tag, circuit_aes, circuit_name_aes, num_tag_in_composed, num_aes_in_composed);

//    std::cout << "Segfault in soldering\n";

//    Soldering of the tree circuits
    auto soldering_begin = GET_TIME();
    duplo_const.SolderGarbledComponents(circuit_name_composed, composed_circuit, num_execs_components);
    auto soldering_end = GET_TIME();

    std::vector<std::pair<std::string, uint32_t>> input_circuits;
    for (int c = 0; c < composed_circuit.num_inp_circuits; ++c) {
        input_circuits.emplace_back(composed_circuit.circuits[c]);
    }

//    std::cout << "Segfault before this print statement\n";

//    Run Auth Preprocessing phase
    auto prepare_eval_begin = GET_TIME();
    duplo_const.PrepareComponents(input_circuits, num_execs_auths);
    auto prepare_eval_end = GET_TIME();

    std::vector<std::vector<uint8_t>> inputs;
    for (int i = 0; i < composed_circuit.num_inp_circuits; ++i) {
        inputs.emplace_back(std::vector<uint8_t>(BITS_TO_BYTES(circuit_id.num_const_inp_wires)));
    }

    std::vector<BYTEArrayVector> const_output_keys;

//    Evaluating components
    auto eval_circuits_begin = GET_TIME();
    duplo_const.EvalComposedComponents(circuit_name_composed, inputs, const_output_keys, num_execs_online);
    auto eval_circuits_end = GET_TIME();

    std::vector<std::vector<uint8_t>> const_outputs;
    std::vector<std::pair<std::string, uint32_t>> output_circuits;
    for (int c = composed_circuit.circuits.size() - composed_circuit.num_out_circuits; c < composed_circuit.circuits.size(); ++c) {
        output_circuits.emplace_back(composed_circuit.circuits[c]);
        const_outputs.emplace_back(std::vector<uint8_t>());
    }

//    Decoding keys
    std::vector<std::vector<uint8_t>> outputs(composed_circuit.num_out_circuits);
    auto decode_keys_begin = GET_TIME();
    duplo_const.DecodeKeys(output_circuits, const_output_keys, const_outputs, num_execs_online);
    auto decode_keys_end = GET_TIME();

//    PrintBin(const_outputs[0].data(), 128); /*Print input*/
//    PrintBin(const_outputs[1].data(), 128); /*Print output*/

    uint64_t setup_time_nano = std::chrono::duration_cast<std::chrono::nanoseconds>(setup_end - setup_begin).count();
    uint64_t preprocess_time_nano = std::chrono::duration_cast<std::chrono::nanoseconds>(preprocess_end - preprocess_begin).count();
    uint64_t soldering_time_nano = std::chrono::duration_cast<std::chrono::nanoseconds>(soldering_end - soldering_begin).count();
    uint64_t prepare_eval_time_nano = std::chrono::duration_cast<std::chrono::nanoseconds>(prepare_eval_end - prepare_eval_begin).count();
    uint64_t eval_circuits_nano = std::chrono::duration_cast<std::chrono::nanoseconds>(eval_circuits_end - eval_circuits_begin).count();
    uint64_t decode_keys_nano = std::chrono::duration_cast<std::chrono::nanoseconds>(decode_keys_end - decode_keys_begin).count();

    ofstream timing_file ("const.timing", ios::out | ios::app);
    printTiming(timing_file, num_tag_in_composed, num_aes_in_composed, num_execs_components, setup_time_nano, preprocess_time_nano, soldering_time_nano, prepare_eval_time_nano, eval_circuits_nano, decode_keys_nano);
}
