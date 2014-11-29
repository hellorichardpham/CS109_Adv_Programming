
// Mustapha Hamade - mhamade
// asg2

#include <cstdlib>
#include <exception>
#include <limits>
#include <stack>
#include <stdexcept>
#include <sstream>

using namespace std;

#include "bigint.h"
#include "trace.h"

#define CDTOR_TRACE TRACE ('~', this << " -> " << small_value)

bigint::bigint (): big_value (new bigvalue_t()) {
}

bigint::bigint (const bigint &that) {
   *this = that;
}

bigint &bigint::operator= (const bigint &that) {
   if (this == &that){
      return *this;
   }
   
   TRACE('~', "operator= that " << that << " this = " << this);
   bigvalue_t copyVec;
   bigitor begin = that.big_value->begin();
   bigitor end = that.big_value->end();
   while (begin!=end) {
      copyVec.push_back(*begin);
      begin++;
   }
   
   big_value = new bigvalue_t(copyVec);
   TRACE('~', "this = " << *this << " this = " << this);
   
   this->negative = that.negative;
   return *this;
   
}

bigint::~bigint() {
  
  TRACE('~', cout << *this);
  big_value->clear();
  delete big_value;
  this->negative = false;
  big_value = NULL;
  
}

void bigint::int_toBigint(int number) {
   int num_set = 0;
   if (number != 0) {
      while (number > 0) {
         num_set = number % 10;
         big_value->push_back((unsigned char)num_set);
         number = number - num_set;
         number /= 10;
      }
   }
}

bigint::bigint (int that) : big_value (new bigvalue_t()) {
  TRACE ('b', that);
  if (that < 0) {
    this->negative = true;
    that = that * -1;
  }
  else {
    this->negative = false;
  }
  int_toBigint(that);
}

void bigint::pop_zero() const {
  bigvalue_t::reverse_iterator itor = big_value->rbegin();
  bigvalue_t::reverse_iterator end = big_value->rend();
  while (*itor == 0 && itor != end) {
    itor++;
    big_value->pop_back();
  }
}

bigint::bigint (const string &that) : big_value(new bigvalue_t()) {
   TRACE ('b', that);
   string::const_iterator itor = that.begin();
   string::const_iterator end = that.end();
   this->negative = false;
   if (*itor == '_') {
      this->negative = true;
      ++itor;
   }
   --end;
   int value = 0;
   for (; end >= itor; --end) {
      value = value * 10 + *end - '0';
      big_value->push_back(value);
      value = 0;
   }
   this->pop_zero();
}

bool bigint::getFlag() const {
   return negative;
}

void bigint::setFlag(bool value) {
   negative = value;
}


bigint bigint::do_bigadd(const bigint &that) const{
   bigint result = 0;
   result.big_value->clear();
   result.negative = false;
   bigvalue_t::iterator itor1 = big_value->begin();
   bigvalue_t::iterator itor2 = that.big_value->begin();
   bigvalue_t::iterator end1 = big_value->end();
   bigvalue_t::iterator end2 = that.big_value->end();
   int carry = 0;
   while (itor1 != end1 || itor2 != end2 || carry != 0) {
      int val1 = 0;
      int val2 = 0;
      if (itor1 != end1) { val1 = (int)*itor1; ++itor1;}
      if (itor2 != end2) { val2 = (int)*itor2; ++itor2;}
      int sum = val1 + val2 + carry;
      carry = 0;
      if (sum > 9) {
         sum %= 10;
         carry = 1;
      }
      result.big_value->push_back(sum);
   }
   return result;
}


bigint bigint::do_bigsub(const bigint &that) const {
   bigint result = 0;
   result.big_value->clear();
   result.negative = false;
   bigvalue_t::iterator itor1 = big_value->begin();
   bigvalue_t::iterator itor2 = that.big_value->begin();
   bigvalue_t::iterator end1 = big_value->end();
   bigvalue_t::iterator end2 = that.big_value->end();
   int borrow = 0;
   while (itor1 != end1 || itor2 != end2 || borrow != 0){
      int val1 = 0;
      int val2 = 0;
      if (itor1 != end1) { val1 = (int)*itor1; ++itor1; }
      if (itor2 != end2) { val2 = (int)*itor2; ++itor2; }
      int sub = val1 - val2 - borrow;
      borrow = 0;
      if (sub  < 0) {
         sub += 10;
         borrow = 1;
      }
      result.big_value->push_back(sub);
   }
   result.pop_zero();
   return result;
}



bigint bigint::operator+ (const bigint &that) const {
  bigint result;
  //signs are the same then do_bigadd
  if (this->negative == that.negative) {
     result = this->do_bigadd(that);
     result.negative = this->negative;
  }
  else {
     if (abscompare(that) >= 0) {
        result = this->do_bigsub(that);
     }
     else {
         result = that.do_bigsub(*this);
         result.negative = this->getFlag();
         result.negative = result.negative ? false : true;
     }
  }
  return result;
}

bigint bigint::operator- (const bigint &that) const {
   //return this->small_value - that.small_value;
   bigint result = 0;
   result.negative = getFlag();
   if (negative == that.negative) {
      if (abscompare(that) >= 0){
         result = this->do_bigsub(that);
      }
      else {
         result = that.do_bigsub(*this);
         result.negative = this->getFlag();
         result.negative = result.negative ? false : true;
      }
   }
   else {
      result = this->do_bigadd(that);
      if (compare(that) < 0) {
         result.negative = this->getFlag();
         result.negative = result.negative ? false : true;
      }
   }
   return result;
}

bigint bigint::operator- () const {
   //return -small_value;
   bigint result = *this;
   result.negative = getFlag();
   result.negative = result.negative ? false : true;
   return result;
}

int bigint::compare (const bigint &that) const {
   if (!negative && that.negative) {
      return 1;
   }
   else if (negative && !that.negative) {
      return -1;
   }
   else {
      if (negative && that.negative) {
         if (abscompare(that) > 0) {
            return -1;
         }
         else {
            return 1;
         }
      }
      else {
         if (abscompare(that) > 0) {
            return 1;
         }
         else {
            return -1;
         }
      }
   }
   return 0;
}

int bigint::abscompare (const bigint &that) const {
   if (this->big_value->size() < that.big_value->size()) {
      return -1;
   }
   else if (this->big_value->size() > that.big_value->size()) {
      return 1;
   }
   else {
      bigvalue_t::reverse_iterator itor1 = this->big_value->rbegin();
      bigvalue_t::reverse_iterator itor2 = that.big_value->rbegin();
      for (; itor1 != this->big_value->rend(); itor1++) {
         if (*itor1 > *itor2) {
            return 1;
         }
         else if (*itor1 < *itor2) {
            return -1;
         }
         itor2++;
      }
      return 0;
   }
}

int bigint::smallint () const {
   return 0;
}

bigint bigint::mul_by_2 () {
   //return this->small_value *= 2;
   this->negative = false;
   bigint result = bigint(this->do_bigadd(*this));
   return result;
}

static bigpair popstack (stack <bigpair> &the_stack) {
   bigpair result = the_stack.top ();
   the_stack.pop();
   return result;
}


//
// Multiplication algorithm.
//
bigint bigint::operator* (const bigint &that) const {
   bigint top = that;
   bigint count = 1;

   stack <bigpair> the_stack;
   while (count.abscompare (*this) <= 0) {

      the_stack.push (bigpair (count, top));
      count = count + count;
      top = top + top;
   }
   count = *this;
   count.negative = false;
   bigint result = 0;
   
 
   while (! the_stack.empty ()) {
      bigpair top = popstack (the_stack);

      if (top.first <= count) {
         count = count - top.first;
         result = result + top.second;

      }
   }

   if ((*this < 0) != (that < 0)) result = - result;
   return result;
}


//
// Division algorithm.
//
bigpair bigint::div_rem (const bigint &that) const {
   if (that == 0) throw range_error ("divide by 0");
   bigint counter = 1;
   bigint top = that;
   top.negative = false;
   TRACE ('/', *this << " /% " << that);
   stack <bigpair> the_stack;
   while (counter < *this) {
      TRACE ('*', "push: " << counter << ", " << top);
      the_stack.push (bigpair (counter, top));
      counter = counter + counter;
      top = top + top;
   }
   bigint quotient = 0;
   bigint remainder = *this;
   remainder.negative = false;
   while (! the_stack.empty ()) {
      bigpair top = popstack (the_stack);
      TRACE ('*', "pop: " << top.first << ", " << top.second);
      if (top.second <= remainder) {
         quotient = quotient + top.first;
         remainder = remainder - top.second;
         TRACE ('*', "quo = " << quotient << ", rem = " << remainder);
      }
   }
   if ((*this < 0) != (that < 0)) quotient = - quotient;
   if (*this < 0) remainder = -remainder;
   return bigpair (quotient, remainder);
}

bigint bigint::operator/ (const bigint &that) const {
   return div_rem (that).first;
}

bigint bigint::operator% (const bigint &that) const {
   return div_rem (that).second;
}


#define TRACE_POW TRACE ('^', "result: " << result \
                  << ", base: " << base << ", expt: " << expt);
bigint bigint::pow (const bigint &that) const {
   bigint base = *this;
   bigint expt = that;
   bigint result = 1;
   TRACE_POW;
   if (expt < 0) {
      base = 1 / base;
      expt = - expt;
   }
   while (expt > 0) {
      TRACE_POW;
      if (*(expt.big_value->begin()) & 1) { //odd
         result = result * base;
         expt = expt - 1;
      }else { //even
         base = base * base;
         expt = expt / 2;
      }
   }
   TRACE_POW;
   return result;
}

//
// Macros can make repetitive code easier.
//

#define COMPARE(OPER) \
   bool bigint::operator OPER (const bigint &that) const { \
      return compare (that) OPER 0; \
   }
COMPARE (==)
COMPARE (!=)
COMPARE (< )
COMPARE (<=)
COMPARE (> )
COMPARE (>=)

#define INT_LEFT(RESULT,OPER) \
   RESULT operator OPER (int left, const bigint &that) { \
      return bigint (left) OPER that; \
   }
INT_LEFT (bigint, +)
INT_LEFT (bigint, -)
INT_LEFT (bigint, *)
INT_LEFT (bigint, /)
INT_LEFT (bigint, %)
INT_LEFT (bool, ==)
INT_LEFT (bool, !=)
INT_LEFT (bool, < )
INT_LEFT (bool, <=)
INT_LEFT (bool, > )
INT_LEFT (bool, >=)

ostream &operator<< (ostream &out, const bigint &that) {
   //out << that.small_value;
   //return out;
   int counter = 0;
   if (that.big_value->empty()) {
   out << "0";
   }
   else {
      if (that.negative) {
         cout << "-";
         counter++;
      }
      bigitor itor = that.big_value->end();
      itor--;
      while (itor >= that.big_value->begin()) {
         if (counter > 68) {
            out << "\\" << endl;
            counter = 0;
         }
         out << (int)*itor;
            counter++;
         itor--;
      }
   }
   
   return out;
}

