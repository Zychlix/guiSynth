#pragma once

float f_sym_constraint(float x, float constraint)
{
    if(x>constraint) return constraint;
    if(x<-constraint) return -constraint;
    return x;
}