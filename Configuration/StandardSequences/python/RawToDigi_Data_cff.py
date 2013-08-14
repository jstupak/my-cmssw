import FWCore.ParameterSet.Config as cms

from Configuration.StandardSequences.RawToDigi_cff import *

RawToDigi = cms.Sequence(csctfDigis
                         +dttfDigis
                         +gctDigis
                         +gtDigis
                         +gtEvmDigis
                         +siPixelDigis
                         +siStripDigis
                         +ecalDigis
                         +ecalPreshowerDigis
                         +hcalDigis
                         +muonCSCDigis
                         +muonDTDigis
                         +muonRPCDigis
                         +castorDigis
                         +scalersRawToDigi)

RawToDigi_woGCT = cms.Sequence(csctfDigis
                               +dttfDigis
                               +gtDigis
                               +gtEvmDigis
                               +siPixelDigis
                               +siStripDigis
                               +ecalDigis
                               +ecalPreshowerDigis
                               +hcalDigis
                               +muonCSCDigis
                               +muonDTDigis
                               +muonRPCDigis
                               +castorDigis
                               +scalersRawToDigi)


gtDigis.DaqGtInputTag = 'source'
gtEvmDigis.EvmGtInputTag = 'source'
siPixelDigis.InputLabel = 'source'
siStripDigis.ProductLabel = 'source'
castorDigis.InputLabel = 'source'
ecalDigis.DoRegional = False

#set those back to "source"
scalersRawToDigi.scalersInputTag = 'source'
csctfDigis.producer = 'source'
dttfDigis.DTTF_FED_Source = 'source'
gctDigis.inputLabel = 'source'
gtDigis.DaqGtInputTag = 'source'
siPixelDigis.InputLabel = 'source'
#False by default ecalDigis.DoRegional = False
ecalDigis.InputLabel = 'source'
ecalPreshowerDigis.sourceTag = 'source'
hcalDigis.InputLabel = 'source'
muonCSCDigis.InputObjects = 'source'
muonDTDigis.inputLabel = 'source'
muonRPCDigis.InputLabel = 'source'
gtEvmDigis.EvmGtInputTag = 'source'
castorDigis.InputLabel = 'source'

