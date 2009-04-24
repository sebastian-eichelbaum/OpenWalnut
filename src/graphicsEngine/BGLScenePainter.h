//---------------------------------------------------------------------------
//
// Project: BrainCognize
//
//---------------------------------------------------------------------------

#ifndef BGLSCENEPAINTER_H
#define BGLSCENEPAINTER_H

/**
 * \ingroup ge
 * This is the base class for rendering the scene graph.
 */
class BGLScenePainter
{
public:
    BGLScenePainter();
    virtual ~BGLScenePainter();

    /**
     * Initialize the OpenGL context, e.g., set main transformation matrices.
     */
    virtual void initGL();

    /**
     * Do the OpenGL painting in every frame.
     */
    virtual void paintGL();

    /**
     * Called, when the window is resized and re-initialization of the
     * window parameters and matrices may need to be done.
     */
    virtual void resizeGL(int w, int h);
};


/**
  \defgroup ge GraphicsEngine

  \brief
  This module implements the graphics engine for BrainCognize.

*/

#endif /* BGLSCENEPAINTER_H */
