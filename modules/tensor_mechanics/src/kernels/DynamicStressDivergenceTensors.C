/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/
#include "DynamicStressDivergenceTensors.h"
#include "ElasticityTensorTools.h"

template<>
InputParameters validParams<DynamicStressDivergenceTensors>()
{
  InputParameters params = validParams<StressDivergenceTensors>();
  params.addClassDescription("Residual due to stress related Rayleigh damping and HHT time integration terms ");
  params.addParam<Real>("zeta", 0, "zeta parameter for the Rayleigh damping");
  params.addParam<Real>("alpha", 0, "alpha parameter for HHT time integration");
  return params;
}

DynamicStressDivergenceTensors::DynamicStressDivergenceTensors(const InputParameters & parameters) :
    StressDivergenceTensors(parameters),
    _stress_older(getMaterialPropertyOlderByName<RankTwoTensor>(_base_name + "stress")),
    _stress_old(getMaterialPropertyOldByName<RankTwoTensor>(_base_name + "stress")),
    _zeta(getParam<Real>("zeta")),
    _alpha(getParam<Real>("alpha"))
{
}

Real
DynamicStressDivergenceTensors::computeQpResidual()
{
  /**
  *This kernel needs to be used only if either Rayleigh damping or numerical damping through HHT
  *time integration scheme needs to be added to the problem thorugh the stiffness dependent damping
  * parameter _zeta or HHT parameter _alpha, respectively.
  *
  * The residual of _zeta*K*[(1+_alpha)vel-_alpha vel_old]+ alpha K [ u - uold] + K u is required
  * = _zeta*[(1+_alpha)d/dt (Div sigma)-alpha d/dt(Div sigma_old)] +alpha [Div sigma - Div sigma_old]+ Div sigma
  * = _zeta*[(1+alpha)(Div sigma - Div sigma_old)/dt - alpha (Div sigma_old - Div sigma_older)/dt]
  *   + alpha [Div sigma - Div sigma_old] +Div sigma
  * = [(1+_alpha)*_zeta/dt +_alpha+1]* Div sigma - [(1+2_alpha)*_zeta/dt + _alpha] Div sigma_old + _alpha*_zeta/dt Div sigma_older
  */
  if ((_dt > 0))
    return _stress[_qp].row(_component) * _grad_test[_i][_qp]*(1+_alpha+(1+_alpha)*_zeta/_dt)
      -(_alpha+(1+2*_alpha)*_zeta/_dt)*_stress_old[_qp].row(_component)* _grad_test[_i][_qp]
      + (_alpha*_zeta/_dt)*_stress_older[_qp].row(_component)*_grad_test[_i][_qp];
  else
    return 0;
}

Real
DynamicStressDivergenceTensors::computeQpJacobian()
{
  if (_dt > 0)
    return ElasticityTensorTools::elasticJacobian(_Jacobian_mult[_qp], _component, _component, _grad_test[_i][_qp], _grad_phi[_j][_qp])*(1+_alpha+_zeta/_dt);
  else
    return 0;
}

Real
DynamicStressDivergenceTensors::computeQpOffDiagJacobian(unsigned int jvar)
{
  unsigned int coupled_component = 0;
  bool active = true;

  for (unsigned int i = 0; i < _ndisp; ++i)
    if (jvar == _disp_var[i])
    {
      coupled_component = i;
      active = true;
    }

  if (active)
  {
    if (_dt > 0)
      return ElasticityTensorTools::elasticJacobian(_Jacobian_mult[_qp], _component, coupled_component, _grad_test[_i][_qp], _grad_phi[_j][_qp])*(1+_alpha+_zeta/_dt);
    else
      return 0;
   }
  if (_temp_coupled && jvar == _temp_var)
    return 0.0;

  return 0;
}
