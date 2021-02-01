//
// Generated file, do not edit! Created by nedtool 5.6 from Snail.msg.
//

#ifndef __SNAIL_M_H
#define __SNAIL_M_H

#if defined(__clang__)
#  pragma clang diagnostic ignored "-Wreserved-id-macro"
#endif
#include <omnetpp.h>

// nedtool version check
#define MSGC_VERSION 0x0506
#if (MSGC_VERSION!=OMNETPP_VERSION)
#    error Version mismatch! Probably this file was generated by an earlier version of nedtool: 'make clean' should help.
#endif



// cplusplus {{
#include "Set.h"
// }}

/**
 * Class generated from <tt>Snail.msg:21</tt> by nedtool.
 * <pre>
 * message Snail
 * {
 *     int distance;
 * 
 *     Set Z;
 * }
 * </pre>
 */
class Snail : public ::omnetpp::cMessage
{
  protected:
    int distance;
    Set Z;

  private:
    void copy(const Snail& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const Snail&);

  public:
    Snail(const char *name=nullptr, short kind=0);
    Snail(const Snail& other);
    virtual ~Snail();
    Snail& operator=(const Snail& other);
    virtual Snail *dup() const override {return new Snail(*this);}
    virtual void parsimPack(omnetpp::cCommBuffer *b) const override;
    virtual void parsimUnpack(omnetpp::cCommBuffer *b) override;

    // field getter/setter methods
    virtual int getDistance() const;
    virtual void setDistance(int distance);
    virtual Set& getZ();
    virtual const Set& getZ() const {return const_cast<Snail*>(this)->getZ();}
    virtual void setZ(const Set& Z);
};

inline void doParsimPacking(omnetpp::cCommBuffer *b, const Snail& obj) {obj.parsimPack(b);}
inline void doParsimUnpacking(omnetpp::cCommBuffer *b, Snail& obj) {obj.parsimUnpack(b);}


#endif // ifndef __SNAIL_M_H

