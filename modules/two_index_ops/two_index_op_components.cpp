/*                                                                           
Developed by Sandeep Sharma and Garnet K.-L. Chan, 2012                      
Copyright (c) 2012, Garnet K.-L. Chan                                        
                                                                             
This program is integrated in Molpro with the permission of 
Sandeep Sharma and Garnet K.-L. Chan
*/

//-------------------------------------------------------------------------------------------------------------------------------------------------------------  
//
//  This is an extension of op_components.C (i.e. used with op_components.h header file)
//
//-------------------------------------------------------------------------------------------------------------------------------------------------------------  

#include <boost/format.hpp>
#include <fstream>
#include <stdio.h>
#include "screen.h"
#include "spinblock.h"
#include "op_components.h"
//#include "screen.h"

namespace SpinAdapted {
  
//===========================================================================================================================================================
// 3PDM operators
//===========================================================================================================================================================

//===========================================================================================================================================================
// (Des,Cre)
//-------------------

//FIXME do we REALLY need to build these operators separately????  (Can we make an algorithm using CD only?)

  template<> string Op_component<DesCre>::get_op_string() const {
    return "DESCRE";
  }

//-------------------------------------------------------------------------------------------------------------------------------------------------------------  

  template<> void Op_component<DesCre>::build_iterators(SpinBlock& b)
    {
      if (b.get_sites().size () == 0) return; // blank construction (used in unset_initialised() Block copy construction, for use with STL)
      const double screen_tol = dmrginp.screen_tol();
//FIXME is this OK?
      vector< pair<int, int> > screened_dc_ix = screened_cd_indices(b.get_sites(), b.get_complementary_sites(), *b.get_twoInt(), screen_tol);
      m_op.set_pair_indices(screened_dc_ix, dmrginp.last_site());
      std::vector<int> orbs(2);
      for (int i = 0; i < m_op.local_nnz(); ++i) {

     orbs = m_op.unmap_local_index(i);
     std::vector<boost::shared_ptr<DesCre> >& vec = m_op.get_local_element(i);
assert( vec.size() == 0);
     SpinQuantum spin1 = SpinQuantum(1, 1, SymmetryOfSpatialOrb(orbs[0]));
     SpinQuantum spin2 = SpinQuantum(1, 1, SymmetryOfSpatialOrb(orbs[1]));

//FIXME plus and minus!!!
     std::vector<SpinQuantum> spinvec = -spin1+spin2;

     vec.resize(spinvec.size());
     for (int j=0; j<spinvec.size(); j++) {
       vec[j]=boost::shared_ptr<DesCre>(new DesCre);
       SparseMatrix& op = *vec[j];
       op.set_orbs() = orbs;
       op.set_initialised() = true;
       op.set_fermion() = false;
       op.set_deltaQuantum() = spinvec[j];

       op.set_quantum_ladder()["(DC)"] = { op.get_deltaQuantum() };
       assert( op.get_deltaQuantum().particleNumber == 0 );
     }
   }

 }
//===========================================================================================================================================================

}
