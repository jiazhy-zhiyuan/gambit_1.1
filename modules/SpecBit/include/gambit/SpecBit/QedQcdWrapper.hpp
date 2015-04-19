//   GAMBIT: Global and Modular BSM Inference Tool
//   *********************************************
///  \file
///
///  This class is used to wrap the QedQcd object used by SoftSUSY
///  and FlexibleSUSY in a Gambit SubSpectrum object. This is to enable
///  access to the parameters of the SM defined as a low-energy effective theory 
///  (as opposed to correspending information defined in a UV model). 
///  Parameters defined this way are often used as input to a physics calculator.
///
///  This is one of the simplest wrappers possible, so it is useful as a guide
///  for designing other SubSpectrum wrappers. To assist this, code is documented
///  with the following markings to distinguish pieces that are essential for
///  all wrappers from those which are specific to this wrapper:
///  /***/ - Required by all wrappers.
///  /*O*/ - Optional (e.g. unused maps fillers can be left undeclared)
///  /*P*/ - Required if map fillers are protected, which is sensible.
///
///  *********************************************
///
///  Authors: 
///  <!-- add name and date if you modify -->
///   
///  \author Ben Farmer
///          (benjamin.farmer@fysik.su.se)
///  \date 2015 Mar 
///
///  *********************************************

#ifndef __QedQcdWrapper_hpp__
#define __QedQcdWrapper_hpp__

#include "lowe.h" ///TODO: wrap using BOSS at some point, i.e. get this from FlexibleSUSY or SoftSUSY
#include "gambit/Utils/SMInputs.hpp"
#include "gambit/Utils/SubSpectrum.hpp"

namespace Gambit {

   // Needed for typename aliases in Spec and MapTypes classes
   struct QedQcdWrapperTraits
   {
      typedef softsusy::QedQcd Model;
      typedef SMInputs         Input;
   };
    
   class QedQcdWrapper : public Spec<QedQcdWrapper,QedQcdWrapperTraits> 
   {
      friend class RunparDer<QedQcdWrapper,QedQcdWrapperTraits>; /*P*/
      friend class PhysDer  <QedQcdWrapper,QedQcdWrapperTraits>; /*P*/

      private:
         typedef MapTypes<QedQcdWrapperTraits> MT; 

         // Keep copies of Model and Input objects internally
         typename QedQcdWrapperTraits::Model qedqcd;
         typename QedQcdWrapperTraits::Input sminputs;

      public:
         // Constructors/destructors
         QedQcdWrapper();
         QedQcdWrapper(const softsusy::QedQcd&, const SMInputs&);
         QedQcdWrapper(const QedQcdWrapper&);
         virtual ~QedQcdWrapper();        /***/
 
         virtual int get_index_offset() const;  /***/   
         virtual int get_numbers_stable_particles() const;  /***/

         /// RunningPars interface overrides
         virtual double GetScale() const;      /***/
         virtual void SetScale(double scale);  /***/
         virtual void RunToScale(double);      /***/

         // Limits for running
         double softup;
         double hardup; // Be careful of order in constructor!

         // Limits for running
         virtual double hard_upper() const {return hardup;} /*O*/
         virtual double soft_upper() const {return softup;} /*O*/
         virtual double soft_lower() const {return 0.;}     /*O*/
         virtual double hard_lower() const {return 0.;}     /*O*/

      protected:
         // These members are inherited from Spec<T> class! Make sure to
         // initialise them from qedqcd and sminputs via Spec<T> constructor,
         // so that they can be passed on through to the map functions.
         // Model* model;          
         // Input* input; 

         // Note; make sure input (sminputs) matches the one used to set up qedqcd!

         /// Map fillers
         /// Used to initialise maps in the RunparDer and PhysDer classes
         /// (specialisations created and stored automatically by Spec<QedQcdWrapper>)
         
         /// RunparDer overrides (access via spectrum.runningpar)
         static typename MT::fmap_extraM fill_mass_map_extraM();   /*O*/
         static typename MT::fmap_extraM fill_mass0_map_extraM();  /*O*/

         /// PhysDer overrides (access via spectrum.phys)
         static typename MT::fmap        fill_PoleMass_map();        /*O*/
         static typename MT::fmap_extraI fill_PoleMass_map_extraI(); /*O*/
 
   };
 


} // end Gambit namespace

#endif
