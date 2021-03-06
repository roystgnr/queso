//-----------------------------------------------------------------------bl-
//--------------------------------------------------------------------------
//
// QUESO - a library to support the Quantification of Uncertainty
// for Estimation, Simulation and Optimization
//
// Copyright (C) 2008-2015 The PECOS Development Team
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the Version 2.1 GNU Lesser General
// Public License as published by the Free Software Foundation.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc. 51 Franklin Street, Fifth Floor,
// Boston, MA  02110-1301  USA
//
//-----------------------------------------------------------------------el-

#ifndef UQ_INTERPOLATION_SURROGATE_BUILDER_H
#define UQ_INTERPOLATION_SURROGATE_BUILDER_H

#include <queso/SurrogateBuilderBase.h>
#include <queso/InterpolationSurrogateData.h>

namespace QUESO
{
  class GslVector;
  class GslMatrix;

  //! Build interpolation-based surrogate
  /*! Interpolation surrogates assume a structured grid. So, given the domain
      and the number of equally space points desired in each dimension, this
      class will handle calling the user's model to populate the values needed
      by the surrogate objects. User should subclass this object and implement
      the evaluate_model method. */
  template<class V = GslVector, class M = GslMatrix>
  class InterpolationSurrogateBuilder : public SurrogateBuilderBase<V>
  {
  public:

    //! Constructor
    /*! We do not take a const& to the data because we want to compute and
        set the values directly. */
    InterpolationSurrogateBuilder( InterpolationSurrogateData<V,M>& data );

    virtual ~InterpolationSurrogateBuilder(){};

    //! Execute the user's model and populate m_values for the given n_points
    void build_values();

  protected:

    InterpolationSurrogateData<V,M>& m_data;

    //! Cache the amount of work for each subenvironment
    std::vector<int> m_njobs;

    //! Ensure that if fullComm() size > 1, then n_subenvironments > 1
    /*! If not, this is a strange configuration and breaks things like inter0Comm
        and would induce redundant work. Thus, this configuration is not supported and
        we error out. */
    void check_process_config();

    //! Partition the workload of model evaluations across the subenvironments
    void partition_work();

    //! Set the starting and ending global indices for the current subenvironment
    /*! This environment will evaluate the model for indices [n_begin,n_end) */
    void set_work_bounds( unsigned int& n_begin, unsigned int& n_end ) const;

    //! Take the local values computed from each process and communicate
    /*! The end result will be that all processes have all the data.
        I.e. m_values will be completely populated on all processes. */
    void sync_data( std::vector<unsigned int>& local_n,
                    std::vector<double>& local_values );

    //! Provide the spatial coordinates for the global index n
    void set_domain_vector( unsigned int n, V& domain_vector ) const;

    //! Helper function to compute strides needed for MPI_Gatherv
    void compute_strides( std::vector<int>& strides ) const;

  private:

    InterpolationSurrogateBuilder();

  };

} // end namespace QUESO

#endif // UQ_INTERPOLATION_SURROGATE_BUILDER_H
