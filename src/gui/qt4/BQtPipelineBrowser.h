//---------------------------------------------------------------------------
//
// Project: BrainCognize
//
//---------------------------------------------------------------------------

#ifndef BQTPIPELINEBROWSER_H
#define BQTPIPELINEBROWSER_H

#include <QtGui/QDockWidget>

/**
 * Represents the browser showing the loaded data sets
 * and applied filter/methods/algorithms/plugins
 */
class BQtPipelineBrowser: public QDockWidget
{
public:
    BQtPipelineBrowser();
    virtual ~BQtPipelineBrowser();

    /**
     * returns the recommended size for the widget to allow
     * parent widgets to give it a proper initial layout
     */
    QSize sizeHint() const;
private:

    /**
     * Holds the recommended size for the widget
     */
    QSize m_recommendedSize;
};

#endif /* BQTPIPELINEBROWSER_H */
