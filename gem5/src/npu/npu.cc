//#include <stdio.h> //should be deleted
#include "npu/npu.hh"

Npu::Npu(const Params *p) : MemObject(p), cpuPort(name() + ".port", this) {

}

Npu::~Npu() {

}

bool
Npu::CpuPort::recvTimingReq(PacketPtr pkt) {
//    printf("Npu::CpuPort::recvTimingReq not implemented.");
    return true;
}

void
Npu::CpuPort::recvRetry() {
//    printf("Npu::CpuPort::recvRetry not implemented.");
}

void
Npu::CpuPort::recvRespRetry() {}

Npu*
NpuParams::create() {
    return new Npu(this);
}
