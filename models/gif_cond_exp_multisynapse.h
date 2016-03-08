/*
 *  gif_cond_exp_multisynapse.h
 *
 *  This file is part of NEST.
 *
 *  Copyright (C) 2004 The NEST Initiative
 *
 *  NEST is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  NEST is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with NEST.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef gif_cond_exp_multisynapse_H
#define gif_cond_exp_multisynapse_H

#include "config.h"

#ifdef HAVE_GSL

// Includes from nestkernel:
#include "event.h"
#include "archiving_node.h"
#include "ring_buffer.h"
#include "connection.h"
#include "universal_data_logger.h"

// Includes from gnu gsl:
#include <gsl/gsl_errno.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_odeiv.h>

#include "nest.h"
#include "poisson_randomdev.h"
#include "gamma_randomdev.h"

/* BeginDocumentation
  Name: gif_cond_exp_multisynapse - Conductance based generalized
  integrate-and-fire neuron model according to Mensi et al. (2012)
  and Pozzorini et al. (2015)

  Description:

  gif_cond_exp_multisynapse is the generalized integrate-and-fire neuron
  according to Mensi et al. (2012)
  and Pozzorini et al. (2015), with post-synaptic conductances in the form of
  truncated exponentials.

  This model features both an adaptation current and a dynamic threshold for
  spike-frequency
  adaptation. The membrane potential (V) is described by the differential
  equation:

  C*dV(t)/dt = -g_L*(V(t)-E_L) - eta_1(t) - eta_2(t) - ... - eta_n(t) + I(t)

  where each eta_i is a spike triggered current (stc), and the neuron model can
  have arbitrary number of them.
  Dynamic of each eta_i is described by:

  Tau_eta_i*d{eta_i}/dt = -eta_i

  and in case of spike emission, its value increased by a constant (which can be
  positive or negative):

  eta_i = eta_i + q_eta_i  (in case of spike emission).

  Neuron produces spikes STOCHASTICALLY according to a point process with the
  firing intensity:

  lambda(t) = lambda0 * exp[(V(t)-V_T(t)/delta_u)]

  where V_T(t) is a time-dependent firing threshold:

  V_T(t) = V_T_star + gamma_1(t) + gamma_2(t) + ... + gamma_m(t)

  where gamma_i is a kernel of spike-frequency adaptation (sfa), and the neuron
  model can have arbitrary number of them.
  Dynamic of each gamma_i is described by:

  Tau_gamma_i*d{gamma_i}/dt = -gamma_i

  and in case of spike emission, its value increased by a constant (which can be
  positive or negative):

  gamma_i = gamma_i + q_gamma_i  (in case of spike emission).

  In the source code and parameter names we use stc and sfa, respectively
  instead of eta and gamma.

  On the postsynapic side, there can be arbitrarily many synaptic time constants
  (gif_psc_exp has exactly
  two: tau_syn_ex and tau_syn_in). This can be reached by specifying separate
  receptor ports, each for a
  different time constant. The port number has to match the respective
  "receptor_type" in the connectors.

  References:

  [1] Mensi, S., Naud, R., Pozzorini, C., Avermann, M., Petersen, C. C., &
  Gerstner, W. (2012). Parameter
  extraction and classification of three cortical neuron types reveals two
  distinct adaptation mechanisms.
  Journal of Neurophysiology, 107(6), 1756-1775.

  [2] Pozzorini, C., Mensi, S., Hagens, O., Naud, R., Koch, C., & Gerstner, W.
  (2015). Automated
  High-Throughput Characterization of Single Neurons by Means of Simplified
  Spiking Models. PLoS
  Comput Biol, 11(6), e1004275.


  Parameters:
  The following parameters can be set in the status dictionary.

  Membrane Parameters:
    C_m        double - Capacity of the membrane in pF
    t_ref      double - Duration of refractory period in ms.
    V_reset    double - Reset value after a spike in mV.
    E_L        double - Leak reversal potential in mV.
    g_L        double - Leak conductance in nS.
    I_e        double - Constant external input current in pA.

  Spike adaptation and firing intensity parameters:
    q_stc      vector of double - Values added to spike triggered currents (stc)
  after each spike emission in nA.
    tau_stc    vector of double - Time constants of stc variables in ms.
    q_sfa      vector of double - Values added to spike-frequency adaptation
  (sfa) after each spike emission in mV.
    tau_sfa    vector of double - Time constants of sfa variables in ms.
    delta_u    double - Stochasticity level in mV.
    lambda0    double - Stochastic intensity at firing threshold V_T in Hz.
    v_t_star   double - Minimum threshold in mV

  Synaptic parameters
    taus_syn   vector of double - Time constants of the synaptic conductance in
  ms (exp function).
    E_ex       double - Excitatory reversal potential in mV.
    E_in       double - Inhibitory reversal potential in mV.


  Sends: SpikeEvent

  Receives: SpikeEvent, CurrentEvent, DataLoggingRequest

  Author: March 2016, Setareh
  SeeAlso: pp_psc_delta, gif_cond_exp, gif_cond_exp, gif_cond_exp_multisynapse
*/

namespace nest
{

extern "C" int gif_cond_exp_multisynapse_dynamics( double, const double*, double*, void* );

class gif_cond_exp_multisynapse : public Archiving_Node
{

public:
  gif_cond_exp_multisynapse();
  gif_cond_exp_multisynapse( const gif_cond_exp_multisynapse& );
  ~gif_cond_exp_multisynapse();

  /**
   * Import sets of overloaded virtual functions.
   * @see Technical Issues / Virtual Functions: Overriding, Overloading, and
   * Hiding
   */
  using Node::handle;
  using Node::handles_test_event;

  port send_test_event( Node&, rport, synindex, bool );

  void handle( SpikeEvent& );
  void handle( CurrentEvent& );
  void handle( DataLoggingRequest& );

  port handles_test_event( SpikeEvent&, rport );
  port handles_test_event( CurrentEvent&, rport );
  port handles_test_event( DataLoggingRequest&, rport );


  void get_status( DictionaryDatum& ) const;
  void set_status( const DictionaryDatum& );

private:
  void init_state_( const Node& proto );
  void init_buffers_();
  void calibrate();

  void update( Time const&, const long_t, const long_t );

  // make dynamics function quasi-member
  friend int gif_cond_exp_multisynapse_dynamics( double, const double*, double*, void* );

  // The next two classes need to be friends to access the State_ class/member
  friend class RecordablesMap< gif_cond_exp_multisynapse >;
  friend class UniversalDataLogger< gif_cond_exp_multisynapse >;

  // ----------------------------------------------------------------

  /**
   * Independent parameters of the model.
   */
  struct Parameters_
  {

    double_t g_L_;
    double_t E_L_;
    double_t V_reset_;
    double_t delta_u_;
    double_t v_t_star_;
    double_t lambda0_;

    /** Refractory period in ms. */
    double_t t_ref_;

    /** Membrane capacitance in pF. */
    double_t c_m_;

    /** List of spike triggered current time constant in ms. */
    std::vector< double_t > tau_stc_;

    /** List of spike triggered current jumps in nA. */
    std::vector< double_t > q_stc_;

    /** List of adaptive threshold time constant in ms. */
    std::vector< double_t > tau_sfa_;

    /** List of adaptive threshold jumps in mV. */
    std::vector< double_t > q_sfa_;

    /** Time constants of synaptic currents in ms */
    std::vector< double_t > tau_syn_;

    /** External DC current. */
    double_t I_e_;

    /** type is long because other types are not put through in GetStatus */
    std::vector< long > receptor_types_;
    size_t num_of_receptors_;

    /** Reversal potential of synapses in mV */
    double_t E_ex_; //!< Excitatory reversal Potential in mV
    double_t E_in_; //!< Inhibitory reversal Potential in mV

    /** boolean flag which indicates whether the neuron has connections */
    bool has_connections_;


    Parameters_(); //!< Sets default parameter values

    void get( DictionaryDatum& ) const; //!< Store current values in dictionary
    void set( const DictionaryDatum& ); //!< Set values from dictionary
  };

  // ----------------------------------------------------------------

  /**
   * State variables of the model.
   */
  struct State_
  {

    //! Symbolic indices to the elements of the state vector y
    enum StateVecElems
    {
      V_M = 0,
      G_EXC,
      G_INH,
      STATE_VEC_SIZE
    };

    static const size_t NUMBER_OF_FIXED_STATES_ELEMENTS = 1; // V_M

    std::vector< double_t > y_; //!< neuron state

    double_t y0_;  //!< This is piecewise constant external current
    double_t q_;   //!< This is the change of the 'threshold' due to adaptation.
    double_t stc_; // Spike triggered current.

    std::vector< double_t > q_sfa_elems_; // Vector of adaptation parameters.
    std::vector< double_t > q_stc_elems_; // Vector of spike triggered parameters.

    int_t r_ref_; // absolute refractory counter (no membrane potential propagation)

    bool initialized_; // it is true if the vectors are initialized
    bool add_stc_sfa_; // in case of true, the stc and sfa ampplitudes should be
                       // added

    State_( const Parameters_& ); //!< Default initialization
    State_( const State_& );
    State_& operator=( const State_& );

    void get( DictionaryDatum&, const Parameters_& ) const;
    void set( const DictionaryDatum&, const Parameters_& );
  };

  // ----------------------------------------------------------------

  /**
   * Buffers of the model.
   */
  struct Buffers_
  {
    Buffers_( gif_cond_exp_multisynapse& );
    Buffers_( const Buffers_&, gif_cond_exp_multisynapse& );

    /** buffers and sums up incoming spikes/currents */
    std::vector< RingBuffer > spike_exc_;
    std::vector< RingBuffer > spike_inh_;
    RingBuffer currents_;

    //! Logger for all analog data
    UniversalDataLogger< gif_cond_exp_multisynapse > logger_;

    /** GSL ODE stuff */
    gsl_odeiv_step* s_;    //!< stepping function
    gsl_odeiv_control* c_; //!< adaptive stepsize control function
    gsl_odeiv_evolve* e_;  //!< evolution function
    gsl_odeiv_system sys_; //!< struct describing system

    // IntergrationStep_ should be reset with the neuron on ResetNetwork,
    // but remain unchanged during calibration. Since it is initialized with
    // step_, and the resolution cannot change after nodes have been created,
    // it is safe to place both here.
    double_t step_;          //!< step size in ms
    double IntegrationStep_; //!< current integration time step, updated by GSL
  };

  // ----------------------------------------------------------------

  /**
   * Internal variables of the model.
   */
  struct Variables_
  {

    std::vector< double_t > Q33_; // for sfa
    std::vector< double_t > Q44_; // for stc

    double_t h_; //!< simulation time step in ms

    librandom::RngPtr rng_;                   // random number generator of my own thread
    librandom::PoissonRandomDev poisson_dev_; // random deviate generator
    librandom::GammaRandomDev gamma_dev_;     // random deviate generator


    int_t RefractoryCounts_;
  };

  // Access functions for UniversalDataLogger -----------------------


  //! Read out state vector elements, used by UniversalDataLogger
  template < State_::StateVecElems elem >
  double_t
  get_y_elem_() const
  {
    return S_.y_[ elem ];
  }

  //! Read out the adaptive threshold potential
  double_t
  get_E_sfa_() const
  {
    return S_.q_;
  }

  // ----------------------------------------------------------------

  /**
   * Instances of private data structures for the different types
   * of data pertaining to the model.
   * @note The order of definitions is important for speed.
   * @{
   */
  Parameters_ P_;
  State_ S_;
  Variables_ V_;
  Buffers_ B_;
  /** @} */

  //! Mapping of recordables names to access functions
  static RecordablesMap< gif_cond_exp_multisynapse > recordablesMap_;
};

inline port
gif_cond_exp_multisynapse::handles_test_event( SpikeEvent&, rport receptor_type )
{
  if ( receptor_type <= 0 || receptor_type > static_cast< port >( P_.num_of_receptors_ ) )
    throw IncompatibleReceptorType( receptor_type, get_name(), "SpikeEvent" );

  P_.has_connections_ = true;
  return receptor_type;
}

inline port
gif_cond_exp_multisynapse::send_test_event( Node& target, rport receptor_type, synindex, bool )
{
  SpikeEvent e;
  e.set_sender( *this );

  return target.handles_test_event( e, receptor_type );
}

inline port
gif_cond_exp_multisynapse::handles_test_event( CurrentEvent&, rport receptor_type )
{
  if ( receptor_type != 0 )
    throw UnknownReceptorType( receptor_type, get_name() );
  return 0;
}

inline port
gif_cond_exp_multisynapse::handles_test_event( DataLoggingRequest& dlr, rport receptor_type )
{
  if ( receptor_type != 0 )
    throw UnknownReceptorType( receptor_type, get_name() );
  return B_.logger_.connect_logging_device( dlr, recordablesMap_ );
}

inline void
gif_cond_exp_multisynapse::get_status( DictionaryDatum& d ) const
{
  P_.get( d );
  S_.get( d, P_ );
  Archiving_Node::get_status( d );
  ( *d )[ names::recordables ] = recordablesMap_.get_list();
}

inline void
gif_cond_exp_multisynapse::set_status( const DictionaryDatum& d )
{
  Parameters_ ptmp = P_; // temporary copy in case of errors
  ptmp.set( d );         // throws if BadProperty
  State_ stmp = S_;      // temporary copy in case of errors
  stmp.set( d, ptmp );   // throws if BadProperty

  // We now know that (ptmp, stmp) are consistent. We do not
  // write them back to (P_, S_) before we are also sure that
  // the properties to be set in the parent class are internally
  // consistent.
  Archiving_Node::set_status( d );

  // if we get here, temporaries contain consistent set of properties
  P_ = ptmp;
  S_ = stmp;
}

} // namespace

#endif // HAVE_GSL
#endif /* #ifndef gif_cond_exp_multisynapse_H */
