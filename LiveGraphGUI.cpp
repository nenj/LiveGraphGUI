//
//  LiveGraph.cpp
//
//  Created by Nicolas Enjalbert on 14/01/2014.
//
//

#include "LiveGraphGUI.h"
#include "cinder/Font.h"

using namespace cinder;
using namespace cinder::app;
using namespace std;

namespace dscrts { namespace lggui {
    
//-----------------------------------------------------------------------------

ColorA LiveGraphGUI::darkColor  = ColorA( .3, .3, .3, 1 );
ColorA LiveGraphGUI::lightColor = ColorA( 1 , 1 , 1 , 1 );
ColorA LiveGraphGUI::bgColor    = ColorA( 0 , 0 , 0 , .3);
ColorA LiveGraphGUI::textColor  = ColorA( 1 , 1 , 1 , 1 );
ColorA LiveGraphGUI::plotColor  = ColorA( 1 , 1 , 1 , 1 );
float  LiveGraphGUI::spacing    = 7;
float LiveGraphGUI::labelHeight = 10;
Vec2f  LiveGraphGUI::padding    = Vec2f(3, 3);
Font   LiveGraphGUI::textFont   = Font();

//-----------------------------------------------------------------------------

LiveGraphGUI::LiveGraphGUI( App* app, float xsize, float ysize )
:
enabled(true),
boxSize( xsize, ysize ),
elements(0),
xwidth(1.)
{
    textFont = Font( loadResource( "pf_tempesta_seven.ttf" ), 8 );
}
 
    
LiveGraphGUI::~LiveGraphGUI( )
{
    for( vector<LGElement*>::iterator it = elements.begin(); it != elements.end(); it++ )
    {
        delete (*it);
    }
}


DoubleLiveGraph* LiveGraphGUI::addGraph( const string& graphName,
                                         vector<double>* xvalues_, vector<double>* yvalues_,
                                         double ymin_, double ymax_ )
{
    DoubleLiveGraph* lg = new DoubleLiveGraph( this, graphName, xvalues_, yvalues_, ymin_, ymax_ );
    elements.push_back( lg );

    update();
    
    return lg;
}
    

void LiveGraphGUI::setAbscissaWidth( double xwidth_ )
{
    if( xwidth_ != 0. )
        xwidth = xwidth_;
    else
        xwidth = 1.;
    
    update();
}
    
    
void LiveGraphGUI::draw( )
{
    if( !enabled )
        return;
    
    gl::pushMatrices();
    gl::setMatricesWindow(getWindowSize());
    gl::disableDepthRead();
    gl::disableDepthWrite();
    gl::enableAlphaBlending();
    
    Vec2f position = Vec2f( getWindowWidth(), getWindowHeight() ) - Vec2f( spacing, spacing ) - boxSize;

    for( vector<LGElement*>::iterator it = elements.begin(); it != elements.end(); it++ )
    {
        position = (*it)->draw( position );
    }
    
    gl::disableAlphaBlending();
    gl::enableDepthRead();
    gl::enableDepthWrite();
    gl::color( ColorA(1,1,1,1) );
    gl::popMatrices();
}
    

void LiveGraphGUI::update( )
{
    for( vector<LGElement*>::iterator it = elements.begin(); it != elements.end(); it++ )
    {
        (*it)->update( );
    }
}
    
//-----------------------------------------------------------------------------
    
LGElement::LGElement(  )
{
}
    

LGElement::~LGElement( )
{
    
}

//-----------------------------------------------------------------------------

DoubleLiveGraph::DoubleLiveGraph( LiveGraphGUI* parentGUI_, const string& name,
                                  vector<double>* xvalues_, vector<double>* yvalues_,
                                  double ymin_, double ymax_ ) :
xvalues( xvalues_ ),
yvalues( yvalues_ ),
ymin( ymin_ ),
ymax( ymax_ ),
iXstart( 0 )
{
    this->parentGUI = parentGUI_;
    this->type = LGElement::DOUBLE_GRAPH;
    this->name = name;
    
    xwidth = this->parentGUI->getAbscissaWidth();
}
    

DoubleLiveGraph::~DoubleLiveGraph( )
{
}
    
    
Vec2f DoubleLiveGraph::draw( Vec2f pos )
{
    // boxes
    gl::color( LiveGraphGUI::bgColor );
    gl::drawSolidRect( Rectf(
        pos - LiveGraphGUI::padding,
        pos + LiveGraphGUI::padding + parentGUI->getBoxSize() ) );
    
    gl::drawString( name, pos, LiveGraphGUI::textColor, LiveGraphGUI::textFont );
    
    // content
    if( xvalues->size() > 1 )
    {
        unsigned int iX, xlen;
        double x1, x2, y1, y2;
        float boxY, plotX, plotY, xmin;
        
        gl::color( LiveGraphGUI::plotColor );
        
        xlen = xvalues->size();
        
        boxY = parentGUI->getBoxSize().y;
        plotX = parentGUI->getBoxSize().x;
        plotY = boxY - LiveGraphGUI::labelHeight - LiveGraphGUI::padding.y;
        xmin = xvalues->at(xlen-1) - xwidth;

        
        if( xlen >= 2 && iXstart >= 0 )
        {
            glEnable( GL_SCISSOR_TEST );
            glScissor( pos.x, getWindowHeight()-pos.y-boxY, plotX, plotY ); // (0,0) ref is window's bottom left
            
            x2 = xvalues->at(iXstart);
            y2 = yvalues->at(iXstart);
            for( iX = iXstart; iX < xlen-2; iX++ )
            {
                x1 = x2;
                y1 = y2;
                x2 = xvalues->at(iX+1);
                y2 = yvalues->at(iX+1);

                gl::drawLine( pos + Vec2f(0.,boxY) + Vec2f( (x1-xmin)*plotX/xwidth, -(y1-ymin)*plotY/(ymax-ymin) ),
                              pos + Vec2f(0.,boxY) + Vec2f( (x2-xmin)*plotX/xwidth, -(y2-ymin)*plotY/(ymax-ymin) ) );
            }
            
            glDisable( GL_SCISSOR_TEST );
        }
    }
    
    // for next element
    pos.y -= parentGUI->getBoxSize().y + LiveGraphGUI::spacing;
    return pos;
}
    
/*---
 
- Updates xwidth from the parent LiveGraphGUI
- Determines the index from where to start graph display.
 
---*/
void DoubleLiveGraph::update( )
{
    double xm1, x, xp1;
    unsigned int len;
    
    len = xvalues->size();
    if( len==0 )
        return;
    
    // lower bound
    xwidth = this->parentGUI->getAbscissaWidth();
    double xLowerBound = xvalues->at(len-1) - xwidth;
    
    // current value at start index
    if( iXstart == -1 )
        iXstart = 0;
    x = xvalues->at(iXstart);
    
    if( x >= xLowerBound )
    {
        if( iXstart == 0 )
            return;
        xm1 = xvalues->at(iXstart-1);
        while( xm1 >= xLowerBound && iXstart > 0 )
        {
            iXstart--;
            xm1 = xvalues->at(iXstart-1);
        }
    }
    else if( x < xLowerBound )
    {
        if( iXstart == len-1 )
        {
            iXstart = -1;
            return;
        }
        xp1 = xvalues->at(iXstart+1);
        while( xp1 < xLowerBound && iXstart < len-1 )
        {
            iXstart++;
            xp1 = xvalues->at(iXstart+1);
        }
    }
    return;
}
    
//-----------------------------------------------------------------------------
}}














