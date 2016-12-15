#include "ezOptionParser/ezOptionParser.hpp"
#include "duplo/duplo-evaluator.h"
#include "duplo/duplo-constructor.h"

using namespace ez;

//Hardcoded constants
static const uint32_t network_dummy_size = 50000000;
static const uint32_t commit_dummy_size = 1000000;
static const uint32_t commit_dummy_input = 128;
static const uint32_t commit_dummy_output = 128;

//Hardcoded default values
static std::string default_num_iters("10");
static std::string default_circuit_name("outsc");
static std::string default_execs("1,1,1");
static std::string default_optimize_online("0");
static std::string default_ip_address("localhost");
static std::string default_port("28001");
static std::string default_print_format("0");

static std::string default_num_commits("10000");
static std::string default_num_commit_execs("1");

void Usage(ezOptionParser& opt) {
    std::string usage;
    opt.getUsage(usage);
    std::cout << usage;
};

void solderIdTagAesToOne(ComposedCircuit& composed_circuit,
                         Circuit& id, std::string circuit_id_name,
                         Circuit& tag, std::string circuit_tag_name,
                         Circuit& aes, std::string circuit_aes_name,
                         int num_tag_circuit,
                         int num_aes_circuits) {

//    Layers in composed circuit is constant as AES is added in parallel
    composed_circuit.circuits_in_layer.resize(2);

//    Updating composed circuit information
    composed_circuit.num_inp_circuits = 1;
    composed_circuit.num_out_circuits = num_tag_circuit + num_aes_circuits;

//    Adding ID circuit to layer 0 as it will be in all of our composed circuits
    composed_circuit.circuits.emplace_back(std::make_pair(circuit_id_name, 0));
    composed_circuit.circuits_in_layer[0].emplace_back(0); /* Id circuit goes into layer 0 */

//    Adding TAG circuit to layer 1
    if (num_tag_circuit) {
        composed_circuit.circuits.emplace_back(std::make_pair(circuit_tag_name, 0));
        composed_circuit.circuits_in_layer[1].emplace_back(1); /* Tag circuit goes into layer 1 */
    }

//    Adding AES circuits to layer 1
    for (int i = 0; i < num_aes_circuits; ++i) {
        composed_circuit.circuits.emplace_back(std::make_pair(circuit_aes_name, i));
        composed_circuit.circuits_in_layer[1].emplace_back(1 + num_tag_circuit + i); /* AES circuit goes into layer 1 */
    }

    composed_circuit.out_wire_holders.resize(composed_circuit.circuits.size());

//    Adding output holder info for TAG circuit
    if (num_tag_circuit) {
        std::vector<uint32_t> out_vals(id.num_out_wires);
        std::iota(std::begin(out_vals), std::end(out_vals), 0);
        composed_circuit.out_wire_holders[1].emplace_back(std::make_pair(0, out_vals));
    }

//    Adding output holder info for AES circuit
    for (int j = 0; j < num_aes_circuits; ++j) {
        std::vector<uint32_t> out_vals_interval(128);
        std::iota (std::begin(out_vals_interval), std::end(out_vals_interval), 0);
        std::vector<uint32_t> out_vals_i2(128);
        std::iota (std::begin(out_vals_i2), std::end(out_vals_i2), id.num_inp_wires / 2);

        out_vals_interval.insert(std::end(out_vals_interval), std::begin(out_vals_i2), std::end(out_vals_i2));

        composed_circuit.out_wire_holders[num_tag_circuit + j + 1].emplace_back(std::make_pair(0, out_vals_interval));
    }

//    Adding output wire pointers
    composed_circuit.out_wire_holder_to_wire_idx.emplace(0, id.num_inp_wires);

    if (num_tag_circuit) {
        composed_circuit.out_wire_holder_to_wire_idx.emplace(1, id.num_inp_wires + id.num_out_wires);
    }

    for (int k = 0; k < num_aes_circuits; ++k) {
        composed_circuit.out_wire_holder_to_wire_idx.emplace(num_tag_circuit + k + 1, id.num_inp_wires + id.num_out_wires + num_tag_circuit * (tag.num_out_wires) + k * (aes.num_out_wires));
    }

};

void printTiming(ofstream& output_file,
                 int num_tag_in_composed,
                 int num_aes_in_composed,
                 int num_parallel_execs,
                 uint64_t setup_time_nano,
                 uint64_t preprocess_time_nano,
                 uint64_t soldering_time_nano,
                 uint64_t prepare_time_nano,
                 uint64_t eval_time_nano,
                 uint64_t decode_time_nano) {
    uint64_t setup_time = setup_time_nano / 1000000;
    double preprocess_time = preprocess_time_nano / 1000000;
    double soldering_time = soldering_time_nano / 1000000;
    double prepare_time = prepare_time_nano / 1000000;
    double eval_time = eval_time_nano / 1000000;
    double decode_time = decode_time_nano / 1000000;


    if (output_file.is_open()) {
        output_file << num_tag_in_composed << ", " <<
                    num_aes_in_composed << ", " <<
                    num_parallel_execs << ", " <<
                    setup_time << ", " <<
                    preprocess_time << ", " <<
                    soldering_time << ", " <<
                    prepare_time << ", " <<
                    eval_time << ", " <<
                    decode_time << std::endl;

        output_file.close();
    }

    std::cout << "Setup ms: " << setup_time << std::endl;
    std::cout << "Circuit Preprocessing ms: " << preprocess_time  << std::endl;
    std::cout << "Circuit Soldering ms: " << soldering_time << std::endl;
    std::cout << "PrepareEval Preprocess ms: " << prepare_time << std::endl;
    std::cout << "Eval circuits ms: " << eval_time << std::endl;
    std::cout << "Decode keys ms: " << decode_time << std::endl;
};