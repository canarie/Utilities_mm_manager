//--------------------------------------------------------------------------------------------------
//
//  File:       ContentPanel.cpp
//
//  Project:    M+M
//
//  Contains:   The class definition for the content area of the primary window of the channel
//              manager application.
//
//  Written by: Norman Jaffe
//
//  Copyright:  (c) 2014 by H Plus Technologies Ltd. and Simon Fraser University.
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
//  Created:    2014-07-21
//
//--------------------------------------------------------------------------------------------------

#include "ContentPanel.h"
#include "ChannelContainer.h"
#include "ChannelEntry.h"
#include "ChannelManagerWindow.h"
#include "EntitiesPanel.h"
#include "EntityData.h"
#include "PortData.h"
#include "ScannerThread.h"

//#include <odl/ODEnableLogging.h>
#include <odl/ODLogging.h>

#if defined(USE_OGDF_POSITIONING)
# include <ogdf/basic/GraphAttributes.h>
# include <ogdf/energybased/FMMMLayout.h>
#endif // defined(USE_OGDF_POSITIONING)

#if defined(__APPLE__)
# pragma clang diagnostic push
# pragma clang diagnostic ignored "-Wunknown-pragmas"
# pragma clang diagnostic ignored "-Wdocumentation-unknown-command"
#endif // defined(__APPLE__)
/*! @file
 
 @brief The class definition for the content area of the primary window of the channel manager
 application. */
#if defined(__APPLE__)
# pragma clang diagnostic pop
#endif // defined(__APPLE__)

#if (! MAC_OR_LINUX_)
# include <Windows.h>
#endif //! MAC_OR_LINUX_

using namespace ChannelManager;
using namespace std;

#if defined(__APPLE__)
# pragma mark Private structures, constants and variables
#endif // defined(__APPLE__)

/*! @brief The colour to be used for the dialog background. */
static const Colour & kDialogBackgroundColour(Colours::whitesmoke);

/*! @brief The first colour to be used for the panel background. */
static const Colour & kFirstBackgroundColour(Colours::darkgrey);

/*! @brief The second colour to be used for the panel background. */
static const Colour & kSecondBackgroundColour(Colours::lightgrey);

/*! @brief The initial thickness of the horizontal and vertical scrollbars. */
static const int kDefaultScrollbarThickness = 16;

/*! @brief The initial single-step size of the horizontal and vertical scrollbars. */
static const int kDefaultSingleStepSize = 10;

/*! @brief After width to be added to display panels. */
static const int kExtraDisplayWidth = 32;

#if defined(__APPLE__)
# pragma mark Local functions
#endif // defined(__APPLE__)

/*! @brief Determine the maximum dimensions of a text string.
 @param dimensions The calculated maximum width and height.
 @param aFont The font to use for the calculations.
 @param aString The string to be analyzed. */
void calculateTextArea(Point<int> &   dimensions,
                       const Font &   aFont,
                       const String & aString)
{
    OD_LOG_ENTER(); //####
    OD_LOG_P2("dimensions = ", &dimensions, "aFont = ", &aFont); //####
    OD_LOG_S1s("aString = ", aString.toStdString()); //####
    float       maxWidth = 0;
    StringArray asLines;
    
    asLines.addLines(aString);
    int numRows = asLines.size();
    
    for (int ii = 0; ii < numRows; ++ii)
    {
        const String & aRow = asLines[ii];
        float          aWidth = aFont.getStringWidthFloat(aRow);
        
        if (maxWidth < aWidth)
        {
            maxWidth = aWidth;
        }
    }
    dimensions = Point<int>(static_cast<int>(maxWidth + 0.5),
                            static_cast<int>((numRows * aFont.getHeight()) + 0.5));
    OD_LOG_EXIT(); //####
} // calculateTextArea

/*! @brief Returns the absolute path to the settings file.
 @returns The absolute path to the settings file. */
static String getPathToSettingsFile(void)
{
    File   baseDir = File::getSpecialLocation(File::userApplicationDataDirectory);
    String baseDirAsString = File::addTrailingSeparator(baseDir.getFullPathName());
    String settingsDir = File::addTrailingSeparator(baseDirAsString + "ChannelManager");
    
    return settingsDir + "settings.txt";
} // getPathToSettingsFile

#if defined(__APPLE__)
# pragma mark Class methods
#endif // defined(__APPLE__)

#if defined(__APPLE__)
# pragma mark Constructors and Destructors
#endif // defined(__APPLE__)

ContentPanel::ContentPanel(ChannelManagerWindow * containingWindow) :
    inherited1(), inherited2(), _entitiesPanel(new EntitiesPanel(this)),
    _containingWindow(containingWindow),
#if (defined(USE_OGDF_POSITIONING) && defined(USE_OGDF_FOR_FIRST_POSITIONING_ONLY))
    _initialPositioningDone(false),
#endif // defined(USE_OGDF_POSITIONING) && defined(USE_OGDF_FOR_FIRST_POSITIONING_ONLY)
    _invertBackground(false), _skipNextScan(false), _whiteBackground(false)
{
    OD_LOG_ENTER(); //####
    _entitiesPanel->setSize(_entitiesPanel->getWidth(),
                            _entitiesPanel->getHeight() - _containingWindow->getTitleBarHeight());
    setSize(_entitiesPanel->getWidth(), _entitiesPanel->getHeight());
    setScrollBarsShown(true, true);
    setScrollBarThickness(kDefaultScrollbarThickness);
    setSingleStepSizes(kDefaultSingleStepSize, kDefaultSingleStepSize);
    _entitiesPanel->setVisible(true);
    setViewedComponent(_entitiesPanel);
    OD_LOG_EXIT_P(this); //####
} // ContentPanel::ContentPanel

ContentPanel::~ContentPanel(void)
{
    OD_LOG_OBJENTER(); //####
    OD_LOG_OBJEXIT(); //####
} // ContentPanel::~ContentPanel

#if defined(__APPLE__)
# pragma mark Actions and Accessors
#endif // defined(__APPLE__)

void ContentPanel::getAllCommands(Array<CommandID> & commands)
{
    OD_LOG_OBJENTER(); //####
    static const CommandID ids[] =
        {
            ChannelManagerWindow::kCommandDoRepaint,
            ChannelManagerWindow::kCommandInvertBackground,
            ChannelManagerWindow::kCommandWhiteBackground
        };
    
    commands.addArray(ids, numElementsInArray(ids));
    OD_LOG_OBJEXIT(); //####
} // ContentPanel::getAllCommands

void ContentPanel::getCommandInfo(CommandID                commandID,
                                  ApplicationCommandInfo & result)
{
    OD_LOG_OBJENTER(); //####
    switch (commandID)
    {
        case ChannelManagerWindow::kCommandDoRepaint :
            result.setInfo("Repaint", "Trigger a repaint of the window", "View", 0);
            result.addDefaultKeypress('R', ModifierKeys::commandModifier);
            break;
            
        case ChannelManagerWindow::kCommandInvertBackground :
            result.setInfo("Invert", "Invert the background gradient", "View", 0);
            result.addDefaultKeypress('I', ModifierKeys::commandModifier);
            break;
            
        case ChannelManagerWindow::kCommandWhiteBackground :
            result.setInfo("White", "Use a white background", "View", 0);
            result.addDefaultKeypress('B', ModifierKeys::commandModifier);
            break;
            
        default :
            break;
            
    }
    OD_LOG_OBJEXIT(); //####
} // ContentPanel::getCommandInfo

ApplicationCommandTarget * ContentPanel::getNextCommandTarget(void)
{
    OD_LOG_OBJENTER(); //####
    ApplicationCommandTarget * nextOne = findFirstTargetParentComponent();
    
    OD_LOG_OBJEXIT_P(nextOne); //####
    return nextOne;
} // ContentPanel::getNextCommandTarget

void ContentPanel::paint(Graphics & gg)
{
    OD_LOG_OBJENTER(); //####
    OD_LOG_P1("gg = ", &gg); //####
    if (_whiteBackground)
    {
        gg.setFillType(_invertBackground ? kFirstBackgroundColour : kSecondBackgroundColour);
    }
    else
    {
        // Set up a gradient background, using a radial gradient from the centre to the furthest
        // edge.
        int   hh = getHeight();
        int   ww = getWidth();
        float halfH = static_cast<float>(hh / 2.0);
        float halfW = static_cast<float>(ww / 2.0);

        if (_invertBackground)
        {
            ColourGradient theGradient2(kFirstBackgroundColour, halfW, halfH,
                                        kSecondBackgroundColour,
                                        static_cast<float>((hh > ww) ? 0 : ww),
                                        static_cast<float>((hh > ww) ? hh : 0), true);
            FillType       theBackgroundFill2(theGradient2);
            
            gg.setFillType(theBackgroundFill2);
        }
        else
        {
            ColourGradient theGradient1(kSecondBackgroundColour, halfW, halfH,
                                        kFirstBackgroundColour,
                                        static_cast<float>((hh > ww) ? 0 : ww),
                                        static_cast<float>((hh > ww) ? hh : 0), true);
            FillType       theBackgroundFill1(theGradient1);
            
            gg.setFillType(theBackgroundFill1);
        }
    }
    gg.fillAll();
#if 0
    // The following code will draw a dashed line around each container - use it for a template
    // for the 'selection rectangle' code.
    // Note that the default thickness of the line is 1 and the offset is set to 5, which should be
    // a constant.
    for (size_t ii = 0, mm = _entitiesPanel->getNumberOfEntities(); mm > ii; ++ii)
    {
        ChannelContainer * aContainer = _entitiesPanel->getEntity(ii);
        
        if (aContainer)
        {
            juce::Rectangle<float> containerShape(aContainer->getBounds().toFloat());
            const float            dashes[] = { 5, 5 };
            const int              numDashes = (sizeof(dashes) / sizeof(*dashes));
            
            containerShape.expand(5, 5);
            Point<float> topLeft(containerShape.getTopLeft());
            Point<float> topRight(containerShape.getTopRight());
            Point<float> bottomLeft(containerShape.getBottomLeft());
            Point<float> bottomRight(containerShape.getBottomRight());
            Line<float>  aLine;
            
            aLine.setStart(topLeft);
            aLine.setEnd(topRight);
            gg.setColour(Colours::cornsilk);
            gg.drawDashedLine(aLine, dashes, numDashes);
            aLine.setStart(topRight);
            aLine.setEnd(bottomRight);
            gg.drawDashedLine(aLine, dashes, numDashes);
            aLine.setStart(bottomRight);
            aLine.setEnd(bottomLeft);
            gg.drawDashedLine(aLine, dashes, numDashes);
            aLine.setStart(bottomLeft);
            aLine.setEnd(topLeft);
            gg.drawDashedLine(aLine, dashes, numDashes);
        }
    }
#endif // 0
    ScannerThread * scanner = _containingWindow->getScannerThread();
    
    if (scanner)
    {
        // Check if there is some 'fresh' data.
        bool locked = scanner->conditionallyAcquireForRead();
        
        for ( ; ! locked; locked = scanner->conditionallyAcquireForRead())
        {
#if MAC_OR_LINUX_
            sleep(SHORT_SLEEP);
#else // ! MAC_OR_LINUX_
            Sleep(SHORT_SLEEP);
#endif // ! MAC_OR_LINUX_
        }
        bool scanDataReady = scanner->checkAndClearIfScanIsComplete();
        
        scanner->relinquishFromRead();
        if (scanDataReady)
        {
            OD_LOG("(scanDataReady)"); //####
            // At this point the background scanning thread is, basically, idle, and we can use its
            // data.
            if (_skipNextScan)
            {
                _skipNextScan = false;
                scanner->doScanSoon();
            }
            else
            {
                updatePanels(*scanner);
                setEntityPositions();
            }
            // Indicate that the scan data has been processed.
            scanner->unconditionallyAcquireForWrite();
            scanner->scanCanProceed();
            scanner->relinquishFromWrite();
        }
    }
    OD_LOG_OBJEXIT(); //####
} // ContentPanel::paint

bool ContentPanel::perform(const InvocationInfo & info)
{
    OD_LOG_OBJENTER(); //####
    bool wasProcessed = false;
    
    switch (info.commandID)
    {
        case ChannelManagerWindow::kCommandDoRepaint :
            requestWindowRepaint();
            wasProcessed = true;
            break;
            
        case ChannelManagerWindow::kCommandInvertBackground :
            flipBackground();
            requestWindowRepaint();
            break;
            
        case ChannelManagerWindow::kCommandWhiteBackground :
            changeBackgroundColour();
            requestWindowRepaint();
            break;
            
        default :
            break;
            
    }
    OD_LOG_OBJEXIT_B(wasProcessed); //####
    return wasProcessed;
} // ContentPanel::perform

void ContentPanel::recallEntityPositions(void)
{
    OD_LOG_OBJENTER(); //####
    String filePath = getPathToSettingsFile();
    File   settingsFile(filePath);
    
    if (settingsFile.existsAsFile())
    {
        OD_LOG("(settingsFile.existsAsFile())"); //####
        StringArray stuffFromFile;
        
        settingsFile.readLines(stuffFromFile);
        for (int ii = 0, max = stuffFromFile.size(); max > ii; ++ii)
        {
            String aLine = stuffFromFile[ii];
            
            if (0 < aLine.length())
            {
                StringArray asPieces;
                
                asPieces.addTokens(aLine, "\t", "");
                if (3 == asPieces.size())
                {
                    String tag = asPieces[0];
                    String xPosString = asPieces[1];
                    String yPosString = asPieces[2];
                    
                    _rememberedPositions[tag.toStdString()] = Position(xPosString.getFloatValue(),
                                                                       yPosString.getFloatValue());
                }
            }
        }
    }
    OD_LOG_OBJEXIT(); //####
} // ContentPanel::recallEntityPositions

void ContentPanel::rememberPositionOfEntity(ChannelContainer * anEntity)
{
    OD_LOG_OBJENTER(); //####
    OD_LOG_P1("anEntity = ", anEntity); //####
    yarp::os::ConstString entityName(anEntity->getName().toStdString());

    _rememberedPositions[entityName] = anEntity->getPositionInPanel();
    OD_LOG_OBJEXIT(); //####
} // ContentPanel::rememberPositionOfEntity

void ContentPanel::requestWindowRepaint(void)
{
    OD_LOG_OBJENTER(); //####
    _containingWindow->repaint();
    OD_LOG_OBJEXIT(); //####
} // ContentPanel::requestWindowRepaint

void ContentPanel::resized(void)
{
    OD_LOG_OBJENTER(); //####
    _entitiesPanel->setSize(getWidth(), getHeight());
    OD_LOG_OBJEXIT(); //####
} // ContentPanel::resized

void ContentPanel::saveEntityPositions(void)
{
    OD_LOG_OBJENTER(); //####
    String filePath = getPathToSettingsFile();
    File   settingsFile(filePath);
    
    if (settingsFile.create().wasOk())
    {
        OD_LOG("(settingsFile.create().wasOk())"); //####
        // Make sure that the file is empty before adding lines to it!
        settingsFile.replaceWithText("");
        for (PositionMap::const_iterator walker(_rememberedPositions.begin());
             _rememberedPositions.end() != walker; ++walker)
        {
            std::stringstream     buff;
            yarp::os::ConstString tag = walker->first;
            Position              where = walker->second;

            buff << "\t" << where.x << "\t" << where.y << std::endl;
            settingsFile.appendText(tag.c_str());
            settingsFile.appendText(buff.str());
        }
    }
    OD_LOG_OBJEXIT(); //####
} // ContentPanel::saveEntityPositions

void ContentPanel::setEntityPositions(void)
{
    OD_LOG_OBJENTER(); //####
    float         fullHeight = static_cast<float>(_entitiesPanel->getHeight());
    float         fullWidth = static_cast<float>(_entitiesPanel->getWidth());
    Random        randomizer(Time::currentTimeMillis());
#if defined(USE_OGDF_POSITIONING)
    ogdf::Graph * gg;
#endif // defined(USE_OGDF_POSITIONING)
    
#if defined(USE_OGDF_POSITIONING)
# if defined(USE_OGDF_FOR_FIRST_POSITIONING_ONLY)
    if (_initialPositioningDone)
    {
        gg = NULL;
    }
    else
    {
        _initialPositioningDone = true;
        gg = new ogdf::Graph;
    }
# else // ! defined(USE_OGDF_FOR_FIRST_POSITIONING_ONLY)
    gg = new ogdf::Graph;
# endif // ! defined(USE_OGDF_FOR_FIRST_POSITIONING_ONLY)
    if (gg)
    {
        ScopedPointer<ogdf::GraphAttributes> ga(new ogdf::GraphAttributes(*gg));
        
        if (ga)
        {
            bool       positionsNeedUpdate = false;
            ogdf::node phantomNode = gg->newNode();
            
            ga->directed(true);
            // If nodes are not connected, OGDF will pile them all at the origin; by adding a
            // 'phantom' node that is connected to every other node, we force OGDF to spread the
            // nodes out.
            ga->width(phantomNode) = 1;
            ga->height(phantomNode) = 1;
            ga->x(phantomNode) = (randomizer.nextFloat() * fullWidth);
            ga->y(phantomNode) = (randomizer.nextFloat() * fullHeight);
            _entitiesPanel->clearNodeValues();
            for (size_t ii = 0, mm = _entitiesPanel->getNumberOfEntities(); mm > ii; ++ii)
            {
                ChannelContainer * aContainer = _entitiesPanel->getEntity(ii);
                
                if (aContainer)
                {
                    float                  newX;
                    float                  newY;
                    juce::Rectangle<float> entityShape(aContainer->getLocalBounds().toFloat());
                    ogdf::node             aNode = gg->newNode();
                    float                  hh = entityShape.getHeight();
                    float                  ww = entityShape.getWidth();
                    
                    ga->width(aNode) = ww;
                    ga->height(aNode) = hh;
                    aContainer->setNode(aNode);
                    if (aContainer->isNew())
                    {
                        OD_LOG("(aContainer->isNew())"); //####
                        // Check if the position was already known
                        yarp::os::ConstString       entityName(aContainer->getName().toStdString());
                        PositionMap::const_iterator match(_rememberedPositions.find(entityName));
                        
                        if (_rememberedPositions.end() == match)
                        {
                            newX = (randomizer.nextFloat() * (fullWidth - ww));
                            newY = (randomizer.nextFloat() * (fullHeight - hh));
                            positionsNeedUpdate = true;
                        }
                        else
                        {
                            newX = match->second.x;
                            newY = match->second.y;
                        }
                        aContainer->setTopLeftPosition(static_cast<int>(newX),
                                                       static_cast<int>(newY));
                    }
                    else
                    {
                        newX = entityShape.getX();
                        newY = entityShape.getY();
                    }
                    ga->x(aNode) = newX;
                    ga->y(aNode) = newY;
                }
            }
            if (positionsNeedUpdate)
            {
                OD_LOG("(positionsNeedUpdate)"); //####
                // Set up the edges (connections)
                for (size_t ii = 0, mm = _entitiesPanel->getNumberOfEntities(); mm > ii; ++ii)
                {
                    ChannelContainer * aContainer = _entitiesPanel->getEntity(ii);
                    
                    if (aContainer)
                    {
                        ogdf::node thisNode = aContainer->getNode();
                        
                        if (thisNode)
                        {
                            bool wasConnected = false;
                            
                            // Add edges between entities that are connected via their entries
                            for (int jj = 0, nn = aContainer->getNumPorts(); nn > jj; ++jj)
                            {
                                ChannelEntry * aChannel = aContainer->getPort(jj);
                                
                                if (aChannel)
                                {
                                    const ChannelConnections & outputs =
                                                                aChannel->getOutputConnections();
                                    
                                    for (size_t kk = 0, ll = outputs.size(); ll > kk; ++kk)
                                    {
                                        ChannelEntry * otherChannel = outputs[kk]._otherChannel;
                                        
                                        if (otherChannel)
                                        {
                                            ChannelContainer * otherEntity =
                                            otherChannel->getParent();
                                            
                                            if (otherEntity)
                                            {
                                                ogdf::node otherNode = otherEntity->getNode();
                                                
                                                if (otherNode && (thisNode != otherNode))
                                                {
                                                    /*ogdf::edge ee =*/ gg->newEdge(thisNode,
                                                                                    otherNode);
                                                    
                                                    wasConnected = true;
                                                }
                                            }
                                        }
                                    }
                                    const ChannelConnections & inputs =
                                                                    aChannel->getInputConnections();
                                    
                                    if (0 < inputs.size())
                                    {
                                        wasConnected = true;
                                    }
                                }
                            }
                            if (! wasConnected)
                            {
                                /*ogdf::edge phantomNodeToThis =*/ gg->newEdge(phantomNode,
                                                                               thisNode);
                                
                            }
                        }
                    }
                }
                // Apply an energy-based layout
                ScopedPointer<ogdf::FMMMLayout> fmmm(new ogdf::FMMMLayout);
                
                if (fmmm)
                {
                    fmmm->useHighLevelOptions(true);
                    fmmm->newInitialPlacement(false); //true);
                    fmmm->qualityVersusSpeed(ogdf::FMMMLayout::qvsGorgeousAndEfficient);
                    fmmm->allowedPositions(ogdf::FMMMLayout::apAll);
                    fmmm->initialPlacementMult(ogdf::FMMMLayout::ipmAdvanced);
                    fmmm->initialPlacementForces(ogdf::FMMMLayout::ipfKeepPositions);
                    fmmm->repForcesStrength(2.0);
                    fmmm->call(*ga);
                    for (size_t ii = 0, mm = _entitiesPanel->getNumberOfEntities(); mm > ii; ++ii)
                    {
                        ChannelContainer * aContainer = _entitiesPanel->getEntity(ii);
                        
                        if (aContainer && aContainer->isNew())
                        {
                            ogdf::node aNode = aContainer->getNode();
                            
                            if (aNode)
                            {
                                // Check if the position was already known
                                yarp::os::ConstString entityName =
                                                                aContainer->getName().toStdString();
                                
                                if (_rememberedPositions.end() ==
                                                            _rememberedPositions.find(entityName))
                                {
                                    aContainer->setTopLeftPosition(static_cast<int>(ga->x(aNode)),
                                                                   static_cast<int>(ga->y(aNode)));
                                }
                            }
                        }
                    }
                }
            }
        }
        delete gg;
    }
    else
    {
        for (size_t ii = 0, mm = _entitiesPanel->getNumberOfEntities(); mm > ii; ++ii)
        {
            ChannelContainer * aContainer = _entitiesPanel->getEntity(ii);
            
            if (aContainer && aContainer->isNew())
            {
                OD_LOG("(aContainer->isNew())"); //####
                float                       newX;
                float                       newY;
                juce::Rectangle<float>      entityShape(aContainer->getLocalBounds().toFloat());
                float                       hh = entityShape.getHeight();
                float                       ww = entityShape.getWidth();
                yarp::os::ConstString       entityName(aContainer->getName().toStdString());
                PositionMap::const_iterator match(_rememberedPositions.find(entityName));

                if (_rememberedPositions.end() == match)
                {
                    newX = (randomizer.nextFloat() * (fullWidth - ww));
                    newY = (randomizer.nextFloat() * (fullHeight - hh));
                }
                else
                {
                    newX = match->second.x;
                    newY = match->second.y;
                }
                aContainer->setTopLeftPosition(static_cast<int>(newX), static_cast<int>(newY));
            }
        }
    }
#else // ! defined(USE_OGDF_POSITIONING)
    for (size_t ii = 0, mm = _entitiesPanel->getNumberOfEntities(); mm > ii; ++ii)
    {
        ChannelContainer * aContainer = _entitiesPanel->getEntity(ii);
        
        if (aContainer && aContainer->isNew())
        {
            OD_LOG("(aContainer->isNew())"); //####
            float                       newX;
            float                       newY;
            juce::Rectangle<float>      entityShape(aContainer->getLocalBounds().toFloat());
            float                       hh = entityShape.getHeight();
            float                       ww = entityShape.getWidth();
            yarp::os::ConstString       entityName(aContainer->getName().toStdString());
            PositionMap::const_iterator match(_rememberedPositions.find(entityName));
            
            if (_rememberedPositions.end() == match)
            {
                newX = (randomizer.nextFloat() * (fullWidth - ww));
                newY = (randomizer.nextFloat() * (fullHeight - hh));
            }
            else
            {
                newX = match->second.x;
                newY = match->second.y;
            }
            aContainer->setTopLeftPosition(static_cast<int>(newX), static_cast<int>(newY));
        }
    }
#endif // ! defined(USE_OGDF_POSITIONING)
    OD_LOG_OBJEXIT(); //####
} // ContentPanel::setEntityPositions

void ContentPanel::skipScan(void)
{
    OD_LOG_OBJENTER(); //####
    _skipNextScan = true;
    OD_LOG_OBJEXIT(); //####
} // ContentPanel::skipScan

void ContentPanel::updatePanels(ScannerThread & scanner)
{
    OD_LOG_OBJENTER(); //####
    OD_LOG_P1("scanner = ", &scanner); //####
    bool                 changeSeen = false;
    const EntitiesData & workingData(scanner.getEntitiesData());
    
    // Retrieve each entity from our new list; if it is known already, ignore it but mark the
    // old entity as known.
    _entitiesPanel->clearAllVisitedFlags();
    _entitiesPanel->clearAllNewlyCreatedFlags();
    _entitiesPanel->invalidateAllConnections();
    for (size_t ii = 0, mm = workingData.getNumberOfEntities(); mm > ii; ++ii)
    {
        EntityData * anEntity = workingData.getEntity(ii);
        
        OD_LOG_P1("anEntity <- ", anEntity); //####
        if (anEntity)
        {
            OD_LOG_S1s("anEntity->getName() = ", anEntity->getName()); //####
            ChannelContainer * oldEntity = _entitiesPanel->findKnownEntity(anEntity->getName());
            
            if (oldEntity)
            {
                OD_LOG("(oldEntity)"); //####
                oldEntity->setVisited();
            }
            else
            {
                // Make a copy of the newly discovered entity, and add it to the active panel.
                ChannelContainer * newEntity = new ChannelContainer(anEntity->getKind(),
                                                                    anEntity->getName(),
                                                                    anEntity->getIPAddress(),
                                                                    anEntity->getBehaviour(),
                                                                    anEntity->getDescription(),
                                                                    anEntity->getRequests(),
                                                                    *_entitiesPanel);
                
                newEntity->setVisited();
                // Make copies of the ports of the entity, and add them to the new entity.
                for (int jj = 0, nn = anEntity->getNumPorts(); nn > jj; ++jj)
                {
                    PortData * aPort = anEntity->getPort(jj);
                    
                    if (aPort)
                    {
                        ChannelEntry * newPort = newEntity->addPort(aPort->getPortName(),
                                                                    aPort->getPortNumber(),
                                                                    aPort->getProtocol(),
                                                                    aPort->getProtocolDescription(),
                                                                    aPort->getUsage(),
                                                                    aPort->getDirection());
                        
                        _entitiesPanel->rememberPort(newPort);
                    }
                }
                _entitiesPanel->addEntity(newEntity);
                changeSeen = true;
            }
        }
    }
    // Convert the detected connections into visible connections.
    const ConnectionList & connections(workingData.getConnections());
    
    for (ConnectionList::const_iterator walker(connections.begin()); connections.end() != walker;
         ++walker)
    {
        ChannelEntry * thisPort = _entitiesPanel->findKnownPort(walker->_outPortName);
        ChannelEntry * otherPort = _entitiesPanel->findKnownPort(walker->_inPortName);
        
        OD_LOG_P2("thisPort <- ", thisPort, "otherPort <- ", otherPort); //####
        if (thisPort && otherPort)
        {
            OD_LOG_S2s("thisPort.name = ", thisPort->getPortName(), //####
                       "otherPort.name = ", otherPort->getPortName()); //####
            thisPort->addOutputConnection(otherPort, walker->_mode);
            otherPort->addInputConnection(thisPort, walker->_mode);
        }
    }
    if (_entitiesPanel->removeUnvisitedEntities())
    {
        changeSeen = true;
    }
    _entitiesPanel->removeInvalidConnections();
    OD_LOG("about to call adjustSize()"); //####
    _entitiesPanel->adjustSize(false);
    if (changeSeen)
    {
        scanner.doScanSoon();
    }
    OD_LOG_OBJEXIT(); //####
} // ContentPanel::updatePanels

#if (! MAC_OR_LINUX_)
# pragma warning(push)
# pragma warning(disable: 4100)
#endif // ! MAC_OR_LINUX_
void ContentPanel::visibleAreaChanged(const juce::Rectangle<int> & newVisibleArea)
{
#if (! defined(OD_ENABLE_LOGGING))
# if MAC_OR_LINUX_
#  pragma unused(newVisibleArea)
# endif // MAC_OR_LINUX_
#endif // ! defined(OD_ENABLE_LOGGING)
    OD_LOG_OBJENTER(); //####
    OD_LOG_LL4("nVA.x = ", newVisibleArea.getX(), "nVA.y = ", newVisibleArea.getY(), //####
               "nVA.w = ", newVisibleArea.getWidth(), "nVA.h = ", newVisibleArea.getHeight()); //####
    OD_LOG_OBJEXIT(); //####
} // ContentPanel::visibleAreaChanged
#if (! MAC_OR_LINUX_)
# pragma warning(pop)
#endif // ! MAC_OR_LINUX_

#if defined(__APPLE__)
# pragma mark Global functions
#endif // defined(__APPLE__)

void ChannelManager::DisplayInformationPanel(Component *    above,
                                             const String & bodyText,
                                             const String & title)
{
    OD_LOG_ENTER(); //####
    OD_LOG_P1("above = ", above); //####
    OD_LOG_S2s("bodyText = ", bodyText.toStdString(), "title = ", title.toStdString()); //####
    DialogWindow::LaunchOptions options;
    String                      monoFontName = Font::getDefaultMonospacedFontName();
    Font                        monoFont(monoFontName, 16, Font::plain);
    Label *                     aLabel = new Label;
    
    aLabel->setFont(monoFont);
    aLabel->setText(bodyText, dontSendNotification);
    options.content.setOwned(aLabel);
    Point<int> dimensions;
    
    calculateTextArea(dimensions, aLabel->getFont(), bodyText);
    options.content->setSize(dimensions.getX(), dimensions.getY());
    options.dialogTitle = title;
    options.escapeKeyTriggersCloseButton = true;
    options.useNativeTitleBar = false;
    options.resizable = false;
    options.dialogBackgroundColour = kDialogBackgroundColour;
    DialogWindow *  aWindow = options.launchAsync();
    BorderSize<int> bt = aWindow->getBorderThickness();
    BorderSize<int> cb = aWindow->getContentComponentBorder();
    int             tw = aLabel->getFont().getStringWidth(title);
    int             minW = max(tw, dimensions.getX());
    int             calcW = minW + bt.getLeftAndRight() + cb.getLeftAndRight() + kExtraDisplayWidth;
    int             calcH = dimensions.getY() + bt.getTopAndBottom() + cb.getTopAndBottom();
    
    aWindow->centreAroundComponent(above, calcW, calcH);
    OD_LOG_EXIT(); //####
} // DisplayInformationPanel
