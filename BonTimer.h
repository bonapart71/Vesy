//#pragma once
//#include <Arduino.h>

class BonTimer
{
  private:
    bool _started = false;
    bool _repeat = true;
    unsigned long _delay = 0;
    unsigned long _currentTime = 0;
    unsigned long _lastTime = 0;

  public:
    BonTimer(unsigned long delay = 1000)  //default delay 1000
    {
      //set default values
      _delay = delay;
      _started = false;
      _started = false;
      _repeat = false;
    }


    // The method starts the timer at the specified interval

    void start(unsigned long delay) // If delay=0, then do not change delay
    {
      _delay = delay;
      _started = true;
      _repeat = false;
      _lastTime = millis();
    }

    // The method stops the timer

    void stop()
    {
      _started = false;
    }

    // The method starts a timer at the specified interval and sets the repeat mode.
    // The timer will start automatically after successfully reading its state.

    void startAndRepeat(unsigned long delay)  // If dalay=0, then do not change delay
    {
      _delay = delay;
      _started = true;
      _repeat = true;
      _lastTime = millis();
    }

    // The method returns "true" when the specified interval has elapsed.
    // If the timer is not running, the method returns "true"
    // "False" is returned until the interval has expired.

    // If the timer is started by the "startAndRepeat" method, then the method returns "true" once,
    // and then starts the timer again.
    // If the doNextStep flag is set, then the timer will not start again until the next execution of the method.
    // Thus, you can poll the status of the timer in several places.

    // The method takes into account the overflow of the millis function

    bool timesUp(bool doNextStep = true) //if DoNextStep=false,timer do not go to next period.
    {
      if (_started)
      {
        _currentTime = millis();
        if (_currentTime > _lastTime)
        {
          if ((_currentTime - _lastTime) > _delay)
          {
            // _lastTime=_currentTime;   //
            if (doNextStep) {
              _lastTime = _lastTime + _delay; //for accurate calculation of delay periods
              _started = _repeat;  //disable timer if repeat is disabled
            }
            return true;

          }
        }
        else
        { //after 50 day millis overflow and this chek for this case
          if ((_lastTime + _delay) < _currentTime)
          {
            // _lastTime=_currentTime;   //
            if (doNextStep) {
              _lastTime = _lastTime + _delay; //for accurate calculation of delay periods
              _started = _repeat; //disable timer if repeat is disabled
            }
            return true;
          }
        };
        return false;
      }
      return true;
    }


};
