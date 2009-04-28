//---------------------------------------------------------------------------
//
// Project: BrainCognize
//
//---------------------------------------------------------------------------

#ifndef BGUI_H
#define BGUI_H

/**
 * This class prescribes the interface to the GUI.
 * \ingroup gui
 */
class BGUI
{
    public:
        virtual ~BGUI();

        /**
         * \brief
         * enable/disable the access to the GUI
         * \post
         * GUI enabled/disabled
         * \param enable true iff gui should be enabled
         */
        virtual void setEnabled( bool enable ) = 0;
};

/**
  \defgroup gui GUI

  \brief
  This module implements the graphical user interface for BrainCognize.

*/

#endif  // BGUI_H
