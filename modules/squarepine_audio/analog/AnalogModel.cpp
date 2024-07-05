AnalogModel::AnalogModel (double sr) :
    sampleRate (sr),
    samplePeriod (1.0 / sampleRate)
{
}

//==============================================================================
void AnalogModel::addComponent (std::unique_ptr<AnalogComponent> component)
{
    if (component == nullptr)
    {
        jassertfalse;
        return;
    }

    for (auto componentNode : component->nodes)
        nodes.addIfNotAlreadyThere (componentNode);

    switch (component->type)
    {
        case AnalogComponent::Type::input:          inputs.add (component.release()); break;
        case AnalogComponent::Type::output:         outputs.add (component.release()); break;
        case AnalogComponent::Type::resistor:       resistors.add (component.release()); break;
        case AnalogComponent::Type::capacitor:      capacitors.add (component.release()); break;
        case AnalogComponent::Type::inductor:       inductors.add (component.release()); break;
        case AnalogComponent::Type::opa:            opas.add (component.release()); break;
        case AnalogComponent::Type::potentiometer:  potentiometers.add (component.release()); break;

        case AnalogComponent::Type::diode:
        case AnalogComponent::Type::transistor:
        case AnalogComponent::Type::triode:
            nonlinears.add (component.release());
        break;

        default:
            jassertfalse;
        break;
    };
}

//==============================================================================
void AnalogModel::process (const dsp::ProcessContextReplacing<float>& /*context*/)
{
#if 0 // TODO
    {
        conversionBuffer.clear();
        // context.getInputBlock().copyTo (conversionBuffer);
        dsp::AudioBlock<double> block (conversionBuffer);
        block = context.getInputBlock();
        dsp::ProcessContextReplacing<double> doubleContext (block);
        process (doubleContext);
    }

    context.getOutputBlock().copyFrom (conversionBuffer);
#endif
}

void AnalogModel::process (const dsp::ProcessContextReplacing<double>& context)
{
    auto block = context.getOutputBlock();
    auto samples = block.getNumSamples();

    // Nonlinear currents
    auto I = AnalogMatrix (V.getNumRows(), 1);

    AnalogMatrix p = ident, out = ident;

    for (int n = 0; n < samples; ++n)
    {
        for (int i = 0; i < inputs.size(); ++i)
            U (i, 0) = block.getSample (i, n);

        p = (G * X) + (H * U);
        solveNonlinearFunc (p, K, I, V);

        out = (D * X) + (E * U) + (F * I);
        X = (A * X) + (B * U) + (C * I);

        for (int i = 0; i < outputs.size(); ++i)
            block.setSample (i, n, out (i, 0));
    }
}

void AnalogModel::prepare (const dsp::ProcessSpec& spec)
{
    conversionBuffer.setSize (jmax (1, (int) spec.numChannels),
                              jmax (1, (int) spec.maximumBlockSize),
                              false, false, true);

    conversionBuffer.clear();

    sampleRate = spec.sampleRate;
    samplePeriod = 1.0 / sampleRate;
    calculateMatrixes();
}

void AnalogModel::reset()
{
    X.clear();
    V.clear();
}

//==============================================================================
void AnalogModel::solveNonlinearFunc (const AnalogMatrix& p, const AnalogMatrix& k, AnalogMatrix& i, AnalogMatrix& v)
{
    auto iter = maxIter;
    AnalogMatrix v0     = v,
                 v1     = v0 + (2.0 * eps),
                 eye    = AnalogMatrix::identity (std::max (v1.getNumRows(), v1.getNumColumns()));

    while (iter > 0 && std::any_of (vectorise (std::abs (v1 - v0)) > eps))
    {
        v0 = v1;
        modelNonlinearity (v1, it, Jt);
        e = p + (k * it) - v1;
        J = (k * Jt) - eye;
        v1 = v0 - solve (J, e);
        --iter;
    }

    v = v1;

    modelNonlinearity (v, i, J);
}

void AnalogModel::modelNonlinearity (const AnalogMatrix& v, AnalogMatrix& i, AnalogMatrix& j)
{
    AnalogMatrix iComp1 (1, 1),
                 jComp1 (1, 1), 
                 iComp2 (2, 1),
                 jComp2 (2, 2);

    for (int nl = 0; nl < nonlinears.size();)
    {
        auto comp = nonlinears.getUnchecked (nl);

        jassert (comp->model != nullptr);
        
        if (comp->numPorts == 1)
        {
            comp->model (v.row (nl), iComp1, jComp1);
            i.submat (nl, 0, nl, 0) = iComp1;
            j.submat (nl, nl, nl, nl) = jComp1;
        }
        else if (comp->numPorts == 2)
        {
            comp->model (v.rows (nl, nl + 1), iComp2, jComp2);
            i.submat (nl, 0, nl + 1, 0) = iComp2;
            j.submat (nl, nl, nl + 1, nl + 1) = jComp2;
        }
        else
        {
            // TODO?
        }

        nl += comp.numPorts;
    }
}

void AnalogModel::calculateMatrixes()
{
    ScopedNoDenormals noDenomals;

    AnalogMatrix Nr (resistors.size(), nodes.size()),
                 Gr (resistors.size(), resistors.size()),
                 Nx (capacitors.size() + inductors.size(), nodes.size()),
                 Gx (capacitors.size() + inductors.size(), capacitors.size() + inductors.size()),
                 Z  (capacitors.size() + inductors.size(), capacitors.size() + inductors.size()),
                 Nu (inputs.size(), nodes.size()),
                 No (outputs.ssize(), nodes.size()),
                 Nn (1);

    {
        int numNonlinearPorts = 0;
        for (const auto& nl : nonlinears)
            numNonlinearPorts += nl.numPorts;

        Nn      = AnalogMatrix (numNonlinearPorts, nodes.size());
        Jt      = AnalogMatrix (numNonlinearPorts, numNonlinearPorts);
        it      = AnalogMatrix (numNonlinearPorts, 1);
        iComp1  = AnalogMatrix (1, 1);
        jComp1  = AnalogMatrix (1, 1);
        iComp2  = AnalogMatrix (2, 1);
        jComp2  = AnalogMatrix (2, 2);
    }

    AnalogMatrix Nopai (opas.size(), nodes.size()),
                 Nopao (opas.size(), nodes.size());

    // Input size
    U = AnalogMatrix (inputs.size(), 1);

    // State size
    X = AnalogMatrix (capacitors.size() + inductors.size(), 1);

    // Nonlinear voltages
    V = AnalogMatrix (Nn.getNumRows(), 1);

    for (int i = 0; i < inputs.size(); ++i)
    {
        const auto& mat = inputs.getUnchecked (i);

        Nu (i, mat->nodes[0]) = 1.0;
        Nu (i, mat->nodes[1]) = -1.0;
    }

    for (int i = 0; i < outputs.size(); ++i)
    {
        const auto& mat = outputs.getUnchecked (i);

        No (i, mat->nodes[0]) = 1.0;
        No (i, mat->nodes[1]) = -1.0;
    }

    // TODO - Potentiometer

    for (int i = 0; i < opas.size(); ++i)
    {
        const auto& mat = opas.getUnchecked (i);

        Nopai (i, mat->nodes[0]) = 1.0;
        Nopai (i, mat->nodes[1]) = -1.0;
        Nopao (i, mat->nodes[2]) = 1.0;
    }

    for (int i = 0; i < resistors.size(); ++i)
    {
        const auto& mat = resistors.getUnchecked (i);

        Gr (i, i) = 1.0 / mat.value;
        Nr (i, mat->nodes[0]) = 1.0;
        Nr (i, mat->nodes[1]) = -1.0;
    }

    for (int i = 0; i < capacitors.size() + inductors.size(); ++i)
    {
        if (i < capacitors.size())
        {
            const auto& mat = capacitors.getUnchecked (i);

            Z (i, i) = 1.0;
            Gx (i, i) = 2.0 * mat->value / samplePeriod;
            Nx (i, mat->nodes[0]) = 1.0;
            Nx (i, mat->nodes[1]) = -1.0;
        }
        else
        {
            const auto& mat = inductors.getUnchecked (i - capacitors.size());

            Z (i, i) = -1.0;
            Gx (i, i) = samplePeriod / (2.0 * mat->value);
            Nx (i, mat->nodes[0]) = 1.0;
            Nx (i, mat->nodes[1]) = -1.0;
        }
    }

    for (int i = 0; i < nonlinears.size(); ++i)
    {
        const auto& mat = nonlinears.getUnchecked (i);

        Nn (i, mat->nodes[0]) = 1.0;
        Nn (i, mat->nodes[1]) = -1.0;

        if (mat.numPorts == 2)
        {
            Nn (i + 1, mat->nodes[2]) = 1.0;
            Nn (i + 1, mat->nodes[3]) = -1.0;
        }
    }

    // Trim ground node of incidence matrixes
    Nr.shed_col (0);
    Nx.shed_col (0);
    Nn.shed_col (0);
//  Nv.shed_col (0);
    Nu.shed_col (0);
    No.shed_col (0);
    Nopao.shed_col (0);
    Nopai.shed_col (0);

    auto numVoltageSources = Nu.getNumRows();

    // Assemble conductance matrix
    // TODO - Add variable resistor matrixes
    auto S = [&]()
    {
        const auto tmp1 = joinHorizontal ((Nr.t() * Gr * Nr) + (Nx.t() * Gx * Nx), Nu.t());
        const auto tmp2 = joinHorizontal (Nu, AnalogMatrix (numVoltageSources, numVoltageSources));
        return joinVertical (tmp1, tmp2);
    }();

    // Extend conductance matrix for OPAs
    if (! opas.isEmpty())
    {
        Nopao = joinHorizontal (Nopao, AnalogMatrix (Nopao.getNumRows(), numVoltageSources));
        Nopai = joinHorizontal (Nopai, AnalogMatrix (Nopai.getNumRows(), numVoltageSources));

        const auto tmp1 = joinHorizontal (S, Nopao.t());
        const auto tmp2 = joinHorizontal (Nopao + (Nopai * 1000000.0),
                                          AnalogMatrix (Nopao.getNumRows(), Nopao.getNumRows()));

        S = joinVertical (tmp1, tmp2);
    }

    auto Nxp    = joinHorizontal (Nx, AnalogMatrix (Nx.getNumRows(), numVoltageSources + opas.size()));
    auto Nnp    = joinHorizontal (Nn, AnalogMatrix (Nn.getNumRows(), numVoltageSources + opas.size()));
    auto Nop    = joinHorizontal (No, AnalogMatrix (No.getNumRows(), numVoltageSources + opas.size()));
    auto Nup    = joinHorizontal (Nu, AnalogMatrix (Nu.getNumRows(), numVoltageSources));
    auto Nup2   = joinVertical (AnalogMatrix (Nu.getNumColumns(), numVoltageSources),
                                AnalogMatrix::identity (numVoltageSources, numVoltageSources));

    if (! opas.isEmpty())
        Nup2 = joinVertical (Nup2, AnalogMatrix (opas.size(), numVoltageSources));

    AnalogMatrix Si     = S.i(),
                 NxpT   = Nxp.t(),
                 NnpT   = Nnp.t();

    A = ((Z * Gx * Nxp * Si * NxpT) * 2.0) - Z;
    B = (Z * Gx * Nxp * Si * Nup2) * 2.0;
    C = (Z * Gx * Nxp * Si * NnpT) * 2.0;
    D = Nop * Si * NxpT;
    E = Nop * Si * Nup2;
    F = Nop * Si * NnpT;
    G = Nnp * Si * NxpT;
    H = Nnp * Si * Nup2;
    K = Nnp * Si * NnpT;
}
