#ifndef Fireworks_Core_FWGeometryTableViewManager_h
#define Fireworks_Core_FWGeometryTableViewManager_h
// -*- C++ -*-
//
// Package:     Core
// Class  :     FWGeometryTableViewManager
// 
/**\class FWGeometryTableViewManager FWGeometryTableViewManager.h Fireworks/Core/interface/FWGeometryTableViewManager.h

 Description: [one line class summary]

 Usage:
    <usage>

*/
//
// Original Author:  Alja Mrak-Tadel
//         Created:  Fri Jul  8 00:40:50 CEST 2011
// $Id: FWGeometryTableViewManager.h,v 1.6 2011/07/15 04:46:59 amraktad Exp $
//

class FWViewBase;
class FWGUIManager;
class TEveWindowSlot;
class TGeoManager;

#include "Fireworks/Core/interface/FWViewManagerBase.h"
#include "Fireworks/Core/interface/FWGeometryTableView.h"

class FWGeometryTableViewManager : public FWViewManagerBase
{
public:
   FWGeometryTableViewManager(FWGUIManager*, std::string fileName);
   virtual ~FWGeometryTableViewManager();

   // dummy functions of FWViewManagerBase
   virtual FWTypeToRepresentations supportedTypesAndRepresentations() const { return FWTypeToRepresentations();}
   virtual void newItem(const FWEventItem*) {}  

   FWViewBase *buildView (TEveWindowSlot *iParent, const std::string& type);
   virtual void colorsChanged();

   TList*  getListOfVolumes() const;
   TGeoNode* getTopTGeoNode();

   static TGeoManager* getGeoMangeur() { return s_geoManager; }

protected:
   // dummy functions of FWViewManagerBase
   virtual void modelChangesComing() {}
   virtual void modelChangesDone() {}

   std::vector<boost::shared_ptr<FWGeometryTableView> > m_views;

private:
   FWGeometryTableViewManager(const FWGeometryTableViewManager&); // stop default
   const FWGeometryTableViewManager& operator=(const FWGeometryTableViewManager&); // stop default
   void beingDestroyed(const FWViewBase* iView);

   static TGeoManager *s_geoManager;
   std::string m_fileName;
   void initGeoManager();
};


#endif