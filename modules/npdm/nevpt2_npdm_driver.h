/*                                                                           
Developed by Sandeep Sharma and Garnet K.-L. Chan, 2012                      
Copyright (c) 2012, Garnet K.-L. Chan                                        
                                                                             
This program is integrated in Molpro with the permission of 
Sandeep Sharma and Garnet K.-L. Chan
*/

#ifndef NEVPT2_NPDM_DRIVER_H
#define NEVPT2_NPDM_DRIVER_H

#include "npdm_driver.h"

namespace SpinAdapted{

//===========================================================================================================================================================

class Nevpt2_npdm_driver : public Npdm_driver_base {

  public:
    Nevpt2_npdm_driver( int sites );
    ~Nevpt2_npdm_driver() {};
  
    void save_data();
    void compute_npdm_elements(std::vector<Wavefunction> & wavefunctions, const SpinBlock & big, int sweepPos, int endPos);
    void compute_matrices();

  private:
    Twopdm_container twopdm_container;
    Threepdm_container threepdm_container;
    Fourpdm_container fourpdm_container;

    Npdm_driver twopdm_driver;
    Npdm_driver threepdm_driver;
    Npdm_driver fourpdm_driver;

//    void update_A16_matrix( Twopdm_driver& twopdm_driver, Threepdm_driver& Threepdm_driver, Fourpdm_driver& fourpdm_driver );

};

//===========================================================================================================================================================

}

#endif
