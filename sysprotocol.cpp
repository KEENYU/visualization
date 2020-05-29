#include "sysprotocol.h"
#include "xmodemprotocol.h"

#include <string>

void createCmdPacket(CmdPacketParameter *p_cmdPacketParameter)
{
    p_cmdPacketParameter->pout[0] = 0xA5;
    p_cmdPacketParameter->pout[1] = DEV_ID;
    p_cmdPacketParameter->pout[2] = 0;
    p_cmdPacketParameter->pout[3] = p_cmdPacketParameter->cmd;
    p_cmdPacketParameter->pout[4] = p_cmdPacketParameter->parameter>>24;
    p_cmdPacketParameter->pout[5] = p_cmdPacketParameter->parameter>>16;
    p_cmdPacketParameter->pout[6] = p_cmdPacketParameter->parameter>>8;
    p_cmdPacketParameter->pout[7] = p_cmdPacketParameter->parameter>>0;
    p_cmdPacketParameter->pout[8] = calculatorCheckSum(&p_cmdPacketParameter->pout[1],7);

    p_cmdPacketParameter->len = 9;
}

void CreateUpdateCycle(CmdPacketParameter *p_cmdPacketParameter)
{
    p_cmdPacketParameter->cmd = 0x01;
    createCmdPacket(p_cmdPacketParameter);
}

void CreateUpdateBaudrate(CmdPacketParameter *p_cmdPacketParameter)
{
    p_cmdPacketParameter->cmd = 0x02;
    createCmdPacket(p_cmdPacketParameter);
}

void CreateUpdateSensorNumber(CmdPacketParameter *p_cmdPacketParameter)
{
    p_cmdPacketParameter->cmd = 0x03;
    createCmdPacket(p_cmdPacketParameter);
}

void CreateSetBtSwitch(CmdPacketParameter *p_cmdPacketParameter)
{
    p_cmdPacketParameter->cmd = 0x04;
    createCmdPacket(p_cmdPacketParameter);
}

void CreateSetFilterType(CmdPacketParameter *p_cmdPacketParameter)
{
    p_cmdPacketParameter->cmd = 0x05;
    createCmdPacket(p_cmdPacketParameter);
}

void CreateUpdateSwitch(CmdPacketParameter *p_cmdPacketParameter)
{
    p_cmdPacketParameter->cmd = 0x06;
    createCmdPacket(p_cmdPacketParameter);
}

void CreateRst(CmdPacketParameter *p_cmdPacketParameter)
{
    p_cmdPacketParameter->cmd = 0x07;
    createCmdPacket(p_cmdPacketParameter);
}

void createRelayPacket(RelayPacketParameter *pRelayPacketPar)
{
    pRelayPacketPar->pout[0] = 0xA6;
    pRelayPacketPar->pout[1] = pRelayPacketPar->cmd;
    pRelayPacketPar->pout[2] = pRelayPacketPar->relayLen;
    memcpy(&pRelayPacketPar->pout[3],pRelayPacketPar->pRelayData,pRelayPacketPar->relayLen);
    pRelayPacketPar->pout[3+pRelayPacketPar->relayLen] =
            calculatorCheckSum(&pRelayPacketPar->pout[1],pRelayPacketPar->relayLen+2);
    pRelayPacketPar->len = pRelayPacketPar->relayLen+4;
}

void CreateBtRelay(RelayPacketParameter *pRelayPacketPar)
{
    pRelayPacketPar->cmd = 0x01;
    createRelayPacket(pRelayPacketPar);
}

void CreateSensorRelay(RelayPacketParameter *pRelayPacketPar)
{
    pRelayPacketPar->cmd = 0x02;
    createRelayPacket(pRelayPacketPar);
}




