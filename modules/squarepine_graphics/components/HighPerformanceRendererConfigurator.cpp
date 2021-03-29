//==============================================================================
#if JUCE_MODULE_AVAILABLE_juce_opengl && JUCE_OPENGL

String getGLString (GLenum value)
{
    return reinterpret_cast<const char*> (glGetString (value));
}

void configureContextWithModernGL (OpenGLContext& context, bool shouldEnableMultisampling)
{
    // NB: On failure, JUCE will backtrack to an earlier version of OpenGL.
    context.setOpenGLVersionRequired (OpenGLContext::openGL3_2);
    context.setContinuousRepainting (false);

    if (shouldEnableMultisampling)
    {
        context.setMultisamplingEnabled (true);
        context.setTextureMagnificationFilter (OpenGLContext::linear);
        OpenGLPixelFormat pf;
        pf.stencilBufferBits = 8;
        pf.multisamplingLevel = 1;
        context.setPixelFormat (pf);
    }
}

void logOpenGLInfoCallback (OpenGLContext&)
{
    GLint major = 0, minor = 0;
    glGetIntegerv (GL_MAJOR_VERSION, &major);
    glGetIntegerv (GL_MINOR_VERSION, &minor);

    String stats;
    stats
    << newLine
    << "--------------------------------------------------" << newLine << newLine
    << "=== OpenGL/GPU Information ===" << newLine << newLine
    << "Vendor: " << getGLString (GL_VENDOR) << newLine
    << "Renderer: " << getGLString (GL_RENDERER) << newLine
    << "OpenGL Version: " << getGLString (GL_VERSION) << newLine
    << "OpenGL Major: " << String (major) << newLine
    << "OpenGL Minor: " << String (minor) << newLine
    << "OpenGL Shading Language Version: " << getGLString (GL_SHADING_LANGUAGE_VERSION) << newLine
    << newLine
    << "OpenGL Extensions:" << newLine
    << newLine;

    auto ext = StringArray::fromTokens (getGLString (GL_EXTENSIONS), " ", "");
    ext.trim();
    ext.removeEmptyStrings();
    ext.removeDuplicates (true);
    ext.sort (true);

    for (const auto& s : ext)
        stats << "\t- " << s << newLine;

    stats
    << newLine
    << "--------------------------------------------------" << newLine;

    Logger::writeToLog (stats);
}

static inline void logGlInfoOnce (OpenGLContext& c)
{
    static std::once_flag openGlLogFlag;
    std::call_once (openGlLogFlag, logOpenGLInfoCallback, c);
}

#endif

//==============================================================================
class HighPerformanceRendererConfigurator::DetachContextMessage final : public MessageManager::MessageBase
{
public:
    DetachContextMessage (HighPerformanceRendererConfigurator& c) :
        configurator (&c)
    {
    }

    void messageCallback() override
    {
       #if JUCE_MODULE_AVAILABLE_juce_opengl && JUCE_OPENGL
        if (auto* c = configurator.get())
            c->context = nullptr;
       #endif
    }

private:
    WeakReference<HighPerformanceRendererConfigurator> configurator;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DetachContextMessage)
};

//==============================================================================
void HighPerformanceRendererConfigurator::configureWithOpenGLIfAvailable (Component& component)
{
   #if JUCE_MODULE_AVAILABLE_juce_opengl && JUCE_OPENGL
    context.reset (new OpenGLContext());
    configureContextWithModernGL (*context.get());
    context->attachTo (component);
    context->setSwapInterval (1); // Make sure vsync is active
   #else
    ignoreUnused (component);
   #endif
}

void HighPerformanceRendererConfigurator::paintCallback()
{
   #if JUCE_MODULE_AVAILABLE_juce_opengl && JUCE_OPENGL
    if (! hasContextBeenForciblyDetached
        && context != nullptr
        && context->isActive()
        && context->isAttached())
    {
        GLint major = 0;
        glGetIntegerv (GL_MAJOR_VERSION, &major);

        if (major < 3)
        {
            (new DetachContextMessage (*this))->post();
            hasContextBeenForciblyDetached = true;
        }
        else
        {
            context->executeOnGLThread (logGlInfoOnce, false);
        }
    }
   #endif
}
