#include "MotorVelocityControl.h"
constexpr double M_PI = 3.14159265358979323846;
constexpr double US_TO_S = (1.0f/(1000.0f*1000.0f));
constexpr double COUNT_TO_RAD = M_PI/10.0f;
constexpr float TICKS_US_TO_RAD_S = COUNT_TO_RAD / US_TO_S;
constexpr float RAD_S_TO_TICKS_US = 1.0f/TICKS_US_TO_RAD_S;

namespace robopi
{

    float SlidingAverageFilter::estimate(double pos, double dT)
    {

        const double dPos = pos - _posLast;

        _posLast = pos;

        const double v = dPos/dT;

        _v -= _vs[_idx] / _size;

        _vs[_idx] = v;

        _v += _vs[_idx] / _size;

        _idx++;
        
        if(_idx >= _size)
        {
            _idx = 0;
        }

        return static_cast<float>(_v);
    }

    float LuenbergerObserver::estimate(double pos, double dT)
    {

        _pos += _v * dT;

        double err = pos - _pos;

        _velIntegr += _ki * err * dT;

        _v = _kp * err + _velIntegr;

        return static_cast<float>(_v);

    }

    void MotorVelocityControl::update(float dT)
    {

        if (dT <= 0.0)
        {
            return;
        }
        const double pos = static_cast<double>(_encoder->wheelTicks())*COUNT_TO_RAD;
      
        _velocityActual = _velEstimator->estimate(pos, static_cast<double>(dT));


        float err = _velocitySet - _velocityActual;


        _errIntegr += err * dT;

        _dutySet = _kp * err + _ki * _errIntegr + _kd * (err - _errLast)/dT;

        _errLast = err;

        _encoder->setDirection(_dutySet > 0);
        _motor->set(_dutySet);
    }

    
    void MotorVelocityControl::set(float velocity) {
        
        _velocitySet = velocity;

    }

    

}		