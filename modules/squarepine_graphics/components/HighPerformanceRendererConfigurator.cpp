//==============================================================================
#if JUCE_MODULE_AVAILABLE_juce_opengl

String getGLString (GLenum value)
{
    return reinterpret_cast<const char*> (gl::glGetString (value)); // NB: glGetString is from v2.0+.
}

String getGLString (GLenum value, GLuint index)
{
    // NB: glGetStringi is from v3.0+.
    if (gl::glGetStringi != nullptr)
        return reinterpret_cast<const char*> (gl::glGetStringi (value, index));

    return {};
}

void configureContextWithModernGL (OpenGLContext& context, bool shouldEnableMultisampling)
{
    gl::loadFunctions();
    gl::loadExtensions();

    // NB: On failure, JUCE will backtrack to an earlier version of OpenGL.
    context.setOpenGLVersionRequired (OpenGLContext::OpenGLVersion::openGL3_2);
    context.setTextureMagnificationFilter (OpenGLContext::linear);

    context.setMultisamplingEnabled (true);
    if (shouldEnableMultisampling)
    {
        OpenGLPixelFormat pf;
        pf.stencilBufferBits = 8;
        pf.multisamplingLevel = 1;
        context.setPixelFormat (pf);
    }
}

void logOpenGLInfoCallback (OpenGLContext&)
{
    using namespace juce::gl;

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

    I suppose cheap bastards will always exist...
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
       #if JUCE_MODULE_AVAILABLE_juce_opengl
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
   #if JUCE_MODULE_AVAILABLE_juce_opengl
    context.reset (new OpenGLContext());
    configureContextWithModernGL (*context.get());

    context->setContinuousRepainting (false);

    context->attachTo (component);
   #else
    ignoreUnused (component);
   #endif
}

void HighPerformanceRendererConfigurator::paintCallback()
{
   #if JUCE_MODULE_AVAILABLE_juce_opengl
    if (! hasContextBeenForciblyDetached
        && context != nullptr
        && context->isActive()
        && context->isAttached())
    {
        GLint major = 0;
        gl::glGetIntegerv (gl::GL_MAJOR_VERSION, &major);

        if (major < 3 || gl::glBindVertexArray == nullptr)
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
