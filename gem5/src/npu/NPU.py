from m5.defines import buildEnv
from m5.params import *
from m5.proxy import *

from MemObject import MemObject

class Npu(MemObject):
    type = 'Npu'
    cxx_class = 'Npu'
    cxx_header = 'npu/npu.hh'
    cpuPort = SlavePort("port that connects with cpu")
    callRetry = Param.Bool(False, 'check whether has to schedule sendRetry')
    numInputs = Param.Int(0, 'number of inputs')
    numOutputs = Param.Int(0, 'number of outputs')
    numLayers = Param.Int(0, 'number of layers')
    numNeurons_1 = Param.Int(0, 'number of hidden layer 1 neurons')
    numNeurons_2 = Param.Int(0, 'number of hidden layer 2 neurons')
    numWeights = Param.Int(0, 'number of weights')
    calculateTick = Param.Tick(2**64-1, 'the time npu spends to calculate')
    weightCount = Param.Int(0, 'weight counting')
    inputCount = Param.Int(0, 'input counting')
