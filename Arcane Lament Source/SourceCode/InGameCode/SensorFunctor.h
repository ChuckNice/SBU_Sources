#pragma once
class Unit;
 class SensorFunctor
 {
   public:
	  virtual void enterCall(Unit* u)=0;
	  virtual void leaveCall(Unit* u)=0;
 };
 template <class AnyClass> class SensorSpecificFunctor : public SensorFunctor
   {
   private:
      void  (AnyClass::*enter)(Unit*);   // pointer to member function
	  void  (AnyClass::*leave)(Unit*); 
      AnyClass* pt2Object;                  // pointer to object

   public:

      // constructor - takes pointer to an object and pointer to a member and stores
      // them in two private variables
      SensorSpecificFunctor(AnyClass* _pt2Object, void  (AnyClass::*_fptr)(Unit*))
         { pt2Object = _pt2Object;  enter=_fptr;//leave=_leave; 
	  }
	  void setLeave( void  (AnyClass::*_fptr)(Unit*)){
		  leave=_fptr;
	  }
	  virtual void enterCall(Unit* u){
		(*pt2Object.*enter)(u);
	  }
	   virtual void leaveCall(Unit* u){
		(*pt2Object.*leave)(u);
	  }
 };