//==============================================================================
#if SQUAREPINE_USE_OPENGL

String getGLString (GLenum value)
{
    return reinterpret_cast<const char*> (glGetString (value));
}

String getGLString (GLenum value, GLuint index)
{
   #if JUCE_WINDOWS
    typedef const GLubyte* (*getGLStringiPtr) (GLenum, GLuint);

    if (auto extFunc = (getGLStringiPtr) OpenGLHelpers::getExtensionFunction ("getGLStringi"))
        return reinterpret_cast<const char*> (extFunc (value, index));

    return {};
   #else
    return reinterpret_cast<const char*> (glGetStringi (value, index));
   #endif
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
    GLint major = 0, minor = 0, numExtensions = 0;
    glGetIntegerv (GL_MAJOR_VERSION, &major);
    glGetIntegerv (GL_MINOR_VERSION, &minor);
    glGetIntegerv (GL_NUM_EXTENSIONS, &numExtensions);

    const char* const separatorLine = "--------------------------------------------------";

    String stats;
    stats
    << newLine << separatorLine << newLine << newLine
    << "=== OpenGL/GPU Information ===" << newLine << newLine
    << "Vendor: " << getGLString (GL_VENDOR) << newLine
    << "Renderer: " << getGLString (GL_RENDERER) << newLine
    << "OpenGL Version: " << getGLString (GL_VERSION) << newLine
    << "OpenGL Major: " << String (major) << newLine
    << "OpenGL Minor: " << String (minor) << newLine
    << "OpenGL Shading Language Version: " << getGLString (GL_SHADING_LANGUAGE_VERSION) << newLine
    << "OpenGL Num Extensions Found: " << numExtensions << newLine
    << "OpenGL Extensions:" << newLine
    << newLine;

    auto extensionsFromGL = getGLString (GL_EXTENSIONS).trim();
    if (extensionsFromGL.isEmpty())
        for (GLuint i = 0; i < (GLuint) numExtensions; ++i)
            extensionsFromGL << getGLString (GL_EXTENSIONS, i) << " ";

    auto ext = StringArray::fromTokens (extensionsFromGL, " ", "");
    ext.trim();
    ext.removeEmptyStrings();
    ext.removeDuplicates (true);
    ext.sort (true);

    for (const auto& s : ext)
        stats << "\t- " << s << newLine;

    stats << newLine << separatorLine << newLine;

    Logger::writeToLog (stats);
}

static inline void logGlInfoOnce (OpenGLContext& c)
{
    static std::once_flag openGlLogFlag;
    std::call_once (openGlLogFlag, logOpenGLInfoCallback, c);
}

#endif

//==============================================================================
/** This is needed in order to deal with idiots that refuse to upgrade
    their hardware dated from the lower 2010s and below.

    People being cheap bastards will always exist...
*/
class HighPerformanceRendererConfigurator::DetachContextMessage final : public MessageManager::MessageBase
{
public:
    DetachContextMessage (HighPerformanceRendererConfigurator& c) :
        configurator (&c)
    {
    }

    void messageCallback() override
    {
       #if SQUAREPINE_USE_OPENGL
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
   #if SQUAREPINE_USE_OPENGL
    context.reset (new OpenGLContext());
    configureContextWithModernGL (*context.get());
    context->attachTo (component);
   #else
    ignoreUnused (component);
   #endif
}

void HighPerformanceRendererConfigurator::paintCallback()
{
   #if SQUAREPINE_USE_OPENGL
    if (! hasContextBeenForciblyDetached
        && context != nullptr
        && context->isActive()
        && context->isAttached())
    {
        GLint major = 0;
        glGetIntegerv (GL_MAJOR_VERSION, &major);

        if (major < 3 || OpenGLHelpers::getExtensionFunction ("glBindVertexArray") == nullptr)
        {
            (new DetachContextMessage (*this))->post();
            hasContextBeenForciblyDetached = true;
            Logger::writeToLog ("WARNING!! --- Forcibly removed the OpenGL context because the system is so crappy...");
        }
        else
        {
            context->executeOnGLThread (logGlInfoOnce, false);
        }
    }
   #endif
}
