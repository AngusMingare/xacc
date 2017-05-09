/***********************************************************************************
 * Copyright (c) 2016, UT-Battelle
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of the xacc nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Contributors:
 *   Initial API and implementation - Alex McCaskey
 *
 **********************************************************************************/
#ifndef QUANTUM_GATE_ACCELERATORS_EIGENACCELERATOR_HPP_
#define QUANTUM_GATE_ACCELERATORS_EIGENACCELERATOR_HPP_

#include "Hadamard.hpp"
#include "Measure.hpp"
#include "CNOT.hpp"
#include "Rz.hpp"
#include "Z.hpp"
#include "X.hpp"
#include "ConditionalFunction.hpp"
#include "Z.hpp"
#include "QPUGate.hpp"
#include "QuantumCircuit.hpp"
#include "SimulatedQubits.hpp"
#include <random>
#include "InstructionIterator.hpp"

using namespace xacc;

namespace xacc {
namespace quantum {

double sqrt2 = std::sqrt(2.0);
using ProductList = std::vector<fire::Tensor<2, fire::EigenProvider, std::complex<double>>>;
using ComplexTensor = fire::Tensor<2, fire::EigenProvider, std::complex<double>>;

class FunctionalGateInstructionVisitor: public BaseInstructionVisitor,
		public InstructionVisitor<CNOT>,
		public InstructionVisitor<Hadamard>,
		public InstructionVisitor<X>,
		public InstructionVisitor<Z>,
		public InstructionVisitor<Measure>,
		public InstructionVisitor<ConditionalFunction> {
protected:
	std::function<void(Hadamard&)> hAction;
	std::function<void(CNOT&)> cnotAction;
	std::function<void(X&)> xAction;
	std::function<void(Z&)> zAction;
	std::function<void(Measure&)> measureAction;
	std::function<void(ConditionalFunction&)> condAction;

public:
	template<typename HF, typename CNF, typename XF, typename MF, typename ZF, typename CF>
	FunctionalGateInstructionVisitor(HF h, CNF cn, XF x, MF m, ZF z, CF c) :
			hAction(h), cnotAction(cn), xAction(x), zAction(z), measureAction(m), condAction(c) {
	}

	void visit(Hadamard& h) {
		hAction(h);
	}
	void visit(CNOT& cn) {
		cnotAction(cn);
	}
	void visit(X& x) {
		xAction(x);
	}
	void visit(Z& z) {
		zAction(z);
	}
	void visit(Measure& m) {
		measureAction(m);
	}
	void visit(ConditionalFunction& c) {
		condAction(c);
	}
	virtual ~FunctionalGateInstructionVisitor() {}
};

/**
 * The FireTensorAccelerator is an XACC Accelerator that simulates
 * gate based quantum computing circuits. It models the QPUGate Accelerator
 * with SimulatedQubit AcceleratorBuffer. It relies on the Fire Scientific Computing
 * Framework's tensor module to model a specific set of quantum gates. It uses these
 * tensors to build up the unitary matrix described by the circuit.
 */
class FireTensorAccelerator : virtual public QPUGate {
public:

	/**
	 * Create, store, and return an AcceleratorBuffer with the given
	 * variable id string. This string serves as a unique identifier
	 * for future lookups and reuse of the AcceleratorBuffer.
	 *
	 * @param varId
	 * @return
	 */
	std::shared_ptr<AcceleratorBuffer> createBuffer(const std::string& varId);

	/**
	 * Create, store, and return an AcceleratorBuffer with the given
	 * variable id string and of the given number of bits.
	 * The string id serves as a unique identifier
	 * for future lookups and reuse of the AcceleratorBuffer.
	 *
	 * @param varId
	 * @param size
	 * @return
	 */
	std::shared_ptr<AcceleratorBuffer> createBuffer(const std::string& varId,
			const int size);

	/**
	 * Return true if this Accelerator can allocated
	 * NBits number of bits.
	 * @param NBits
	 * @return
	 */
	virtual bool isValidBufferSize(const int NBits);

	/**
	 * Execute the simulation. Requires both a valid SimulatedQubits buffer and
	 * XACC IR Function instance modeling the quantum circuit.
	 *
	 * @param ir
	 */
	virtual void execute(std::shared_ptr<AcceleratorBuffer> buffer, const std::shared_ptr<xacc::Function> kernel);

	/**
	 * The destructor
	 */
	virtual ~FireTensorAccelerator() {}

};


}
}




#endif
