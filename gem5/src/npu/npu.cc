//#include <stdio.h> //should be deleted
#include "npu/npu.hh"
#include <cstdint>

Npu::Npu(const Params *p) :
	MemObject(p),
	clock(p->clk_domain->clockPeriod()),
	cpuPort(name() + ".port", this),
	tickEvent(this),
	callRetry(p->callRetry),
	numInputs(p->numInputs),
	numOutputs(p->numOutputs),
	numLayers(p->numLayers),
	numNeurons_1(p->numNeurons_1),
	numNeurons_2(p->numNeurons_2),
	numWeights(p->numWeights),
	calculateTick(p->calculateTick)
{
}

Npu::~Npu() {

}

bool
Npu::CpuPort::recvTimingReq(PacketPtr pkt) {
	return ownerNpu->processPacket(pkt);
}

void
Npu::writeData(PacketPtr pkt) {
	uint8_t data;
	pkt->writeData(&data);

	if(numInputs == 0) {
		numInputs = data;
		return;
	}
	else if(numOutputs == 0) {
		numOutputs = data;
		return;
	}
	else if(numLayers == 0) {
		numLayers = data;
		return;
	}
	else if(numNeurons_1 == 0) {
		numNeurons_1 = data;
		if(numLayers == 1) {
			numWeights = numInputs*numNeurons_1 + numNeurons_1*numOutputs;
		}
		return;
	}
	else if(numLayers == 2 && numNeurons_2 == 0) {
		numNeurons_2 = data;
		numWeights = numInputs*numNeurons_1 + numNeurons_1*numNeurons_2 +
					numNeurons_2*numOutputs;
		return;
	}
	else if(configQueue.size() < numWeights) {
		configQueue.push(data);
		return;
	}
	else if(inputQueue.size() < numInputs) {
		inputQueue.push(data);
		if(inputQueue.size() == numInputs) {
			calculate();
		}
		return;
	}
	else {
		panic("There should be no other inputs");
	}
}

void
Npu::calculate() {
	unsigned numCycles;
	if(numLayers == 1) {
		numCycles = ((numNeurons_1-1)/8+1)*(numInputs   +1) + numNeurons_1 +
					((numOutputs  -1)/8+1)*(numNeurons_1+1) + numOutputs   ;
		calculateTick = ticks(numCycles);
	}
	else if(numLayers == 2) {
		numCycles = ((numNeurons_1-1)/8+1)*(numInputs   +1) + numNeurons_1 +
					((numNeurons_2-1)/8+1)*(numNeurons_1+1) + numNeurons_2 +
					((numOutputs  -1)/8+1)*(numNeurons_2+1) + numOutputs   ;
		calculateTick = ticks(numCycles);
	}
	else {
		panic("numLayers should be 1 or 2.");
	}

	for(int i = 0; i != numOutputs; ++i) {
		outputQueue.push(0);
	}

	if(callRetry) {
		if(!tickEvent.scheduled()) {
			tickEvent.schedule(calculateTick);
		}
		else if(tickEvent.when() > calculateTick) {
			tickEvent.deschedule();
			tickEvent.schedule(calculateTick);
		}
	}
}

void
Npu::readData(PacketPtr pkt) {
	assert(!outputQueue.empty());
	const uint8_t p = outputQueue.front();
	pkt->setData(&p);
}

bool
Npu::processPacket(PacketPtr pkt) {
	if(pkt->isWrite()) {  // write to input or config
		writeData(pkt);
	}
	else {  // read output
		assert(pkt->isRead());
		if(calculateTick > curTick()) { // npu is not done yet
			// schedule a sendRetry call
			callRetry = true;
			if(!tickEvent.scheduled()) {
				tickEvent.schedule(calculateTick);
			}
			return false;
		}
		else {  // npu is done
			readData(pkt);
		}
	}
	// prepare response packet
	Tick doneAt = earliestReturnTime() + ticks(2);
	return returnQueuePush(std::make_pair(doneAt, pkt));
}

bool
Npu::returnQueuePush(std::pair<Tick, PacketPtr> thePair) {
	returnQueue.push(thePair);
	if(!tickEvent.scheduled()) {
		process();
	}
	return true;
}

void
Npu::process()
{
    Tick now = clockEdge();
	if(callRetry && calculateTick <= now) {
		callRetry = false;
		cpuPort.sendRetryReq();//_masterPort->recvRetry();//
	}

    // send back completed packets
    while (!returnQueue.empty() && returnQueue.front().first <= now) {
        PacketPtr packet = returnQueue.front().second;
 
        packet->makeTimingResponse();

        returnQueue.pop();

        bool success = cpuPort.sendTimingResp(packet);

        if (!success) {
            panic("have not handled timing responses being NACK'd when sent"
                            "back");
        }
    }
    // determine the next wakeup time
    if (!returnQueue.empty() || callRetry) {

		Tick next; // next = min(returnTick, calculateTick)
		if(!callRetry) next = returnQueue.front().first;
		else if(returnQueue.empty()) next = calculateTick;
		else {
			if(returnQueue.front().first < calculateTick) 
				next = returnQueue.front().first;
			else
				next = calculateTick;
		}

        if (tickEvent.scheduled()) {
            if (next < tickEvent.when()) {
                tickEvent.deschedule();
                tickEvent.schedule(next);
            }
        } else {
            tickEvent.schedule(next);
        }
    }

}

void
Npu::TickEvent::process() {
	npu->process();
}


Npu*
NpuParams::create() {
    return new Npu(this);
}
