#ifndef LINEAR_H
#define LINEAR_H

#include "Element.h"

//Forward Declarations
class SymmElasticityTensor;
namespace elk
{
namespace solid_mechanics
{

class Linear : public Element
{
public:
  Linear(const std::string & name, InputParameters parameters);
  virtual ~Linear();

protected:

  virtual void computeStrain( const unsigned qp,
                              const SymmTensor & total_strain_old,
                              SymmTensor & total_strain_new,
                              SymmTensor & strain_increment );

  VariableGradient & _grad_disp_x;
  VariableGradient & _grad_disp_y;
  VariableGradient & _grad_disp_z;

};

}
}

#endif
