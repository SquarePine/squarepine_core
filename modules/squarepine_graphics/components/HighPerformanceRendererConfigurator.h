//==============================================================================
#if JUCE_MODULE_AVAILABLE_juce_opengl

/** @returns a juce::String from an OpenGL style string. */
String getGLString (GLenum value);

/** @returns a juce::String from a list of strings in OpenGL style. */
String getGLString (GLenum value, GLuint index);

/** Attemps configuring an OpenGL context with version 3.1 and fancy things
    like continuous repainting.

    By default, this attempts to enable multisampling.
*/
void configureContextWithModernGL (OpenGLContext&, bool shouldEnableMultisampling = true);

/** Logs any GPU/driver related information it can find.

    Note that this must be called from the OpenGL rendering thread
    or it won't work at best, or crash at worst!
*/
void logOpenGLInfoCallback (OpenGLContext&);

/** @returns the list of available OpenGL extensions (if any).
    Be warned that this can only be called on the OpenGL thread!

    @param peer If you pass a valid ComponentPeer, this will try to resolve
                the additional extension as provided by wglGetExtensionsStringARB()
                on Windows.
*/
StringArray getOpenGLExtensions (ComponentPeer* peer = nullptr);

#endif // JUCE_MODULE_AVAILABLE_juce_opengl

//==============================================================================
/** A type of OpenGL configurator that will automatically select the
    most performant means of rendering the targeted Component.

    If the driver supplies OpenGL 3.1, this will configure the window to use that.
    Otherwise the software renderer will be used.
*/
class HighPerformanceRendererConfigurator final
{
public:
    //==============================================================================
    /** Handy shortcut because typing long junk like this is tedious. */
    using WeakRef = WeakReference<HighPerformanceRendererConfigurator>;

    //==============================================================================
    /** Constructor. */
    HighPerformanceRendererConfigurator() = default;

    //==============================================================================
    /** This will try to configure the window with OpenGL.

        If OpenGL is unavailable on the running platform, this function will do nothing.

        If the version of OpenGL present is too old (ie: pre 3.1),
        the context will be detached automatically (on the message thread),
        only to fallback to software rendering.

        @warning You must call this BEFORE calling DocumentWindow::setContentOwned().

        @param component            The target Component to configure with OpenGL.
        @param continuouslyRepaint  If set to true, this will force the context to continuously repaint
                                    the specified Component.
        @param allowVsync           If set to true, this will try to figure out of vsync is available,
                                    and if an adaptable option is available. It will then set this
                                    up accordingly, prioritising adaptive first.
    */
    void configureWithOpenGLIfAvailable (Component& component,
                                         bool continuouslyRepaint = false,
                                         bool allowVsync = true);

    //==============================================================================
   #if JUCE_MODULE_AVAILABLE_juce_opengl
    std::unique_ptr<OpenGLContext> context;
   #endif

private:
    //==============================================================================
    class DetachContextMessage;
    friend class DetachContextMessage;
    std::atomic<bool> hasContextBeenForciblyDetached { false };

    //==============================================================================
    JUCE_DECLARE_WEAK_REFERENCEABLE (HighPerformanceRendererConfigurator)
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HighPerformanceRendererConfigurator)
};
