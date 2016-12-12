#ifndef __NPU_HH__
#define __NPU_HH__

#include <queue>

#include "mem/mem_object.hh"
#include "mem/port.hh"
#include "params/Npu.hh"

class Npu : public MemObject {
  public:
    typedef NpuParams Params;
    Npu(const Params *p);
    ~Npu();

  protected:
	class TickEvent: public Event {
		Npu *npu;

	  public:
		TickEvent(Npu *_npu) : npu(_npu) {}
		void schedule(Tick t) { npu->schedule(this, t); }
		void deschedule() { npu->deschedule(this); }
		void process();
	};

    class CpuPort : public SlavePort {
      public:
          CpuPort(const std::string &_name, Npu* _owner) : SlavePort(_name, _owner), ownerNpu(_owner){}
      protected:
          Npu* ownerNpu;

          virtual bool
          recvTimingReq(PacketPtr pkt);

          virtual Tick
          recvAtomic(PacketPtr pkt){ return 0; }

          virtual void
          recvFunctional(PacketPtr pkt) {}

          virtual void
          recvRangeChange() {}

          virtual void
          recvRetry() {}

          virtual void
          recvRespRetry() {}

          virtual AddrRangeList
          getAddrRanges() const
          {
            AddrRangeList ranges;
//            ranges.push_back(ownerNpu->getAddrRange());
            return ranges;
          }

//          template<typename T>
//          void
//          loadData(PacketPtr packet);
//
//          template<typename T>
//          void
//          storeData(PacketPtr packet);
//
//          template<typename T>
//          void
//          atomicOperation(PacketPtr packet);
    };

  public:
	void process();
	bool returnQueuePush(std::pair<Tick, PacketPtr> thePair);
	bool processPacket(PacketPtr packet);
	void readData(PacketPtr packet);
	void writeData(PacketPtr packet);
	void calculate();
	Tick earliestReturnTime() const {
		Tick t;
		if(returnQueue.empty() || 
		  (!returnQueue.empty() && returnQueue.back().first < curTick())) t = curTick();
		else t = returnQueue.back().first; 
		return t;
	}
	Tick ticks(unsigned numCycles) const { return clock*(Tick)numCycles; }

//	bool getCallRetry() const { return callRetry; }
//	Tick getCalculateTick() const { return calculateTick; }
//	void setCallRetry(const bool set) { callRetry = set; }

  public:
	Tick clock;

	CpuPort cpuPort;
	TickEvent tickEvent;

	bool callRetry;

	int numInputs;
	int numOutputs;
	int numLayers;
	int numNeurons_1;
	int numNeurons_2;
	int numWeights;
	Tick calculateTick;

	std::queue<uint8_t> inputQueue, configQueue, outputQueue;
	std::queue<std::pair<Tick, PacketPtr>> returnQueue;
};

#endif
