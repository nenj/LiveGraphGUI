//
//  LiveGraphGUI.h
//
//  Created by Nicolas Enjalbert on 14/01/2014.
//
//
/*

The 'xvalues' array pointers passed to 'LGElement's are meant as time values,
and for proper visualization they must be ordered.
 
*/

#ifndef ____LiveGraph__
#define ____LiveGraph__

#include <iostream>

#ifndef STL_vector_include
#define STL_vector_include
#include <vector>
#endif

#ifndef STL_string_include
#define STL_string_include
#include <string>
#endif

#include "cinder/app/App.h"

//-----------------------------------------------------------------------------

namespace cinder {
    namespace app {
        class App;
    }
    template<typename T> class Vec2;
    typedef Vec2<float> Vec2f;
    template<typename T> class ColorAT;
    typedef ColorAT<float> ColorA;
    class Font;
}

//-----------------------------------------------------------------------------

namespace dscrts { namespace lggui {
    
class LiveGraphGUI;
class LGElement;
class DoubleLiveGraph;
    
//-----------------------------------------------------------------------------
    
class LiveGraphGUI
{
public:
    static cinder::ColorA   darkColor;
    static cinder::ColorA   lightColor;
    static cinder::ColorA   bgColor;
    static cinder::ColorA   textColor;
    static cinder::ColorA   plotColor;
	static float            spacing;
    static float            labelHeight;
    static cinder::Vec2f    padding;
    static cinder::Font     textFont;
    static cinder::Font     initFont();
    
public:
    LiveGraphGUI( cinder::app::App*, float xsize, float ysize );
    ~LiveGraphGUI( );
    
    DoubleLiveGraph*    addGraph( const std::string& graphName,
                                  std::vector<double>* xvalues_, std::vector<double>* yvalues_,
                                  double ymin_, double ymax_ );
    
    void setAbscissaWidth( double );
    void draw();
    void update();
    
protected:
    bool enabled;
    cinder::Vec2f boxSize;
    std::vector<LGElement*> elements;
    
public:
    cinder::Vec2f getBoxSize() { return boxSize; }

public:
    double xwidth;
};
    
    
class LGElement
{
public:
    enum Type {
        DOUBLE_GRAPH
    };

public:
    LGElement( );
    virtual ~LGElement( ) = 0;
    virtual cinder::Vec2f draw( cinder::Vec2f pos ) = 0;
    virtual void update( ) = 0;
    
protected:
    LiveGraphGUI*   parentGUI;
    Type            type;
    std::string     name;
};
    
    
class DoubleLiveGraph : public LGElement
{
public:
    DoubleLiveGraph( LiveGraphGUI*, const std::string&,
                     std::vector<double>*, std::vector<double>*,
                     double, double );
    ~DoubleLiveGraph( );
    cinder::Vec2f draw( cinder::Vec2f pos );
    
    void update( );
    
protected:
    std::vector<double>* xvalues;
    std::vector<double>* yvalues;
    double ymin, ymax;
    
    double xwidth;
    int iXstart;
};
    
}}

#endif /* defined(____LiveGraph__) */
