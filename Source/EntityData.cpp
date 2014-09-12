//--------------------------------------------------------------------------------------------------
//
//  File:       EntityData.cpp
//
//  Project:    M+M
//
//  Contains:   The class definition for an entity detected by the background scanner.
//
//  Written by: Norman Jaffe
//
//  Copyright:  (c) 2014 by HPlus Technologies Ltd. and Simon Fraser University.
//
//              All rights reserved. Redistribution and use in source and binary forms, with or
//              without modification, are permitted provided that the following conditions are met:
//                * Redistributions of source code must retain the above copyright notice, this list
//                  of conditions and the following disclaimer.
//                * Redistributions in binary form must reproduce the above copyright notice, this
//                  list of conditions and the following disclaimer in the documentation and/or
//                  other materials provided with the distribution.
//                * Neither the name of the copyright holders nor the names of its contributors may
//                  be used to endorse or promote products derived from this software without
//                  specific prior written permission.
//
//              THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
//              EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
//              OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
//              SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
//              INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
//              TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
//              BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
//              CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
//              ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
//              DAMAGE.
//
//  Created:    2014-08-06
//
//--------------------------------------------------------------------------------------------------

#include "EntityData.h"
#include "PortData.h"

//#include <odl/ODEnableLogging.h>
#include <odl/ODLogging.h>

#if defined(__APPLE__)
# pragma clang diagnostic push
# pragma clang diagnostic ignored "-Wdocumentation-unknown-command"
#endif // defined(__APPLE__)
/*! @file
 
 @brief The class definition for an entity detected by the background scanner. */
#if defined(__APPLE__)
# pragma clang diagnostic pop
#endif // defined(__APPLE__)

using namespace ChannelManager;

#if defined(__APPLE__)
# pragma mark Private structures, constants and variables
#endif // defined(__APPLE__)

#if defined(__APPLE__)
# pragma mark Local functions
#endif // defined(__APPLE__)

#if defined(__APPLE__)
# pragma mark Class methods
#endif // defined(__APPLE__)

#if defined(__APPLE__)
# pragma mark Constructors and Destructors
#endif // defined(__APPLE__)

EntityData::EntityData(const ContainerKind           kind,
                       const yarp::os::ConstString & name,
                       const yarp::os::ConstString & behaviour,
                       const yarp::os::ConstString & description) :
    _behaviour(behaviour), _description(description), _name(name), _kind(kind)
{
    OD_LOG_ENTER(); //####
    OD_LOG_S3s("name = ", name, "behaviour = ", behaviour, "description = ", description); //####
    OD_LOG_EXIT_P(this); //####
} // EntityData::EntityData

EntityData::~EntityData(void)
{
    OD_LOG_OBJENTER(); //####
    OD_LOG_S1s("getName() = ", getName()); //####
    for (Ports::iterator walker(_ports.begin()); _ports.end() != walker; ++walker)
    {
        PortData * aPort = *walker;
        
        if (aPort)
        {
            delete aPort;
        }
    }
    _ports.clear();
    OD_LOG_OBJEXIT(); //####
} // EntityData::~EntityData

#if defined(__APPLE__)
# pragma mark Actions and Accessors
#endif // defined(__APPLE__)

PortData * EntityData::addPort(const yarp::os::ConstString & portName,
                               const yarp::os::ConstString & portProtocol,
                               const PortUsage               portKind,
                               const PortDirection           direction)
{
    OD_LOG_OBJENTER(); //####
    OD_LOG_S2s("portName = ", portName, "portProtocol = ", portProtocol); //####
    PortData * aPort = new PortData(portName, portProtocol, portKind, direction);
    
    _ports.push_back(aPort);
    OD_LOG_OBJEXIT_P(aPort); //####
    return aPort;
} // EntityData::addPort

int EntityData::getNumPorts(void)
const
{
    OD_LOG_OBJENTER(); //####
    int result = _ports.size();
    
    OD_LOG_OBJEXIT_L(result); //####
    return result;
} // EntityData::getNumPorts

PortData * EntityData::getPort(const int num)
const
{
    OD_LOG_OBJENTER(); //####
    PortData * result;
    
    if ((0 <= num) && (static_cast<int>(_ports.size()) > num))
    {
        result = _ports.at(num);
    }
    else
    {
        result = NULL;
    }
    OD_LOG_OBJEXIT_P(result);
    return result;
} // EntityData::getPort

#if defined(__APPLE__)
# pragma mark Global functions
#endif // defined(__APPLE__)