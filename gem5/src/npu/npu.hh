#include "mem/mem_object.hh"
#include "params/Npu.hh"

class Npu : public MemObject {
  public:
    typedef NpuParams Params;
    Npu(const Params *p);
    ~Npu();

  protected:
    class CpuPort : public SlavePort {
      public:
          CpuPort(const std::string &_name, Npu* _owner) : SlavePort(_name, _owner), ownerNpu(_owner) {}
      protected:
          Npu* ownerNpu;

          virtual bool
          recvTimingReq(PacketPtr pkt);
/*
          virtual Tick
          recvAtomic(PacketPtr pkt)
          {
            return 0;
          }

          virtual void
          recvFunctional(PacketPtr pkt) {}

          virtual void
          recvRangeChange()
          {
          }
*/
          virtual void
          recvRetry();

          virtual void
          recvRespRetry();

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

  private:
	CpuPort cpuPort;
};
