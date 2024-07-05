/**

    @see https://github.com/joaorossi/dkmethod/tree/master
    @see https://arma.sourceforge.net/docs.html
*/
class AnalogModel final : public dsp::ProcessorBase
{
public:
    /** */
    AnalogModel (double initialSampleRate = 48000.0);

    //==============================================================================
    /** Add an analog component to the current net list.

        @warning You must add the complete circuit before
        calling any of the methods below! This is more of a TODO.

        @see AnalogComponentFactory
    */
    void addComponent (std::unique_ptr<AnalogComponent>);

    //==============================================================================
    /** Prepare model for playing.

        Update matrixes for new sample rate and calculate system steady state.
        Should be called before starting a new audio stream.
    */
    void prepare (const dsp::ProcessSpec&) override;

    /** Process model.

        The AudioBlock should at least contain <numInputs> channels,
        and in the same order the input components were added.
    */
    void process (const dsp::ProcessContextReplacing<double>&);

    /** Process model.

        The AudioBlock should at least contain <numInputs> channels,
        and in the same order the input components were added.
    */
    void process (const dsp::ProcessContextReplacing<float>&) override;

    /** Reset model internal state.

        Sets the state matrix X and non-linear voltages vector V to zeros.
    */
    void reset() override;

private:
    //==============================================================================
    static inline constexpr int maxIter     = 10;
    static inline constexpr double eps      = 0.05;
    static inline const AnalogMatrix ident  = AnalogMatrix::identity (1);
/*
    struct NonlinearModel final
    {
        AnalogMatrix it = ident, Jt = ident, e = ident, J = ident;
    };
*/
    // System Matrixes
    AnalogMatrix A = ident, B = ident, C = ident, D = ident, E = ident,
                 F = ident, G = ident, H = ident, K = ident;

    // Input
    AnalogMatrix U = ident;

    // State
    AnalogMatrix X = ident;

    // Non-linear voltages
    AnalogMatrix V = ident;

    // Model non-linearities helpers (avoids memory allocation in processing)
    AnalogMatrix it = ident, Jt = ident, e = ident, J = ident,
                 iComp1 = ident, jComp1 = ident, iComp2 = ident, jComp2 = ident;

    double sampleRate = 48000.0, samplePeriod = 0.0;

    OwnedArray<AnalogComponent> inputs, outputs, resistors, capacitors,
                                inductors, opas, potentiometers, nonlinears;

    Array<int> nodes;
    AudioBuffer<double> conversionBuffer;

    //==============================================================================
    /** Model non-linearity

        Whole model non-linear function. Contatenates the non-linear function of all components.
        Takes a voltage vector v as input.
        Returns the current vector i and the Jacobian matrix J.
    */
    void modelNonlinearity (const AnalogMatrix& v, AnalogMatrix& i, AnalogMatrix& J);

    /** Non-linear solver

        Solves model non-linearity.
        Takes the p and k matrixes as inputs, and return a matrix where the first
        columns is the voltages vector and the second columns the currents vector.
    */
    void solveNonlinearFunc (const AnalogMatrix& p, const AnalogMatrix& k, AnalogMatrix& i, AnalogMatrix& v);

    void calculateMatrixes();

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AnalogModel)
};
