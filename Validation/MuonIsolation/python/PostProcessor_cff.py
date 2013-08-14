import FWCore.ParameterSet.Config as cms

postProcessorMuonIsolation = cms.EDAnalyzer(
    "DQMGenericClient",
    subDirs = cms.untracked.vstring("Muons/MuonIsolationV*"),
    efficiency = cms.vstring(""),
    resolution = cms.vstring(""),
    normalization = cms.untracked.vstring("sumPt",
                                          "emEt",
                                          "hadEt",
                                          "hoEt",
                                          "nTracks",
                                          "nJets",
                                          "trackerVetoPt",
                                          "emVetoEt",
                                          "hadVetoEt",
                                          "hoVetoEt",
                                          "muonPt",
                                          "muonEta",
                                          "muonPhi",
                                          "avgPt",
                                          "weightedEt",
                                          "sumPt_cd",
                                          "emEt_cd",
                                          "hadEt_cd",
                                          "hoEt_cd",
                                          "nTracks_cd",
                                          "nJets_cd",
                                          "trackerVetoPt_cd",
                                          "emVetoEt_cd",
                                          "hadVetoEt_cd",
                                          "hoVetoEt_cd",                                          
                                          "muonPt_cd",
                                          "muonEta_cd",
                                          "muonPhi_cd",
                                          "avgPt_cd",
                                          "weightedEt_cd"
                                          ),
    cumulativeDists = cms.untracked.vstring("sumPt_cd",
                                            "emEt_cd",
                                            "hadEt_cd",
                                            "hoEt_cd",
                                            "nTracks_cd",
                                            "nJets_cd",
                                            "trackerVetoPt_cd",
                                            "emVetoEt_cd",
                                            "hadVetoEt_cd",
                                            "hoVetoEt_cd",
                                            "muonPt_cd",
                                            "avgPt_cd",
                                            "weightedEt_cd"
                                            ),
    outputFileName = cms.untracked.string("")
)        

MuIsoValPostProcessor = cms.Sequence(postProcessorMuonIsolation)