//
// Generated file, do not edit! Created by nedtool 5.6 from Packets/HLPacket.msg.
//

#ifndef __HLPACKET_M_H
#define __HLPACKET_M_H

#if defined(__clang__)
#  pragma clang diagnostic ignored "-Wreserved-id-macro"
#endif
#include <omnetpp.h>

// nedtool version check
#define MSGC_VERSION 0x0506
#if (MSGC_VERSION!=OMNETPP_VERSION)
#    error Version mismatch! Probably this file was generated by an earlier version of nedtool: 'make clean' should help.
#endif



/**
 * Class generated from <tt>Packets/HLPacket.msg:16</tt> by nedtool.
 * <pre>
 * packet HLPacket
 * {
 *     int source;
 *     int destination;
 *     int hopCnt;
 *     char payload[];
 * }
 * </pre>
 */
class HLPacket : public ::omnetpp::cPacket
{
  protected:
    int source;
    int destination;
    int hopCnt;
    char *payload; // array ptr
    unsigned int payload_arraysize;

  private:
    void copy(const HLPacket& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const HLPacket&);

  public:
    HLPacket(const char *name=nullptr, short kind=0);
    HLPacket(const HLPacket& other);
    virtual ~HLPacket();
    HLPacket& operator=(const HLPacket& other);
    virtual HLPacket *dup() const override {return new HLPacket(*this);}
    virtual void parsimPack(omnetpp::cCommBuffer *b) const override;
    virtual void parsimUnpack(omnetpp::cCommBuffer *b) override;

    // field getter/setter methods
    virtual int getSource() const;
    virtual void setSource(int source);
    virtual int getDestination() const;
    virtual void setDestination(int destination);
    virtual int getHopCnt() const;
    virtual void setHopCnt(int hopCnt);
    virtual void setPayloadArraySize(unsigned int size);
    virtual unsigned int getPayloadArraySize() const;
    virtual char getPayload(unsigned int k) const;
    virtual void setPayload(unsigned int k, char payload);
};

inline void doParsimPacking(omnetpp::cCommBuffer *b, const HLPacket& obj) {obj.parsimPack(b);}
inline void doParsimUnpacking(omnetpp::cCommBuffer *b, HLPacket& obj) {obj.parsimUnpack(b);}


#endif // ifndef __HLPACKET_M_H

