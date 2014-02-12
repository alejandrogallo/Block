/*                                                                           
Developed by Sandeep Sharma and Garnet K.-L. Chan, 2012                      
Copyright (c) 2012, Garnet K.-L. Chan                                        
                                                                             
This program is integrated in Molpro with the permission of 
Sandeep Sharma and Garnet K.-L. Chan
*/

#include "twopdm_container.h"

namespace SpinAdapted{

//===========================================================================================================================================================

Twopdm_container::Twopdm_container( int sites )
{
  store_full_spin_array_ = false;
  store_full_spatial_array_ = true;
  store_sparse_spin_array_ = false;
  store_sparse_spatial_array_ = true;

  if ( store_full_spin_array_ ) {
    twopdm.resize(2*sites,2*sites,2*sites,2*sites);
    twopdm.Clear();
  }
  if ( store_full_spatial_array_ ) {
    spatial_twopdm.resize(sites,sites,sites,sites);
    spatial_twopdm.Clear();
  }
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------
  
void Twopdm_container::save_npdms(const int& i, const int& j)
{

//FIXME parallel    
boost::mpi::communicator world;
world.barrier();
    Timer timer;
    // accumulate_npdm();
    if ( store_full_spin_array_ ) {
      save_npdm_text(i, j);
      save_npdm_binary(i, j);
    }
    if ( store_full_spatial_array_ ) {
      save_spatial_npdm_text(i, j);
      save_spatial_npdm_binary(i, j);
    }

world.barrier();
    pout << "2PDM save full array time " << timer.elapsedwalltime() << " " << timer.elapsedcputime() << endl;
//  }
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------

void Twopdm_container::save_npdm_text(const int &i, const int &j)
{
  if( mpigetrank() == 0)
  {
    char file[5000];
    sprintf (file, "%s%s%d.%d%s", dmrginp.save_prefix().c_str(),"/twopdm.", i, j,".txt");
    ofstream ofs(file);
    ofs << twopdm.dim1() << endl;
    double trace = 0.0;
    for(int k=0;k<twopdm.dim1();++k)
      for(int l=0;l<twopdm.dim2();++l)
        for(int m=0;m<twopdm.dim3();++m)
          for(int n=0;n<twopdm.dim4();++n) {
            ofs << boost::format("%d %d %d %d %20.14e\n") % k % l % m % n % twopdm(k,l,m,n);
            if ( (k==n) && (l==m) ) trace += twopdm(k,l,m,n);
          }
    ofs.close();
    std::cout << "Spin-orbital 2PDM trace = " << trace << "\n";
  }
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------

void Twopdm_container::save_spatial_npdm_text(const int &i, const int &j)
{
  if( mpigetrank() == 0)
  {
    char file[5000];
    sprintf (file, "%s%s%d.%d%s", dmrginp.save_prefix().c_str(),"/spatial_twopdm.", i, j,".txt");
    ofstream ofs(file);
    ofs << spatial_twopdm.dim1() << endl;
    double trace = 0.0;
    for(int k=0;k<spatial_twopdm.dim1();++k)
      for(int l=0;l<spatial_twopdm.dim2();++l)
        for(int m=0;m<spatial_twopdm.dim3();++m)
          for(int n=0;n<spatial_twopdm.dim4();++n) {
            ofs << boost::format("%d %d %d %d %20.14e\n") % k % l % m % n % spatial_twopdm(k,l,m,n);
            if ( (k==n) && (l==m) ) trace += spatial_twopdm(k,l,m,n);
          }
    ofs.close();
    std::cout << "Spatial      2PDM trace = " << trace << "\n";
  }
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------

void Twopdm_container::save_npdm_binary(const int &i, const int &j)
{
  if( mpigetrank() == 0)
  {
    char file[5000];
    sprintf (file, "%s%s%d.%d%s", dmrginp.save_prefix().c_str(),"/twopdm.", i, j,".bin");
    std::ofstream ofs(file, std::ios::binary);
    boost::archive::binary_oarchive save(ofs);
    save << twopdm;
    ofs.close();
  }
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------

void Twopdm_container::save_spatial_npdm_binary(const int &i, const int &j)
{
  if( mpigetrank() == 0)
  {
    char file[5000];
    sprintf (file, "%s%s%d.%d%s", dmrginp.save_prefix().c_str(),"/spatial_twopdm.", i, j,".bin");
    std::ofstream ofs(file, std::ios::binary);
    boost::archive::binary_oarchive save(ofs);
    save << spatial_twopdm;
    ofs.close();
  }
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------

void Twopdm_container::load_npdm_binary(const int &i, const int &j)
{
assert(false); // <<<< CAN WE RETHINK USE OF DISK FOR NPDM?
  if( mpigetrank() == 0)
  {
    char file[5000];
    sprintf (file, "%s%s%d.%d%s", dmrginp.save_prefix().c_str(),"/twopdm.", i, j,".bin");
    std::ifstream ifs(file, std::ios::binary);
    boost::archive::binary_iarchive load(ifs);
    load >> twopdm;
    ifs.close();
  }
//#ifndef SERIAL
//  mpi::communicator world;
//  mpi::broadcast(world,twopdm,0);
//FIXME this is a contradiction --- BUT IS IT GOOD TO CLEAR TO SAVE MEMORY???
//  if( mpigetrank() != 0)
//    twopdm.Clear();
//#endif
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------

void Twopdm_container::accumulate_npdm()
{
#ifndef SERIAL
  array_4d<double> tmp_recv;
  mpi::communicator world;
  if( mpigetrank() == 0) {
    for(int i=1;i<world.size();++i) {
      world.recv(i, i, tmp_recv);
      for(int k=0;k<twopdm.dim1();++k)
        for(int l=0;l<twopdm.dim2();++l)
          for(int m=0;m<twopdm.dim3();++m)
            for(int n=0;n<twopdm.dim4();++n)
              if(tmp_recv(k,l,m,n) != 0.) twopdm(k,l,m,n) = tmp_recv(k,l,m,n);
	 }
  }
  else {
    world.send(0, mpigetrank(), twopdm);
  }
#endif
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------

void Twopdm_container::update_full_spin_array( std::map< std::tuple<int,int,int,int>, double >& spin_batch ) 
{
  for (auto it = spin_batch.begin(); it != spin_batch.end(); ++it) {
    int i = std::get<0>( it->first );
    int j = std::get<1>( it->first );
    int k = std::get<2>( it->first );
    int l = std::get<3>( it->first );

    double val = it->second;
    //if ( abs(val) > 1e-8 ) pout << "so-twopdm val: i,j,k,l = " << i << "," << j << "," << k << "," << l << "\t\t" << val << endl;
    //pout << "so-twopdm val: i,j,k,l = " << i << "," << j << "," << k << "," << l << "\t\t" << val << endl;

    // Test for duplicates
    if ( twopdm(i, j, k, l) != 0.0 && abs(twopdm(i,j,k,l)-val) > 1e-6) {
      void *array[10];
      size_t size;
      size = backtrace(array, 10);
      cout << "WARNING: Already calculated "<<i<<" "<<j<<" "<<k<<" "<<l<<endl;
      //backtrace_symbols_fd(array, size, 2);
      cout << "earlier value: "<<twopdm(i,j,k,l)<<endl<< "new value:     "<<val<<endl;
      assert( false );
    }
    if ( abs(val) > 1e-14 ) twopdm(i,j,k,l) = val;
  }

}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------

void Twopdm_container::build_spatial_elements( std::map< std::tuple<int,int,int,int>, double >& spin_batch, 
                                               std::map< std::tuple<int,int,int,int>, double >& spatial_batch )
{
  //Note we multiply the spatial 2PDM by a factor of 1/2 to be consistent with the old BLOCK code, but this doesn't seem conventional?
  double factor = 0.5;

  for (auto it = spatial_batch.begin(); it != spatial_batch.end(); ++it) {
    int i = std::get<0>( it->first );
    int j = std::get<1>( it->first );
    int k = std::get<2>( it->first );
    int l = std::get<3>( it->first );
    // Sum over spin indices 
    double val = 0.0;
    for (int s=0; s<2; s++) {
      for (int t =0; t<2; t++) {
        val += spin_batch[ std::make_tuple( 2*i+s, 2*j+t, 2*k+t, 2*l+s ) ];
      }
    }
    // Store significant elements only
    if ( abs(val) > 1e-14 ) {
      if ( store_sparse_spatial_array_ ) sparse_spatial_pdm[ it->first ] = factor * val;
      if ( store_full_spatial_array_ ) {
        if ( abs( spatial_twopdm(i,j,k,l) ) > 1e-14 ) { 
          cout << "repeated spatial indices!\n";
          assert(false);
        }
        spatial_twopdm(i,j,k,l) = factor * val;
      }
    }
  }

}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------

void Twopdm_container::store_npdm_elements( const std::vector< std::pair< std::vector<int>, double > > & new_spin_orbital_elements)
{
  assert( new_spin_orbital_elements.size() == 6 );
  // Temporary batches of npdm elements
  std::map< std::tuple<int,int,int,int>, double > spin_batch;
  std::map< std::tuple<int,int,int,int>, double > spatial_batch;

  for (int idx=0; idx < new_spin_orbital_elements.size(); ++idx) {
    // Get all spin-index permutations
    Twopdm_permutations p;
    std::map< std::tuple<int,int,int,int>, int > spin_indices = p.get_spin_permutations( new_spin_orbital_elements[idx].first );
    double val = new_spin_orbital_elements[idx].second;
    for (auto it = spin_indices.begin(); it != spin_indices.end(); ++it) {
      // Initialize spatial indices
      int i = std::get<0>( it->first );
      int j = std::get<1>( it->first );
      int k = std::get<2>( it->first );
      int l = std::get<3>( it->first );
      spatial_batch[ std::make_tuple(i/2, j/2, k/2, l/2) ] = 0.0;
      // Assign temporary batch of spin-orbital elements
      spin_batch[ it->first ] = it->second * val;
      if ( store_sparse_spin_array_ && (abs(val) > 1e-14) ) sparse_spin_pdm[ it->first ] = it->second * val;
    }
  }

  // Build and store new spatial elements
  build_spatial_elements( spin_batch, spatial_batch );
  if ( store_full_spin_array_ ) update_full_spin_array( spin_batch );

}

//===========================================================================================================================================================

}

