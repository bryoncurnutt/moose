/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/

#include "PorousFlow2PhasePS_VG.h"

template<>
InputParameters validParams<PorousFlow2PhasePS_VG>()
{
  InputParameters params = validParams<PorousFlow2PhasePS>();
  params.addRequiredRangeCheckedParam<Real>("sat_lr", "sat_lr >= 0 & sat_lr <= 1", "Liquid residual saturation.  Must be between 0 and 1");
  params.addRequiredRangeCheckedParam<Real>("sat_ls", "sat_ls >= 0 & sat_ls <= 1", "Liquid fully saturated saturation.  Must be between 0 and 1");
  params.addRequiredRangeCheckedParam<Real>("m", "m >= 0 & m <= 1", "van Genuchten exponent m");
  params.addRequiredParam<Real>("pc_max", "Maximum capillary pressure");
  params.addRequiredParam<Real>("p0", "Capillary pressure coefficient P0");
  params.addClassDescription("This Material calculates the 2 porepressures and the 2 saturations in a 2-phase isothermal situation using a van Genucten capillary pressure, and derivatives of these with respect to the PorousFlowVariables");
  return params;
}

PorousFlow2PhasePS_VG::PorousFlow2PhasePS_VG(const InputParameters & parameters) :
    PorousFlow2PhasePS(parameters),

    _sat_lr(getParam<Real>("sat_lr")),
    _sat_ls(getParam<Real>("sat_ls")),
    _m(getParam<Real>("m")),
    _pc_max(getParam<Real>("pc_max")),
    _p0(getParam<Real>("p0"))
{
  if (_dictator.numPhases() != 2)
    mooseError("The Dictator proclaims that the number of phases is " << _dictator.numPhases() << " whereas PorousFlow2PhasePS_VG can only be used for 2-phase simulation.  Be aware that the Dictator has noted your mistake.");
}

Real
PorousFlow2PhasePS_VG::capillaryPressure(Real saturation) const
{
  return PorousFlowCapillaryPressureVG::capillaryPressure(saturation, _m, _sat_lr, _sat_ls, _p0, _pc_max);
}

Real
PorousFlow2PhasePS_VG::dCapillaryPressure_dS(Real saturation) const
{
  return PorousFlowCapillaryPressureVG::dCapillaryPressure(saturation, _m, _sat_lr, _sat_ls, _p0, _pc_max);
}

Real
PorousFlow2PhasePS_VG::d2CapillaryPressure_dS2(Real saturation) const
{
  return PorousFlowCapillaryPressureVG::d2CapillaryPressure(saturation, _m, _sat_lr, _sat_ls, _p0, _pc_max);
}
