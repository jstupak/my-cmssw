/**
 * \class L1GtUtils
 * 
 * 
 * Description: various methods for L1 GT, to be called in an EDM analyzer, producer or filter.
 *
 * Implementation:
 *    <TODO: enter implementation details>
 *   
 * \author: Vasile Mihai Ghete - HEPHY Vienna
 * 
 * $Date$
 * $Revision$
 *
 */

// this class header
#include "L1Trigger/GlobalTriggerAnalyzer/interface/L1GtUtils.h"

// system include files
#include <iomanip>

// user include files
#include "DataFormats/L1GlobalTrigger/interface/L1GlobalTriggerReadoutSetupFwd.h"
#include "DataFormats/L1GlobalTrigger/interface/L1GlobalTriggerReadoutRecord.h"
#include "DataFormats/L1GlobalTrigger/interface/L1GlobalTriggerRecord.h"

#include "FWCore/Framework/interface/ESHandle.h"

#include "CondFormats/L1TObjects/interface/L1GtStableParameters.h"
#include "CondFormats/DataRecord/interface/L1GtStableParametersRcd.h"

#include "CondFormats/L1TObjects/interface/L1GtPrescaleFactors.h"
#include "CondFormats/DataRecord/interface/L1GtPrescaleFactorsAlgoTrigRcd.h"
#include "CondFormats/DataRecord/interface/L1GtPrescaleFactorsTechTrigRcd.h"

#include "CondFormats/L1TObjects/interface/L1GtTriggerMask.h"
#include "CondFormats/DataRecord/interface/L1GtTriggerMaskAlgoTrigRcd.h"
#include "CondFormats/DataRecord/interface/L1GtTriggerMaskTechTrigRcd.h"

#include "CondFormats/DataRecord/interface/L1GtTriggerMaskVetoAlgoTrigRcd.h"
#include "CondFormats/DataRecord/interface/L1GtTriggerMaskVetoTechTrigRcd.h"

#include "CondFormats/L1TObjects/interface/L1GtTriggerMenu.h"
#include "CondFormats/DataRecord/interface/L1GtTriggerMenuRcd.h"

#include "FWCore/MessageLogger/interface/MessageLogger.h"

// constructor(s)
L1GtUtils::L1GtUtils() :

    m_l1GtStableParCacheID(0ULL), m_numberAlgorithmTriggers(0),

    m_numberTechnicalTriggers(0),

    m_l1GtPfAlgoCacheID(0ULL), m_l1GtPfTechCacheID(0ULL),

    m_l1GtTmAlgoCacheID(0ULL), m_l1GtTmTechCacheID(0ULL),

    m_l1GtTmVetoAlgoCacheID(0ULL), m_l1GtTmVetoTechCacheID(0ULL),

    m_l1GtMenuCacheID(0ULL),

    m_l1EventSetupValid(false),

    m_l1GtMenuLiteValid(false),

    m_physicsDaqPartition(0),

    m_retrieveL1EventSetup(false),

    m_retrieveL1GtTriggerMenuLite(false)

    {

    // empty
}

// destructor
L1GtUtils::~L1GtUtils() {

    // empty

}

const std::string L1GtUtils::triggerCategory(
        const TriggerCategory& trigCategory) const {

    switch (trigCategory) {
        case AlgorithmTrigger: {
            return "Algorithm Trigger";
        }
            break;
        case TechnicalTrigger: {
            return "Technical Trigger";
        }

            break;
        default: {
            return EmptyString;
        }
            break;
    }
}


void L1GtUtils::retrieveL1EventSetup(const edm::EventSetup& evSetup) {

    //
    m_retrieveL1EventSetup = true;

    m_l1EventSetupValid = true;
    // FIXME test for each record if valid; if not set m_l1EventSetupValid = false;

    // get / update the stable parameters from the EventSetup
    // local cache & check on cacheIdentifier

    unsigned long long l1GtStableParCacheID =
            evSetup.get<L1GtStableParametersRcd>().cacheIdentifier();

    if (m_l1GtStableParCacheID != l1GtStableParCacheID) {

        edm::ESHandle<L1GtStableParameters> l1GtStablePar;
        evSetup.get<L1GtStableParametersRcd>().get(l1GtStablePar);
        m_l1GtStablePar = l1GtStablePar.product();

        // number of algorithm triggers
        m_numberAlgorithmTriggers = m_l1GtStablePar->gtNumberPhysTriggers();

        // number of technical triggers
        m_numberTechnicalTriggers =
                m_l1GtStablePar->gtNumberTechnicalTriggers();

        int maxNumberTrigger = std::max(m_numberAlgorithmTriggers,
                m_numberTechnicalTriggers);

        m_triggerMaskSet.reserve(maxNumberTrigger);
        m_prescaleFactorSet.reserve(maxNumberTrigger);

        //
        m_l1GtStableParCacheID = l1GtStableParCacheID;

    }

    // get / update the prescale factors from the EventSetup
    // local cache & check on cacheIdentifier

    unsigned long long l1GtPfAlgoCacheID =
            evSetup.get<L1GtPrescaleFactorsAlgoTrigRcd>().cacheIdentifier();

    if (m_l1GtPfAlgoCacheID != l1GtPfAlgoCacheID) {

        edm::ESHandle<L1GtPrescaleFactors> l1GtPfAlgo;
        evSetup.get<L1GtPrescaleFactorsAlgoTrigRcd>().get(l1GtPfAlgo);
        m_l1GtPfAlgo = l1GtPfAlgo.product();

        m_prescaleFactorsAlgoTrig = &(m_l1GtPfAlgo->gtPrescaleFactors());

        m_l1GtPfAlgoCacheID = l1GtPfAlgoCacheID;

    }

    unsigned long long l1GtPfTechCacheID = evSetup.get<
            L1GtPrescaleFactorsTechTrigRcd>().cacheIdentifier();

    if (m_l1GtPfTechCacheID != l1GtPfTechCacheID) {

        edm::ESHandle<L1GtPrescaleFactors> l1GtPfTech;
        evSetup.get<L1GtPrescaleFactorsTechTrigRcd>().get(l1GtPfTech);
        m_l1GtPfTech = l1GtPfTech.product();

        m_prescaleFactorsTechTrig = &(m_l1GtPfTech->gtPrescaleFactors());

        m_l1GtPfTechCacheID = l1GtPfTechCacheID;

    }

    // get / update the trigger mask from the EventSetup
    // local cache & check on cacheIdentifier

    unsigned long long l1GtTmAlgoCacheID =
            evSetup.get<L1GtTriggerMaskAlgoTrigRcd>().cacheIdentifier();

    if (m_l1GtTmAlgoCacheID != l1GtTmAlgoCacheID) {

        edm::ESHandle<L1GtTriggerMask> l1GtTmAlgo;
        evSetup.get<L1GtTriggerMaskAlgoTrigRcd>().get(l1GtTmAlgo);
        m_l1GtTmAlgo = l1GtTmAlgo.product();

        m_triggerMaskAlgoTrig = &(m_l1GtTmAlgo->gtTriggerMask());

        m_l1GtTmAlgoCacheID = l1GtTmAlgoCacheID;

    }

    unsigned long long l1GtTmTechCacheID =
            evSetup.get<L1GtTriggerMaskTechTrigRcd>().cacheIdentifier();

    if (m_l1GtTmTechCacheID != l1GtTmTechCacheID) {

        edm::ESHandle<L1GtTriggerMask> l1GtTmTech;
        evSetup.get<L1GtTriggerMaskTechTrigRcd>().get(l1GtTmTech);
        m_l1GtTmTech = l1GtTmTech.product();

        m_triggerMaskTechTrig = &(m_l1GtTmTech->gtTriggerMask());

        m_l1GtTmTechCacheID = l1GtTmTechCacheID;

    }

    unsigned long long l1GtTmVetoAlgoCacheID =
            evSetup.get<L1GtTriggerMaskVetoAlgoTrigRcd>().cacheIdentifier();

    if (m_l1GtTmVetoAlgoCacheID != l1GtTmVetoAlgoCacheID) {

        edm::ESHandle<L1GtTriggerMask> l1GtTmVetoAlgo;
        evSetup.get<L1GtTriggerMaskVetoAlgoTrigRcd>().get(l1GtTmVetoAlgo);
        m_l1GtTmVetoAlgo = l1GtTmVetoAlgo.product();

        m_triggerMaskVetoAlgoTrig = &(m_l1GtTmVetoAlgo->gtTriggerMask());

        m_l1GtTmVetoAlgoCacheID = l1GtTmVetoAlgoCacheID;

    }

    unsigned long long l1GtTmVetoTechCacheID =
            evSetup.get<L1GtTriggerMaskVetoTechTrigRcd>().cacheIdentifier();

    if (m_l1GtTmVetoTechCacheID != l1GtTmVetoTechCacheID) {

        edm::ESHandle<L1GtTriggerMask> l1GtTmVetoTech;
        evSetup.get<L1GtTriggerMaskVetoTechTrigRcd>().get(l1GtTmVetoTech);
        m_l1GtTmVetoTech = l1GtTmVetoTech.product();

        m_triggerMaskVetoTechTrig = &(m_l1GtTmVetoTech->gtTriggerMask());

        m_l1GtTmVetoTechCacheID = l1GtTmVetoTechCacheID;

    }

    // get / update the trigger menu from the EventSetup
    // local cache & check on cacheIdentifier

    unsigned long long l1GtMenuCacheID =
            evSetup.get<L1GtTriggerMenuRcd>().cacheIdentifier();

    if (m_l1GtMenuCacheID != l1GtMenuCacheID) {

        edm::ESHandle<L1GtTriggerMenu> l1GtMenu;
        evSetup.get<L1GtTriggerMenuRcd>().get(l1GtMenu);
        m_l1GtMenu = l1GtMenu.product();

        m_algorithmMap = &(m_l1GtMenu->gtAlgorithmMap());
        m_algorithmAliasMap = &(m_l1GtMenu->gtAlgorithmAliasMap());

        m_technicalTriggerMap = &(m_l1GtMenu->gtTechnicalTriggerMap());

        m_l1GtMenuCacheID = l1GtMenuCacheID;

    }

}

void L1GtUtils::retrieveL1GtTriggerMenuLite(const edm::Event& iEvent) {

    // cache the L1GtTriggerMenuLite

    const edm::Run& iRun = iEvent.getRun();
    const edm::RunID* runID = &(iRun.runAuxiliary().id());

    if (runID != m_provRunIDCache) {

        edm::InputTag l1GtTriggerMenuLiteInputTag;
        getL1GtTriggerMenuLiteInputTag(iEvent, l1GtTriggerMenuLiteInputTag);

        retrieveL1GtTriggerMenuLite(iEvent, l1GtTriggerMenuLiteInputTag);

        m_provRunIDCache = runID;

    }
}

void L1GtUtils::retrieveL1GtTriggerMenuLite(const edm::Event& iEvent,
        edm::InputTag& l1GtMenuLiteInputTag) {

    //
    m_retrieveL1GtTriggerMenuLite = true;

    // get Run Data - the same code can be run in beginRun, with getByLabel from edm::Run
    const edm::Run& iRun = iEvent.getRun();
    const edm::RunID* runID = &(iRun.runAuxiliary().id());

    if (runID != m_runIDCache) {

        // get L1GtTriggerMenuLite
        edm::Handle<L1GtTriggerMenuLite> l1GtMenuLite;
        iRun.getByLabel(l1GtMenuLiteInputTag, l1GtMenuLite);

        if (!l1GtMenuLite.isValid()) {

            LogDebug("L1GtUtils") << "\nL1GtTriggerMenuLite with \n  "
                    << l1GtMenuLiteInputTag
                    << "\nrequested in configuration, but not found in the event."
                    << std::endl;

            m_l1GtMenuLiteValid = false;
        } else {
            m_l1GtMenuLite = l1GtMenuLite.product();
            m_l1GtMenuLiteValid = true;

            LogDebug("L1GtUtils") << "\nL1GtTriggerMenuLite with \n  "
                    << l1GtMenuLiteInputTag << "\nretrieved for run "
                    << iRun.runAuxiliary().run() << std::endl;

            m_algorithmMapLite = &(m_l1GtMenuLite->gtAlgorithmMap());
            m_algorithmAliasMapLite = &(m_l1GtMenuLite->gtAlgorithmAliasMap());
            m_technicalTriggerMapLite
                    = &(m_l1GtMenuLite->gtTechnicalTriggerMap());

            m_triggerMaskAlgoTrigLite
                    = &(m_l1GtMenuLite->gtTriggerMaskAlgoTrig());
            m_triggerMaskTechTrigLite
                    = &(m_l1GtMenuLite->gtTriggerMaskTechTrig());

            m_prescaleFactorsAlgoTrigLite
                    = &(m_l1GtMenuLite->gtPrescaleFactorsAlgoTrig());
            m_prescaleFactorsTechTrigLite
                    = &(m_l1GtMenuLite->gtPrescaleFactorsTechTrig());

        }

        m_runIDCache = runID;
    }

}



void L1GtUtils::getInputTag(const edm::Event& iEvent,
        edm::InputTag& l1GtRecordInputTag,
        edm::InputTag& l1GtReadoutRecordInputTag) const {

    typedef std::vector<edm::Provenance const*> Provenances;
    Provenances provenances;
    std::string friendlyName;
    std::string modLabel;
    std::string instanceName;
    std::string processName;

    // to be sure that the input tags are correctly initialized
    edm::InputTag l1GtRecordInputTagVal;
    edm::InputTag l1GtReadoutRecordInputTagVal;

    bool foundL1GtRecord = false;
    bool foundL1GtReadoutRecord = false;

    //edm::LogVerbatim("L1GtUtils") << "\nTry to get AllProvenance for event "
    //        << iEvent.id().event() << std::endl;

    iEvent.getAllProvenance(provenances);

    //edm::LogVerbatim("L1GtUtils") << "\n" << "Event contains "
    //        << provenances.size() << " product" << (provenances.size()==1 ? "" : "s")
    //        << " with friendlyClassName, moduleLabel, productInstanceName and processName:"
    //        << std::endl;

    for (Provenances::iterator itProv = provenances.begin(), itProvEnd =
            provenances.end(); itProv != itProvEnd; ++itProv) {

        friendlyName = (*itProv)->friendlyClassName();
        modLabel = (*itProv)->moduleLabel();
        instanceName = (*itProv)->productInstanceName();
        processName = (*itProv)->processName();

        //edm::LogVerbatim("L1GtUtils") << friendlyName << " \"" << modLabel
        //        << "\" \"" << instanceName << "\" \"" << processName << "\""
        //        << std::endl;

        if (friendlyName == "L1GlobalTriggerRecord") {
            l1GtRecordInputTagVal = edm::InputTag(modLabel, instanceName,
                    processName);
            foundL1GtRecord = true;
        } else if (friendlyName == "L1GlobalTriggerReadoutRecord") {

            l1GtReadoutRecordInputTagVal = edm::InputTag(modLabel, instanceName,
                    processName);
            foundL1GtReadoutRecord = true;
        }
    }

    // copy the input tags found to the returned arguments
    l1GtRecordInputTag = l1GtRecordInputTagVal;
    l1GtReadoutRecordInputTag = l1GtReadoutRecordInputTagVal;

    //if (foundL1GtRecord) {
    //    edm::LogVerbatim("L1GtUtils")
    //            << "\nL1GlobalTriggerRecord found in the event with \n  "
    //            << l1GtRecordInputTag << std::endl;
    //
    //}

    //if (foundL1GtReadoutRecord) {
    //    edm::LogVerbatim("L1GtUtils")
    //            << "\nL1GlobalTriggerReadoutRecord found in the event with \n  "
    //           << l1GtReadoutRecordInputTag << std::endl;
    //}

}

void L1GtUtils::getL1GtTriggerMenuLiteInputTag(const edm::Event& iEvent,
        edm::InputTag& l1GtTriggerMenuLiteInputTag) const {

    typedef std::vector<edm::Provenance const*> Provenances;
    Provenances provenances;
    std::string friendlyName;
    std::string modLabel;
    std::string instanceName;
    std::string processName;

    // to be sure that the input tag is correctly initialized
    edm::InputTag l1GtTriggerMenuLiteInputTagVal;
    bool foundL1GtTriggerMenuLite = false;

    // get Run Data
    const edm::Run& iRun = iEvent.getRun();

    //edm::LogVerbatim("L1GtUtils") << "\nTry to get AllProvenance for run "
    //        << iRun.runAuxiliary().run() << " event " << iEvent.id().event()
    //        << std::endl;

    iRun.getAllProvenance(provenances);

    //edm::LogVerbatim("L1GtUtils") << "\n" << "Run contains "
    //        << provenances.size() << " product"
    //        << (provenances.size() == 1 ? "" : "s")
    //        << " with friendlyClassName, moduleLabel, productInstanceName and processName:"
    //        << std::endl;

    for (Provenances::iterator itProv = provenances.begin(), itProvEnd =
            provenances.end(); itProv != itProvEnd; ++itProv) {

        friendlyName = (*itProv)->friendlyClassName();
        modLabel = (*itProv)->moduleLabel();
        instanceName = (*itProv)->productInstanceName();
        processName = (*itProv)->processName();

        //edm::LogVerbatim("L1GtUtils") << friendlyName << " \"" << modLabel
        //        << "\" \"" << instanceName << "\" \"" << processName << "\""
        //        << std::endl;

        if (friendlyName == "L1GtTriggerMenuLite") {
            l1GtTriggerMenuLiteInputTagVal = edm::InputTag(modLabel,
                    instanceName, processName);
            foundL1GtTriggerMenuLite = true;
        }

    }

    // copy the input tags found to the returned arguments
    l1GtTriggerMenuLiteInputTag = l1GtTriggerMenuLiteInputTagVal;

    //if (foundL1GtTriggerMenuLite) {
    //    edm::LogVerbatim("L1GtUtils")
    //            << "\nL1GtTriggerMenuLite found in the event with \n  "
    //            << l1GtTriggerMenuLiteInputTag << std::endl;
    //}

}

const bool L1GtUtils::l1AlgoTechTrigBitNumber(
        const std::string& nameAlgoTechTrig, TriggerCategory& trigCategory,
        int& bitNumber) const {

    trigCategory = AlgorithmTrigger;
    bitNumber = -1;

    if (m_retrieveL1GtTriggerMenuLite) {
        if (m_l1GtMenuLiteValid) {

            // test if the name is an algorithm alias
            for (L1GtTriggerMenuLite::CItL1Trig itTrig =
                    m_algorithmAliasMapLite->begin(); itTrig
                    != m_algorithmAliasMapLite->end(); itTrig++) {

                if (itTrig->second == nameAlgoTechTrig) {

                    trigCategory = AlgorithmTrigger;
                    bitNumber = itTrig->first;

                    return true;
                }
            }

            // test if the name is an algorithm name
            for (L1GtTriggerMenuLite::CItL1Trig itTrig =
                    m_algorithmMapLite->begin(); itTrig
                    != m_algorithmMapLite->end(); itTrig++) {

                if (itTrig->second == nameAlgoTechTrig) {

                    trigCategory = AlgorithmTrigger;
                    bitNumber = itTrig->first;

                    return true;
                }
            }

            // test if the name is a technical trigger
            for (L1GtTriggerMenuLite::CItL1Trig itTrig =
                    m_technicalTriggerMapLite->begin(); itTrig
                    != m_technicalTriggerMapLite->end(); itTrig++) {

                if (itTrig->second == nameAlgoTechTrig) {

                    trigCategory = TechnicalTrigger;
                    bitNumber = itTrig->first;

                    return true;
                }
            }

        } else if (m_retrieveL1EventSetup) {

            // test if the name is an algorithm alias
            CItAlgo itAlgo = m_algorithmAliasMap->find(nameAlgoTechTrig);
            if (itAlgo != m_algorithmAliasMap->end()) {
                trigCategory = AlgorithmTrigger;
                bitNumber = (itAlgo->second).algoBitNumber();

                return true;
            }

            // test if the name is an algorithm name
            itAlgo = m_algorithmMap->find(nameAlgoTechTrig);
            if (itAlgo != m_algorithmMap->end()) {
                trigCategory = AlgorithmTrigger;
                bitNumber = (itAlgo->second).algoBitNumber();

                return true;
            }

            // test if the name is a technical trigger
            itAlgo = m_technicalTriggerMap->find(nameAlgoTechTrig);
            if (itAlgo != m_technicalTriggerMap->end()) {
                trigCategory = TechnicalTrigger;
                bitNumber = (itAlgo->second).algoBitNumber();

                return true;
            }

        } else {
            // only L1GtTriggerMenuLite requested, but it is not valid
            return false;

        }
    } else if (m_retrieveL1EventSetup) {

        // test if the name is an algorithm alias
        CItAlgo itAlgo = m_algorithmAliasMap->find(nameAlgoTechTrig);
        if (itAlgo != m_algorithmAliasMap->end()) {
            trigCategory = AlgorithmTrigger;
            bitNumber = (itAlgo->second).algoBitNumber();

            return true;
        }

        // test if the name is an algorithm name
        itAlgo = m_algorithmMap->find(nameAlgoTechTrig);
        if (itAlgo != m_algorithmMap->end()) {
            trigCategory = AlgorithmTrigger;
            bitNumber = (itAlgo->second).algoBitNumber();

            return true;
        }

        // test if the name is a technical trigger
        itAlgo = m_technicalTriggerMap->find(nameAlgoTechTrig);
        if (itAlgo != m_technicalTriggerMap->end()) {
            trigCategory = TechnicalTrigger;
            bitNumber = (itAlgo->second).algoBitNumber();

            return true;
        }

    } else {
        // L1 trigger configuration not retrieved
        return false;

    }

    // all possibilities already tested, so it should not arrive here
    return false;


}

// deprecated
const bool L1GtUtils::l1AlgTechTrigBitNumber(
        const std::string& nameAlgoTechTrig, int& triggerAlgoTechTrig,
        int& bitNumber) const {

    TriggerCategory trigCategory = AlgorithmTrigger;
    bitNumber = -1;

    const bool trigCategBitNr = l1AlgoTechTrigBitNumber(nameAlgoTechTrig,
            trigCategory, bitNumber);
    triggerAlgoTechTrig = trigCategory;

    return trigCategBitNr;

}


const int L1GtUtils::l1Results(const edm::Event& iEvent,
        const edm::InputTag& l1GtRecordInputTag,
        const edm::InputTag& l1GtReadoutRecordInputTag,
        const std::string& nameAlgoTechTrig, bool& decisionBeforeMask,
        bool& decisionAfterMask, int& prescaleFactor, int& triggerMask) const {

    // initial values for returned results
    decisionBeforeMask = false;
    decisionAfterMask = false;
    prescaleFactor = -1;
    triggerMask = -1;

    // initialize error code and L1 configuration code
    int iError = 0;
    int l1ConfCode = 0;

    // check if L1 configuration is available

    if (!availableL1Configuration(iError, l1ConfCode)) {
        return iError;
    }

    // at this point, a valid L1 configuration is available, so the if/else if/else
    // can be simplified

    // if the given name is not an algorithm trigger alias, an algorithm trigger name
    // or a technical trigger in the current menu, return with error code 1

    TriggerCategory trigCategory = AlgorithmTrigger;
    int bitNumber = -1;


    if (!l1AlgoTechTrigBitNumber(nameAlgoTechTrig, trigCategory, bitNumber)) {

        iError = l1ConfCode + 1;

        if (m_retrieveL1GtTriggerMenuLite) {
            if (m_l1GtMenuLiteValid) {

                LogDebug("L1GtUtils") << "\nAlgorithm/technical trigger \n  "
                        << nameAlgoTechTrig
                        << "\not found in the trigger menu \n  "
                        << m_l1GtMenuLite->gtTriggerMenuImplementation()
                        << "\nretrieved from L1GtTriggerMenuLite" << std::endl;

            } else {

                // fall through: L1 trigger configuration from event setup
                LogDebug("L1GtUtils") << "\nAlgorithm/technical trigger \n  "
                        << nameAlgoTechTrig
                        << "\not found in the trigger menu \n  "
                        << m_l1GtMenu->gtTriggerMenuImplementation()
                        << "\nretrieved from Event Setup" << std::endl;

            }

        } else {
            // L1 trigger configuration from event setup only
            LogDebug("L1GtUtils") << "\nAlgorithm/technical trigger \n  "
                    << nameAlgoTechTrig
                    << "\not found in the trigger menu \n  "
                    << m_l1GtMenu->gtTriggerMenuImplementation()
                    << "\nretrieved from Event Setup" << std::endl;

        }

        return iError;

    }

    // check here if a positive bit number was retrieved
    // exit in case of negative bit number, before retrieving L1 GT products, saving time

    if (bitNumber < 0) {

        iError = l1ConfCode + 2;

        if (m_retrieveL1GtTriggerMenuLite) {
            if (m_l1GtMenuLiteValid) {
                LogDebug("L1GtUtils") << "\nNegative bit number for "
                        << triggerCategory(trigCategory) << "\n  "
                        << nameAlgoTechTrig << "\nfrom menu \n  "
                        << m_l1GtMenuLite->gtTriggerMenuImplementation()
                        << "\nretrieved from L1GtTriggerMenuLite" << std::endl;

            } else {
                // fall through: L1 trigger configuration from event setup
                LogDebug("L1GtUtils") << "\nNegative bit number for "
                        << triggerCategory(trigCategory) << "\n  "
                        << nameAlgoTechTrig << "\nfrom menu \n  "
                        << m_l1GtMenu->gtTriggerMenuImplementation()
                        << "\nretrieved from Event Setup" << std::endl;

            }

        } else {
            // L1 trigger configuration from event setup only
            LogDebug("L1GtUtils") << "\nNegative bit number for "
                    << triggerCategory(trigCategory) << "\n  "
                    << nameAlgoTechTrig << "\nfrom menu \n  "
                    << m_l1GtMenu->gtTriggerMenuImplementation()
                    << "\nretrieved from Event Setup" << std::endl;

        }

        return iError;
    }


    // retrieve L1GlobalTriggerRecord and 1GlobalTriggerReadoutRecord product
    // intermediate error code for the records
    // the module returns an error code only if both the lite and the readout record are missing

    int iErrorRecord = 0;

    bool validRecord = false;
    bool gtRecordValid = false;
    bool gtReadoutRecordValid = false;

    edm::Handle<L1GlobalTriggerRecord> gtRecord;
    iEvent.getByLabel(l1GtRecordInputTag, gtRecord);

    if (gtRecord.isValid()) {

        gtRecordValid = true;
        validRecord = true;

    } else {

        iErrorRecord = 10;
        LogDebug("L1GtUtils") << "\nL1GlobalTriggerRecord with \n  "
                << l1GtRecordInputTag << "\nnot found in the event."
                << std::endl;
    }

    edm::Handle<L1GlobalTriggerReadoutRecord> gtReadoutRecord;
    iEvent.getByLabel(l1GtReadoutRecordInputTag, gtReadoutRecord);

    if (gtReadoutRecord.isValid()) {

        gtReadoutRecordValid = true;
        validRecord = true;

    } else {

        iErrorRecord = iErrorRecord + 100;
        LogDebug("L1GtUtils") << "\nL1GlobalTriggerReadoutRecord with \n  "
                << l1GtReadoutRecordInputTag << "\nnot found in the event."
                << std::endl;

    }

    // get the prescale factor index from
    //  L1GlobalTriggerReadoutRecord if valid
    //  if not, from L1GlobalTriggerRecord if valid
    //  else return an error


    int pfIndexTechTrig = -1;
    int pfIndexAlgoTrig = -1;

    if (validRecord) {
        if (gtReadoutRecordValid) {

            pfIndexTechTrig
                    = (gtReadoutRecord->gtFdlWord()).gtPrescaleFactorIndexTech();
            pfIndexAlgoTrig
                    = (gtReadoutRecord->gtFdlWord()).gtPrescaleFactorIndexAlgo();

        } else {

            pfIndexTechTrig
                    = static_cast<int> (gtRecord->gtPrescaleFactorIndexTech());
            pfIndexAlgoTrig
                    = static_cast<int> (gtRecord->gtPrescaleFactorIndexAlgo());

        }

    } else {

        LogDebug("L1GtUtils") << "\nError: "
                << "\nNo valid L1GlobalTriggerRecord with \n  "
                << l1GtRecordInputTag << "\nfound in the event."
                << "\nNo valid L1GlobalTriggerReadoutRecord with \n  "
                << l1GtReadoutRecordInputTag << "\nfound in the event."
                << std::endl;

        iError = l1ConfCode + iErrorRecord;
        return iError;

    }

    // depending on trigger category (algorithm trigger or technical trigger)
    // get the correct quantities

    // number of sets of prescale factors
    // index of prescale factor set retrieved from data
    // pointer to the actual prescale factor set
    // pointer to the set of trigger masks

    size_t pfSetsSize = 0;
    int pfIndex = -1;
    const std::vector<int>* prescaleFactorsSubset = 0;
    const std::vector<unsigned int>* triggerMasksSet = 0;

    switch (trigCategory) {
        case AlgorithmTrigger: {
            if (m_retrieveL1GtTriggerMenuLite) {
                if (m_l1GtMenuLiteValid) {
                    pfSetsSize = m_prescaleFactorsAlgoTrigLite->size();
                    triggerMasksSet = m_triggerMaskAlgoTrigLite;

                } else {
                    // fall through: L1 trigger configuration from event setup
                    pfSetsSize = m_prescaleFactorsAlgoTrig->size();
                    triggerMasksSet = m_triggerMaskAlgoTrig;

                }

            } else {
                // L1 trigger configuration from event setup only
                pfSetsSize = m_prescaleFactorsAlgoTrig->size();
                triggerMasksSet = m_triggerMaskAlgoTrig;

            }

            pfIndex = pfIndexAlgoTrig;

        }
            break;
        case TechnicalTrigger: {
            if (m_retrieveL1GtTriggerMenuLite) {
                if (m_l1GtMenuLiteValid) {
                    pfSetsSize = m_prescaleFactorsTechTrigLite->size();
                    triggerMasksSet = m_triggerMaskTechTrigLite;

                } else {
                    // fall through: L1 trigger configuration from event setup
                    pfSetsSize = m_prescaleFactorsTechTrig->size();
                    triggerMasksSet = m_triggerMaskTechTrig;

                }

            } else {
                // L1 trigger configuration from event setup only
                pfSetsSize = m_prescaleFactorsTechTrig->size();
                triggerMasksSet = m_triggerMaskTechTrig;

            }

            pfIndex = pfIndexTechTrig;

        }
            break;
        default: {
            // should not be the case
            iError = l1ConfCode + iErrorRecord + 3;
            return iError;

        }
            break;
    }


    // test prescale factor set index correctness, then retrieve the actual set of prescale factors

    if (pfIndex < 0) {

        iError = l1ConfCode + iErrorRecord + 1000;
        LogDebug("L1GtUtils")
                << "\nError: index of prescale factor set retrieved from the data \n"
                << "less than zero."
                << "\n  Value of index retrieved from data = " << pfIndex
                << std::endl;

        return iError;

    } else if (pfIndex >= (static_cast<int>(pfSetsSize))) {
        iError = l1ConfCode + iErrorRecord + 2000;
        LogDebug("L1GtUtils")
                << "\nError: index of prescale factor set retrieved from the data \n"
                << "greater than the size of the vector of prescale factor sets."
                << "\n  Value of index retrieved from data = " << pfIndex
                << "\n  Vector size = " << pfSetsSize << std::endl;

        return iError;

    } else {
        switch (trigCategory) {
            case AlgorithmTrigger: {
                if (m_retrieveL1GtTriggerMenuLite) {
                    if (m_l1GtMenuLiteValid) {
                        prescaleFactorsSubset
                                = &((*m_prescaleFactorsAlgoTrigLite).at(pfIndex));

                    } else {
                        // fall through: L1 trigger configuration from event setup
                        prescaleFactorsSubset
                                = &((*m_prescaleFactorsAlgoTrig).at(pfIndex));

                    }

                } else {
                    // L1 trigger configuration from event setup only
                    prescaleFactorsSubset
                            = &((*m_prescaleFactorsAlgoTrig).at(pfIndex));

                }

            }
                break;
            case TechnicalTrigger: {
                if (m_retrieveL1GtTriggerMenuLite) {
                    if (m_l1GtMenuLiteValid) {
                        prescaleFactorsSubset
                                = &((*m_prescaleFactorsTechTrigLite).at(pfIndex));

                    } else {
                        // fall through: L1 trigger configuration from event setup
                        prescaleFactorsSubset
                                = &((*m_prescaleFactorsTechTrig).at(pfIndex));

                    }

                } else {
                    // L1 trigger configuration from event setup only
                    prescaleFactorsSubset
                            = &((*m_prescaleFactorsTechTrig).at(pfIndex));

                }

            }
                break;
            default: {
                // do nothing - it was tested before, with return

            }
                break;
        }

    }


    // algorithm result before applying the trigger masks
    // the bit number is positive (tested previously)

    switch (trigCategory) {
        case AlgorithmTrigger: {
            if (gtReadoutRecordValid) {
                const DecisionWord& decWord = gtReadoutRecord->decisionWord();
                decisionBeforeMask = trigResult(decWord, bitNumber,
                        nameAlgoTechTrig, trigCategory, iError);
                if (iError) {
                    return (iError + l1ConfCode + iErrorRecord);
                }

            } else {

                const DecisionWord& decWord =
                        gtRecord->decisionWordBeforeMask();
                decisionBeforeMask = trigResult(decWord, bitNumber,
                        nameAlgoTechTrig, trigCategory, iError);
                if (iError) {
                    return (iError + l1ConfCode + iErrorRecord);
                }

            }

        }
            break;
        case TechnicalTrigger: {
            if (gtReadoutRecordValid) {
                const DecisionWord& decWord =
                        gtReadoutRecord->technicalTriggerWord();
                decisionBeforeMask = trigResult(decWord, bitNumber,
                        nameAlgoTechTrig, trigCategory, iError);
                if (iError) {
                    return (iError + l1ConfCode + iErrorRecord);
                }

            } else {

                const DecisionWord& decWord =
                        gtRecord->technicalTriggerWordBeforeMask();
                decisionBeforeMask = trigResult(decWord, bitNumber,
                        nameAlgoTechTrig, trigCategory, iError);
                if (iError) {
                    return (iError + l1ConfCode + iErrorRecord);
                }

            }

        }
            break;
        default: {
            // do nothing - it was tested before, with return

        }
            break;
    }

    // prescale factor
    // the bit number is positive (tested previously)

    if (bitNumber < (static_cast<int> (prescaleFactorsSubset->size()))) {
        prescaleFactor = (*prescaleFactorsSubset)[bitNumber];
    } else {
        iError = l1ConfCode + iErrorRecord + 4000;
        LogDebug("L1GtUtils") << "\nError: bit number " << bitNumber
                << " retrieved for " << triggerCategory(trigCategory) << "\n  "
                << nameAlgoTechTrig
                << "\ngreater than size of actual L1 GT prescale factor set: "
                << prescaleFactorsSubset->size()
                << "\nError: Inconsistent L1 trigger configuration!"
                << std::endl;

        return iError;
    }

    // trigger mask and trigger result after applying the trigger masks

    if (bitNumber < (static_cast<int> ((*triggerMasksSet).size()))) {

        if (m_retrieveL1GtTriggerMenuLite) {
            if (m_l1GtMenuLiteValid) {
                triggerMask = (*triggerMasksSet)[bitNumber];

            } else {
                // fall through: L1 trigger configuration from event setup
                // masks in event setup are for all partitions
                triggerMask = ((*triggerMasksSet)[bitNumber]) & (1
                        << m_physicsDaqPartition);

            }

        } else {
            // L1 trigger configuration from event setup only
            // masks in event setup are for all partitions
            triggerMask = ((*triggerMasksSet)[bitNumber]) & (1
                    << m_physicsDaqPartition);

        }


    } else {
        iError = l1ConfCode + iErrorRecord + 5000;
        LogDebug("L1GtUtils") << "\nError: bit number " << bitNumber
                << " retrieved for " << triggerCategory(trigCategory) << "\n  "
                << nameAlgoTechTrig
                << "\ngreater than size of L1 GT trigger mask set: "
                << (*triggerMasksSet).size()
                << "\nError: Inconsistent L1 trigger configuration!"
                << std::endl;

        return iError;

    }

    decisionAfterMask = decisionBeforeMask;

    if (triggerMask) {
        decisionAfterMask = false;
    }

    return iError;

}


const int L1GtUtils::l1Results(const edm::Event& iEvent,
        const std::string& nameAlgoTechTrig, bool& decisionBeforeMask,
        bool& decisionAfterMask, int& prescaleFactor, int& triggerMask) const {

    edm::InputTag l1GtRecordInputTag;
    edm::InputTag l1GtReadoutRecordInputTag;

    // initial values for returned results
    decisionBeforeMask = false;
    decisionAfterMask = false;
    prescaleFactor = -1;
    triggerMask = -1;

    getInputTag(iEvent, l1GtRecordInputTag, l1GtReadoutRecordInputTag);

    int l1ErrorCode = 0;

    l1ErrorCode = l1Results(iEvent, l1GtRecordInputTag,
            l1GtReadoutRecordInputTag, nameAlgoTechTrig, decisionBeforeMask,
            decisionAfterMask, prescaleFactor, triggerMask);

    return l1ErrorCode;

}

//

const bool L1GtUtils::decisionBeforeMask(const edm::Event& iEvent,
        const edm::InputTag& l1GtRecordInputTag,
        const edm::InputTag& l1GtReadoutRecordInputTag,
        const std::string& nameAlgoTechTrig, int& errorCode) const {

    // initial values
    bool decisionBeforeMask = false;
    bool decisionAfterMask = false;
    int prescaleFactor = -1;
    int triggerMask = -1;

    errorCode = l1Results(iEvent, l1GtRecordInputTag,
            l1GtReadoutRecordInputTag, nameAlgoTechTrig, decisionBeforeMask,
            decisionAfterMask, prescaleFactor, triggerMask);

    return decisionBeforeMask;

}

const bool L1GtUtils::decisionBeforeMask(const edm::Event& iEvent,
        const std::string& nameAlgoTechTrig, int& errorCode) const {

    // initial values
    bool decisionBeforeMask = false;
    bool decisionAfterMask = false;
    int prescaleFactor = -1;
    int triggerMask = -1;

    errorCode = l1Results(iEvent, nameAlgoTechTrig, decisionBeforeMask,
            decisionAfterMask, prescaleFactor, triggerMask);

    return decisionBeforeMask;

}

//

const bool L1GtUtils::decisionAfterMask(const edm::Event& iEvent,
        const edm::InputTag& l1GtRecordInputTag,
        const edm::InputTag& l1GtReadoutRecordInputTag,
        const std::string& nameAlgoTechTrig, int& errorCode) const {

    // initial values
    bool decisionBeforeMask = false;
    bool decisionAfterMask = false;
    int prescaleFactor = -1;
    int triggerMask = -1;

    errorCode = l1Results(iEvent, l1GtRecordInputTag,
            l1GtReadoutRecordInputTag, nameAlgoTechTrig, decisionBeforeMask,
            decisionAfterMask, prescaleFactor, triggerMask);

    return decisionAfterMask;

}

const bool L1GtUtils::decisionAfterMask(const edm::Event& iEvent,
        const std::string& nameAlgoTechTrig, int& errorCode) const {

    // initial values
    bool decisionBeforeMask = false;
    bool decisionAfterMask = false;
    int prescaleFactor = -1;
    int triggerMask = -1;

    errorCode = l1Results(iEvent, nameAlgoTechTrig, decisionBeforeMask,
            decisionAfterMask, prescaleFactor, triggerMask);

    return decisionAfterMask;

}

//

const bool L1GtUtils::decision(const edm::Event& iEvent,
        const edm::InputTag& l1GtRecordInputTag,
        const edm::InputTag& l1GtReadoutRecordInputTag,
        const std::string& nameAlgoTechTrig, int& errorCode) const {

    // initial values
    bool decisionBeforeMask = false;
    bool decisionAfterMask = false;
    int prescaleFactor = -1;
    int triggerMask = -1;

    errorCode = l1Results(iEvent, l1GtRecordInputTag,
            l1GtReadoutRecordInputTag, nameAlgoTechTrig, decisionBeforeMask,
            decisionAfterMask, prescaleFactor, triggerMask);

    return decisionAfterMask;

}

const bool L1GtUtils::decision(const edm::Event& iEvent,
        const std::string& nameAlgoTechTrig, int& errorCode) const {

    // initial values
    bool decisionBeforeMask = false;
    bool decisionAfterMask = false;
    int prescaleFactor = -1;
    int triggerMask = -1;

    errorCode = l1Results(iEvent, nameAlgoTechTrig, decisionBeforeMask,
            decisionAfterMask, prescaleFactor, triggerMask);

    return decisionAfterMask;

}

//

const int L1GtUtils::prescaleFactor(const edm::Event& iEvent,
        const edm::InputTag& l1GtRecordInputTag,
        const edm::InputTag& l1GtReadoutRecordInputTag,
        const std::string& nameAlgoTechTrig, int& errorCode) const {

    // initial values
    bool decisionBeforeMask = false;
    bool decisionAfterMask = false;
    int prescaleFactor = -1;
    int triggerMask = -1;

    errorCode = l1Results(iEvent, l1GtRecordInputTag,
            l1GtReadoutRecordInputTag, nameAlgoTechTrig, decisionBeforeMask,
            decisionAfterMask, prescaleFactor, triggerMask);

    return prescaleFactor;

}

const int L1GtUtils::prescaleFactor(const edm::Event& iEvent,
        const std::string& nameAlgoTechTrig, int& errorCode) const {

    // initial values
    bool decisionBeforeMask = false;
    bool decisionAfterMask = false;
    int prescaleFactor = -1;
    int triggerMask = -1;

    errorCode = l1Results(iEvent, nameAlgoTechTrig, decisionBeforeMask,
            decisionAfterMask, prescaleFactor, triggerMask);

    return prescaleFactor;

}

const int L1GtUtils::triggerMask(const edm::Event& iEvent,
        const edm::InputTag& l1GtRecordInputTag,
        const edm::InputTag& l1GtReadoutRecordInputTag,
        const std::string& nameAlgoTechTrig, int& errorCode) const {

    // initial values
    bool decisionBeforeMask = false;
    bool decisionAfterMask = false;
    int prescaleFactor = -1;
    int triggerMask = -1;

    errorCode = l1Results(iEvent, l1GtRecordInputTag,
            l1GtReadoutRecordInputTag, nameAlgoTechTrig, decisionBeforeMask,
            decisionAfterMask, prescaleFactor, triggerMask);

    return triggerMask;

}

const int L1GtUtils::triggerMask(const edm::Event& iEvent,
        const std::string& nameAlgoTechTrig, int& errorCode) const {

    // initial values
    bool decisionBeforeMask = false;
    bool decisionAfterMask = false;
    int prescaleFactor = -1;
    int triggerMask = -1;

    errorCode = l1Results(iEvent, nameAlgoTechTrig, decisionBeforeMask,
            decisionAfterMask, prescaleFactor, triggerMask);

    return triggerMask;

}

const int L1GtUtils::triggerMask(const std::string& nameAlgoTechTrig,
        int& errorCode) const {

    // initial values for returned results
    int triggerMaskValue = -1;

    // initialize error code and L1 configuration code
    int iError = 0;
    int l1ConfCode = 0;

    // check if L1 configuration is available

    if (!availableL1Configuration(iError, l1ConfCode)) {
        errorCode = iError;
        return triggerMaskValue;
    }

    // at this point, a valid L1 configuration is available, so the if/else if/else
    // can be simplified

    // if the given name is not an algorithm trigger alias, an algorithm trigger name
    // or a technical trigger in the current menu, return with error code 1

    TriggerCategory trigCategory = AlgorithmTrigger;
    int bitNumber = -1;

    if (!l1AlgoTechTrigBitNumber(nameAlgoTechTrig, trigCategory, bitNumber)) {

        iError = l1ConfCode + 1;

        if (m_retrieveL1GtTriggerMenuLite) {
            if (m_l1GtMenuLiteValid) {

                LogDebug("L1GtUtils") << "\nAlgorithm/technical trigger \n  "
                        << nameAlgoTechTrig
                        << "\not found in the trigger menu \n  "
                        << m_l1GtMenuLite->gtTriggerMenuImplementation()
                        << "\nretrieved from L1GtTriggerMenuLite" << std::endl;

            } else {

                // fall through: L1 trigger configuration from event setup
                LogDebug("L1GtUtils") << "\nAlgorithm/technical trigger \n  "
                        << nameAlgoTechTrig
                        << "\not found in the trigger menu \n  "
                        << m_l1GtMenu->gtTriggerMenuImplementation()
                        << "\nretrieved from Event Setup" << std::endl;

            }

        } else {
            // L1 trigger configuration from event setup only
            LogDebug("L1GtUtils") << "\nAlgorithm/technical trigger \n  "
                    << nameAlgoTechTrig
                    << "\not found in the trigger menu \n  "
                    << m_l1GtMenu->gtTriggerMenuImplementation()
                    << "\nretrieved from Event Setup" << std::endl;

        }

        errorCode = iError;
        return triggerMaskValue;

    }

    // check here if a positive bit number was retrieved
    // exit in case of negative bit number, before retrieving L1 GT products, saving time

    if (bitNumber < 0) {

        iError = l1ConfCode + 2;

        if (m_retrieveL1GtTriggerMenuLite) {
            if (m_l1GtMenuLiteValid) {
                LogDebug("L1GtUtils") << "\nNegative bit number for "
                        << triggerCategory(trigCategory) << "\n  "
                        << nameAlgoTechTrig << "\nfrom menu \n  "
                        << m_l1GtMenuLite->gtTriggerMenuImplementation()
                        << "\nretrieved from L1GtTriggerMenuLite" << std::endl;

            } else {
                // fall through: L1 trigger configuration from event setup
                LogDebug("L1GtUtils") << "\nNegative bit number for "
                        << triggerCategory(trigCategory) << "\n  "
                        << nameAlgoTechTrig << "\nfrom menu \n  "
                        << m_l1GtMenu->gtTriggerMenuImplementation()
                        << "\nretrieved from Event Setup" << std::endl;

            }

        } else {
            // L1 trigger configuration from event setup only
            LogDebug("L1GtUtils") << "\nNegative bit number for "
                    << triggerCategory(trigCategory) << "\n  "
                    << nameAlgoTechTrig << "\nfrom menu \n  "
                    << m_l1GtMenu->gtTriggerMenuImplementation()
                    << "\nretrieved from Event Setup" << std::endl;

        }

        errorCode = iError;
        return triggerMaskValue;
    }

    // depending on trigger category (algorithm trigger or technical trigger)
    // get the correct quantities

    // pointer to the set of trigger masks

    const std::vector<unsigned int>* triggerMasksSet = 0;

    switch (trigCategory) {
        case AlgorithmTrigger: {
            if (m_retrieveL1GtTriggerMenuLite) {
                if (m_l1GtMenuLiteValid) {
                    triggerMasksSet = m_triggerMaskAlgoTrigLite;

                } else {
                    // fall through: L1 trigger configuration from event setup
                    triggerMasksSet = m_triggerMaskAlgoTrig;

                }

            } else {
                // L1 trigger configuration from event setup only
                triggerMasksSet = m_triggerMaskAlgoTrig;

            }

        }
            break;
        case TechnicalTrigger: {
            if (m_retrieveL1GtTriggerMenuLite) {
                if (m_l1GtMenuLiteValid) {
                    triggerMasksSet = m_triggerMaskTechTrigLite;

                } else {
                    // fall through: L1 trigger configuration from event setup
                    triggerMasksSet = m_triggerMaskTechTrig;

                }

            } else {
                // L1 trigger configuration from event setup only
                triggerMasksSet = m_triggerMaskTechTrig;

            }

        }
            break;
        default: {
            // should not be the case
            iError = l1ConfCode + 3;

            errorCode = iError;
            return triggerMaskValue;

        }
            break;
    }

    // trigger mask

    if (bitNumber < (static_cast<int> ((*triggerMasksSet).size()))) {

        if (m_retrieveL1GtTriggerMenuLite) {
            if (m_l1GtMenuLiteValid) {
                triggerMaskValue = (*triggerMasksSet)[bitNumber];

            } else {
                // fall through: L1 trigger configuration from event setup
                // masks in event setup are for all partitions
                triggerMaskValue = ((*triggerMasksSet)[bitNumber]) & (1
                        << m_physicsDaqPartition);

            }

        } else {
            // L1 trigger configuration from event setup only
            // masks in event setup are for all partitions
            triggerMaskValue = ((*triggerMasksSet)[bitNumber]) & (1
                    << m_physicsDaqPartition);

        }

    } else {
        iError = l1ConfCode + 5000;
        LogDebug("L1GtUtils") << "\nError: bit number " << bitNumber
                << " retrieved for " << triggerCategory(trigCategory) << "\n  "
                << nameAlgoTechTrig
                << "\ngreater than size of L1 GT trigger mask set: "
                << (*triggerMasksSet).size()
                << "\nError: Inconsistent L1 trigger configuration!"
                << std::endl;

        errorCode = iError;
        return triggerMaskValue;

    }

    errorCode = iError;
    return triggerMaskValue;

}

const int L1GtUtils::prescaleFactorSetIndex(const edm::Event& iEvent,
        const edm::InputTag& l1GtRecordInputTag,
        const edm::InputTag& l1GtReadoutRecordInputTag,
        const TriggerCategory& trigCategory, int& errorCode) const {

    // initialize the index to a negative value
    int pfIndex = -1;

    // initialize error code and L1 configuration code
    int iError = 0;
    int l1ConfCode = 0;

    // check if L1 configuration is available

    if (!availableL1Configuration(iError, l1ConfCode)) {
        errorCode = iError;
        return pfIndex;
    }

    // at this point, a valid L1 configuration is available, so the if/else if/else
    // can be simplified

    // retrieve L1GlobalTriggerRecord and 1GlobalTriggerReadoutRecord product
    // intermediate error code for the records
    // the module returns an error code only if both the lite and the readout record are missing

    int iErrorRecord = 0;

    bool validRecord = false;
    bool gtRecordValid = false;
    bool gtReadoutRecordValid = false;

    edm::Handle<L1GlobalTriggerRecord> gtRecord;
    iEvent.getByLabel(l1GtRecordInputTag, gtRecord);

    if (gtRecord.isValid()) {

        gtRecordValid = true;
        validRecord = true;

    } else {

        iErrorRecord = 10;
        LogDebug("L1GtUtils") << "\nL1GlobalTriggerRecord with \n  "
                << l1GtRecordInputTag << "\nnot found in the event."
                << std::endl;
    }

    edm::Handle<L1GlobalTriggerReadoutRecord> gtReadoutRecord;
    iEvent.getByLabel(l1GtReadoutRecordInputTag, gtReadoutRecord);

    if (gtReadoutRecord.isValid()) {

        gtReadoutRecordValid = true;
        validRecord = true;

    } else {

        iErrorRecord = iErrorRecord + 100;
        LogDebug("L1GtUtils") << "\nL1GlobalTriggerReadoutRecord with \n  "
                << l1GtReadoutRecordInputTag << "\nnot found in the event."
                << std::endl;

    }

    // get the prescale factor index from
    //  L1GlobalTriggerReadoutRecord if valid
    //  if not, from L1GlobalTriggerRecord if valid
    //  else return an error


    int pfIndexTechTrig = -1;
    int pfIndexAlgoTrig = -1;

    if (validRecord) {
        if (gtReadoutRecordValid) {

            pfIndexTechTrig
                    = (gtReadoutRecord->gtFdlWord()).gtPrescaleFactorIndexTech();
            pfIndexAlgoTrig
                    = (gtReadoutRecord->gtFdlWord()).gtPrescaleFactorIndexAlgo();

        } else {

            pfIndexTechTrig
                    = static_cast<int> (gtRecord->gtPrescaleFactorIndexTech());
            pfIndexAlgoTrig
                    = static_cast<int> (gtRecord->gtPrescaleFactorIndexAlgo());

        }

    } else {

        LogDebug("L1GtUtils") << "\nError: "
                << "\nNo valid L1GlobalTriggerRecord with \n  "
                << l1GtRecordInputTag << "\nfound in the event."
                << "\nNo valid L1GlobalTriggerReadoutRecord with \n  "
                << l1GtReadoutRecordInputTag << "\nfound in the event."
                << std::endl;

        iError = l1ConfCode + iErrorRecord;

        errorCode = iError;
        return pfIndex;

    }

    // depending on trigger category (algorithm trigger or technical trigger)
    // get the correct quantities

    // number of sets of prescale factors
    // index of prescale factor set retrieved from data
    // pointer to the actual prescale factor set
    // pointer to the set of trigger masks

    size_t pfSetsSize = 0;

    switch (trigCategory) {
        case AlgorithmTrigger: {
            if (m_retrieveL1GtTriggerMenuLite) {
                if (m_l1GtMenuLiteValid) {
                    pfSetsSize = m_prescaleFactorsAlgoTrigLite->size();

                } else {
                    // fall through: L1 trigger configuration from event setup
                    pfSetsSize = m_prescaleFactorsAlgoTrig->size();

                }

            } else {
                // L1 trigger configuration from event setup only
                pfSetsSize = m_prescaleFactorsAlgoTrig->size();

            }

            pfIndex = pfIndexAlgoTrig;

        }
            break;
        case TechnicalTrigger: {
            if (m_retrieveL1GtTriggerMenuLite) {
                if (m_l1GtMenuLiteValid) {
                    pfSetsSize = m_prescaleFactorsTechTrigLite->size();

                } else {
                    // fall through: L1 trigger configuration from event setup
                    pfSetsSize = m_prescaleFactorsTechTrig->size();

                }

            } else {
                // L1 trigger configuration from event setup only
                pfSetsSize = m_prescaleFactorsTechTrig->size();

            }

            pfIndex = pfIndexTechTrig;

        }
            break;
        default: {
            // should not be the case
            iError = l1ConfCode + iErrorRecord + 3;
            return iError;

        }
            break;
    }


    // test prescale factor set index correctness, then retrieve the actual set of prescale factors

    if (pfIndex < 0) {

        iError = l1ConfCode + iErrorRecord + 1000;
        LogDebug("L1GtUtils")
                << "\nError: index of prescale factor set retrieved from the data \n"
                << "less than zero."
                << "\n  Value of index retrieved from data = " << pfIndex
                << std::endl;

        errorCode = iError;
        return pfIndex;

    } else if (pfIndex >= (static_cast<int>(pfSetsSize))) {
        iError = l1ConfCode + iErrorRecord + 2000;
        LogDebug("L1GtUtils")
                << "\nError: index of prescale factor set retrieved from the data \n"
                << "greater than the size of the vector of prescale factor sets."
                << "\n  Value of index retrieved from data = " << pfIndex
                << "\n  Vector size = " << pfSetsSize << std::endl;

        errorCode = iError;
        return pfIndex;

    } else {

        errorCode = iError;
        return pfIndex;
    }

    errorCode = iError;
    return pfIndex;

}


const int L1GtUtils::prescaleFactorSetIndex(const edm::Event& iEvent,
        const TriggerCategory& trigCategory, int& errorCode) const {

    // initialize error code and return value
    int iError = 0;
    int pfIndex = -1;

    edm::InputTag l1GtRecordInputTag;
    edm::InputTag l1GtReadoutRecordInputTag;

    getInputTag(iEvent, l1GtRecordInputTag, l1GtReadoutRecordInputTag);

    pfIndex = prescaleFactorSetIndex(iEvent, l1GtRecordInputTag,
            l1GtReadoutRecordInputTag, trigCategory, iError);

    // return the error code and the index value
    // if the  error code is 0, the index returned is -1
    errorCode = iError;
    return pfIndex;

}


// deprecated
const int L1GtUtils::prescaleFactorSetIndex(const edm::Event& iEvent,
        const edm::InputTag& l1GtRecordInputTag,
        const edm::InputTag& l1GtReadoutRecordInputTag,
        const std::string& triggerAlgoTechTrig, int& errorCode) const {

    // initialize error code and return value
    int iError = 0;
    int l1ConfCode = 0;
    int pfIndex = -1;

    // check if L1 configuration is available

    if (!availableL1Configuration(iError, l1ConfCode)) {
        errorCode = iError;
        return pfIndex;
    }

    // test if the argument for the "trigger algorithm type" is correct
    TriggerCategory trigCategory = AlgorithmTrigger;

    if (triggerAlgoTechTrig == "TechnicalTriggers") {
        trigCategory = TechnicalTrigger;

    } else if (triggerAlgoTechTrig == "PhysicsAlgorithms") {
        trigCategory = AlgorithmTrigger;

    } else {

        LogDebug("L1GtUtils")
                << "\nErrr : prescale factor set index cannot be retrieved for the argument "
                << triggerAlgoTechTrig
                << "\n  Supported arguments: 'PhysicsAlgorithms' or 'TechnicalTriggers'"
                << "\nWarning: this method is deprecated, please use method with TriggerCategory."
                << std::endl;

        iError = l1ConfCode + 6000;

        errorCode = iError;
        return pfIndex;

    }

    pfIndex = prescaleFactorSetIndex(iEvent, l1GtRecordInputTag,
            l1GtReadoutRecordInputTag, trigCategory, errorCode);

    errorCode = iError;
    return pfIndex;

}

// deprecated
const int L1GtUtils::prescaleFactorSetIndex(const edm::Event& iEvent,
        const std::string& triggerAlgoTechTrig, int& errorCode) const {

    // initialize error code and return value
    int iError = 0;
    int pfIndex = -1;

    edm::InputTag l1GtRecordInputTag;
    edm::InputTag l1GtReadoutRecordInputTag;

    getInputTag(iEvent, l1GtRecordInputTag, l1GtReadoutRecordInputTag);

    pfIndex = prescaleFactorSetIndex(iEvent, l1GtRecordInputTag,
            l1GtReadoutRecordInputTag, triggerAlgoTechTrig, iError);

    // return the error code and the index value
    // if the  error code is not 0, the index returned is -1
    errorCode = iError;
    return pfIndex;

}

const std::vector<int>& L1GtUtils::prescaleFactorSet(const edm::Event& iEvent,
        const edm::InputTag& l1GtRecordInputTag,
        const edm::InputTag& l1GtReadoutRecordInputTag,
        const TriggerCategory& trigCategory, int& errorCode) {

    // clear the vector before filling it
    m_prescaleFactorSet.clear();

    // initialize error code
    int iError = 0;

    const int pfIndex = prescaleFactorSetIndex(iEvent, l1GtRecordInputTag,
            l1GtReadoutRecordInputTag, trigCategory, iError);

    if (iError == 0) {

        switch (trigCategory) {
            case AlgorithmTrigger: {
                if (m_retrieveL1GtTriggerMenuLite) {
                    if (m_l1GtMenuLiteValid) {
                        m_prescaleFactorSet
                                = (*m_prescaleFactorsAlgoTrigLite).at(pfIndex);

                    } else {
                        // fall through: L1 trigger configuration from event setup
                        m_prescaleFactorSet = (*m_prescaleFactorsAlgoTrig).at(
                                pfIndex);

                    }

                } else {
                    // L1 trigger configuration from event setup only
                    m_prescaleFactorSet = (*m_prescaleFactorsAlgoTrig).at(
                            pfIndex);

                }

            }
                break;
            case TechnicalTrigger: {
                if (m_retrieveL1GtTriggerMenuLite) {
                    if (m_l1GtMenuLiteValid) {
                        m_prescaleFactorSet
                                = (*m_prescaleFactorsTechTrigLite).at(pfIndex);

                    } else {
                        // fall through: L1 trigger configuration from event setup
                        m_prescaleFactorSet = (*m_prescaleFactorsTechTrig).at(
                                pfIndex);

                    }

                } else {
                    // L1 trigger configuration from event setup only
                    m_prescaleFactorSet = (*m_prescaleFactorsTechTrig).at(
                            pfIndex);

                }

            }
                break;
            default: {
                // do nothing - it was tested before, with return

            }
                break;
        }

    }

    errorCode = iError;
    return m_prescaleFactorSet;

}

const std::vector<int>& L1GtUtils::prescaleFactorSet(const edm::Event& iEvent,
        const TriggerCategory& trigCategory, int& errorCode) {

    // clear the vector before filling it
    m_prescaleFactorSet.clear();

    // initialize error code
    int iError = 0;

    edm::InputTag l1GtRecordInputTag;
    edm::InputTag l1GtReadoutRecordInputTag;

    getInputTag(iEvent, l1GtRecordInputTag, l1GtReadoutRecordInputTag);

    m_prescaleFactorSet = prescaleFactorSet(iEvent, l1GtRecordInputTag,
            l1GtReadoutRecordInputTag, trigCategory, iError);

    errorCode = iError;
    return m_prescaleFactorSet;

}


// deprecated
const std::vector<int>& L1GtUtils::prescaleFactorSet(const edm::Event& iEvent,
        const edm::InputTag& l1GtRecordInputTag,
        const edm::InputTag& l1GtReadoutRecordInputTag,
        const std::string& triggerAlgoTechTrig, int& errorCode) {

    // clear the vector before filling it
    m_prescaleFactorSet.clear();

    // initialize error code and return value
    int iError = 0;
    int l1ConfCode = 0;

    // check if L1 configuration is available

    if (!availableL1Configuration(iError, l1ConfCode)) {
        errorCode = iError;
        return m_prescaleFactorSet;
    }

    // test if the argument for the "trigger algorithm type" is correct
    TriggerCategory trigCategory = AlgorithmTrigger;

    if (triggerAlgoTechTrig == "TechnicalTriggers") {
        trigCategory = TechnicalTrigger;

    } else if (triggerAlgoTechTrig == "PhysicsAlgorithms") {
        trigCategory = AlgorithmTrigger;

    } else {

        LogDebug("L1GtUtils")
                << "\nErrr : prescale factor set cannot be retrieved for the argument "
                << triggerAlgoTechTrig
                << "\n  Supported arguments: 'PhysicsAlgorithms' or 'TechnicalTriggers'"
                << "\nWarning: this method is deprecated, please use method with TriggerCategory."
                << std::endl;

        iError = l1ConfCode + 6000;

        errorCode = iError;
        return m_prescaleFactorSet;

    }

    m_prescaleFactorSet = prescaleFactorSet(iEvent, l1GtRecordInputTag,
            l1GtReadoutRecordInputTag, trigCategory, iError);

    errorCode = iError;
    return m_prescaleFactorSet;
}


// deprecated
const std::vector<int>& L1GtUtils::prescaleFactorSet(const edm::Event& iEvent,
        const std::string& triggerAlgoTechTrig, int& errorCode) {

    // initialize error code
    int iError = 0;

    edm::InputTag l1GtRecordInputTag;
    edm::InputTag l1GtReadoutRecordInputTag;

    getInputTag(iEvent, l1GtRecordInputTag, l1GtReadoutRecordInputTag);

    m_prescaleFactorSet = prescaleFactorSet(iEvent, l1GtRecordInputTag,
            l1GtReadoutRecordInputTag, triggerAlgoTechTrig, iError);

    errorCode = iError;
    return m_prescaleFactorSet;

}


const std::vector<unsigned int>& L1GtUtils::triggerMaskSet(
        const TriggerCategory& trigCategory, int& errorCode) {

    // clear the vector before filling it
    m_triggerMaskSet.clear();

    // initialize error code and L1 configuration code
    int iError = 0;
    int l1ConfCode = 0;

    // check if L1 configuration is available

    if (!availableL1Configuration(iError, l1ConfCode)) {
        errorCode = iError;
        return m_triggerMaskSet;
    }

    // at this point, a valid L1 configuration is available, so the if/else if/else
    // can be simplified


    // depending on trigger category (algorithm trigger or technical trigger)
    // get the correct quantities

    // pointer to the set of trigger masks

    switch (trigCategory) {
        case AlgorithmTrigger: {
            if (m_retrieveL1GtTriggerMenuLite) {
                // L1GtTriggerMenuLite has masks for physics partition only
                // avoid copy to m_triggerMaskSet, return directly m_triggerMaskAlgoTrigLite
               if (m_l1GtMenuLiteValid) {
                    errorCode = iError;
                    return (*m_triggerMaskAlgoTrigLite);

                } else {
                    // fall through: L1 trigger configuration from event setup
                    for (unsigned i = 0; i < m_triggerMaskAlgoTrig->size(); i++) {
                        m_triggerMaskSet.push_back(
                                ((*m_triggerMaskAlgoTrig)[i]) & (1
                                        << m_physicsDaqPartition));
                    }

                }

            } else {
                // L1 trigger configuration from event setup only
                for (unsigned i = 0; i < m_triggerMaskAlgoTrig->size(); i++) {
                    m_triggerMaskSet.push_back(((*m_triggerMaskAlgoTrig)[i])
                            & (1 << m_physicsDaqPartition));
                }

            }
        }
            break;
        case TechnicalTrigger: {
            if (m_retrieveL1GtTriggerMenuLite) {
                if (m_l1GtMenuLiteValid) {
                    errorCode = iError;
                    return (*m_triggerMaskTechTrigLite);

                } else {
                    // fall through: L1 trigger configuration from event setup
                    for (unsigned i = 0; i < m_triggerMaskTechTrig->size(); i++) {
                        m_triggerMaskSet.push_back(
                                ((*m_triggerMaskTechTrig)[i]) & (1
                                        << m_physicsDaqPartition));
                    }

                }

            } else {
                // L1 trigger configuration from event setup only
                for (unsigned i = 0; i < m_triggerMaskTechTrig->size(); i++) {
                    m_triggerMaskSet.push_back(((*m_triggerMaskTechTrig)[i])
                            & (1 << m_physicsDaqPartition));
                }

            }
        }
            break;
        default: {
            // should not be the case
            iError = l1ConfCode + 3;

            errorCode = iError;
            return m_triggerMaskSet;

        }
            break;
    }

    errorCode = iError;
    return m_triggerMaskSet;

}


//deprecated
const std::vector<unsigned int>& L1GtUtils::triggerMaskSet(
        const std::string& triggerAlgoTechTrig, int& errorCode) {

    // clear the vector before filling it
    m_triggerMaskSet.clear();

    // initialize error code and return value
    int iError = 0;
    int l1ConfCode = 0;

    // check if L1 configuration is available

    if (!availableL1Configuration(iError, l1ConfCode)) {
        errorCode = iError;
        return m_triggerMaskSet;
    }

    // test if the argument for the "trigger algorithm type" is correct
    TriggerCategory trigCategory = AlgorithmTrigger;

    if (triggerAlgoTechTrig == "TechnicalTriggers") {
        trigCategory = TechnicalTrigger;

    } else if (triggerAlgoTechTrig == "PhysicsAlgorithms") {
        trigCategory = AlgorithmTrigger;

    } else {

        LogDebug("L1GtUtils")
                << "\nErrr : trigger mask set cannot be retrieved for the argument "
                << triggerAlgoTechTrig
                << "\n  Supported arguments: 'PhysicsAlgorithms' or 'TechnicalTriggers'"
                << "\nWarning: this method is deprecated, please use method with TriggerCategory."
                << std::endl;

        iError = l1ConfCode + 6000;

        errorCode = iError;
        return m_triggerMaskSet;

    }

    m_triggerMaskSet = triggerMaskSet(trigCategory, iError);

    errorCode = iError;
    return m_triggerMaskSet;

}

const std::string& L1GtUtils::l1TriggerMenu() const {

    if (m_retrieveL1GtTriggerMenuLite) {
        if (m_l1GtMenuLiteValid) {
            return m_l1GtMenuLite->gtTriggerMenuName();

        } else if (m_retrieveL1EventSetup) {
            return m_l1GtMenu->gtTriggerMenuName();

        } else {
            // only L1GtTriggerMenuLite requested, but it is not valid
            return EmptyString;

        }
    } else if (m_retrieveL1EventSetup) {
        return m_l1GtMenu->gtTriggerMenuName();

    } else {
        // L1 trigger configuration not retrieved
        return EmptyString;

    }

}

const std::string& L1GtUtils::l1TriggerMenuImplementation() const {

    if (m_retrieveL1GtTriggerMenuLite) {
        if (m_l1GtMenuLiteValid) {
            return m_l1GtMenuLite->gtTriggerMenuImplementation();

        } else if (m_retrieveL1EventSetup) {
            return m_l1GtMenu->gtTriggerMenuImplementation();

        } else {
            // only L1GtTriggerMenuLite requested, but it is not valid
            return EmptyString;

        }
    } else if (m_retrieveL1EventSetup) {
        return m_l1GtMenu->gtTriggerMenuImplementation();

    } else {
        // L1 trigger configuration not retrieved
        return EmptyString;

    }

}

const L1GtTriggerMenu* L1GtUtils::ptrL1TriggerMenuEventSetup(int& errorCode) {

    // initialize error code and return value
    int iError = 0;
    int l1ConfCode = 0;

    // check if L1 configuration is available

    if (!availableL1Configuration(iError, l1ConfCode)) {
        errorCode = iError;
        return 0;
    }

    if (m_retrieveL1EventSetup) {
        errorCode = iError;
        return m_l1GtMenu;
    } else {
        iError = l1ConfCode;

        errorCode = iError;
        return 0;

    }

    errorCode = iError;
    return m_l1GtMenu;
}

const L1GtTriggerMenuLite* L1GtUtils::ptrL1GtTriggerMenuLite(int& errorCode) {

    // initialize error code and return value
    int iError = 0;
    int l1ConfCode = 0;

    // check if L1 configuration is available

    if (!availableL1Configuration(iError, l1ConfCode)) {
        errorCode = iError;
        return 0;
    }

    if (m_retrieveL1GtTriggerMenuLite) {
        if (m_l1GtMenuLiteValid) {

            errorCode = iError;
            return m_l1GtMenuLite;

        } else {
            iError = l1ConfCode;

            errorCode = iError;
            return 0;
        }
    } else {
        iError = l1ConfCode;

        errorCode = iError;
        return 0;
    }

    errorCode = iError;
    return m_l1GtMenuLite;

}

const bool L1GtUtils::availableL1Configuration(int& errorCode, int& l1ConfCode) const {

    if (m_retrieveL1GtTriggerMenuLite) {
        if (!m_retrieveL1EventSetup) {
            LogDebug("L1GtUtils")
                    << "\nRetrieve L1 trigger configuration from L1GtTriggerMenuLite only\n"
                    << std::endl;
            l1ConfCode = 0;
        } else {
            LogDebug("L1GtUtils")
                    << "\nFall through: retrieve L1 trigger configuration from L1GtTriggerMenuLite"
                    << "\n  if L1GtTriggerMenuLite not valid, try to retrieve from event setup "
                    << std::endl;
            l1ConfCode = 100000;
        }

        if (m_l1GtMenuLiteValid) {
            LogDebug("L1GtUtils")
                    << "\nRetrieve L1 trigger configuration from L1GtTriggerMenuLite, valid product\n"
                    << std::endl;
            l1ConfCode = l1ConfCode  + 10000;
            errorCode = 0;

            return true;

        } else if (m_retrieveL1EventSetup) {
            if (m_l1EventSetupValid) {
                LogDebug("L1GtUtils")
                        << "\nFall through: retrieve L1 trigger configuration from event setup."
                        << "\nFirst option was L1GtTriggerMenuLite - but product is not valid.\n"
                        << std::endl;
                l1ConfCode = l1ConfCode  + 20000;
                errorCode = 0;

                return true;

            } else {
                LogDebug("L1GtUtils")
                        << "\nFall through: L1GtTriggerMenuLite not valid, event setup not valid.\n"
                        << std::endl;
                l1ConfCode = l1ConfCode  + L1GtNotValidError;
                errorCode = l1ConfCode;

                return false;


            }

        } else {
            LogDebug("L1GtUtils")
                    << "\nError: L1 trigger configuration requested from L1GtTriggerMenuLite only"
                    << "\nbut L1GtTriggerMenuLite is not valid.\n" << std::endl;
            l1ConfCode = l1ConfCode  + L1GtNotValidError;
            errorCode = l1ConfCode;

            return false;

        }
    } else if (m_retrieveL1EventSetup) {

        LogDebug("L1GtUtils")
                << "\nRetrieve L1 trigger configuration from event setup."
                << "\nL1GtTriggerMenuLite product was not requested.\n"
                << std::endl;
        l1ConfCode = 200000;

        if (m_l1EventSetupValid) {
            LogDebug("L1GtUtils")
                    << "\nRetrieve L1 trigger configuration from event setup only."
                    << "\nValid L1 trigger event setup.\n"
                    << std::endl;
            l1ConfCode = l1ConfCode  + 10000;
            errorCode = 0;

            return true;

        } else {
            LogDebug("L1GtUtils")
                    << "\nRetrieve L1 trigger configuration from event setup only."
                    << "\nNo valid L1 trigger event setup.\n"
                    << std::endl;
            l1ConfCode = l1ConfCode  + L1GtNotValidError;
            errorCode = l1ConfCode;

            return false;


        }

    } else {
        LogDebug("L1GtUtils")
                << "\nError: no L1 trigger configuration requested to be retrieved."
                << "\nMust call before either retrieveL1GtTriggerMenuLite or retrieveL1EventSetup.\n"
                << std::endl;
        l1ConfCode = 300000;
        errorCode = l1ConfCode;

        return false;

    }
}

// private methods

const bool L1GtUtils::trigResult(const DecisionWord& decWord,
        const int bitNumber, const std::string& nameAlgoTechTrig,
        const TriggerCategory& trigCategory, int& errorCode) const {

    bool trigRes = false;
    errorCode = 0;

    if (bitNumber < (static_cast<int> (decWord.size()))) {
        trigRes = decWord[bitNumber];
    } else {
        errorCode = 3000;
        LogDebug("L1GtUtils") << "\nError: bit number " << bitNumber
                << " retrieved for " << triggerCategory(trigCategory) << "\n  "
                << nameAlgoTechTrig
                << "\ngreater than size of L1 GT decision word: "
                << decWord.size()
                << "\nError: Inconsistent L1 trigger configuration!"
                << std::endl;
    }

    return trigRes;
}


const std::string L1GtUtils::EmptyString = "";
const int L1GtUtils::L1GtNotValidError = 99999;
