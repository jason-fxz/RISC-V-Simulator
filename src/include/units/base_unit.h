/**
 * @file base_unit.h
 * @author JasonFan (jasonfanxz@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-07-25
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef BASE_UNIT_H
#define BASE_UNIT_H



namespace jasonfxz {

class State;

class BaseUnit {
  public:

    // set unit state to current state
    virtual void Flush(State *cur_state) = 0;

    // base on current state, execute the instruction and update the next state
    virtual void Execute(State *cur_state, State *next_state) = 0;
};


} // namespace jasonfxz



#endif // BASE_UNIT_H
