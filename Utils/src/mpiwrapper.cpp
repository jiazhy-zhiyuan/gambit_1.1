//   GAMBIT: Global and Modular BSM Inference Tool
//   *********************************************
///  \file
///
///  Definitions for Gambit MPI C++ bindings.
//
///  *********************************************
///
///  Authors (add name and date if you modify):
///   
///  \author Ben Farmer
///          (benjamin.farmer@fysik.su.se)
///  \date 2015 Apr
///  *********************************************

#ifdef WITH_MPI // Contents of this file ignored if MPI not enabled

#include <iostream>
#include <vector>
#include <iostream>

#include "gambit/Utils/mpiwrapper.hpp"
#include "gambit/Utils/new_mpi_datatypes.hpp"

namespace Gambit
{

   namespace GMPI
   {
 
      /// @{ Main "Communicator" class
            
      /// @{ Constructors
      /// Default (attaches to MPI_COMM_WORLD):
      Comm::Comm() : boundcomm(MPI_COMM_WORLD)
      {
      }

      /// Copy existing communicator
      Comm::Comm(const MPI_Comm& comm) : boundcomm(comm)
      {
      }

      /// Duplicate input communicator into boundcomm
      /// (creates new context)
      /// NOTE! MPI_Comm_dup is a COLLECTIVE call, so all processes
      /// must call it! Beware deadlocks. May be better to duplicate
      /// first and then wrap in a communicator.
      void Comm::dup(const MPI_Comm& comm)
      {
         int errflag = MPI_Comm_dup(comm, &boundcomm);
         if(errflag!=0) {
           std::ostringstream errmsg;
           errmsg << "Error performing MPI_Comm_dup! Received error flag: "<<errflag; 
           utils_error().raise(LOCAL_INFO, errmsg.str());
         }
      }

      /// @}      
  
      /// Get total number of MPI tasks in this communicator group
      int Comm::Get_size() const
      {
        int size;
        MPI_Comm_size(boundcomm,&size);
        return size;
      }

      /// Get "rank" (ID number) of current task in this communicator group
      int Comm::Get_rank() const
      {
        int rank;
        MPI_Comm_rank(boundcomm,&rank);
        return rank;
      }

      /// @}

      /// Check if MPI_Init has been called (it is an error to call it twice)
      bool Is_initialized() 
      { 
        int flag;
        MPI_Initialized(&flag);
        return (flag!=0);
      }

      /// @{ Helpers for registration of compound datatypes

      /// Get vector storing functions to be run when MPI initialises.
      std::vector<MpiIniFunc>& get_mpi_ini_functions()
      {
         static std::vector<MpiIniFunc> mpi_ini_functions;
         return mpi_ini_functions;
      }

      /// Constructor for AddMpiInitFunc
      ///
      /// AddMpiInitFunc will add functions to the map when it is constructed. Works
      /// on the same idea as the "ini_code" struct, except it doesn't
      /// cause the functions to be run, just "queues them up" so to speak.
      AddMpiIniFunc::AddMpiIniFunc(std::string local_info, std::string name, void(*func)())
      {
         get_mpi_ini_functions().push_back(MpiIniFunc(local_info,name,func));
      }

      /// @}

      /// Initialise MPI
      void Init()
      {
        // Dummies; can't rely on being able to use these seriously as the MPI standard doesn't mandate it.
        int argc = 0;
        char** argv = NULL;

        // Run any functions needed to queue up MPI datatype definition functions
        // (still a little hacky, but works)
        Printers::queue_mpidefs();

        // Do basic interrogation
        std::cout << "Hooking up to MPI..." << std::endl;
        if(Is_initialized())
        {
           std::ostringstream errmsg;
           errmsg << "Error initialising MPI! It is already initialised!"; 
           utils_error().raise(LOCAL_INFO, errmsg.str());
        } 
        else
        {
           int errflag;
           errflag = MPI_Init(&argc,&argv);

           // // Test case for thread-safe MPI. Probably not going to use this though.
           // int provided; // output; level of thread support provided (may not meet the requested level)    
           // errflag = MPI_Init_thread(&argc,&argv,MPI_THREAD_FUNNELED,&provided);
           // if(provided<MPI_THREAD_MULTIPLE)
           // {
           //    std::ostringstream errmsg;
           //    errmsg << "Error initialising MPI with thread support level MPI_THREAD_FUNNELED. The implementation did not provide the requested level of thread support; the best it could do was ";
           //    switch(provided)
           //    {
           //       case MPI_THREAD_SINGLE:     errmsg<<"MPI_THREAD_SINGLE"; break;
           //       case MPI_THREAD_FUNNELED:   errmsg<<"MPI_THREAD_FUNNELED"; break;
           //       case MPI_THREAD_SERIALIZED: errmsg<<"MPI_THREAD_SERIALIZED"; break;
           //       case MPI_THREAD_MULTIPLE:   errmsg<<"MPI_THREAD_MULTIPLE"; break;
           //       default: errmsg<<"[UNRECOGNISED CODE!!!]"; break;
           //    }
           //    utils_error().raise(LOCAL_INFO, errmsg.str());
           // }

           if(errflag!=0) {
              std::ostringstream errmsg;
              errmsg << "Error initialising MPI! Received error flag: "<<errflag; 
              utils_error().raise(LOCAL_INFO, errmsg.str());
           }
        }

        // Create communicator and check out basic info
        Comm COMM_WORLD;

        std::cout << "  Process pool size : " << COMM_WORLD.Get_size() << std::endl;
        std::cout << "  I am process number " << COMM_WORLD.Get_rank() << std::endl;

        // Run externally defined initialisation functions
        std::cout << "  Running MPI initialisation functions..." << std::endl;
        for (std::vector<MpiIniFunc>::iterator it=get_mpi_ini_functions().begin();
              it != get_mpi_ini_functions().end(); it++)
        {
          std::cout << "    - Running function '"<<it->myname()<<"'" << std::endl;
          try
          {
             it->runme(); // Run function.
          }
          catch (const std::exception& e)
          {
             std::cout << "Gambit has failed to initialise MPI due to fatal exception: " << e.what() << std::endl;
             std::cout << "raised from an mpi_ini_function (with label="<<it->myname()<<") declared at: " << it->mylocation() << std::endl;
             throw(e);
          }
        }
        std::cout << "  MPI initialisation complete." << std::endl;
      }
      
   }
}


#endif