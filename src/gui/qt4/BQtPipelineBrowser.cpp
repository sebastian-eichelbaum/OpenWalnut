//---------------------------------------------------------------------------
//
// Project: BrainCognize
//
//---------------------------------------------------------------------------

#include "BQtPipelineBrowser.h"

BQtPipelineBrowser::BQtPipelineBrowser()
    : QDockWidget( "Pipeline Browser" ),
      m_recommendedSize()
{
    setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );
    m_recommendedSize.setWidth( 200 );
    m_recommendedSize.setHeight( 300 );
}

BQtPipelineBrowser::~BQtPipelineBrowser()
{
    // NOTE Auto-generated destructor stub
}

QSize BQtPipelineBrowser::sizeHint() const
{
    return m_recommendedSize;
}
