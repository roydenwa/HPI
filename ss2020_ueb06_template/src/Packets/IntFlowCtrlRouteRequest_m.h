//
// Generated file, do not edit! Created by nedtool 5.6 from Packets/IntFlowCtrlRouteRequest.msg.
//

#ifndef __INTFLOWCTRLROUTEREQUEST_M_H
#define __INTFLOWCTRLROUTEREQUEST_M_H

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
#include "IntFlowCtrl_m.h"
// }}

/**
 * Class generated from <tt>Packets/IntFlowCtrlRouteRequest.msg:22</tt> by nedtool.
 * <pre>
 * packet IntFlowCtrlRouteRequest extends IntFlowCtrl
 * {
 *     int source;
 *     int destination;
 *     int action = INT_ROUTE_REQUEST;
 * }
 * </pre>
 */
class IntFlowCtrlRouteRequest : public ::IntFlowCtrl
{
  protected:
    int source;
    int destination;
    int action;

  private:
    void copy(const IntFlowCtrlRouteRequest& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const IntFlowCtrlRouteRequest&);

  public:
    IntFlowCtrlRouteRequest(const char *name=nullptr, short kind=0);
    IntFlowCtrlRouteRequest(const IntFlowCtrlRouteRequest& other);
    virtual ~IntFlowCtrlRouteRequest();
    IntFlowCtrlRouteRequest& operator=(const IntFlowCtrlRouteRequest& other);
    virtual IntFlowCtrlRouteRequest *dup() const override {return new IntFlowCtrlRouteRequest(*this);}
    virtual void parsimPack(omnetpp::cCommBuffer *b) const override;
    virtual void parsimUnpack(omnetpp::cCommBuffer *b) override;

    // field getter/setter methods
    virtual int getSource() const;
    virtual void setSource(int source);
    virtual int getDestination() const;
    virtual void setDestination(int destination);
    virtual int getAction() const;
    virtual void setAction(int action);
};

inline void doParsimPacking(omnetpp::cCommBuffer *b, const IntFlowCtrlRouteRequest& obj) {obj.parsimPack(b);}
inline void doParsimUnpacking(omnetpp::cCommBuffer *b, IntFlowCtrlRouteRequest& obj) {obj.parsimUnpack(b);}


#endif // ifndef __INTFLOWCTRLROUTEREQUEST_M_H

