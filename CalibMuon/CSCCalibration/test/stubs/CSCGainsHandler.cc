#include "CalibMuon/CSCCalibration/test/stubs/CSCGainsHandler.h"
#include "FWCore/ParameterSet/interface/ParameterSetfwd.h"
#include<iostream>

#include "CondFormats/CSCObjects/interface/CSCDBGains.h"
#include "CalibMuon/CSCCalibration/interface/CSCGainsDBConditions.h"

popcon::CSCDBGainsImpl::CSCDBGainsImpl(const edm::ParameterSet& pset): m_name(pset.getUntrackedParameter<std::string>("name","CSCDBGainsImpl"))
{}

popcon::CSCDBGainsImpl::~CSCDBGainsImpl()
{
}

void popcon::CSCDBGainsImpl::getNewObjects()
{

  std::cout << "CSCGainsHandler - time before filling object:"<< std::endl;
  int id=system("date");
  std::cout << "------- CSC src - > getNewObjects\n"<<m_name;
  
  // fill object from file
  CSCDBGains * cngains = CSCGainsDBConditions::prefillDBGains();
  //std::cout << "crosstalk size " << cngains->gains.size() << std::endl;
  
  std::cout << "CSCGainsHandler - time after filling object:"<< std::endl;
  id=system("date");
  
  //check whats already inside of database
  std::cerr<<"got offlineInfo"<<std::endl;
  std::cerr << tagInfo().name << " , last object valid since " 
	    << tagInfo().lastInterval.first << std::endl; 
  
  unsigned int snc;
  
  std::cout << "Source implementation test ::getNewObjects : enter since ? \n";
  std::cin >> snc;
 
  
  id=system("date");
  m_to_transfer.push_back(std::make_pair(cngains,snc));
  
  std::cout << "------- " << m_name << "CSC src - > getNewObjects -----------\n"<< std::endl;
  
  std::cout << "CSCGainsHandler - time before writing into DB:"<< std::endl;
  id=system("date");
}